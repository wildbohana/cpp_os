#ifndef NITI_H
#define NITI_H

#include <thread>
#include <list>

#include "obrtanje.h"

using namespace std;

/*
Implementacija obrtanja redosleda elemenata u listi pokretanjem jedne nove niti

ulazna  - Ulazna lista, koja se obrađuje

Rezultat je rezultujuća lista (popunjena elementima u obrnutom redosledu)
*/

// Implementirati ...
list<int> izracunaj(const list<int>& ulazna)
{
	list<int> rez;

	thread t(obrniRedosled, ulazna, ref(rez));
	t.join();

	return rez;
}

#endif // NITI_H
