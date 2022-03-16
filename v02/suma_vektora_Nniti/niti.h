#ifndef NITI_H_INCLUDED
#define NITI_H_INCLUDED

#include <vector>
#include <thread>

#include "suma_vektora.h"

using namespace std;

// v - vektor čije elemente treba sumirati
// n - koliko niti treba pokrenuti
// povratna vrednost - suma svih elemenata vektora, izračunata pokretanjem n niti
// svaka nit treba da obradi jedan deo elemenata
double sumiraj(vector<double> v, int n) {
    thread t[n];
    vector<double> zbir(n);
    ci pocetak = v.cbegin();

    for(int i = 0; i < n; i++) {
        ci kraj = (i < n - 1) ? pocetak + v.size() / n : v.cend();

        t[i] = thread(f, pocetak, kraj, ref(zbir[i]));

        pocetak = kraj;
    }

    for(int i = 0; i < n; i++) t[i].join();

    double suma;
    f(zbir.cbegin(), zbir.cend(), suma);
    return suma;
}

#endif // NITI_H_INCLUDED
