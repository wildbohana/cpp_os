#ifndef RC_H_INCLUDED
#define RC_H_INCLUDED

#include "student.h"

#include <mutex>
#include <condition_variable>

using namespace std;

// maksimalni broj racunara u ucionici
#define MAX 50 

class RC 
{
	private:
		Student& student;
		mutex m;
		condition_variable cv;
		bool slobodan[MAX];
		int broj_racunara;
		int broj_slobodnih;

	public:
		// Prosiriti po potrebi ...
		RC(Student& st, int br) : student(st) 
		{
			broj_racunara = br;
			broj_slobodnih = broj_racunara;

			for (int i = 0; i < broj_slobodnih; i++)
				slobodan[i] = true;
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

			while (broj_slobodnih == 0)
			{
				student.ceka(rbr);
				cv.wait(l);
			}

			broj_slobodnih--;

			int id;
			for (int i = 0; i < broj_racunara; i++)
				if (slobodan[i])
				{
					id = i;
					break;
				}

			student.zauzeo(rbr, id);	
			slobodan[id] = false;	

			return id;	
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

			broj_slobodnih++;
			student.oslobodio(rbr, id_racunara);
			slobodan[id_racunara] = true;

			cv.notify_one();
		}
};

#endif // RC_H_INCLUDED
