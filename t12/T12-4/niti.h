#ifndef NITI_H
#define NITI_H

#include <thread>
#include <vector>
#include <algorithm>

#include <math.h>

#include "pretraga.h"

using namespace std;

/*
Implementacija pokretanja tri niti koje treba da pretraže (svaka svoju trećinu) ulazni vektor brojeva i nađu element najbliži nuli

ulaz - Ulazni vektor brojeva koji se pretražuje

Povratna vrednost je nađeni element, najbliži nuli od svih elemenata koji su obuhvaćeni pretragom
*/

// Implementirati ...
double izracunaj(vector<double> ulaz)
{
	double temp[3];

	int segment = ulaz.size() / 3;

	thread t1(pretrazi, ulaz.cbegin(), ulaz.cbegin() + segment, ref(temp[0]));
	thread t2(pretrazi, ulaz.cbegin() + segment, ulaz.cbegin() + 2* segment, ref(temp[1]));
	thread t3(pretrazi, ulaz.cbegin() + 2 * segment, ulaz.cend(), ref(temp[2]));

	t1.join();
	t2.join();
	t3.join();

	double retVal = temp[0];
	
	for (int i = 1; i < 3; i++)
		if (abs(temp[i]) < abs(retVal))
			retVal = temp[i];

	return retVal;
}

#endif // NITI_H
