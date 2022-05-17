#ifndef RADNA_MEMORIJA_H_INCLUDED
#define RADNA_MEMORIJA_H_INCLUDED

#include "dijagnostika.h"

using namespace std;

class Radna_memorija 
{
	private:
		Dijagnostika& dijagnostika;
		mutex m;
		condition_variable cv;
		int ukupno_okvira;
		int slobodno_okvira;
		vector<int> mem;

	public:
		// dijagnostika  - referenca na instancu klase Dijagnostika
		// ukupno_okvira - ukupan broj okvira u memoriji
		
		// Proširiti po potrebi ...
		Radna_memorija(Dijagnostika& d, int ukupno_okvira) : dijagnostika(d), ukupno_okvira(ukupno_okvira), slobodno_okvira(ukupno_okvira) 
		{
			unique_lock<mutex> l(m);
	        mem.resize(ukupno_okvira, -1);
		}

		Dijagnostika& getDijagnostika() 
		{
			return dijagnostika;
		}

		/*
		Metoda koju poziva nit koja simulira izvršenje procesa kako bi pokušala da učita proces u radnu memoriju
		
		broj_stranica - ukupan broj stranica koje proces sadrži
		id_procesa    - identifikator procesa
		
		Ukoliko trenutno nema na raspolaganju dovoljno memorije (broj stranica u procesu je veći 
		od broja raspoloživih okvira memorije), potrebno je pozvati metodu dijagnostika.proces_ceka, 
		a nakon toga proces treba da čeka dok se memorija ne oslobodi.
		Kada proces uspe da dođe do potrebne radne memorije, treba da se pozove dijagnostika.proces_izvrsava. 
		Nakon toga, kada proces zauzme okvire radne memorije, potrebno je pozvati dijagnostika.ispisi_okvire 
		kako bi se prikazalo trenutno zauzece svih okvira (podrazumeva se da zelimo da prikazemo sliku svih okvira, 
		tako da ce se videti i okviri koje su zauzeli drugi procesi).
		*/

		// Implementirati ...
		void ucitaj(int broj_stranica, int id_procesa) 
		{
			unique_lock<mutex> l(m);

			while (slobodno_okvira < broj_stranica)
			{
				dijagnostika.proces_ceka(id_procesa);
				cv.wait(l);
			}

			dijagnostika.proces_se_izvrsava(id_procesa);
			int zauzeo = 0;
			int i = 0;

			vector<int> zauzeti;

			while (zauzeo < broj_stranica && i < ukupno_okvira) 
			{
				if (mem[i] == -1) 
				{
					mem[i] = id_procesa;
					zauzeti.push_back(i);
					zauzeo++;
				}
				i++;
			}

			slobodno_okvira -= zauzeo;
			dijagnostika.ispisi_okvire(okviri.begin(), okviri.end());
		}

		/*
		Metoda koju poziva nit koja simulira izvršenje procesa kako bi oslobodila radnu memoriju koju je koristila tokom izvršenja
		
		id_procesa - identifikator procesa koji oslobađa memoriju
		*/

		// Implementirati ...
		void oslobodi(int id_procesa) 
		{
			// is it really necessary?
			unique_lock<mutex> l(m);

			int oslobodio = 0;

			for (auto it = mem.begin(); it != mem.end(); it++) 
				if (*it == id_procesa) 
				{
					*it = -1;
					oslobodio++;
				}

			slobodno_okvira += oslobodio;
			cv.notify_all();
		}
};

#endif // RADNA_MEMORIJA_H_INCLUDED
