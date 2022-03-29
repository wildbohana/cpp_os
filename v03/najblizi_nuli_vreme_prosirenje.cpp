/*
Proširiti zadatak najbilzi_nuli_vreme ispisom u sekundama i minutima.
*/

#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

using namespace std;
using namespace chrono;

typedef vector<double>::const_iterator cit;

struct vreme {
    steady_clock::time_point pocetak;
    steady_clock::time_point kraj;
};

void f(cit pocetak, cit kraj, double &min, struct vreme * v) {
	v -> pocetak = steady_clock::now();

	min = abs(*pocetak);
	for(; pocetak != kraj; pocetak++)
        if(abs(*pocetak) < abs(min))
            min = *pocetak;

	v -> kraj = steady_clock::now();

}

const int BROJ_NITI = 3;
const int DUZINA = 900000;

int main() {
    int SEGMENT = DUZINA / BROJ_NITI;
	
    srand(time(NULL));

    vector<double> v(DUZINA);
    for (int i = 0; i < DUZINA; i++) {
        v[i] = rand();
	}
	
    double minimumi[BROJ_NITI];  
    struct vreme vremena[BROJ_NITI]; 
	
    thread niti[BROJ_NITI];

    for(int i = 0; i < BROJ_NITI; i++) {
        niti[i] = thread(f, v.cbegin(), v.cbegin() + (i + 1) * SEGMENT, ref(minimumi[i]), &vremena[i]);
	}

    for(int i = 0; i < BROJ_NITI; i++) {
        niti[i].join();

        cout << "Nit[" << i << "]" << endl;

	// Dodato
        duration<double, milli> dms = vremena[i].kraj - vremena[i].pocetak;
        duration<double> ds = dms;
        duration<double, ratio<60>> dm = dms;

        cout << "Trajanje: " << dms.count()  << " ms" << endl;
        cout << "Trajanje: " << ds.count()  << " s" << endl;
        cout << "Trajanje: " << dm.count()  << " min" << endl;
        cout << "Min[" << i << "] = " << minimumi[i] << endl << endl;
    }

    cout << endl << "Najmanji: " << *(min_element(minimumi, minimumi + 3)) << endl;

    return 0;
}
