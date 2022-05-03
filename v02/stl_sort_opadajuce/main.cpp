/*
Uneti 5 celih brojeva sa standardnog ulaza.
Korišćenjem STL algoritma sortirati brojeve u opadajućem redosledu.
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

bool poredi(int a, int b) 
{ 
	return a > b; 
}

int main()
{
	vector<int> brojevi;
	int temp;

	cout << "Unesite 5 celih brojeva: " << endl;
	
	for (int i = 0; i < 5; i++)
	{
		cin >> temp;
		brojevi.push_back(temp);
	}

	sort(brojevi.begin(), brojevi.end(), poredi);

	cout << "Sortirani brojevi:" << endl;

	vector<int>::const_iterator ci;

	for (ci = brojevi.begin(); ci != brojevi.end(); ci++)
		cout << *ci << endl;

    return 0;
}
