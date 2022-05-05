#ifndef AMBULANTA_H_INCLUDED
#define AMBULANTA_H_INCLUDED

#include "student.h"

#include <mutex>
#include <condition_variable>

using namespace std;

class Studentska_ambulanta 
{
	private:
		Student& student;
		// Dodato:
		int brojSlobodnihMesta[2];
	    condition_variable ambulantaSlobodna[2];
	    mutex m;
	public:
		// Prosiriti po potrebi ...
		Studentska_ambulanta(Student& st, int muskih, int zenskih) : student(st) 
		{
			brojSlobodnihMesta[0] = muskih;
	        brojSlobodnihMesta[1] = zenskih;
		}

		/*
		Metoda koju poziva nit koja simulira studenta u ambulanti kada student pokusava da udje u nju.
		Ukoliko je ambulanta zauzeta, izvrsenje ove metode ce cekati da se ona oslobodi.
		
		rbr - Redni broj studenta
		p   - Pol studenta
		
		Potrebno je pozvati metodu student.ceka kada student ne moze da stupi u ambulantu.
		Potrebno je pozvati metodu student.ulazi kada student udje u ambulantu.
		*/

		// Implementirati ...
		void udji(int rbr, Pol p) 
		{
			unique_lock<mutex> l(m);

        	// ako nema slobodnih mesta - cekaj
			while (brojSlobodnihMesta[p] == 0)
			{
				ambulantaSlobodna[p].wait(l);
				student.ceka(rbr, p);
			}

			// ako ima slobodnih mesta - udji
			student.ulazi(rbr, p);
			brojSlobodnihMesta[p]--;
		}

		/*
		Metoda koju poziva nit koja simulira studenta u ambulanti nakon sto je student pregledan i izlazi iz nje.
		
		rbr - Redni broj studenta
		p   - Pol studenta
		*/

		// Implementirati ...
		void izadji(int rbr, Pol p) 
		{
			unique_lock<mutex> l(m);
	        brojSlobodnihMesta[p]++;
	        ambulantaSlobodna[p].notify_one();
		}
};

#endif // AMBULANTA_H_INCLUDED
