#ifndef NITI_H
#define NITI_H

#include <thread>

#include "cenovnik.h"

using namespace std;

// Implementirati ...
map<string, double> izracunaj(map<string, double>& a, map<string, double>& b)
{
    map<string, double> novi;

    thread nit(nadjiPovoljno, a, b, ref(novi));
    nit.join();

    return novi;
}

#endif // NITI_H
