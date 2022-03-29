/* 
Napraviti funkciju:

vector<int> min(const vector<int>& a, const vector<int>& b);

Ova funkcija vraca vektor koji sadrzi minimume korespodentnih elemenata vetora a i b.
Npr: r[0]=min(a[0],b[0]), r[1]=...

Podrazumeva se: a.size()==b.size()
*/

#include <iostream>
#include <vector>

using namespace std;

// Funkcija koja vraca vektor koji sadrzi minimume korespodentnih elemenata
vector<int> min(const vector<int> &a, const vector<int> &b)
{
    vector<int> c(a.size());

    auto it_a = a.cbegin(), it_b = b.cbegin();
    auto it_c = c.begin();

    while (it_a != a.cend()) {
        *it_c++ = std::min(*it_a++, *it_b++);
	}

    return c;
}

int main()
{
    	vector<int> a = {1, 2, 3, 4, 5};
    	vector<int> b = {4, 3, 7, 3, 2};

    	vector<int> c = min(a, b);

	// Ispis rezultata
    	cout << "\nRezultat:";
    	for (int i = 0; i < n; i++) {
        	cout << c[i] << " ";
    	}
    	cout << endl;

	/*
	cout << "Ispis rezultata - drugi nacin: " << endl;
    	for (int x : c) {
		cout << x << endl;
	}
	*/

    	return 0;
}
