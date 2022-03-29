/*
Korisnik treba da unese 5 celih brojeva koristeci tastaturu (standardni ulaz).
Koriscenjem STL algoritma, medju tih 5 brojeva potrebno je pronaci najveći.
*/

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main()
{
	// vector: 0 0 0 0 0
	vector<int> brojevi(5);	

	// Unos brojeva
    cout << "Unesite brojeve: " << endl;
    for (int i = 0; i < brojevi.size(); i++) {
        cin >> brojevi[i];
    }

	// Pronalazenje maksimalnog elementa
    cout << "Najveći element je: " << *max_element(brojevi.cbegin(), brojevi.cend()) << endl;
	
    return 0;
}
