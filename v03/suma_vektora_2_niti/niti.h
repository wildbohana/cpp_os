#ifndef NITI_H_INCLUDED
#define NITI_H_INCLUDED

#include <vector>
#include <thread>

#include "suma_vektora.h"

using namespace std;

// Dodato:
typedef vector<double>::const_iterator ci;

// v - vektor čije elemente treba sumirati
// povratna vrednost - suma svih elemenata vektora, izračunata pokretanjem 2 niti (svaka treba da obradi jednu polovinu elemenata)

// Implementirati ...
double sumiraj(vector<double> v) 
{
    double zbir1, zbir2;

	ci polovina = v.begin() + v.size()/2;

	// ne zaboravi da posaljes zbirove po referenci !!!
	thread t1(f, v.begin(), polovina, ref(zbir1)); 
	thread t2(f, polovina, v.end(), ref(zbir2)); 

	t1.join();
	t2.join();

	return zbir1 + zbir2;
}

#endif // NITI_H_INCLUDED
