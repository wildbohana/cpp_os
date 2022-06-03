#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#include "dijagnostika.h"

#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace chrono;

// Struktura UI zahteva
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
		// Red POKAZIVAČA na UI zahteve. Neophodno je ovako jer CV unutar zahteva ne moze da se kopira.
		queue<UI_zahtev*> zahtevi;   
		mutex m;
		condition_variable ui;
		bool kraj;

	public:
		// Proširiti po potrebi ...
		OS(Dijagnostika& d) : dijagnostika(d) 
		{
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
		
		Metoda treba da formira novi zahtev i uputi ga U/I uređaju na obradu. 
		Proces će biti blokiran dok god se ovaj zahtev ne izvrši. 
		Pre nego što stupi u blokadu, potrebno je pozvati dijagnostika.proces_ceka.
		*/

		// Implementirati ...
		void uputi_UI_zahtev(int id_procesa, int broj_staze) 
		{
			unique_lock<mutex> l(m);

			// Zahtev se generiše dinamički i u okviru njega se prosleđuje broj staze za obradu
			// Zahtev se stavlja na kraj reda (FIFO) i šalje se signal OS niti da obradi zahtev
			UI_zahtev* z = new UI_zahtev(broj_staze);   
			zahtevi.push(z);
			ui.notify_one();
			
			// Dokle god zahtev nije obrađen, čeka se na ovom mestu
			while (!z->obradjen) 
			{
				dijagnostika.proces_ceka(id_procesa, broj_staze);
				z->cv.wait(l);
			}

			delete z;
		}

		/*
		Metoda koju poziva nit koja simulira deo operativnog sistema koji upravlja diskom, kako bi se obradio jedan pristigli zahtev
		
		Prema uslovima zadatka, metoda treba da obradi zahtev koji je najranije pristigao.
		
		Povratna vrednost metode treba da bude broj staze koji je obrađen.
		*/

		// Implementirati ...
		int obradi_zahtev() 
		{
			unique_lock<mutex> l(m);

			// Nema obrade zahteva dok ne postoji ni jedan zahtev u redu zahteva
			while (zahtevi.empty() && !kraj) ui.wait(l);

			while (kraj) this_thread::sleep_for(seconds(1));
			
			// Kada ima neki zahtev, uzima se onaj sa vrha (FIFO) reda
			// Briše se pokazivač na zahtev sa vrha - moguće jer je kopiran u z
			UI_zahtev* z = zahtevi.front();
			zahtevi.pop();
		
			// Obrada zahteva
			l.unlock();
			this_thread::sleep_for(milliseconds(300));
			l.lock();
			
			// Postavljanje flag-a da je zahtev obrađen, slanje signala niti koja čeka na obradu zahteva
			z->obradjen = true;
			z->cv.notify_one();

			// Preuzimanje broja staze i skidanje zahteva sa vrha reda.
			return z->broj_staze;
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
