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

void increment(int &a) {
    a++;
}

int main()
{
    int a = 3;
    cout << a << endl;

    increment(a);
    cout << a << endl;

    /*
    // ovo u komentaru neće raditi jer će se umesto adrese od a funkciji poslati vrednost direktno (tj. 3)
    thread t(increment, a);
    t.join();
    cout << a << endl;
    */

    thread t(increment, ref(a));
    t.join();
    cout << a << endl;

    return 0;
}
