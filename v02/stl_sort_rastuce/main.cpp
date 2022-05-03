/*
Uneti 5 celih brojeva sa standardnog ulaza.
Korišćenjem STL algoritma sortirati brojeve u rastućem redosledu.
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
	vector<int> brojevi;
	int temp;

	cout << "Unesite 5 celih brojeva:" << endl;

	for (int i = 0; i < 5; i++)
	{
		cin >> temp;
		brojevi.push_back(temp);
	}

	sort(brojevi.begin(), brojevi.end());

	cout << "Sortirani brojevi:" << endl;

	vector<int>::const_iterator ci;

	for (ci = brojevi.begin(); ci != brojevi.end(); ci++)
	{
		cout << *ci << " ";
	}
	cout << endl;
	
    return 0;
}
