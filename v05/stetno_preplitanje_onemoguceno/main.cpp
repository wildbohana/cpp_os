/*
Napraviti konkurentni program koji u funkciji visina pita korisnika koliko je visok. 
Nakon toga korisnik unosi svoju visinu. Na kraju u funkciji se ispisuje uneta visina.

Sprečiti štetno preplitanje na terminalu korišćenjem klase mutex.

Kreirati 2 niti od date funkcije. Ispratiti ispis.
*/

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

mutex m;

void visina()
{
	int v;

	m.lock();

	cout << "Koliko ste visoki [cm]?" << endl;
	cin >> v;

	cout << "Vasa visina je " << v << " cm." << endl;
	cout << endl;

	m.unlock();
}

int main()
{
	thread t1(visina);
	thread t2(visina);

	t1.join();
	t2.join();

	return 0;
}
