#ifndef RECNIK_H
#define RECNIK_H

#include <map>
#include <string>

using namespace std;

/*
Funkcija koja će biti iskorišćena kao funkcija niti - od zadatog englesko-srpskog rečnika ona pravi obrnuti rečnik - srpsko-engleski

engSrp - Englesko-srpski rečnik (ključ je reč na engleskom, podatak je njen prevod na srpski)
srpEng - Srpsko-engleski rečnik (ključ je reč na srpskom, podatak je njen prevod na engleski) - ovaj parametar treba da se popuni izvršenjem ove funkcije
*/

// Implementirati ...
void napraviRecnik(map<string, string> engSrp, map<string, string>& srpEng)
{
    auto it_eng_srp = engSrp.cbegin();

	// insert - sluzi za ubacivanje u recnik
	// sta je bilo first, sad je second i obrnuto
    for (; it_eng_srp != engSrp.cend(); it_eng_srp++)
       srpEng.insert({it_eng_srp -> second, it_eng_srp -> first});
}


#endif // RECNIK_H
