#ifndef NITI_H
#define NITI_H

#include <thread>
#include <vector>

#include "sabiranje.h"

using namespace std;

/*
Implementacija sabiranja korespondentnih elemenata celih vektora A i B pokretanjem onoliko niti koliko ima elemenata u vektorima

a - Vektor A
b - Vektor B

Rezultat izvršavanja treba da bude vector<int> koji sadrži rezultat sabiranja korespondentnih elemenata.
*/

// Dodato:
typedef vector<int>::const_iterator ci;

// Implementirati ...
vector<int> izracunaj(vector<int>& a, vector<int>& b)
{
	int n = a.size();
	thread niti[n];

	vector<int> rez(n);

	ci a_pocetak = a.cbegin();
	ci b_pocetak = b.cbegin();
	vector<int>::iterator c_pocetak = rez.begin();

	for (int i = 0; i < n; i++, a_pocetak++, b_pocetak++, c_pocetak++)
		niti[i] = thread(saberiPar, a_pocetak, b_pocetak, c_pocetak);

	for (int i = 0; i < n; i++)
		niti[i].join();

	return rez;
}

#endif // NITI_H
