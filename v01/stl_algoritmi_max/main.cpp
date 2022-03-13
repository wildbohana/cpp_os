/*
Korisnik treba da unese 5 celih brojeva koristeći tastaturu (standardni ulaz).
Korišćenjem STL algoritma, među tih 5 brojeva potrebno je pronaći najveći.
*/

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main()
{
	vector<int> brojevi;

    cout << "Unesite brojeve: " << endl;
    for (int i = 0; i < 5; i++) {
        int br;
        cin >> br;
        brojevi.push_back(br);
    }

    cout << "Najveći element je: " << *max_element(brojevi.begin(), brojevi.end()) << endl;
	
    return 0;
}
