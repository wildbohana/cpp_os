/*
Napisati konkurentni program koji stvara 2 niti od funkcije f. Funkcija f treba da prolazi kroz petlju dužine 10000 elemenata i ispisuje parne ili neparne brojeve (među tih 10000 elemenata).

Ispis parnih ili neparnih brojeva se vrši pozivom metoda ispisi_parne i ispisi_neparne. U metodi ispisi_parne prvo se pre ispisa zaključava muteks m1, a potom muteks m2. U metodi ispisi_neparne prvo se pre ispisa zaključava muteks m2, a potom muteks m1.

Sva zaključavanja se vrše kroz objekte klase unique_lock.

Pratiti izvršavanje programa.
*/

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

const int SIZE = 10000;

mutex m1, m2;

void ispisi_parne(int i)
{
	// ukrsteno zakljucavanje mutex-a -> potencijalni deadlock
	unique_lock<mutex> l1(m1);
	unique_lock<mutex> l2(m2);

	cout << " " << i << " ";
}

void ispisi_neparne(int i)
{
	unique_lock<mutex> l2(m2);
	unique_lock<mutex> l1(m1);

	cout << " " << i << " ";
}

void f()
{
	for (int i = 0; i < SIZE; i++)
	{
		if (i%2 == 0)
			ispisi_parne(i);
		else
			ispisi_neparne(i);
	}
}

int main()
{
	thread t1(f);
	thread t2(f);

	t1.join();
	t2.join();

	return 0;
}
