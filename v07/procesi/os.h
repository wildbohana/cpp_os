#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#include "dijagnostika.h"
#include "proces.h"

#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace chrono;

// Struktura UI zahteva. Neophodna radi efikasne obrade zahteva. Moguće su i drugačije implementacije.
// Npr. odvojeni redovi čekanja, integera i bool-ova ali je efikasnije spakovati sve u jednu 
// strukturu i onda date strukture stavljati u red (queue) (preko pokazivača, dinamički alocirane)
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
		int maks_uzastopnih_instrukcija;
		int aktivni_proces;
		queue<UI_zahtev*> red_UI_zahteva;
		condition_variable spremni; 
		condition_variable ui; 
		bool gotovo;

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
		
		Ukoliko proces ne može da se izvrši (procesor je zauzet), proces mora da se uveže u red spremnih i treba pozvati metodu dijagnostika.proces_ceka.
		Ukoliko dođe do toga da izvršenje procesa prestaje (dostignut je maksimalan dozvoljen broj uzastopnih STANDARD instrukcija ili su sve instrukcije procesa izvršene), potrebno je pozvati dijagnostika.proces_oslobadja_procesor.
		Kada se izvrši STANDARD instrukcija nekog procesa, potrebno je pozvati dijagnostika.proces_izvrsio_standardnu.
		Kada se izvršava UI instrukcija, potrebno je pozvati par metoda: dijagnostika.proces_ceka_na_UI kada se pokrene U/I operacija (nakon čega sledi čekanje na U/I podsistem) i dijagnostika.proces_zavrsio_UI kada se završi U/I operacija.
		*/

		// Implementirati ...
		void izvrsi_proces(Proces& p) 
		{
			// Broj uzastopno izvršenih instrukcija
			int uzastopnih_instrukcija = 0; 

			for (vector<INS_TYPE>::iterator it = p.instrukcije.begin(); it != p.instrukcije.end(); it++) 
			{
				unique_lock<mutex> l(m);

				// Proces mora da sačeka ako je procesor trenutno dodeljen nekom drugom procesu:
				while (aktivni_proces != p.id && aktivni_proces != -1) 
				{
					dijagnostika.proces_ceka(p.id);
					spremni.wait(l);
				}
				
				// Proces zauzima procesor
				aktivni_proces = p.id; 

				if (*it == STANDARD) 
				{
					// Simulacija trajanja izvršavanja standardne instrukcije:
					l.unlock();
					this_thread::sleep_for(milliseconds(rand()%40 + 10));
					l.lock();

					dijagnostika.proces_izvrsio_standardnu(p.id);

					// Ako je izvršen maksimalan broj uzastopnih instrukcija ili su izvršene sve instrukcije procesa:
					// Oslobodi procesor
					// Aktiviraj sledeći iz reda spremnih
					if (++uzastopnih_instrukcija == maks_uzastopnih_instrukcija || it == p.instrukcije.end() - 1) 
					{
						dijagnostika.proces_oslobadja_procesor(p.id);
						uzastopnih_instrukcija = 0;
						aktivni_proces = -1;
						spremni.notify_one();

						// Cooldown period, da ne bi ovaj isti proces odmah ponovo preuzeo procesor:
						l.unlock();
						this_thread::sleep_for(milliseconds(100));

						dijagnostika.proces_zavrsio(p.id);		
					}
				} 
				else 
				{
					// Formiramo UI zahtev i dodamo zahtev u red zahteva:
					UI_zahtev* z = new UI_zahtev(p.id);
					red_UI_zahteva.push(z);
					ui.notify_one();

					dijagnostika.proces_ceka_na_UI(p.id);	
					
					// Čeka se na obradu zahteva
					// Resetuje se brojač uzastopnih instrukcija, oslobađa se procesor
					// Javlja se sledećem spremnom da preuzme procesor
					while (!z->obradjen)
					{
						z->cv.wait(l);

						uzastopnih_instrukcija = 0;     
						aktivni_proces = -1;
						spremni.notify_one();
					}

					dijagnostika.proces_zavrsio_UI(p.id);

					// Tek kada se sve završi moguće je brisati memoriju zauzetu za zahtev
					delete z;
				}
			}
		}

		/*
		Metoda koju poziva nit koja simulira obrađivač U/I zahteva kako bi se obradili trenutno pohranjeni U/I zahtevi.
		
		Potrebno je pozvati dijagnostika.ui_ceka kada ne postoje trenutno pohranjeni U/I zahtevi i obrađivač čeka na prvi.
		Potrebno je pozvati dijagnostika.ui_zapocinje kada obrađivač krene u obradu U/I zahteva. Kada obrađivač završi taj zahtev, potrebno je pozvati dijagnostika.ui_zavrsio.
		*/

		// Implementirati ...
		void obradi_zahteve() 
		{
			while (true) 
			{
				unique_lock<mutex> l(m);
				
				// Obrada zahteva ne može da počne dok god nema ni jedan zahtev u redu
				while (red_UI_zahteva.empty() && !gotovo) 
				{
					dijagnostika.ui_ceka();
					ui.wait(l);
				}

				if (gotovo) return;
				
				// Kada ima zahteva uzima se prvi iz reda (front)
				// Odmah nakon toga se taj isti zahtev briše iz reda jer je uzet pre toga
				UI_zahtev* z = red_UI_zahteva.front();
				red_UI_zahteva.pop();

				dijagnostika.ui_zapocinje(z->id_procesa);

				l.unlock();
				this_thread::sleep_for(milliseconds(rand()%200 + 100));
				l.lock();
				
				// Postavljanje flag-a da je zahtev obrađen i javljanje niti koja čeka na obradu zahteva
				z->obradjen = true;
				z->cv.notify_one();

				dijagnostika.ui_zavrsio(z->id_procesa);
			}
		}

		// Signaliziranje kraja izvršavanja niti koja simulira obrađivač U/I zahteva
		// Naknadno dodato
		void zavrsi() 
		{
			gotovo = true;
			ui.notify_one();
		}
};

#endif // OS_H_INCLUDED
