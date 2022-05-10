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
    int n = a.size();
	vector<int> c(n);

	thread t1(izvrsiXOR, a.cbegin(), a.cbegin() + n/2, b.cbegin(), c.begin());
	thread t2(izvrsiXOR, a.cbegin() + n/2, a.cend(), b.cbegin() + n/2, c.begin() + n/2);

	t1.join();
	t2.join();

	return c;
}

#endif // NITI_H
