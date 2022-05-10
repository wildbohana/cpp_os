#ifndef NITI_H
#define NITI_H

#include <thread>

#include "cenovnik.h"

using namespace std;

// Implementirati ...
map<string, double> izracunaj(map<string, double>& a, map<string, double>& b)
{
    map<string, double> jeftino;

	thread t(nadjiPovoljno, a, b, ref(jeftino));
	t.join();

	return jeftino;
}

#endif // NITI_H
