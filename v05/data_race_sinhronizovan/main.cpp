/*
Izmeniti program data_race i realizovati zaštitu pristupa brojaču 
uz pomoć klase mutex i njenih operacija lock() i unlock(). 
Primetiti koliko sada traje izvršavanje programa.
*/

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

const int ITERATIONS = 100000000;

int brojac = 0;
mutex m;

void inkrement()
{
	for (int i = 0; i < ITERATIONS; i++)
	{
		m.lock();
		brojac++;
		m.unlock();
	}
}

void dekrement()
{
	for (int i = 0; i < ITERATIONS; i++)
	{
		m.lock();
		brojac--;
		m.unlock();
	}
}

int main()
{
	thread t1(inkrement);
	thread t2(dekrement);

	t1.join();
	t2.join();
	
	// Traje Boga oca, ali se dobije 0 na kraju
	cout << brojac << endl;

	return 0;
}
