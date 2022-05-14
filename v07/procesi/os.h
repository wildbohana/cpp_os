#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#include "dijagnostika.h"
#include "proces.h"

#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

struct UI_zahtev 
{
	condition_variable cv;
	int id_procesa;
	bool obradjen;
	UI_zahtev(int i): id_procesa(i), obradjen(false) {}
};

class OS 
{
	private:
		Dijagnostika& dijagnostika;
		mutex m;
		condition_variable spremni;
		condition_variable ui;
		int maks_uzastopnih_instrukcija;
		int aktivni_proces;
		bool gotovo;
		queue<UI_zahtev*> red_UI_zahteva;

	public:
		// d                 - Referenca na objekat Dijagnostika, koji služi za ispis stanja simulacije
		// maks_uzastopnih_i - Maksimalan dozvoljen broj uzastopnih STANDARD instrukcija, nakon kojeg obavezno sledi preključivanje
		
		// Proširiti po potrebi ...
		OS(Dijagnostika& d, int maks_uzastopnih_i) : dijagnostika(d) 
		{
			maks_uzastopnih_instrukcija = maks_uzastopnih_i;
			aktivni_proces = -1;
			gotovo = false;
		}

		// Pomoćna metoda koja vraća objekat klase Dijagnostika koji služi za ispis stanja simulacije
		Dijagnostika& getDijagnostika() 
		{
			return dijagnostika;
		}

		/*
		Metoda koju poziva nit koja simulira proces kako bi se izvršile sve instrukcije procesa.
		
		p - Struktura koja sadrži opis procesa (ID procesa i instrukcije koje treba izvršiti)
		
		Ukoliko proces ne može da se izvrši (procesor je zauzet), proces mora da se uveže u red spremnih i 
		treba pozvati metodu dijagnostika.proces_ceka.
		Ukoliko dođe do toga da izvršenje procesa prestaje (dostignut je maksimalan dozvoljen broj uzastopnih STANDARD 
		instrukcija ili su sve instrukcije procesa izvršene), potrebno je pozvati dijagnostika.proces_oslobadja_procesor.
		Kada se izvrši STANDARD instrukcija nekog procesa, potrebno je pozvati dijagnostika.proces_izvrsio_standardnu.
		Kada se izvršava UI instrukcija, potrebno je pozvati par metoda: dijagnostika.proces_ceka_na_UI kada se pokrene 
		U/I operacija (nakon čega sledi čekanje na U/I podsistem) i dijagnostika.proces_zavrsio_UI kada se završi U/I operacija.
		*/
		
		// Implementirati ...
		void izvrsi_proces(Proces& p) 
		{
			int uzastopnih_instrukcija = 0;

			for (vector<INS_TYPE>::iterator it = p.instrukcije.begin(); it != p.instrukcije.end(); it++) 
			{
				unique_lock<mutex> l(m);
				
				while (aktivni_proces != p.id && aktivni_proces != -1) 
				{
					dijagnostika.proces_ceka(p.id);
					spremni.wait(l);
				}
				aktivni_proces = p.id;

				if (*it == STANDARD) 
				{				
					l.unlock();
					this_thread::sleep_for(milliseconds(rand()%40 + 10));
					l.lock();

					dijagnostika.proces_izvrsio_standardnu(p.id);
					
					if (++uzastopnih_instrukcija == maks_uzastopnih_instrukcija || it == p.instrukcije.end()-1) 
					{
						dijagnostika.proces_oslobadja_procesor(p.id);
						uzastopnih_instrukcija = 0;
						aktivni_proces = -1; 
						spremni.notify_one();

						// cooldown
						l.unlock();
						this_thread::sleep_for(milliseconds(100));
					}
				} 
				else 
				{
					UI_zahtev* z = new UI_zahtev(p.id);
					red_UI_zahteva.push(z);
					ui.notify_one();

					dijagnostika.proces_ceka_na_UI(p.id);

					uzastopnih_instrukcija = 0;
					aktivni_proces = -1;
					spremni.notify_one();

					while (!z->obradjen) 
						z->cv.wait(l);
					
					dijagnostika.proces_zavrsio_UI(p.id);
					delete z;
				}
			}
		}

		/*
		Metoda koju poziva nit koja simulira obrađivač U/I zahteva kako bi se obradili trenutno pohranjeni U/I zahtevi.
		
		Potrebno je pozvati dijagnostika.ui_ceka kada ne postoje trenutno pohranjeni U/I zahtevi i obrađivač čeka na prvi.
		Potrebno je pozvati dijagnostika.ui_zapocinje kada obrađivač krene u obradu U/I zahteva. Kada obrađivač završi 
		taj zahtev, potrebno je pozvati dijagnostika.ui_zavrsio.
		*/

		// Implementirati ...
		void obradi_zahteve() 
		{
			while (true) 
			{
				unique_lock<mutex> l(m);
				
				while (red_UI_zahteva.empty() && !gotovo) 
				{
					dijagnostika.ui_ceka();
					ui.wait(l);
				}

				if (gotovo) return;

				UI_zahtev* z = red_UI_zahteva.front();
				red_UI_zahteva.pop();

				dijagnostika.ui_zapocinje(z->id_procesa);

				l.unlock();
				this_thread::sleep_for(milliseconds(rand()%200 + 100));
				l.lock();

				z->obradjen = true;
				z->cv.notify_one();

				dijagnostika.ui_zavrsio(z->id_procesa);
			}
		}

		// Dodato:
		void zavrsi() 
		{
			gotovo = true;
			ui.notify_one();
		}
};

#endif // OS_H_INCLUDED
