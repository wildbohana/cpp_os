/*
Definisati funkciju: void increment(int& a);
koja povećava (za jedan) vrednost argumenta.

Napraviti program koji:
a) poziva funkciju increment()
b) stvara nit od funkcije increment()
*/

#include <iostream>
#include <thread>

using namespace std;

void inkrement(int& a)
{
	a++;
}

int main()
{
	int a = 0;
	int b = 0;

	inkrement(a);

	// bez ref(b) uopste ne kompajlira !!!
	thread t(inkrement, ref(b));
	t.join();

	cout << "Preko reference: a = " << a << endl;
	cout << "Preko niti:      b = " << b << endl;

    return 0;
}
