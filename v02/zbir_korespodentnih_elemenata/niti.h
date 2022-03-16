#ifndef NITI_H_INCLUDED
#define NITI_H_INCLUDED

#include <vector>
#include <thread>

#include "suma_vektora.h"

using namespace std;

// v - vektor A čije elemente treba sumirati
// v - vektor B čije elemente treba sumirati
// povratna vrednost - suma korespondentnih elemenata vektora A i B, izračunata pozivanjem dve niti
vector<double> sumiraj(vector<double> a, vector<double> b) {
    vector<double> c (a.size());
    unsigned polovina = (a.size() % 2 == 0 ? (a.size() / 2) : (a.size() / 2) + 1);

    thread t1 = thread(f, a.begin(), a.begin() + polovina, b.begin(), c.begin());
    thread t2 = thread(f, a.begin() + polovina, a.end(), b.begin() + polovina, c.begin() + polovina);

    t1.join();
    t2.join();

    return c;
}

#endif // NITI_H_INCLUDED
