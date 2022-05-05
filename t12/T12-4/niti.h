#ifndef NITI_H
#define NITI_H

#include <thread>
#include <vector>
#include <algorithm>

#include "pretraga.h"

using namespace std;

/*
Implementacija pokretanja tri niti koje treba da pretraže (svaka svoju trećinu) ulazni vektor brojeva i nađu element najbliži nuli

ulaz - Ulazni vektor brojeva koji se pretražuje

Povratna vrednost je nađeni element, najbliži nuli od svih elemenata koji su obuhvaćeni pretragom
*/

// Implementirati ...
double izracunaj(vector<double> ulaz)
{
    thread niti[3];
    const unsigned segment = ulaz.size() / 3;
    double najblizi[3];

	// ne zaboravi ref kod najblizi[x]
    niti[0] = thread(pretrazi, ulaz.cbegin(), ulaz.cbegin() + segment, ref(najblizi[0]));
    niti[1] = thread(pretrazi, ulaz.cbegin() + segment, ulaz.cbegin() + 2 * segment, ref(najblizi[1]));
    niti[2] = thread(pretrazi, ulaz.cbegin() + 2 * segment, ulaz.cend(), ref(najblizi[2]));

    niti[0].join();
    niti[1].join();
    niti[2].join();

    double min = abs(najblizi[0]);

	// i = 1, i = 2 (za 0 smo vec "proverili")
    for (int i = 1; i < 3; i++)
        if (najblizi[i] < min)
            min = abs(najblizi[i]);

    return min;
}

#endif // NITI_H
