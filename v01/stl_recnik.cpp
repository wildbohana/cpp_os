/*
Napraviti jednostavan program za prevođenje reči sa engleskog na srpski jezik.
Nakon unosa reči sa standardnog ulaza, ispisuje se prevod (ukoliko unesena reč postoji u rečniku). 

Uneti 3 engleske reči i njihove prevode na srpski.
*/

#include <iostream>
#include <map>
#include <string>

using namespace std;

int main() {
	
	map<string,string> recnik;

    recnik["white"] = "belo";
    recnik["black"] = "crno";
    recnik["gold"] = "zlatno";

    cout << "Unesite rec za prevod:" << endl;

    string unos;
	cout << "Unesite reč koju želite da prevedete: ";
    cin >> unos;

    if (recnik[unos] != "")
        cout<< "Prevod je: " << recnik[unos] << endl;
    else
        cout << "Nema prevoda." << endl;

    return 0;
}

// radili smo ovo drugačije na vežbama
