/* 
Napraviti konkurentni program sa tri niti.
Nit a ispisuje: Ja sam nit a.
Nit b ispisuje: Ja sam nit b.
Nit c ispisuje: Ja sam nit c.

Obezbediti da se niti a i b, uvek izvrše pre niti c.

Implementirati označene metode u klasi Koordinator u fajlu koordinator.h
*/

#include <iostream>
#include <thread>
#include <mutex>

#include "koordinator.h"

using namespace std;

// Globalni mutex terminala da bi se sprečila štetna preplitanja na terminalu.
mutex term_mx;                          

// Nit a. Prosleđuje se referenca na Koordinator, kako bi sve niti radile na istom k.
void a(Koordinator& k) 
{
    {
        unique_lock<mutex> lock(term_mx);
        cout << "Ja sam nit a." << endl;
    }

    k.zavrsio();
}

// Nit b. Prosleđuje se referenca na Koordinator, kako bi sve niti radile na istom k.
void b(Koordinator& k) 
{
    {
        unique_lock<mutex> lock(term_mx);
        cout << "Ja sam nit b." << endl;
    }

    k.zavrsio();
}

// Nit c. Prosleđuje se referenca na Koordinator, kako bi sve niti radile na istom k.
void c(Koordinator& k) {
    k.cekam();

    unique_lock<mutex> lock(term_mx);
    cout << "Ja sam nit c." << endl;
}

int main() 
{
	// Kreiranje objekta Koordinatora koji se prosleđuje nitima po referenci.
    Koordinator k(2);

    thread ta(a, ref(k));
    thread tb(b, ref(k));
    thread tc(c, ref(k));

    ta.join();
    tb.join();
    tc.join();

	return 0;
}
