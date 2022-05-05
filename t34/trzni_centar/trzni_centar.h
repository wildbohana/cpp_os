#ifndef TRZNI_CENTAR_H_INCLUDED
#define TRZNI_CENTAR_H_INCLUDED

#include "kupac.h"

#include <mutex>
#include <condition_variable>

using namespace std;

using namespace chrono;

class Trzni_centar 
{
	private:
		Kupac& kupac;
		// Dodato:
		int cekaju[2];
		bool kasaSlobodna[2];
		condition_variable redovi[2];
		mutex m;
	public:
		// Prosiriti po potrebi ...
		Trzni_centar(Kupac& k) : kupac(k) 
		{
			kasaSlobodna[0] = true;
			kasaSlobodna[1] = true;
			cekaju[0] = 0;
			cekaju[1] = 0;
		}

		/*
		Metoda koju poziva nit koja simulira kupca kako bi se obavila kupovina.
		Metoda je blokirajuca - ako su sve kase zauzete, onda se izvrsenje blokira dok se neka ne oslobodi.
		
		rbr           - Redni broj kupca
		broj_artikala - Broj artikala koje kupac zeli da kupi
		
		Potrebno je pozvati metodu kupac.ceka kada je izabrana kasa zauzeta i kupac mora da ceka.
		Potrebno je pozvati metodu kupac.kupuje kada se kasa oslobodi i kupac pocinje kupovinu.
		Potrebno je pozvati metodu kupac.zavrsio kada je kupac zavrsio kupovinu.
		*/

		// Implementirati ...
		int kupi(int rbr, int broj_artikala) 
		{
			unique_lock<mutex> l(m);

			int mojaKasa;

			// idi na kasu gde je manji red
			if (cekaju[0] <= cekaju[1])
				mojaKasa = 0;
			else
				mojaKasa = 1;

			// dok kasa nije slobodna, cekaj u redu
			while (!kasaSlobodna[mojaKasa])
			{
				kupac.ceka(rbr, mojaKasa);
				cekaju[mojaKasa]++;
				redovi[mojaKasa].wait(l);
				cekaju[mojaKasa]--;
			}

			// zauzmi kasu kad dodjes na red
			kasaSlobodna[mojaKasa] = false;
			kupac.kupuje(rbr, mojaKasa, broj_artikala);

			// kupovina traje onoliko sekundi koliko ima artikala
			l.unlock();
			this_thread::sleep_for(seconds(broj_artikala));
			l.lock();

			// oslobodi kasu
			kasaSlobodna[mojaKasa] = true;
			kupac.zavrsio(rbr);
			redovi[mojaKasa].notify_one();

			// vrati ??
			return mojaKasa + 1;
		}
};


#endif // TRZNI_CENTAR_H_INCLUDED
