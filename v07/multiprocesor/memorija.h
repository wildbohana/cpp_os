#ifndef MEMORIJA_H_INCLUDED
#define MEMORIJA_H_INCLUDED

#include <mutex>
#include <condition_variable>

#include "dijagnostika.h"

using namespace std;
using namespace chrono;

enum Stanje {SLOBODAN, CITANJE, UPIS};

// Veličina reči je 4 bajta
const int VELICINA_RECI = 4;

// Struktura koja predstavlja opis trenutnih pristupa jednoj memorijskoj reči
struct Pristup 
{
	Stanje stanje; 
	int aktivnih_citaca; 
	int citaca_ceka;
	condition_variable citanje;
	condition_variable upis; 

	// Na početku je stanje slobodno i nema čitača i pisača
	Pristup() : stanje(SLOBODAN), aktivnih_citaca(0), citaca_ceka(0) {} 
};    

class Memorija 
{
	private:
		Dijagnostika& dijagnostika;
		// Memorija je predstavljena vektorom karaktera
		// Drugi vektor je vektor pokazivača na strukture koje beleže pristupe datoj reči
		Pristup pristup;
		mutex m;
    	vector<char> mem;
    	vector<Pristup*> pristupi;

	public:
		// Proširiti po potrebi ...
		Memorija(Dijagnostika& d, int bajtova) : dijagnostika(d), mem(bajtova) 
		{
			// Broj reči se zaokružuje na viši celi broj
			// Primer - 10 bajta = 3 reči (12 bajta). Generišu se 3 instance strukture Pristup.
			int reci = (bajtova + (VELICINA_RECI - 1)) / VELICINA_RECI;    
			
			for (int i = 0; i < reci; i++) 
				pristupi.push_back(new Pristup);
		}

		// Naknadno dodat destruktor 
		// Pošto su svi pristupi dinamički generisani moraju se u destruktoru i izbrisati
		~Memorija() 
		{
			// vector<Pristup *>::iterator == auto
			for (auto it = pristupi.begin(); it != pristupi.end(); it++)
				delete *it;
		}

		/*
		Metoda koju poziva nit koja simulira proces koji pristupa memoriji kako bi obavila čitanje iz nje
		
		rbp    - Redni broj procesa
		adresa - Lokacija sa koje se čita vrednost prethodno upisana u memoriju
		
		Potrebno je pozvati dijagnostika.proces_ceka_citanje kada je memorijska reč zaključana i proces mora da čeka.
		Potrebno je pozvati dijagnostika.proces_procitao onda kada je ostvaren pristup memoriji i kada je vrednost učitana iz nje.
		*/

		// Implementirati ...
		char citaj(int rbp, int adresa) 
		{
			// Izračunavanje pozicije reči koja se čita. Adresa je memorijska adresa u bajtima
			int rec = adresa / VELICINA_RECI;

			unique_lock<mutex> l(m);
			
			// Dok god je stanje date reči takvo da neko trenutno piše u nju:
			// - povećaj broj onih koji žele da čitaju iz reči
			// - čekaj na signal za dozvolu čitanja iz reči
			// - smanji broj onih koji žele da čitaju iz reči
			while (pristupi[rec]->stanje == UPIS) 
			{
				dijagnostika.proces_ceka_citanje(rbp, adresa);
				(pristupi[rec]->citaca_ceka)++;
				pristupi[rec]->citanje.wait(l);
				(pristupi[rec]->citaca_ceka)--;
			}

			// Povecaj broj aktivnih čitaca i prebaci stanje reči u to da se ona čita
			(pristupi[rec]->aktivnih_citaca)++;
			pristupi[rec]->stanje = CITANJE;

			// Čitanje traje 1 sekundu
			l.unlock();
			this_thread::sleep_for(seconds(1));
			l.lock();
			
			// Ako nema više aktivnih čitača, TEK TADA može obavestiti pisače
			// Efektivno ovo daje veći prioritet čitačima
			// Oslobađa datu reč
			if (--(pristupi[rec]->aktivnih_citaca) == 0) 
			{ 
				pristupi[rec]->upis.notify_one();
				pristupi[rec]->stanje = SLOBODAN;
			}

			dijagnostika.proces_procitao(rbp, adresa, mem[adresa]);
			
			// Vrati sadržaj sa pročitane memorijske adrese
			return mem[adresa];
		}

		/*
		Metoda koju poziva nit koja simulira proces koji pristupa memoriji kako bi obavila upis u nju
		
		rbp      - Redni broj procesa
		adresa   - Lokacija u koju se upisuje nova vrednost
		vrednost - Vrednost koja se upisuje u memoriju
		
		Potrebno je pozvati dijagnostika.proces_ceka_upis kada je memorijska reč zaključana i proces mora da čeka.
		Potrebno je pozvati dijagnostika.proces_upisao onda kada je ostvaren pristup memoriji i kada je vrednost upisana u nju.
		*/
	
		// Implementirati ...
		void pisi(int rbp, int adresa, char vrednost) 
		{
			// Izračunavanje pozicije reči koja se čita. Adresa je memorijska adresa u bajtima
			int rec = adresa / VELICINA_RECI;

			unique_lock<mutex> l(m);
			
			// Dok god stanje memorijske reči nije slobodno, čekaj da se oslobodi
			// Ako ima čitača, svi čitači prvo čitaju pa tek onda pisač piše
			while (pristupi[rec]->stanje != SLOBODAN) 
			{
				dijagnostika.proces_ceka_upis(rbp, adresa);
				pristupi[rec]->upis.wait(l);
			}
			
			// Stanje se menja u upis
			pristupi[rec]->stanje = UPIS;               

			// Pisanje traje 1 sekundu
			l.unlock();
			this_thread::sleep_for(seconds(1));
			l.lock();
			
			// Nova vrednost se upisuje u adresu. Obratiti pažnju da se ovo radi tek
			// nakon sleep-a. Konzistentnost se ne narušava jer nit koja upisuje drži propusnicu
			mem[adresa] = vrednost;
			
			// Stanje nakon upisa je slobodno
			pristupi[rec]->stanje = SLOBODAN;

			dijagnostika.proces_upisao(rbp, adresa, vrednost);
		
			// Dok god ima čitača koji čekaju, obavesti ih SVE da mogu da čitaju. Ovo je
			// zato što više čitača smeju da čitaju istovremeno
			// Ako nema čitača, obavesti pisače da mogu vršiti upis (ako ih ima naravno)
			if (pristupi[rec]->citaca_ceka)        
				pristupi[rec]->citanje.notify_all();    
			else
				pristupi[rec]->upis.notify_one();       
		}
};

#endif // MEMORIJA_H_INCLUDED
