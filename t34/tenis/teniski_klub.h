#ifndef TENISKI_KLUB_H_INCLUDED
#define TENISKI_KLUB_H_INCLUDED

#include "podaci.h"
#include "mec.h"

#define MAX_TERENA 30

#include <mutex>
#include <condition_variable>

using namespace std;

// Klasa deljenog resursa. Pravi se maksimalno 30 terena, ali ce u realnosti biti samo 3 terena.
class Teniski_klub 
{                             
	private:
		Mec& mec;
		mutex m;
		condition_variable cv[MAX_TERENA];
		bool slobodan[MAX_TERENA];
	public:
		// Prosiriti po potrebi ...
		Teniski_klub(Mec& m, int ukupno_terena) : mec(m) 
		{
			for (int i = 0; i < ukupno_terena; i++)
				slobodan[i] = true;
		}

		// Metoda koju poziva nit koja simulira mec kako bi izvrsila teniski mec.
		// Metoda je blokirajuca - ako je zeljeni teren zauzet, izvrsenje ce blokirati dok se on ne oslobodi.
		//
		// broj_meca - Redni broj meca
		// na_terenu - Indeks terena na kojem mec treba da se odigra
		//
		// Potrebno je pozvati metodu mec.ceka kada je izabrani teren zauzet i mec mora da ceka.
		// Potrebno je pozvati metodu mec.pocinje kada se isprazni izabrani teren i mec moze da pocne.
		// Potrebno je pozvati metodu mec.zavrsen kada se mec zavrsi i mogu se proslediti izmereni vremenski intervali.
		
		// Implementirati ...
		void odigraj_mec(int broj_meca, int na_terenu) 
		{
			unique_lock<mutex> l(m);

			Podaci p;
			p.dosao = system_clock::now();
			
			while (!slobodan[na_terenu])
			{
				mec.ceka(broj_meca, na_terenu);
				cv[na_terenu].wait(l);
			}
			
			slobodan[na_terenu] = false;
			p.pocetak = system_clock::now();

			mec.pocinje(broj_meca, na_terenu);

			l.unlock();
			this_thread::sleep_for(seconds(rand()%5 + 1));
			l.lock();

			p.trajanje = system_clock::now() - p.pocetak;
			mec.zavrsen(broj_meca, p);			

			slobodan[na_terenu] = true;

			cv[na_terenu].notify_one();
		}
};

#endif // TENISKI_KLUB_H_INCLUDED
