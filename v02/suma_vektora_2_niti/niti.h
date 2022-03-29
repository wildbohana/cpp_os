#ifndef NITI_H_INCLUDED
#define NITI_H_INCLUDED

#include <vector>
#include <thread>

#include "suma_vektora.h"

using namespace std;

/* 
v - vektor cije elemente treba sumirati
povratna vrednost - suma svih elemenata vektora, izracunata pokretanjem 2 niti 
(svaka treba da obradi jednu polovinu elemenata)
*/

double sumiraj(vector<double> v) {
    // Promenljive za cuvanje rezultata
    double zbir1, zbir2;

    // Iteratori
    ci pocetak = v.begin();
    ci sredina = v.begin() + v.size() / 2;
    ci kraj = v.end();

    // Pokretanje niti
    thread t1(f, pocetak, sredina, ref(zbir1));
    thread t2(f, sredina, kraj, ref(zbir2));

    // Cekanje da se niti zavrse
    t1.join();
    t2.join();

    return zbir1 + zbir2;
}

#endif // NITI_H_INCLUDED
