/*
Napraviti konkurentni program (koristeći funkciju f()) koji sabira korespondentne elemente
kontejnera a i b, a rezultat smešta u vektor zbir.

typedef vector<double>::const_iterator ci;
void f(ci a_begin, ci a_end, ci b_begin, vector<double>::iterator sum_begin);

Funkciju f implementirati u fajlu suma_vektora.cpp.

Rezultat se upisuje koriscenjem iteratora sum_begin. Zato je neophodno da vektor nad kojim je
kreiran taj iterator bude kreiran pre poziva funkcije f. Taj vektor mora da sadrzi dovoljno mesta
za upis svih elemenata.

Funkciju sumiraj implementirati u fajlu vise_niti.h. Ova funkcija treba da pokrene dve niti
koje ce, koristeci funkciju f, da izracunaju korespondentne sume vektora, i to tako da svaka
nit obradi svoju polovinu elemenata (prva nit prvu polovinu, druga nit drugu polovinu).

Program optimizovati za procesor sa 2 jezgra.

Napomene:
Podrazumeva se da su kontejneri a i b iste dužine.
Sabiranje korespodentnih elemenata je sabiranje matrica (vektora).
*/

#include <iostream>
#include "suma_vektora.h"
#include "niti.h"

using namespace std;

void testirajSumu() {
	vector<double> a = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	vector<double> b = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	vector<double> sum(a.size());

	// Sekvencijalno sumiranje
	f(a.begin(), a.end(), b.begin(), sum.begin());

	cout << "Suma vektora pozivom funkcije f:" << endl;

	for (vector<double>::const_iterator it = sum.begin(); it != sum.end(); it++) {
		cout << *it << endl;
	}
}

void testirajNiti() {
	vector<double> a = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	vector<double> b = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	vector<double> sum(a.size());

	// Sumiranje preko niti
	sum = sumiraj(a, b);

	cout << "Suma vektora, koriscenjem 2 niti: " << endl;

	for (vector<double>::const_iterator it = sum.begin(); it != sum.end(); it++) {
		cout << *it << endl;
	}
}

int main() {
	testirajSumu();

	// Otkomentarisati kada implementirate funkciju sumiraj:
	testirajNiti();

	return 0;
}
