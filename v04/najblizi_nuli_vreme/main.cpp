/*
Napraviti konkurentni program koji pronalazi element najbliži broju 0 u zadatom vektoru brojeva.

Pretragu podeliti u 3 programske niti. 
Ulazni vektor brojeva treba da sadrži 900.000 slučajno generisanih brojeva.

Za svaku nit, evidentirati trenutke kada je izvršavanje počelo i kada se završilo. 
Na kraju programa, ispisati koliko je trajalo izvršavanje svake niti u milisekundama.
*/

#include <iostream>
#include <thread>
#include <vector>
#include <cmath>

using namespace std;
using namespace chrono;

typedef vector<double>::const_iterator cit;

struct vreme {
    system_clock::time_point pocetak;
    system_clock::time_point kraj;
};

// Implementirati...
void f(cit pocetak, cit kraj, double& min, struct vreme* v) 
{
	v->pocetak = system_clock::now();

	// inicijalno, minimalni je nulti element
	min = *pocetak;

	for (auto it = pocetak + 1; it != kraj; it++)
		if (abs(*it) < abs(min))
			min = *it;

	v->kraj = system_clock::now();
}

const int BROJ_NITI = 3;
const int DUZINA = 900000;

int main() 
{
    int SEGMENT = DUZINA / BROJ_NITI;

	// Postavljamo seme random generatora na trenutno vreme, što dovodi do kvalitetnijeg izbora slučajnih brojeva.
    srand(time(NULL));   

    vector<double> v(DUZINA);
    // Puni se vektor pseudo-slučajnim brojevima
    for (int i = 0; i < DUZINA; i++)
		v[i] = rand();
	
	// Niz elemenata najbližih nuli - svaka programska nit će dati svoj međurezltat
    double minimumi[BROJ_NITI];  
	// Niz struktura u koji će biti upisani podaci o trajanju izvršavanja svake niti
    struct vreme vremena[BROJ_NITI]; 

    thread niti[BROJ_NITI];

	// Implementirati...
	cit pocetak = v.begin();
	cit kraj = v.begin() + SEGMENT;

	for (int i = 0; i < BROJ_NITI; i++)
	{
		niti[i] = thread(f, pocetak, kraj, ref(minimumi[i]), &vremena[i]);
		pocetak += SEGMENT;
		kraj += SEGMENT;
	}

	for (int i = 0; i < BROJ_NITI; i++)
		niti[i].join();

	double rez = minimumi[0];

	for (int i = 1; i < BROJ_NITI; i++)
		if (abs(minimumi[i]) < abs(rez))
			rez = minimumi[i];

	cout << "Najblizi element nuli je: " << rez << endl;

	for (int i = 0; i < BROJ_NITI; i++)
	{
		duration<double, milli> trajanje = vremena[i].kraj - vremena[i].pocetak;
		cout << "Nit " << i+1 << " se izvrsavala " << trajanje.count() << " milisekundi," << endl;
	}
}

