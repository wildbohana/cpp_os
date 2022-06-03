#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include "dijagnostika.h"
#include "naredba.h"
#include "povratna_vrednost.h"

#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

struct Zahtev 
{
    public:
		// Koliko VM je potrebno zauzeti
        int zauzmi;             
        condition_variable cv;

        Zahtev(int zauzmi_memorije) 
		{
            zauzmi = zauzmi_memorije;
        }

        void cekaj(unique_lock<mutex>& l) 
		{
            while(zauzmi)
                cv.wait(l);
        }

        void obradjen() 
		{
            zauzmi = 0;
            cv.notify_one();
        }
};

class Program 
{
	private:
		Dijagnostika& dijagnostika;
		int slobodno_ram; 
		int zauzeto_virtuelna;
		mutex m;
		condition_variable virtuelna;
		bool zavrsen;
		queue<Zahtev*> zahtevi;

		// Pomoćna metoda koja izvršava alokaciju memorije
		// kolicina_memorije - koliko memorije je potrebno zauzeti
		void izvrsi_malloc(int kolicina_memorije) 
		{
			unique_lock<mutex> l(m);
			
			// Ukoliko ima manje RAM-a nego što je zahtevano notifikuje se CV. Aktivira se zauzimanje VM.
			if (slobodno_ram < kolicina_memorije) 
			{
				virtuelna.notify_one();
				
				// Pravi se novi zahtev koji se dodaje u red zahteva
				Zahtev* z = new Zahtev(kolicina_memorije);
				zahtevi.push(z);
				z->cekaj(l);
			} 
			// Ako ima dovoljno RAM-a, on se zauzima
			else
			{
				slobodno_ram -= kolicina_memorije;   
			}

			// sleep (ako ima višestruki malloc). Simulira se da zauzimanje memorije traje neko vreme
			this_thread::sleep_for(seconds(1));
		}

	public:
		// Proširiti po potrebi ...
		Program(Dijagnostika& d, int kapacitet) : dijagnostika(d) 
		{
			slobodno_ram = kapacitet;
	        zauzeto_virtuelna = 0;
    	    zavrsen = false;
		}

		Dijagnostika& getDijagnostika() 
		{
			return dijagnostika;
		}

		/*
		Metoda koju poziva nit koja simulira izvršenje programa, kako bi zauzela potrebnu memoriju
		
		naredba - naredba koja se izvršava (naredba kojom se zauzima memorija)
		Vraća instancu klase Povratna_vrednost koja sadrži opis zauzeća memorije NAKON izvršenja naredbe.
		*/

		// Implementirati ...
		Povratna_vrednost izvrsi_naredbu(Naredba naredba) 
		{
			// Ukoliko je tip naredbe repeat, operacija malloc se ponavlja zadat broj puta
			if (naredba.tip == "repeat") 
			{
				for (int i = 0; i < naredba.ponavljanja; ++i)
					izvrsi_malloc(naredba.kolicina_memorije);
			} 
			// U suprotnom, jednom se izvrši malloc.
			else 
			{
				izvrsi_malloc(naredba.kolicina_memorije);           
			}
			
			// Nakon zauzimanja memorije vraća se koliko ima slobodnog RAM-a i koliko je zauzeto virtuelne memorije.
			return {slobodno_ram, zauzeto_virtuelna};
		}

		/*
		Metoda koju poziva nit koja simulira deo operativnog sistema koji se bavi zauzimanjem virtuelne memorije kako bi se zauzela određena količina VM
		Potrebnu količinu VM (kao i bilo koju drugu neophodnu informaciju) preneti u nekom izabranom polju klase.
		*/
		
		// Implementirati ...
		int zauzmi_virtuelnu_memoriju() 
		{
			unique_lock<mutex> l(m);
			
			// Dokle god nema aktiviranja OS i niti koje zauzimaju memoriju nisu završene, čeka se komanda
			while (zahtevi.empty() && !zavrsen)    
				virtuelna.wait(l);
			
			// Procesor se prepušta nekoj od spremnih niti
			// Ovo se radi da uslovna promenljiva ne bi ostala u čekanju
			// Zato što bi to blokiralo uništavanje objekta i onemogućilo program da se završi
			while (zavrsen) 
				this_thread::yield();
			
			// Zauzimanje virtuelne memorije traje 1 sekund.
			this_thread::sleep_for(seconds(1));         

			// Preuzmi zahtev sa vrha reda zahteva, povećaj količinu zauzete virtuelne memorije
			Zahtev* z = zahtevi.front();
			zauzeto_virtuelna += z->zauzmi;

			// Označi zahtev kao obrađen, ukloni obrađeni zahtev iz reda zahteva
			z->obradjen();
			zahtevi.pop();

			return zauzeto_virtuelna;
		}

		// Naknadno dodato
		void zavrsi() 
		{
        	zavrsen = true;
        	virtuelna.notify_one();
    	}
};

#endif // PROGRAM_H_INCLUDED
