/*
Napraviti konkurentni program koji stvara nit iz koje pita korisnika za visinu u centimetrima i potom ispisuje: “Vasa visina je <uneta_visina> cm.”

Testirati program tako što će main nit, koristeći objekat koji predstavlja novu nit, da pozove metodu join. Potom promeniti taj poziv u poziv metode detach.
*/

#include <iostream>
#include <thread>

using namespace std;

void f() {
    int x;
    cout << "Unesite visinu: ";
    cin >> x;
    cout << "Vasa visina je: " << x << "." << endl;
}

int main()
{
    /*
    // ovo je greška jer će napraviti dve niti koje će jedna drugoj smetati
    thread(f).detach();     // umre odmah
    thread(f).join();       // radi

    // ovo je manja greška ali opet je greška
    thread(f).join();
    thread(f).detach();
    */

    // ovo je najpravilnije
    thread t(f);
    t.join();

    return 0;
}
