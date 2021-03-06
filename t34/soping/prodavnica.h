#ifndef PRODAVNICA_H_INCLUDED
#define PRODAVNICA_H_INCLUDED

#include "povratna.h"
#include "kupac.h"

#include <mutex>
#include <condition_variable>

using namespace std;

class Prodavnica 
{
	private:
		Kupac& kupac;
		mutex m;
		condition_variable cv;
		int slobodnih_kabina;		
	public:
		// Prosiriti po potrebi ...
		Prodavnica(Kupac& k, int n): kupac(k) 
		{
			slobodnih_kabina = n;
		}

		/*
		Metoda koju poziva nit koja simulira kupca, kada kupac obavlja soping (procedura je opisana u zaglavlju main.cpp)
		Metoda je blokirajuca - ako kupac mora da ceka da se oslobodi kabina za probu, izvrsenje ce blokirati dok se za to ne steknu uslovi.
		
		rbr - Redni broj kupca
		
		Potrebno je pozvati metodu kupac.ceka kada su kabine zauzete i kupac mora da ceka.
		Potrebno je pozvati metodu kupac.proba_odecu kada kupac udje u kabinu da proba odecu.
		Potrebno je pozvati metodu kupac.zavrsio kada je kupac zavrsio probu odece.
		*/
		
		// Implementirati ...
		povratna_vrednost kupi(int rbr) 
		{
			unique_lock<mutex> l(m);

			srand(time(NULL));
			povratna_vrednost pv;

			system_clock::time_point dosao = system_clock::now();

			while (slobodnih_kabina == 0)
			{
				kupac.ceka(rbr);
				cv.wait(l);
			}

			pv.cekao_na_kabinu = system_clock::now() - dosao;

			slobodnih_kabina--;
			kupac.proba_odecu(rbr);

			l.unlock();
			this_thread::sleep_for(seconds(1));
			l.lock();

			slobodnih_kabina++;

			pv.kupio = rand()%2;

			kupac.zavrsio(rbr, pv);
			cv.notify_one();

			return pv;
		}
};

#endif // PRODAVNICA_H_INCLUDED
