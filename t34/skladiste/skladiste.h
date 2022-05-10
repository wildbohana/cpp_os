#ifndef SKLADISTE_H_INCLUDED
#define SKLADISTE_H_INCLUDED

#include "kamion.h"
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

class Skladiste 
{
	private:
		Kamion& kamion;
		mutex m;
		condition_variable cv_boom;
		condition_variable cv_normal;
		int zapaljivih_ceka;
		bool zapaljiv;
		bool zauzeta_rampa[2];
	public:
		// Prosiriti po potrebi ...
		Skladiste(Kamion& k) : kamion(k) 
		{
			zapaljivih_ceka = 0;
			zauzeta_rampa[0] = false;
			zauzeta_rampa[1] = false;
		}

		// Metoda koju poziva nit koja simulira kretanje kamiona kada on pokusava da istovari robu.
		// Metoda je blokirajuca - ako su sve rampe za istovar zauzete, izvrsenje ce blokirati dok se neka ne oslobodi.
		//
		// rbr       - Redni broj kamiona
		// kolicina  - Kolicina robe koja se prevozi (u tonama)
		// zapaljivo - Ukazuje na to da li je roba koja se prevozi zapaljiva (takva roba ima prioritet pri istovaru!)
		//
		// Potrebno je pozvati metodu kamion.ceka kada su rampe zauzete i kamion mora da ceka.
		// Potrebno je pozvati metodu kamion.istovara kada zapocne istovar robe iz kamiona (primetiti da ta metoda zahteva da joj se prosledi indeks rampe na kojoj se obavlja istovar!)
		// Potrebno je pozvati metodu kamion.odlazi kada je kamion zavrsio istovar i odlazi.

		void istovari(int rbr, int kolicina, bool zapaljivo) 
		{
			unique_lock<mutex> l(m);

			while (zauzeta_rampa[0] && zauzeta_rampa[1])
			{
				if (zapaljivo)
				{
					zapaljivih_ceka++;
					kamion.ceka(rbr, kolicina, zapaljivo);
					cv_boom.wait(l);
					zapaljivih_ceka--;
				}
				else
				{
					kamion.ceka(rbr, kolicina, zapaljivo);
					cv_normal.wait(l);
				}
			}

			int rampa = zauzeta_rampa[0] ? 1 : 0;
			zauzeta_rampa[rampa] = true;

			kamion.istovara(rbr, kolicina, zapaljivo, rampa);

			l.unlock();
			this_thread::sleep_for(chrono::seconds(kolicina));
			l.lock();

			zauzeta_rampa[rampa] = false;

			kamion.odlazi(rbr);

			if (zapaljivih_ceka)
				cv_boom.notify_one();
			else
				cv_normal.notify_one();
		}
};

#endif // SKLADISTE_H_INCLUDED
