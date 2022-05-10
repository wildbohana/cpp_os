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
		mutex m;
		condition_variable cv[2];
		int cekaju[2];
		bool slobodna[2];		
	public:
		// Prosiriti po potrebi ...
		Trzni_centar(Kupac& k) : kupac(k) 
		{
			slobodna[0] = true;
			slobodna[1] = true;
			cekaju[0] = 0;
			cekaju[1] = 0;
		}

		// Metoda koju poziva nit koja simulira kupca kako bi se obavila kupovina.
		// Metoda je blokirajuca - ako su sve kase zauzete, onda se izvrsenje blokira dok se neka ne oslobodi.
		//
		// rbr           - Redni broj kupca
		// broj_artikala - Broj artikala koje kupac zeli da kupi
		//
		// Potrebno je pozvati metodu kupac.ceka kada je izabrana kasa zauzeta i kupac mora da ceka.
		// Potrebno je pozvati metodu kupac.kupuje kada se kasa oslobodi i kupac pocinje kupovinu.
		// Potrebno je pozvati metodu kupac.zavrsio kada je kupac zavrsio kupovinu.
		
		// Implementirati ...
		int kupi(int rbr, int broj_artikala) 
		{
			unique_lock<mutex> l(m);

			int mojaKasa;
			if (cekaju[0] < cekaju[1])
				mojaKasa = 0;
			else
				mojaKasa = 1;

			while (!slobodna[mojaKasa])
			{
				kupac.ceka(rbr, mojaKasa);
				cekaju[mojaKasa]++;
				cv[mojaKasa].wait(l);
			}

			cekaju[mojaKasa]--;
			slobodna[mojaKasa] = false;

			kupac.kupuje(rbr, mojaKasa, broj_artikala);

			l.unlock();
			this_thread::sleep_for(chrono::seconds(broj_artikala));
			l.lock();

			slobodna[mojaKasa] = true;
			
			kupac.zavrsio(rbr);
			cv[mojaKasa].notify_one();

			return mojaKasa;
		}
};


#endif // TRZNI_CENTAR_H_INCLUDED
