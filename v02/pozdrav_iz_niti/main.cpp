/*
Napraviti program koji kreira jednu nit i u okviru niti ispisuje proizvoljnu rečenicu.
*/

#include <iostream>
#include <thread>

using namespace std;

void f() {
    cout << "Hello world!" << endl;
}

int main()
{
    thread(f).join();

    return 0;
}
