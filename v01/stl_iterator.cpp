/*
Napraviti program koji prihvata proizvoljan broj linija sa standardnog ulaza 
i zatim ih ispisuje u redosledu kojim su uneti.
*/

#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
	// Unos broja linija
    int n = -1;
    while (n < 0) {
        cout << "Unesite broj linija koji zelite da unesete: ";
        cin >> n;
    }

    vector<string> unos;
    string temp;

    // Ucitavanje stringova
    cout << "Unesite linije:" << endl;
	for (int i = 0; i < n; i++) {
		cout << "Linija " << i+1 << ": ";
		cin >> temp;

		// Dodavanje novog stringa u vektor
		unos.push_back(temp);
    }

    cout << endl;

    // Ispis (koristimo obicne iteratore)
    cout << "Ispis preko iteratora:" << endl;
    for (auto it = unos.cbegin(); it != unos.cend(); it++) {
        cout << *it << endl;
	}

	/*
    cout << endl << "Ispis preko iteratora - drugi način:" << endl;
    for (auto it : unos) {
        cout << it << endl;
	}
	*/

    return 0;
}
