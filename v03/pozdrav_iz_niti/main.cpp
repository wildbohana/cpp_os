/* 
Napraviti program koji kreira jednu nit i u okviru niti ispisuje proizvoljnu rečenicu.
*/

#include <iostream>
#include <thread>

using namespace std;

void ispis()
{
	cout << "Pozdrav iz niti!" << endl;
}

int main()
{
	thread t(ispis);
	t.join();

    return 0;
}
