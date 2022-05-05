#ifndef OBRTANJE_H
#define OBRTANJE_H

#include <list>

using namespace std;

/* 
Implementacija obrtanja redosleda elemenata u listi
Ova funkcija treba da bude iskorišćena za funkciju niti koje ovaj program pokreće.

ulazna  - Ulazna lista, koja se obrađuje
izlazna - Rezultujuća lista (biće popunjena elementima u obrnutom redosledu)
*/

// Dodato
typedef list<int>::const_iterator cit;

// Implementirati ...
void obrniRedosled(const list<int>& ulazna, list<int>& izlazna)
{
    cit it = ulazna.cbegin();
	cit kraj = ulazna.cend();

	for (; it != kraj; it++)
		izlazna.push_back(*it);

	izlazna.reverse();
}

#endif // OBRTANJE_H
