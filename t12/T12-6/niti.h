#ifndef NITI_H
#define NITI_H

#include <thread>
#include <map>

using namespace std;

#include "antonim.h"

/* 
Implementacija pokretanja dodatne niti koja treba da pronađe reč suprotnog značenja 
(koristeći funkciju "nadjiAntonim")

recnik   - Rečnik koji sadrži parove reč - reč suprotnog značenja
rec      - Reč za koju se traži reč suprotnog značenja

Povratna vrednost treba da bude reč suprotnog značenja (ako je pronađena)
*/

// Implementirati ...
string izracunaj(map<string, string> recnik, string rec)
{
    string antonim;

    thread nit(nadjiAntonim, recnik, rec, ref(antonim));
    nit.join();

    return antonim;
}


#endif // NITI_H
