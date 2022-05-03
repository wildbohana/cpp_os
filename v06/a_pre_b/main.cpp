/* 
Napraviti konkurentni program sa dve niti.
Nit a ispisuje: Ja sam nit a.
Nit b ispisuje: Ja sam nit b.
Obezbediti da se uvek izvrši prvo nit a.

Implementirati označene metode u klasi Koordinator u fajlu koordinator.h
*/

#include <iostream>
#include <thread>

#include "koordinator.h"

using namespace std;

// Globalni objekat Koordinatora (kako bi mu pristupale obe niti).
Koordinator k;

// Telo niti a.
void a() 
{
    cout << "Ja sam nit a." << endl;
    k.prvi_zavrsio();
}

// Telo niti b.
void b() 
{
    k.drugi_ceka();
    cout << "Ja sam nit b." << endl;
}

void testirajSve() 
{
    thread ta(a);
    thread tb(b);

    ta.join();
    tb.join();
}

int main() 
{
    testirajSve();

    return 0;
}
