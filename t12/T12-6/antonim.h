#ifndef ANTONIM_H
#define ANTONIM_H

#include <map>
#include <string>

using namespace std;

/*
Funkcija koja će biti iskorišćena kao funkcija niti - u zadatom rečniku 
(parametar "recnik") pronaći suprotnu reč (antonim) od zadate reči "rec"

recnik   - Rečnik koji sadrži parove reč - reč suprotnog značenja
rec      - Reč za koju se traži reč suprotnog značenja
rezultat - Reč suprotnog značenja (ako je pronađena, ako nije pronađena, 
ovde treba da se upiše prazan string)
*/

// Dodato:
typedef map<string, string>::const_iterator ci;

// Implementirati ...
void nadjiAntonim(map<string, string> recnik, string rec, string& rezultat)
{
	// po kljucu
	for (ci a_begin = recnik.begin(); a_begin != recnik.end(); a_begin++)
	{
		if (a_begin->first == rec)
		{
			rezultat = a_begin->second;
			return;
		}
	}

	// po vrednosti
	for (ci a_begin = recnik.begin(); a_begin != recnik.end(); a_begin++)
	{
		if (a_begin->second == rec)
		{
			rezultat = a_begin->first;
			return;
		}
	}

	// do ovog dela će doći samo ako ne pronađe reč
    rezultat = "";
}

#endif // ANTONIM_H
