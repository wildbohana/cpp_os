#ifndef NITI_H
#define NITI_H

#include <thread>

#include "obracun.h"

using namespace std;

// Implementirati ...
double izracunaj(list<string> kupljeno, map<string, double> cenovnik)
{
    double racun = 0;

	thread t(obracunajUkupno, kupljeno, cenovnik, ref(racun));
	t.join();

	return racun;
}

#endif // NITI_H
