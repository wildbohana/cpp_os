/*
Napraviti program koji prihvata proizvoljan broj linija sa standardnog ulaza i zatim ih ispisuje u redosledu kojim su uneti.
*/

#include <iostream>
#include <vector>

using namespace std;

int main()
{
	string linija;
	vector<string> linije;

	cout << "Unesite linije (ctrl + d za kraj unosa):" << endl;

	while (getline(cin, linija))
	{
		linije.push_back(linija);
	}	

	cout << endl << "Uneli ste sledece linije:" << endl;

	vector<string>::const_iterator ci = linije.begin();

	while (ci != linije.end())
		cout << *ci++ << endl;

    return 0;
}
