/*
Napraviti funkciju:
	
vector<int> min_n(const vector<int>& v,	int n);

Ova funkcija vraca vektor koji sadrzi n najmanjih elemenata iz vektora v.
Podrazumeva se: v.size()>=n
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> min_n(const vector<int> &v, int n) {
	// Pravimo pomocni vektor (kopija od v)
   	vector<int> temp = v;

	// Sortiramo pomocni vektor)
    	sort(temp.begin(), temp.end());

	// Pravimo novi vektor
    	vector<int> pov;

	// U taj vektor ubacujemo n najmanjih elemenata iz sortiranog niza
    	for(int i = 0; i < n; i++) {
        	pov.push_back(temp[i]);
	}

    	return pov;
}

int main() {
    	int niz[] = {188, 12, 1, 6, 22, 14};

	// n = sizeof(niz) / sizeof(int)
	// pocetak je niz, kraj je niz+n
    	vector<int> a(niz, niz + sizeof(niz) / sizeof(int));

    	vector<int> b = min_n(a, 3);
	
    	for(auto it : b) {
        	cout << it << endl;
	}

	/*
	// Ispis rezultata - drugi nacin
	cout << "Rezultujuci vektor: ";
	for (int i = 0; i < m; i++) {
        	cout << w[i] << " ";
	}
	cout << endl;
	*/
    
	return 0;
}
