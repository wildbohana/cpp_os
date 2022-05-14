#ifndef RADNA_MEMORIJA_H_INCLUDED
#define RADNA_MEMORIJA_H_INCLUDED

#include "dijagnostika.h"

using namespace std;

class Radna_memorija 
{
	private:
		Dijagnostika& dijagnostika;

	public:
		// dijagnostika  - referenca na instancu klase Dijagnostika
		// ukupno_okvira - ukupan broj okvira u memoriji
		
		// Proširiti po potrebi ...
		Radna_memorija(Dijagnostika& d, int ukupno_okvira) : dijagnostika(d) 
		{
			
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
			
		}

		/*
		Metoda koju poziva nit koja simulira izvršenje procesa kako bi oslobodila radnu memoriju koju je koristila tokom izvršenja
		
		id_procesa - identifikator procesa koji oslobađa memoriju
		*/

		// Implementirati ...
		void oslobodi(int id_procesa) 
		{
			
		}
};

#endif // RADNA_MEMORIJA_H_INCLUDED
