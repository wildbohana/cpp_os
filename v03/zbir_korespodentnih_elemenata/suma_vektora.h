#ifndef SUMA_VEKTORA_H_INCLUDED
#define SUMA_VEKTORA_H_INCLUDED

#include <vector>

using namespace std;

typedef vector<double>::const_iterator ci;

/*
Funkcija niti. Funkcija preuzima sledeće parametre:
1) ci a_begin -> iterator na početak dela vektora a
2) ci a_end   -> iterator na kraj dela vektora a (dovoljan je samo jedan kraj)
3) ci b_begin -> iterator na početak dela vektora b
4) vector<double>::iterator sum_begin  -> iterator na početak dela vektora suma
*/

// Implementirati ...
void f(ci a_begin, ci a_end, ci b_begin, vector<double>::iterator sum_begin) {
    for (; a_begin != a_end; a_begin++, b_begin++, sum_begin++)
		*sum_begin = *a_begin + *b_begin;
}

#endif // SUMA_VEKTORA_H_INCLUDED
