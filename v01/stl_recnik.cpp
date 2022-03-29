/*
Napraviti jednostavan program za prevodjenje reci sa engleskog na srpski jezik.
Nakon unosa reci sa standardnog ulaza, ispisuje se prevod (ukoliko unesena rec postoji u recniku). 

Uneti 3 engleske reci i njihove prevode na srpski.
*/

#include <iostream>
#include <string>
#include <map>

using namespace std;

int main() {
	// Mapa - kljuc je engleska rec, a vrednost srpska
    map<string, string> recnik;

	// Kljuc stoji u [], a vrednost posle =
    recnik["red"] = "crveno";
    recnik["green"] = "zeleno";
    recnik["blue"] = "plavo";

    // Ucitavanje i prevod reci
	string rec;
    while (cin >> rec) {
        map<string,string>::const_iterator it = recnik.find(rec);

        if (it == recnik.end()) {
            cout << "Trazena rec ne postoji u recniku." << endl;
        }
        else {
            // it->second - first je kljuc, second je vrednost
            // zbog it koji već pokazuje na tu rec u recniku, mozemo ga iskoristiti da procitamo vrednot
            cout << "Prevod: " << it->second << endl;
        }
    }

    // while (cin >> rec) je true sve dok se ne unese Ctrl+D

    return 0;
}
