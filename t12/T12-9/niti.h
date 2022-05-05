#ifndef NITI_H
#define NITI_H

#include <thread>

#include "obracun.h"

using namespace std;

// Implementirati ...
double izracunaj(list<string> kupljeno, map<string, double> cenovnik)
{
    double racun = 0;

	// ne zaboravi ref !!!
    thread nit(obracunajUkupno, kupljeno, cenovnik, ref(racun));
    nit.join();

    return racun;
}

#endif // NITI_H
