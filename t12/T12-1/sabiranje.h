#ifndef SABIRANJE_H
#define SABIRANJE_H

#include <vector>

using namespace std;

typedef vector<int>::const_iterator cit;

/* 
Implementacija sabiranja jednog para korespondentnih elemenata vektora
Ova funkcija treba da bude iskorišćena za funkciju niti koje ovaj program pokreće.

a   - Iterator na element vektora A koji se obrađuje
b   - Iterator na element vektora B koji se obrađuje
rez - Iterator u rezultujućem vektoru - element na ovoj poziciji će biti popunjen rezultatom sabiranja
*/

// Implementirati ...

void saberiPar(cit a, cit b, vector<int>::iterator rez)
{
	*rez = *a + *b;
}

#endif // SABIRANJE_H
