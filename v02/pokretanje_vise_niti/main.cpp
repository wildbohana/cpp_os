/*
Napraviti konkurentni program koji pokrece vise niti u petlji.
Svakoj niti proslediti njen redni broj prilikom kreiranja i 
svaka nit treba da ispise sopstveni redni broj u okviru tela niti.
*/

#include <iostream>
#include <thread>

using namespace std;

void f(int x) {
    // Niti mogu jedna drugu da prekinu u pola ispisa
    cout << "Nit: " << x << endl;
}

const int br_niti = 5;

int main()
{

    /*
    // nije konkurentno
    for(int i = 0; i < br_niti; i++) {
        thread(f, i+1).join();
    }
    */

	// Kreiranje niza niti
    thread t[br_niti];

	// Inicijalizacija svih nizi
    for(int i = 0; i < br_niti; i++) {
        t[i] = thread(f, i+1);
    }

	// Cekanje da se sve niti izvrse
    for(int i = 0; i < br_niti; i++) {
        t[i].join();
    }

    return 0;
}
