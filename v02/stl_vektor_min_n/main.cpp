/*
Napraviti funkciju:
	
vector<int> min_n(const vector<int>& v,	int n);

Ova funkcija vraća vektor koji sadrži n najmanjih elemenata iz vektora v.
Podrazumeva se: v.size()>=n
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> min_n(const vector<int>& v,	int n)
{
	vector<int> original = v;
	sort(original.begin(), original.end());
	
	vector<int> ret_val;		// povratni vektor
	ret_val.resize(n);			// alociranje n mesta

	copy(original.begin(), original.begin() + n, ret_val.begin());

	return ret_val;
}

int main()
{
	vector<int> brojevi = {5, 9, 12, 7, 3, 1, 28, 16}; 

	vector<int> najmanji_brojevi = min_n(brojevi, 5);

	vector<int>::iterator it;
	
	for (it = najmanji_brojevi.begin(); it != najmanji_brojevi.end(); it++)
		cout << *it << " ";
	cout << endl;

    return 0;
}
