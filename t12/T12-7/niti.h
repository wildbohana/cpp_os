#ifndef NITI_H
#define NITI_H

#include <thread>
#include <vector>

#include "logicko_kolo.h"

using namespace std;

/*
Implementacija operacije XOR nad ulaznim vektorima A i B izvršene uz pomoć dve niti (svaka nit obrađuje polovinu)

a - Vektor A (16 binarnih cifara)
b - Vektor B (16 binarnih cifara)

Rezultat izvršavanja treba da bude vector<int> koji sadrži rezultat izvršenja XOR operacije nad ulaznim vektorima.
*/

// Implementirati ...
vector<int> izracunaj(vector<int>& a, vector<int>& b)
{
    vector<int> rez(16);

    thread niti[2];

    niti[0] = thread(izvrsiXOR, a.cbegin(), a.cbegin() + 8, b.cbegin(), rez.begin());
    niti[1] = thread(izvrsiXOR, a.cbegin() + 8, a.cend(), b.cbegin() + 8, rez.begin() + 8);

    niti[0].join();
    niti[1].join();

    return rez;
}

#endif // NITI_H
