/*
Napraviti konkurentni program koji pokreće više niti u petlji.
Svakoj niti proslediti njen redni broj prilikom kreiranja i svaka nit treba da ispiše sopstveni redni broj u okviru tela niti.
*/

#include <iostream>
#include <thread>

using namespace std;

void f(int x) {
    // niti mogu jedna drugu da prekinu u pola ispisa
    cout << "Nit: " << x << endl;
}

int main()
{
    const int br_niti = 5;

    /*
    // nije konkurentno
    for(int i = 0; i < br_niti; i++) {
        thread(f, i+1).join();
    }
    */

    thread t[br_niti];

    for(int i = 0; i < br_niti; i++) {
        t[i] = thread(f, i+1);
    }

    for(int i = 0; i < br_niti; i++) {
        t[i].join();
    }

    return 0;
}
