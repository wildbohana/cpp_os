#ifndef CENOVNIK_H
#define CENOVNIK_H

#include <string>
#include <map>

using namespace std;

// Dodato:
typedef map<string, double>::const_iterator ci;

// Implementirati ...
void nadjiPovoljno(map<string, double> a, map<string, double> b, map<string, double> &jeftino)
{
	ci a_begin = a.cbegin();

    for (; a_begin != a.cend(); a_begin++)
		jeftino[a_begin->first] = min(a_begin->second, b[a_begin->first]);
}

// first (kljuc) - ime proizvoda
// seconds (vrednost) - cena proizvoda

// malo duza varijanta
/*
void nadjiPovoljno(map<string, double> a, map<string, double> b, map<string, double> &jeftino)
{
    for (ci it = a.cbegin(); it != a.cend(); it++)
        for (ci jt = b.cbegin(); jt != b.cend(); jt++)
			if (it -> first == jt -> first) 
			{
                if (it -> second < jt -> second)
                    jeftino.insert({it -> first, it -> second});
                else
                    jeftino.insert({jt -> first, jt -> second});
            }
}
*/

#endif // CENOVNIK_H
