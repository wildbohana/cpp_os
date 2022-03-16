#ifndef NITI_H_INCLUDED
#define NITI_H_INCLUDED

#include <vector>
#include <thread>

#include "suma_vektora.h"

using namespace std;

// v - vektor čije elemente treba sumirati
// povratna vrednost - suma svih elemenata vektora, izračunata pokretanjem 2 niti
// svaka nit treba da obradi jednu polovinu elemenata
double sumiraj(vector<double> v) {
    double zbir1, zbir2;

    ci mid = v.begin() + v.size() / 2;

    thread t1(f, v.begin(), mid, ref(zbir1));    //thread 1
    thread t2(f, mid, v.end(), ref(zbir2));      //thread 2

    t1.join();
    t2.join();

    return zbir1 + zbir2;
}

#endif // NITI_H_INCLUDED
