#ifndef VALUTA_H_INCLUDED
#define VALUTA_H_INCLUDED

#include <string>

using namespace std;

// Tipovi valuta
enum Valuta {DINAR = 0, EURO}; 

string naziv_valute(Valuta valuta) 
{
    if (valuta == Valuta::DINAR)
        return "DIN";
    else
        return "EUR";
}

#endif // VALUTA_H_INCLUDED
