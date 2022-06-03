#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#include "dijagnostika.h"

#include <deque>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace chrono;

// Struktura UI zahteva. Istovetna kao kod zadatka 02-procesi u vezbama 09 procesi.
struct UI_zahtev 
{
	condition_variable cv;
	int broj_staze;
	bool obradjen;

	UI_zahtev(int b): broj_staze(b), obradjen(false) {}
};

class OS 
{
	private:
		Dijagnostika& dijagnostika;
		
		// Red pokazivača na UI zahteve. Neophodno je ovako jer CV unutar zahteva ne moze da se kopira.
		deque<UI_zahtev*> zahtevi;   
		mutex m;
		condition_variable ui;
		int trenutna_poz;
		bool kraj;

	public:
		// Proširiti po potrebi ...
		OS(Dijagnostika& d, int t) : dijagnostika(d) 
		{
			trenutna_poz = t;
        	kraj = false;
		}

		Dijagnostika& getDijagnostika() 
		{
			return dijagnostika;
		}

		/*
		Metoda koju poziva nit koja simulira izvršenje procesa, kako bi se uputio zahtev ka U/I uređaju
		
		id_procesa - ID procesa koji upućuje zahtev
		broj_staze - broj staze diska kojoj nit želi da pristupi
		
		Metoda treba da formira novi zahtev i uputi ga U/I uređaju na obradu. Proces će biti blokiran dok god 
		se ovaj zahtev ne izvrši. Pre nego što stupi u blokadu, potrebno je pozvati dijagnostika.proces_ceka.
		*/

		// Implementirati ...
		void uputi_UI_zahtev(int id_procesa, int broj_staze) 
		{
			unique_lock<mutex> l(m);

			UI_zahtev* z = new UI_zahtev(broj_staze);
			zahtevi.push_back(z);
			ui.notify_one();

			while (!z->obradjen) 
			{
				dijagnostika.proces_ceka(id_procesa, broj_staze);
				z->cv.wait(l);
			}

			delete z;
		}

		/*
		Metoda koju poziva nit koja simulira deo operativnog sistema koji upravlja diskom, kako bi se obradio jedan pristigli zahtev
		
		Prema uslovima zadatka, metoda treba da obradi zahtev kojim se pristupa onoj stazi koja je najbliža stazi na kojoj je trenutno glava diska
		
		Povratna vrednost metode treba da bude broj staze koji je obrađen.
		*/
		
		// Implementirati ...
		int obradi_zahtev() 
		{
			unique_lock<mutex> l(m);
			
			// Dogod nema zahteva ceka se da se neki stvori
			while (zahtevi.empty() && !kraj) ui.wait(l);

			if (kraj) return -1;

			// Nadji zahtev najblizi trenutnoj poziciji
			deque<UI_zahtev*>::iterator najblizi = zahtevi.begin();
			for (auto it = zahtevi.begin(); it != zahtevi.end(); it++) 
				if (abs(trenutna_poz - ((*it)->broj_staze)) < abs(trenutna_poz - (*najblizi)->broj_staze)) 
					najblizi = it;
			
			// Obrada zahteva
			l.unlock();
			this_thread::sleep_for(milliseconds(300));		
			l.lock();
			
			// Disk se pomerio na novu poziciju i obradio zahtev
			trenutna_poz = (*najblizi)->broj_staze; 
			
			// Javi najblizam zahtevu da je obradjen i izbrisi ga iz deque-a
			(*najblizi)->obradjen = true;			
			(*najblizi)->cv.notify_one();
			int ret_val = (*najblizi)->broj_staze;
			zahtevi.erase(najblizi);

			return ret_val;
		}

		/*
		Metoda koja postavlja signal za kraj izvršavanja i pokreće obrađivač iz čekanja kako bi program mogao da se završi
		*/
		
		// Implementirati ...
		void zavrsi() 
		{
			kraj = true;
	        ui.notify_one();
		}
};

#endif // OS_H_INCLUDED
