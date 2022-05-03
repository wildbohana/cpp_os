/*
Napraviti program koji prihvata proizvoljan broj linija sa standardnog ulaza i zatim ih ispisuje u obrnutom redosledu 
(poslednja prihvaćena linija se ispisuje prva).
*/

#include <iostream>
#include <vector>

using namespace std;

int main()
{
	string linija;
	vector<string> linije;

	cout << "Unesite liniju po liniju (ctrl+d za kraj unosa):" << endl;
	while(getline(cin, linija))
	{
		linije.push_back(linija);
	}

	cout << "Uneti stringovi su: " << endl;

	vector<string>::const_reverse_iterator cri = linije.rbegin();

	while (cri != linije.rend())
	{
		cout << *cri++ << endl;
	}

    return 0;
}
