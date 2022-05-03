/*
Napraviti konkurentni program u kom se u funkciji niti pravi veliki niz 
od 1.000.000.000.000 elemenata. Niz se pravi dinamički. 
Kreiranje niza zaštititi try - catch blokom. 
U okviru try catch bloka zaključati mutex pre pravljenja niza i otključati ga nakon pravljenja niza. 

Posmatrati ponašanje programa.

Nakon toga promeniti kod tako da se ne zaključava mutex eksplicitno, već da se koristi klasa unique_lock.
*/

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

mutex m;

void f()
{
	int* veliki_niz;

	try 
	{
		// unique_lock<mutex> l(m);
		m.lock();
		veliki_niz = new int[100000000000000];
		m.unlock();
	}
	catch (const bad_alloc& e)
	{
		cout << "Alokacija memorije neuspesna! " << e.what() << endl;
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
