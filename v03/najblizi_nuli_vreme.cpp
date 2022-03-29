/*
Napraviti konkurentni program koji pronalazi element najblizi broju 
0 u zadatom vektoru brojeva.

Pretragu podeliti u 3 programske niti. Ulazni vektor brojeva treba da sadrzi 
900.000 slučajno generisanih brojeva.

Za svaku nit, evidentirati trenutke kada je izvrsavanje pocelo i kada se zavrsilo.
Na kraju programa, ispisati koliko je trajalo izvrsavanje svake niti u milisekundama.
*/

#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

using namespace std;
using namespace chrono;

typedef vector<double>::const_iterator cit;

// Oke ovo je zanimljivo
struct vreme {
steady_clock::time_point pocetak;
steady_clock::time_point kraj;
};

// Funkcija koja trazi minimum
void f(cit pocetak, cit kraj, double &min, struct vreme *v) {
// Uzimanje vremena pre pocetka izvrsavanja
v->pocetak = steady_clock::now();

// Trazenje minimuma
min = abs(*pocetak);
for(; pocetak != kraj; pocetak++) {
if(abs(*pocetak) < abs(min)) {
    min = *pocetak;
}
}

// Uzimanje vremena nakon završetka izvršavanja
v->kraj = steady_clock::now();
}

const int BROJ_NITI = 3;
const int DUZINA = 900000;

int main() {
int SEGMENT = DUZINA / BROJ_NITI;

// Postavljamo seme random generatora na trenutno vreme, sto dovodi do kvalitetnijeg izbora slucajnih brojeva
srand(time(NULL));  

// Inicijalizujemo vektor i punimo ga pseudo-slucajnim brojevima
vector<double> v(DUZINA);
for (int i = 0; i < DUZINA; i++) {
v[i] = rand();
}

// Niz elemenata najblizih nuli - svaka programska nit ce dati svoj medjurezultat
double minimumi[BROJ_NITI];  
// Niz struktura u koji ce biti upisani podaci o trajanju izvrsavanja svake niti
struct vreme vremena[BROJ_NITI]; 

// Potrebne niti
thread niti[BROJ_NITI];

// Pokretanje niti
for(int i = 0; i < BROJ_NITI; i++) {
niti[i] = thread(f, v.cbegin(), v.cbegin() + (i + 1) * SEGMENT, ref(minimumi[i]), &vremena[i]);
}

// join i ispis niti
for(int i = 0; i < BROJ_NITI; i++) {
niti[i].join();

cout << "Nit[" << i << "]" << endl;
duration<double, milli> dms = vremena[i].kraj - vremena[i].pocetak;

cout << "Trajanje: " << dms.count()  << "ms" << endl;
cout << "Min[" << i << "] = " << minimumi[i] << endl << endl;
}

// Pronalazenje vremena najblizeg nuli
cout << endl << "Najmanji: " << *(min_element(minimumi, minimumi + 3)) << endl;

return 0;
}
