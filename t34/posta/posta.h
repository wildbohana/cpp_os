#ifndef POSTA_H_INCLUDED
#define POSTA_H_INCLUDED

#include "klijent.h"

#include <mutex>
#include <condition_variable>

using namespace std;
using namespace chrono;

class Posta 
{
	private:
		Klijent& klijent;
		mutex m;
		condition_variable cv;
		int uplaceno[2];
		bool slobodan[2];

	public:
		// Prosiriti po potrebi ...
		Posta(Klijent& kl) : klijent(kl) 
		{
			slobodan[0] = true;
			slobodan[1] = true;
			uplaceno[0] = 0;
			uplaceno[1] = 0;
		}

		// Metoda koju poziva nit koja simulira klijenta kada on uplacuje neki iznos.
		// Metoda je blokirajuca - ako je salter zauzet, metoda ce stajati blokirana dok se on ne oslobodi.
		//
		// rbr   - Redni broj klijenta
		// svota - Svota koju klijent zeli da uplati
		//
		// Potrebno je pozvati metodu klijent.ceka kada su salteri zauzeti i klijent mora da ceka.
		// Potrebno je pozvati metodu klijent.uplacuje kada klijent stupi na salter i vrsi svoje placanje.
		// Potrebno je pozvati metodu klijent.napusta kada klijent zavrsi placanje i napusta salter.
		
		// Implementirati ...
		void uplati(int rbr, int svota) 
		{
			unique_lock<mutex> l(m);

			while (!slobodan[0] && !slobodan[1])
			{
				klijent.ceka(rbr, svota);
				cv.wait(l);
			}

			int salter = (slobodan[0] ? 0 : 1);
			slobodan[salter] = false;

			klijent.uplacuje(rbr, salter, svota);

			l.unlock();
			this_thread::sleep_for(chrono::seconds(svota));
			l.lock();

			uplaceno[salter] += svota;

			slobodan[salter] = true;
			klijent.napusta(rbr, salter, uplaceno[salter]);

			cv.notify_one();
		}
};

#endif // POSTA_H_INCLUDED
