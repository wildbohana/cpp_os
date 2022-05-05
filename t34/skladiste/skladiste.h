#ifndef SKLADISTE_H_INCLUDED
#define SKLADISTE_H_INCLUDED

#include "kamion.h"
#include <mutex>
#include <condition_variable>
#include <chrono>

// UOPŠTE NISAM SIGURNA DA LI OVO VALJA ALI ŠTA JE TU JE

using namespace std;

class Skladiste 
{
	private:
		Kamion& kamion;
		mutex m;
		condition_variable normal;
		condition_variable boom;
		// Dodato:
		bool rampa_slobodna[2];
		int zapaljivih_ceka;

	public:
		// Prosiriti po potrebi ...
		Skladiste(Kamion& k) : kamion(k), zapaljivih_ceka(0)
		{
			rampa_slobodna[0] = true;
			rampa_slobodna[1] = true;
		}

		/*
		Metoda koju poziva nit koja simulira kretanje kamiona kada on pokusava da istovari robu.
		Metoda je blokirajuca - ako su sve rampe za istovar zauzete, izvrsenje ce blokirati dok se neka ne oslobodi.
		
		rbr       - Redni broj kamiona
		kolicina  - Kolicina robe koja se prevozi (u tonama)
		zapaljivo - Ukazuje na to da li je roba koja se prevozi zapaljiva (takva roba ima prioritet pri istovaru!)
		
		Potrebno je pozvati metodu kamion.ceka kada su rampe zauzete i kamion mora da ceka.
		Potrebno je pozvati metodu kamion.istovara kada zapocne istovar robe iz kamiona (primetiti da ta metoda zahteva da joj se prosledi indeks rampe na kojoj se obavlja istovar!)
		Potrebno je pozvati metodu kamion.odlazi kada je kamion zavrsio istovar i odlazi.
		*/

		void istovari(int rbr, int kolicina, bool zapaljivo) 
		{
			{
				unique_lock<mutex> l(m);

				while (rampa_slobodna[0] == false && rampa_slobodna[1] == false) 
				{ 
					kamion.ceka(rbr, kolicina, zapaljivo);

					if (zapaljivo) 
					{ 
						zapaljivih_ceka++;
						boom.wait(l);
						zapaljivih_ceka--; 
					}
					else
						normal.wait(l);
				}
			}

			int rampa = rampa_slobodna[0] ? 0 : 1;
      		rampa_slobodna[rampa] = false; 

			kamion.istovara(rbr, kolicina, zapaljivo, rampa);
			// istovar traje onoliko milisekundi koliko ima tona
			this_thread::sleep_for(chrono::milliseconds(kolicina * 1000));

			{
				unique_lock<mutex> l(m);

				// oslobodi rampu nakon istovara
				rampa_slobodna[rampa] = true; 
				
				if (zapaljivih_ceka) 
					boom.notify_one();
				else
					normal.notify_one();
			}
		}
};

#endif // SKLADISTE_H_INCLUDED
