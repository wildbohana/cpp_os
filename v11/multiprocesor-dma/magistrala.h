#ifndef MAGISTRALA_H_INCLUDED
#define MAGISTRALA_H_INCLUDED

#include "dijagnostika.h"

#include <mutex>
#include <condition_variable>

using namespace std;
using namespace chrono;


// Klasa magistrale (sabirnice). 
// Glavna deljena promenljiva za sinhronizaciju pristupa procesora memoriji i dma kontroleru.
class Magistrala 
{
	public:
    // Stanja sabirnice - slobodna, čitanje iz memorije, pisanje u memoriju, dma transfer.
	// Primetiti sličnost sa stanjima iz zadatka "multiprocesor":
    enum Stanje { SLOBODNA, MEM_CITAJ, MEM_PISI, DMA};

    // Struktura u koju se beleže parametri DMA transfera.
	// Odakle, kome i koliko bajtova se čita ili piše.
    struct DMA_transfer {
        int odakle;
        int koliko;
        int kome;
    };

	private:
		Dijagnostika& dijagnostika;
		// Referenca (ne kopija) na memorijski blok
		Memorija& memorija;    
		mutex m;
		Stanje stanje;
		condition_variable magistrala_slobodna;
		condition_variable DMA_kontroler; 
		 // Polje za komunikaciju izmedju dma() i okidac_dma_kontrolera(), struktura
		DMA_transfer transfer;
		bool kraj;

	public:		
		// Proširiti po potrebi ...
		Magistrala(Dijagnostika& d, Memorija& mem) : dijagnostika(d), memorija(mem)
		{
			stanje = SLOBODNA;
        	kraj = false;
		}

		Dijagnostika& getDijagnostika() 
		{
			return dijagnostika;
		}

		// Implementirati ...
		char citaj_memoriju(int adresa) 
		{
			unique_lock<mutex> l(m);

			// Čeka se dok god stanje magistrale nije slobodno. Drugim rečima, nema mogućnosti
			// za više čitača, istovremeno kao što je bilo u zadatku "multiprocesor".
			while (stanje != SLOBODNA) magistrala_slobodna.wait(l);
			
			// Zaključavanje magistrale traje 700ms.
			// Stanje magistrale postaje da se čita memorija.
			this_thread::sleep_for(milliseconds(700));  
			stanje = MEM_CITAJ;                         
			
			// Otključava se KS. Magistralu i dalje niko ne može da koristi ali barem može da se uveže u čekanje.
			l.unlock(); 

			// Pozivi blokirajućih operacija treba (ako je moguće) da su van kritične sekcije (tako je efikasnije).
			// Čitanje iz memorije. Obratiti pažnju da memorija ima svoj poseban muteks.
			char vrednost = memorija.citaj(adresa);     

			l.lock();
			// Stanje magistrale ukazuje da je slobodna.
			// Notifikuju se niti koje čekaju na čitanje iz memorije, upis ili dma, da mogu
			stanje = SLOBODNA;                          
			magistrala_slobodna.notify_one();    

			return vrednost;    
		}

		// Implementirati ...
		void pisi_u_memoriju(int adresa, char vrednost) 
		{
			unique_lock<mutex> l(m);
			
			// Čeka se dok god stanje magistrale nije slobodno.
			while (stanje != SLOBODNA) magistrala_slobodna.wait(l);
			
			// Zaključavanje magistrale traje 700ms.
			// Stanje magistrale postaje da se piše u memoriju.
			this_thread::sleep_for(milliseconds(700));  
			stanje = MEM_PISI;
			
			// Otključava se KS. Magistralu i dalje niko ne može da koristi ali barem može da se uveže u čekanje.
			l.unlock();                                 
			
			// Pisanje u memoriju.
			memorija.pisi(adresa, vrednost);            

			l.lock();

			// Stanje magistrale ukazuje da je slobodna.
			// Notifikuju se niti koje čekaju na čitanje iz memorije, upis ili dma.
			stanje = SLOBODNA;
			magistrala_slobodna.notify_one();
		}

		// Implementirati ...
		void dma(DMA_transfer t) 
		{
			unique_lock<mutex> l(m);

			// Kao i kod prethodnih metoda, potrebno je sačekati da se magistrala oslobodi.
			while (stanje != SLOBODNA) magistrala_slobodna.wait(l);
			
			// Zaključavanje magistrale traje 700ms.
			// Stanje se menja u DMA. Magistrala je namenjena samo za DMA.
			this_thread::sleep_for(milliseconds(700));  
			stanje = DMA;                               
			
			// Transfer deljeno polje dobija vrednost t. 
			// Posmatrati kao razmenu podataka između dve niti (procesor i DMA kontroler).
			transfer = t;                               
			DMA_kontroler.notify_one();
		}
		
		// Implementirati ...
		DMA_transfer okidac_dma_kontrolera() 
		{
			unique_lock<mutex> l(m);
			
			// Kontroler čeka da jedan od procesora zatraži DMA prenos.
			while (stanje != DMA && !kraj) DMA_kontroler.wait(l);

			if (kraj) return {-1, -1, -1};

			l.unlock();
			
			// Tokom DMA transfera, direktno se čita i piše u memoriju, bez posredovanja procesora.
			for (int i = 0; i < transfer.koliko; i++) 
			{
				char vrednost = memorija.citaj(transfer.odakle + i);
				memorija.pisi(transfer.kome + i, vrednost);
			}

			l.lock();
			
			// DMA transfer je završen, magistrala se oslobađa.
			stanje = SLOBODNA;                          
			magistrala_slobodna.notify_one();

			return transfer;
		}
		
		// Implementirati ...
		void zavrsi() 
		{
			kraj = true;
	        DMA_kontroler.notify_one();	
		}
};

#endif // MAGISTRALA_H_INCLUDED
