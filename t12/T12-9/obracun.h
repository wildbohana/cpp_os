#ifndef OBRACUN_H
#define OBRACUN_H

#include <string>
#include <map>
#include <list>

using namespace std;

// Implementirati ...
void obracunajUkupno(list<string> kupljeno, map<string, double> cenovnik, double& ukupno)
{
	ukupno = 0;

	list<string>::const_iterator spisak = kupljeno.cbegin();
	for (; spisak != kupljeno.cend(); spisak++)
		ukupno += cenovnik[*spisak];
}

// malo duza varijanta
/*
void obracunajUkupno(list<string> kupljeno, map<string, double> cenovnik, double& ukupno)
{
    for (auto it = kupljeno.cbegin(); it != kupljeno.cend(); it++)
        for (auto jt = cenovnik.cbegin(); jt != cenovnik.cend(); jt++)
            if (*it == jt -> first) 
			{
                ukupno += jt -> second;
                break;
            }
}
*/

#endif // OBRACUN_H
