#ifndef SUMA_VEKTORA_H_INCLUDED
#define SUMA_VEKTORA_H_INCLUDED

#include <vector>
using namespace std;

typedef vector<double>::const_iterator ci;

/*
pocetak - iterator na prvi element koji se uzima u obzir pri racunanju sume
kraj - iterator iza poslednjeg elementa koji se uzima u obzir pri racunanju sume
zbir - parametar kroz koji funkcija treba da vrati izracunatu sumu
*/

void f(ci pocetak, ci kraj, double& zbir) {
	// Ne mora, ali nam garantuje da ce zbir na početku biti 0
	zbir = 0;       
    
	while (pocetak != kraj) {
        zbir += *pocetak++;
    }

	/*
	// A moze i preko for petlje:
	for (ci it = pocetak; it != kraj; it++) {
        zbir += *it;
    }
	*/
}

#endif // SUMA_VEKTORA_H_INCLUDED
