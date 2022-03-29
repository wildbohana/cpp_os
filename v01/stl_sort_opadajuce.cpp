/*
Uneti 5 celih brojeva sa standardnog ulaza.
Koriscenjem STL algoritma sortirati brojeve u opadajucem redosledu
*/

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main()
{
	// vector: 0 0 0 0 0
    	vector<int> brojevi(5);

	cout << "Unesite 5 brojeva: ";
    	for (int i = 0; i < brojevi.size(); i++) { 
		cin >> brojevi[i];
	}

	// Koristimo rbegin i rend za opadajuci redosled
    	sort(brojevi.rbegin(), brojevi.rend());

    	cout << endl;

	// Ispis
	cout << "Niz sortiran u opadajucem poretku: ";
    	for (int i = 0; i < brojevi.size(); i++) {
        	cout << brojevi[i] << " "l;
    	}
		cout << endl;

    	return 0;
}
