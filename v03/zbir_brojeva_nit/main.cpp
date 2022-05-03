/*
Napraviti program koji kreira jednu nit kojoj se prosleđuju dva cela broja a i b. 
U okviru niti sabrati brojeve i ispisati na ekran njihov zbir.
*/

#include <iostream>
#include <thread>

using namespace std;

void zbir(const int a, const int b)
{
	cout << "a+b = " << a + b << endl;
}

int main()
{
	thread t(zbir, 3, 5);

	t.join();

    return 0;
}
