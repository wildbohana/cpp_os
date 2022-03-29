/*
Uneti 5 celih brojeva sa standardnog ulaza.
Koriscenjem STL algoritma sortirati brojeve u rastucem redosledu
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
	// vector: 0 0 0 0 0
    	vector<int> brojevi(5);     

	// Unos 5 brojeva
    	cout << "Unesite 5 brojeva: ";
    	for (int i = 0; i < brojevi.size(); i++) {
		cin >> brojevi[i];
	}

	// Koristimo begin i end za rastuci redosled
    	sort(brojevi.begin(), brojevi.end());

    	cout << endl;

	// Ispis
    	cout << "Sortiran niz" << endl;
    	for (int i = 0; i < brojevi.size(); i++) {
        	cout << brojevi[i] << " ";
    	}
	cout << endl;

    	return 0;
}
