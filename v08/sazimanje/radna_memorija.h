#ifndef RADNA_MEMORIJA_H_INCLUDED
#define RADNA_MEMORIJA_H_INCLUDED

#include "dijagnostika.h"

#include <mutex>
#include <condition_variable>
#include <vector>

using namespace std;
using namespace chrono;

class Radna_memorija 
{
	private:
		Dijagnostika& dijagnostika;
		// Indeksi su indeksi lokacija a elementi su identifikatori procesa koji ih koriste
		vector<int> lokacije; 
		mutex m;
		bool aktiviraj_sazimanje;
		bool kraj;
		condition_variable cv_sazimanje;
		condition_variable cv_slobodno;

		// br_lokacija_procesa - koliko lokacija je procesu potrebno da bi se izvršio
		// Metoda vraća indeks od kojeg počinju lokacije koje su dodeljene procesu ili -1 ako nema takvog bloka
		int pocetak_bloka(int br_lokacija_procesa) 
		{
			int broj_uzastopnih = 0;
			int i = 0;

			// Tražimo dovoljan broj uzastopnih slobodnih okvira:
			for (auto it_lok = lokacije.begin(); it_lok != lokacije.end() ; it_lok++, i++) 
			{
				// Ako je okvir slobodan broji se koliko ih ima uzastopnih slobodnih lokacija.
				if (*it_lok == -1) 
				{    
					broj_uzastopnih++;
				}
				// Kad god se naiđe na zauzet okvir, resetuje se broj uzastopnih slobodnih okvira.
				else 
				{ 
					broj_uzastopnih = 0;
				}

				// Ako je pronađen dovoljno veliki blok, vraća se početna lokacija bloka
				if (broj_uzastopnih == br_lokacija_procesa)
					return i - br_lokacija_procesa + 1;
			}

			// Ako nema dovoljno uzastopnih slobodnih okvira, vraća se -1
			return -1;
		}

	public:
		// Proširiti po potrebi ...
		Radna_memorija(Dijagnostika& d, int ukupno_lokacija) : dijagnostika(d) 
		{
			aktiviraj_sazimanje = false;
			lokacije.resize(ukupno_lokacija, -1);
			kraj = false;
		}

		Dijagnostika& getDijagnostika() 
		{
			return dijagnostika;
		}

		/*
		Metoda koju poziva nit koja simulira izvršenje procesa kako bi proces zauzeo potrebnu 
		memoriju i krenuo sa radom
		
		id_procesa          - identifikator procesa
		br_lokacija_procesa - ukupan broj lokacija memorije koje su neophodne za izvršenje procesa
		trajanje            - koliko dugo se proces izvršava (u sekundama)
		
		Ukoliko trenutno nema na raspolaganju dovoljno slobodne memorije (moraju se zauzeti 
		uzastopni okviri u memoriji!), potrebno je pozvati metodu dijagnostika.proces_ceka, a nakon 
		toga proces treba da čeka dok se memorija ne oslobodi. Kada proces uspe da dođe do potrebne 
		radne memorije, treba da se pozove dijagnostika.proces_zauzeo_okvire.
		Kada se proces završi, potrebno je pozvati metodu dijagnostika.proces_zavrsio.
		Metodu dijagnostika.ispisi_memoriju potrebno je pozvati u sledećim momentima: 
		nakon što proces zauzme memoriju i nakon što proces oslobodi memoriju.
		*/

		// Implementirati ...
		void koristi(int id_procesa, int br_lokacija_procesa, int trajanje) 
		{
			unique_lock<mutex> l(m);
			int pocetak;
			
			// Ako nema dovoljno slobodnih uzastopnih okvira
			while ((pocetak = pocetak_bloka(br_lokacija_procesa)) == -1) 
			{
				dijagnostika.proces_ceka(id_procesa);
				cv_slobodno.wait(l);
			}

			// Obeležavanje okvira - okviri koji počinju od indeksa "pocetak" će biti označeni ID-jem ovog procesa:
			for (int i = 0; i < br_lokacija_procesa; i++)
				lokacije[pocetak+i] = id_procesa;

			dijagnostika.proces_zauzeo_okvire(id_procesa, pocetak, pocetak + br_lokacija_procesa);
			dijagnostika.ispisi_memoriju(lokacije.begin(), lokacije.end());

			l.unlock();
			this_thread::sleep_for(seconds(trajanje));
			l.lock();

			// Ponovo se pronalazi gde je početak bloka ovog procesa jer je u međuvremenu možda došlo do sažimanja:
			for (pocetak = 0; pocetak < lokacije.size(); pocetak++) 
			{
				if (lokacije[pocetak] == id_procesa) break;
			}

			// Okviri procesa se proglašavaju slobodnim:
			for (int i = 0; i < br_lokacija_procesa; i++)
				lokacije[pocetak+i] = -1;

			dijagnostika.proces_zavrsio(id_procesa);
			dijagnostika.ispisi_memoriju(lokacije.begin(), lokacije.end());
			
			// Aktivira se sažimanje
			aktiviraj_sazimanje = true;
			cv_sazimanje.notify_one();
		}

		/*
		Metoda koju poziva nit operativnog sistema koji se bavi sažimanjem memorije
		
		Nakon što je sažimanje obavljeno potrebno je pozvati metodu dijagnostika.sazimanje_obavljeno, 
		a nakon toga i metodu dijagnostika.ispisi_memoriju kako bi se prikazali efekti sažimanja memorije.
		*/
		// Implementirati ...
		void sazimanje() 
		{
			unique_lock<mutex> l(m);

			while (!aktiviraj_sazimanje && !kraj) 
				cv_sazimanje.wait(l);
			
			// Ako je kraj izvršavanja, nit se šalje u sleep
			// Ovo se radi da uslovna promenljiva ne bi ostala u čekanju
			// Zato što bi to blokiralo uništavanje objekta i onemogućilo program da se završi
			while (kraj) 
				this_thread::sleep_for(seconds(1)); 
													

			aktiviraj_sazimanje = false;

			// Stvori novi vektor iste dužine kao stari
			vector<int> kopija;    
			kopija.resize(lokacije.size(), -1);

			
			// Kopiraj korišćene lokacije iz starog u novi vektor, na početak
			int a = 0;

			for (int i = 0; i < lokacije.size(); i++) 
			{
				if (lokacije[i] != -1)
					kopija[a++] = lokacije[i];
			}
			
			// Stari vektor dobija vrednost novog
			lokacije = kopija;  

			dijagnostika.sazimanje_obavljeno();
			dijagnostika.ispisi_memoriju(lokacije.begin(), lokacije.end());
			
			// Javi svima jer je moguce da ima dovoljno memorije za vise procesa sada
			cv_slobodno.notify_all();  
		}

		// Naknadno dodato
		void zavrsi() 
		{
			kraj = true;
			cv_sazimanje.notify_one();
		}
};

#endif // RADNA_MEMORIJA_H_INCLUDED
