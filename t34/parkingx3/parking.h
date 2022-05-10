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
		mutex m;
		int trenutni_ulaz;
		bool slobodan;
		condition_variable cv[3];

	public:
		// Prosiriti po potrebi ...
		Parking(Automobil& a) : automobil(a) 
		{
			trenutni_ulaz = 0;
			slobodan = true;
		}

		// Metoda koju poziva nit koja simulira kretanje automobila kada on pokusava da udje na parking.
		// Metoda je blokirajuca - ukoliko nema slobodnog mesta, izvrsenje je u blokadi dok se ono ne oslobodi.
		//
		// rbr  - Redni broj automobila
		// ulaz - Ulaz na koji automobil pokusava da udje
		//
		// Potrebno je pozvati metodu automobil.ceka kada automobil ne moze da pridje parkingu i mora da ceka.
		// Potrebno je pozvati metodu automobil.zauzima kada automobil zauzme parking mesto.
		
		// Implementirati ...
		void udji(int rbr, int ulaz) 
		{
			unique_lock<mutex> l(m);

			while (!slobodan || trenutni_ulaz != ulaz)
			{
				automobil.ceka(rbr, ulaz);
				cv[ulaz].wait(l);
			}

			slobodan = false;
			automobil.zauzima(rbr);	
			
			trenutni_ulaz = (trenutni_ulaz + 1) % 3;
		}

		// Metoda koju poziva nit koja simulira kretanje automobila kada on napusta parking na koji je prethodno usao.
		//
		// rbr  - Redni broj automobila
		//
		// Potrebno je pozvati metodu automobil.napusta kada automobil napusti parking mesto.
		
		// Implementirati ...
		void izadji(int rbr) 
		{
			unique_lock<mutex> l(m);

			slobodan = true;
			automobil.napusta(rbr);

			cv[trenutni_ulaz].notify_one();
		}
};

#endif // PARKING_H_INCLUDED
