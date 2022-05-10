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
typedef list<int>::const_iterator ci;

// Implementirati ...
void obrniRedosled(const list<int>& ulazna, list<int>& izlazna)
{
    ci pocetak = ulazna.cbegin();
	ci kraj = ulazna.cend();
	
	for (; pocetak != kraj; pocetak++)
	{
		izlazna.push_front(*pocetak);
	}
}

#endif // OBRTANJE_H
