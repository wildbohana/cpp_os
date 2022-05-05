#ifndef NITI_H
#define NITI_H

#include <thread>
#include <map>

#include "recnik.h"

using namespace std;

/*
Implementacija pokretanja dodatne niti koja treba da napravi novi rečnik (koristeći funkciju "napraviRecnik")

brojNiti - Broj niti koje je potrebno startovati
*/

// Implementirati ...
map<string, string> izracunaj(map<string, string> engSrp)
{
    map<string, string> srpEng;

	// ne zaboravi ref !!!
    thread nit(napraviRecnik, engSrp, ref(srpEng));
    nit.join();

    return srpEng;
}

#endif // NITI_H
