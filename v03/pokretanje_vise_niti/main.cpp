/*
Napraviti konkurentni program koji pokreće više niti u petlji. 
Svakoj niti proslediti njen redni broj prilikom kreiranja 
i svaka nit treba da ispiše sopstveni redni broj u okviru tela niti.
*/

#include <iostream>
#include <thread>

using namespace std;

void f(int rbr)
{
	cout << rbr;
}

const int br_niti = 7;

int main()
{
	thread t[br_niti];

	for (int i = 0; i < br_niti; i++)
		t[i] = thread(f, i);
	for (int i = 0; i < br_niti; i++)
		t[i].join();

	cout << endl;

    return 0;
}
