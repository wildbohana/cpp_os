#ifndef PARKING_H_INCLUDED
#define PARKING_H_INCLUDED

#include "automobil.h"
#include <mutex>
#include <condition_variable>

using namespace std;

class Parking 
{
	private:
		Automobil& automobil;
		int slobodno;
		mutex m;
		condition_variable cv;
	public:
		// Proširiti po potrebi ... 
		Parking(Automobil& a, int kapacitet) : automobil(a) 
		{
			slobodno = kapacitet;
		}
		
		/*
		Metoda koju poziva nit koja simulira kretanje automobila kako bi automobil pokušao da se parkira.
		Ako je parking zauzet, izvršenje ove metode će trajati dok se parking ne oslobodi i auto se parkira.
		
		rbr - Redni broj automobila
		
		Potrebno je pozvati metodu automobil.ceka kada je parking zauzet i auto ne može da se parkira.
		Potrebno je pozvati metodu automobil.parkira kada auto uspe da se parkira.
		*/
		
		// Implementirati ...
		void udji(int rbr) 
		{
			unique_lock<mutex> l(m);
        	
			// dok je parking pun, cekaj da se oslobodi
			while (slobodno == 0)
	            cv.wait(l);

			// kad automobil zauzme mesto, smanji broj slobodnih za jedno
        	slobodno--; 
	        automobil.parkira(rbr);
		}

		/* 
		Metoda koju poziva nit koja simulira kretanje automobila kada auto izlazi sa parkinga (nakon što je bio parkiran).
		
		rbr - Redni broj automobila
		
		Potrebno je pozvati metodu automobil.napusta kada auto napušta parking mesto.
		*/

		// Implementirati ...
		void izadji(int rbr) 
		{
			unique_lock<mutex> l(m);

	        automobil.napusta(rbr);

	        // jedno mesto se oslobodi - moze samo jedan automobil da udje
			cv.notify_one(); 
	        slobodno++; 
		}
};

#endif // PARKING_H_INCLUDED
