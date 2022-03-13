/*
Napraviti program koji prihvata proizvoljan broj linija sa standardnog ulaza i zatim ih ispisuje u redosledu kojim su uneti.
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

    cout << "Ispis preko iteratora:" << endl;
    for (auto it = linije.begin(); it != linije.end(); it++) {
        cout << *it << endl;
	}

	/*
    cout << endl << "Ispis preko iteratora - drugi način:" << endl;
    for (auto it : linije) {
        cout << it << endl;
	}
	*/

    return 0;
}
