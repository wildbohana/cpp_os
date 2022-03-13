/*
Napraviti program koji prihvata proizvoljan broj linija sa standardnog ulaza i zatim ih ispisuje u obrnutom redosledu 
(poslednja prihvaćena linija se ispisuje prva).
*/

#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
   
    vector<string> linije;
    string linija;
	
	cout << "Unesite linije:" << endl;
    
	while (getline(cin, linija)) {
        linije.push_back(linija);
	}

    cout << "Ispis obrnuto:" << endl;
	
    for (auto it = linije.rbegin(); it != linije.rend(); it++) {
        cout << *it << endl;
	}

    return 0;
}
