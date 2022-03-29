#ifndef NITI_H_INCLUDED
#define NITI_H_INCLUDED

#include <vector>
#include <thread>

#include "suma_vektora.h"

using namespace std;

/*
v - vektor A cije elemente treba sumirati
v - vektor B cije elemente treba sumirati
povratna vrednost - suma korespondentnih elemenata vektora A i B, izracunata pozivanjem dve niti
*/

vector<double> sumiraj(vector<double> a, vector<double> b) {
	// Vektor u koji smestamo zbirove parova
	vector<double> c (a.size());

	unsigned polovina = (a.size() % 2 == 0 ? (a.size() / 2) : (a.size() / 2) + 1);

	// void f(ci a_begin, ci a_end, ci b_begin, vector<double>::iterator sum_begin)
	// t1 sabira korespodentne elemente u prvoj polovini, t2 sabira elemente u drugoj polovini
	thread t1 = thread(f, a.begin(), a.begin() + polovina, b.begin(), c.begin());
	thread t2 = thread(f, a.begin() + polovina, a.end(), b.begin() + polovina, c.begin() + polovina);

	t1.join();
	t2.join();

	return c;
}

#endif // NITI_H_INCLUDED
