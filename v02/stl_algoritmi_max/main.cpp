/*
Korisnik treba da unese 5 celih brojeva koristeći tastaturu (standardni ulaz).
Korišćenjem STL algoritma, među tih 5 brojeva potrebno je pronaći najveći.
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
	vector<int> niz;
	int x;

	cout << "Unesite 5 celih brojeva: " << endl;
	for (int i = 0; i < 5; i++)
	{
		cin >> x;
		niz.push_back(x);
	}

	int najveci = *max_element(niz.begin(), niz.end());

	cout << "Najveci element je: " << najveci << endl;	

    return 0;
}
