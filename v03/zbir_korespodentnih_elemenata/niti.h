#ifndef NITI_H_INCLUDED
#define NITI_H_INCLUDED

#include <vector>
#include <thread>

#include "suma_vektora.h"

using namespace std;

typedef vector<double>::const_iterator ci;

// v - vektor A čije elemente treba sumirati
// v - vektor B čije elemente treba sumirati
// povratna vrednost - suma korespondentnih elemenata vektora A i B, izračunata pozivanjem dve niti

// Implementirati ...
vector<double> sumiraj(vector<double> a, vector<double> b) 
{
	vector<double> sum(a.size());

    thread t1(f, a.begin(), a.begin() + a.size()/2, b.begin(), sum.begin());
    thread t2(f, a.begin() + a.size()/2, a.end(), b.begin() + a.size()/2, sum.begin() + a.size()/2);

	t1.join();
	t2.join();

	return sum;
}

#endif // NITI_H_INCLUDED
