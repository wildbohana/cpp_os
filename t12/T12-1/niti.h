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
typedef vector<int>::const_iterator cit;

// Implementirati ...
vector<int> izracunaj(vector<int>& a, vector<int>& b)
{
	vector<int> rez(a.size());

	const int BROJ_NITI = a.size();
	thread niti[BROJ_NITI];

	cit pocetak_a = a.cbegin();
	cit pocetak_b = a.cbegin();
	auto pocetak_c = rez.begin();

	for (int i = 0; i < BROJ_NITI; i++, pocetak_a++, pocetak_b++, pocetak_c++)
		niti[i] = thread(saberiPar, pocetak_a, pocetak_b, pocetak_c);

	for (int i = 0; i < BROJ_NITI; i++)
		niti[i].join();

	return rez;
}

#endif // NITI_H
