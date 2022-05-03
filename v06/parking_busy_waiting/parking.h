#ifndef PARKING_H_INCLUDED
#define PARKING_H_INCLUDED

#include "automobil.h"
#include <mutex>

using namespace std;

class Parking 
{
	private:
		Automobil& automobil;
		bool slobodno;
		mutex m;
	public:
		// Proširiti po potrebi ...
		Parking(Automobil& a) : automobil(a), slobodno(true) {}

		/*
		Metoda koju poziva nit koja simulira kretanje automobila kako bi automobil pokušao da se parkira.
		Ako je parking zauzet, izvršenje ove metode će trajati dok se parking ne oslobodi i auto se parkira.
		
		rbr - Redni broj automobila
		
		Potrebno je pozvati metodu automobil.ceka kada je parking zauzet i auto mora da čeka.
		Potrebno je pozvati metodu automobil.parkira kada auto uspe da se parkira.
		*/

		// Implementirati ...
		void udji(int rbr) 
		{
			unique_lock<mutex> l(m);
            
			if(!slobodno)
            {
                automobil.ceka(rbr);

                while(!slobodno)
                {
                    l.unlock();
                    l.lock();
                }
            }

            automobil.parkira(rbr);
            slobodno = false;
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
            slobodno = true;
		}
};

#endif // PARKING_H_INCLUDED
