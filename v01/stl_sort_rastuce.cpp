/*
Uneti 5 celih brojeva sa standardnog ulaza.
Korišćenjem STL algoritma sortirati brojeve u rastućem redosledu.
*/
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main() {

	vector<int> brojevi;
	
	cout << "Unesite 5 brojeva:" << endl;

    int br;
    for (int i = 0; i < 5; i++) {
        cin >> br;
        brojevi.push_back(br);
    }

    sort(brojevi.begin(), brojevi.end());
    // reverse(brojevi.begin(), brojevi.end());

	// Ispis
    cout << "Sortirano rastuce: " << endl;
    for(auto it : brojevi)
        cout << it << endl;

    return 0;
}
