#ifndef RADNA_MEMORIJA_H_INCLUDED
#define RADNA_MEMORIJA_H_INCLUDED

#include "dijagnostika.h"

using namespace std;
using namespace chrono;

class Radna_memorija 
{
	private:
		Dijagnostika& dijagnostika;

	public:
		// Proširiti po potrebi ...
		Radna_memorija(Dijagnostika& d, int ukupno_lokacija) : dijagnostika(d) 
		{
			
		}

		Dijagnostika& getDijagnostika() 
		{
			return dijagnostika;
		}

		/*
		Metoda koju poziva nit koja simulira izvršenje procesa kako bi proces zauzeo potrebnu memoriju i krenuo sa radom
		
		id_procesa          - identifikator procesa
		br_lokacija_procesa - ukupan broj lokacija memorije koje su neophodne za izvršenje procesa
		trajanje            - koliko dugo se proces izvršava (u sekundama)
		
		Ukoliko trenutno nema na raspolaganju dovoljno slobodne memorije (moraju se zauzeti uzastopni okviri u memoriji!), 
		potrebno je pozvati metodu dijagnostika.proces_ceka a nakon toga proces treba da čeka dok se memorija ne oslobodi.
		Kada proces uspe da dođe do potrebne radne memorije, treba da se pozove dijagnostika.proces_zauzeo_okvire.
		Kada se proces završi, potrebno je pozvati metodu dijagnostika.proces_zavrsio.
		Metodu dijagnostika.ispisi_memoriju potrebno je pozvati u sledećim momentima: nakon što proces 
		zauzme memoriju i nakon što proces oslobodi memoriju.
		*/

		// Implementirati ...
		void koristi(int id_procesa, int br_lokacija_procesa, int trajanje) 
		{
			
		}

		/* 
		Metoda koju poziva nit operativnog sistema koji se bavi sažimanjem memorije
		
		Nakon što je sažimanje obavljeno potrebno je pozvati metodu dijagnostika.sazimanje_obavljeno, 
		a nakon toga i metodu dijagnostika.ispisi_memoriju kako bi se prikazali efekti sažimanja memorije.
		*/

		// Implementirati ...
		void sazimanje() 
		{
			
		}
};

#endif // RADNA_MEMORIJA_H_INCLUDED
