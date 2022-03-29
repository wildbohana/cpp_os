/*
Definisati funkciju: void increment(int& a);
koja povecava (za jedan) vrednost argumenta.

Napraviti program koji:
a) poziva funkciju increment()
b) stvara nit od funkcije increment()
*/

#include <iostream>
#include <thread>

using namespace std;

// Funkcija koja inkrementira prosledjeni parametar
void increment(int &a) {
	a++;
}

int main() {
	int a = 3;
	cout << a << endl;

	// Obicno pozivanje funkcije
	increment(a);
	cout << a << endl;

	// Pozivanje funkcije iz niti
	// ref() - operator reference, obavezan ako rezultat treba da se sacuva
	// Nece raditi ako posaljemo samo b bez ref() ispred
	thread t(increment, ref(a));

	// Tek nakon izvrsenja ove linije može se smatrati da je b dobilo zeljenu vrednost
	t.join();

	// Ispis vrednosti a
	cout << a << endl;

	return 0;
}
