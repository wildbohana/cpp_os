#ifndef LOGICKO_KOLO_H
#define LOGICKO_KOLO_H

#include <vector>

using namespace std;

typedef vector<int>::const_iterator cit;

/*
Implementacija XOR operacije - treba da se koristi kao funkcija niti

a_pocetak - Iterator na prvi element vektora A koji se obrađuje
a_kraj    - Iterator iza poslednjeg elementa vektora A koji se obrađuje
b_pocetak - Iterator na prvi element vektora B koji se obrađuje
rez       - Iterator u rezultujućem vektoru - elementi počevši sa ovom pozicijom trebaju biti popunjeni rezultatima XOR operacije
*/

// Implementirati ...
void izvrsiXOR(cit a_pocetak, cit a_kraj, cit b_pocetak, vector<int>::iterator rez)
{
    for (; a_pocetak != a_kraj; a_pocetak++, b_pocetak++, rez++) 
		*rez = *a_pocetak ^ *b_pocetak;
}

#endif // LOGICKO_KOLO_H
