/*
Kreirati globalnu celobrojnu promeljivu brojač. Nakon toga kreirati 2 funkcije 
inkrement i dekrement koje povećavaju i smanjuju dati brojač ITERACIJA puta. 
ITERACIJA je konstanta koja predstavlja proizvoljno velik broj (npr. 100.000.000). 

Kreirati jednu nit od funkcije inkrement i jednu nit od funkcije dekrement.

Nakon završetka rada niti ispisati vrednosti brojača. Da li je data vrednost očekivana? 
*/

// BRO THIS FILE WAS SKROZ EMPTY

#include <iostream>
#include <thread>

using namespace std;

const int ITERATIONS = 100000000;

int brojac = 0;

void inkrement()
{
	for (int i = 0; i < ITERATIONS; i++)
		brojac++;
}

void dekrement()
{
	for (int i = 0; i < ITERATIONS; i++)
		brojac--;
}

int main()
{
	thread t1(inkrement);
	thread t2(dekrement);

	t1.join();
	t2.join();

	// Necemo dobiti 0 nego neku random vrednost
	cout << brojac << endl;

	return 0;
}
