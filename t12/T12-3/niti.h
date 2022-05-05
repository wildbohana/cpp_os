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
    list<int> izlazna;

	thread nit(obrniRedosled, ulazna, ref(izlazna));
	nit.join();

	return izlazna;
}

#endif // NITI_H
