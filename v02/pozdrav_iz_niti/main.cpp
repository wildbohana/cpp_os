/*
Napraviti program koji kreira jednu nit i 
u okviru niti ispisuje proizvoljnu recenicu.
*/

#include <iostream>
#include <thread>

using namespace std;

void pozdrav() {
    cout << "Pozdrav iz niti!" << endl;
}

int main()
{
	// join moze i odmah da se pozove
    thread(pozdrav).join();

    return 0;
}
