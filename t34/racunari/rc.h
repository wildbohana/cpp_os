#ifndef RC_H_INCLUDED
#define RC_H_INCLUDED

#include "student.h"

#include <mutex>
#include <condition_variable>

using namespace std;

// Maksimalan broj racunara u ucionici
#define MAX 50 

class RC 
{
	private:
		Student& student;
		// Dodato:
		bool slobodni[MAX];
		int brojSlobodnih;
		int brojRacunara;
		condition_variable cv;
		mutex m;
	public:
		// Prosiriti po potrebi ...
		RC(Student& st, int br) : student(st) 
		{
			brojRacunara = br;
			brojSlobodnih = brojRacunara;

			for (int i = 0; i < brojSlobodnih; ++i)
				slobodni[i] = true;
		}

		/*
		Metoda koju poziva nit koja simulira studenta kako bi student zauzeo mesto za racunarom.
		Ova metoda je blokirajuca - ako nema slobodnih mesta, ona ce cekati dok se neko ne oslobodi.
		
		rbr - Redni broj studenta
		
		Potrebno je pozvati metodu student.ceka kada su racunari zauzeti i student mora da ceka.
		Potrebno je pozvati metodu student.zauzeo kada student zauzme racunar.
		*/

		// Implementirati ...
		int zauzmi(int rbr) 
		{
			unique_lock<mutex> l(m);

			// ako nema slobodnih racunara, cekaj
			while (brojSlobodnih == 0) 
			{
				student.ceka(rbr);
				cv.wait(l);
			}

			// indeks zauzetog racunara
			int retVal;

			// ako ima slobodnih, trazi prvi koji je slobodan i zauzmi ga
			for (int i = 0; i < brojRacunara; i++)
				if (slobodni[i])
				{
					student.zauzeo(rbr, i);
					slobodni[i] = false;
					retVal = i + 1;
					break;
				}
			
			brojSlobodnih--;

			return retVal;
		}

		/* 
		Metoda koju poziva nit koja simulira studenta kako bi oslobodio racunar koji je prethodno zauzeo.
		
		rbr         - Redni broj studenta
		id_racunara - Redni broj racunara kojeg student oslobadja (prethodno je zauzeo taj racunar)
		
		Potrebno je pozvati metodu student.oslobodio kada student oslobodi racunar.
		*/

		// Implementirati ...
		void oslobodi(int rbr, int id_racunara) 
		{
			unique_lock<mutex> l(m);
			brojSlobodnih++;
			student.oslobodio(rbr, id_racunara - 1);
			slobodni[id_racunara - 1] = true;
			cv.notify_one();
		}
};

#endif // RC_H_INCLUDED
