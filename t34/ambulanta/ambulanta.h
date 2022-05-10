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
		mutex m;
		condition_variable cv[2];
		int brojSlobodnih[2];

	public:
		// Prosiriti po potrebi ...
		Studentska_ambulanta(Student& st, int muskih, int zenskih) : student(st) 
		{
			brojSlobodnih[0] = muskih;
			brojSlobodnih[1] = zenskih;
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

			while (brojSlobodnih[p] == 0)
			{
				student.ceka(rbr, p);
				cv[p].wait(l);
			}

			brojSlobodnih[p]--;
			student.ulazi(rbr, p);
		}

		void izadji(int rbr, Pol p)
		{
			unique_lock<mutex> l(m);
			
			brojSlobodnih[p]++;
			cv[p].notify_one();
		}
};

#endif // AMBULANTA_H_INCLUDED
