/*
Napraviti jednostavan program za prevođenje reči sa engleskog na srpski jezik.
Nakon unosa reči sa standardnog ulaza, ispisuje se prevod (ukoliko unesena reč postoji u rečniku). 

Uneti 3 engleske reči i njihove prevode na srpski.
*/

#include <iostream>
#include <map>
#include <string>

using namespace std;

int main()
{
	map<string, string> recnik;

	recnik["black"] = "crna";
	recnik["white"] = "bela";
	recnik["red"] = "crvena";

	string uneta_engl_rec;

	while(getline(cin, uneta_engl_rec))
		cout << "Prevod: " << recnik[uneta_engl_rec] << endl;

    return 0;
}
