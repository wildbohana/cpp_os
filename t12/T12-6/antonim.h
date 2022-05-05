#ifndef ANTONIM_H
#define ANTONIM_H

#include <map>
#include <string>

using namespace std;

/*
Funkcija koja će biti iskorišćena kao funkcija niti - u zadatom rečniku (
parametar "recnik") pronaći suprotnu reč (antonim) od zadate reči "rec"

recnik   - Rečnik koji sadrži parove reč - reč suprotnog značenja
rec      - Reč za koju se traži reč suprotnog značenja
rezultat - Reč suprotnog značenja (ako je pronađena, ako nije pronađena, 
ovde treba da se upiše prazan string)
*/

// Dodato:
typedef map<string, string>::const_iterator cit;

// Implementirati ...
void nadjiAntonim(map<string, string> recnik, string rec, string& rezultat)
{
    // pretraga po kljcu
    for (cit it = recnik.cbegin(); it != recnik.cend(); it++) 
	{
        if (it -> first == rec) 
		{
            rezultat = it -> second;
            return;
        }
    }

	// pretraga po vrednosti
    for (cit it = recnik.cbegin(); it != recnik.cend(); it++) 
	{
        if (it -> second == rec) 
		{
            rezultat = it -> first;
            return;
        }
    }

	// do ovog dela ce doci samo ako ne pronadje rec -> vraca prazan string
    rezultat = ""; 
}

#endif // ANTONIM_H
