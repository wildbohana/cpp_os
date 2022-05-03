#ifndef NITI_H_INCLUDED
#define NITI_H_INCLUDED

#include <vector>

#include "suma_vektora.h"
#include <vector>
#include <thread>

using namespace std;

// Dodato:
typedef vector<double>::const_iterator ci;

// v - vektor čije elemente treba sumirati
// n - koliko niti treba pokrenuti
// povratna vrednost - suma svih elemenata vektora, izračunata pokretanjem n niti (svaka nit treba da obradi jedan deo elemenata)

// Implementirati ...
double sumiraj(vector<double> v, int n) 
{
	double zbir[n];    
	thread t[n];

	ci pocetak = v.begin();
	ci kraj = pocetak + v.size()/n;

	// obrati paznju: IDE DO N-1, NE DO N
	for (int i = 0; i < n-1; i++)
	{
		t[i] = thread(f, pocetak, kraj, ref(zbir[i]));
		pocetak = kraj;
		kraj += v.size()/n;
	}

	// za N imamo poseban slucaj
	t[n-1] = thread(f, pocetak, v.end(), ref(zbir[n-1]));

	for (int i = 0; i < n; i++)
		t[i].join();

	double povr_vr = 0;
	
	for (int i = 0; i < n; ++i)
	{
		cout << "zbir[" << i << "] = " << zbir[i] << endl;
		povr_vr += zbir[i];
	}

	return povr_vr;
}

#endif // NITI_H_INCLUDED
