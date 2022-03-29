/*
Napraviti program koji prihvata proizvoljan broj linija sa standardnog ulaza i zatim 
ih ispisuje u obrnutom redosledu (poslednja prihvacena linija se ispisuje prva).
*/

#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
	vector<string> linije;
	string linija;
	
	cout << "Unesite linije:" << endl;
    
	// Ucitavanje stringova
	while (getline(cin, linija)) {
        	linije.push_back(linija);
	}

	cout << "Ispis obrnuto:" << endl;
	
	// Za obrnuti ispis koristimo reverse iteratore
	for (auto it = linije.crbegin(); it != linije.crend(); it++) {
        	cout << "Recenica" << i << ": " << *it << endl;
	}

	return 0;
}
