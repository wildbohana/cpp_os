/*
Napraviti konkurentni program koji stvara nit iz koje pita korisnika za visinu u 
centimetrima i potom ispisuje: “Vasa visina je <uneta_visina> cm.”

Testirati program tako sto ce main nit, koristeci objekat koji predstavlja novu nit,
da pozove metodu join. Potom promeniti taj poziv u poziv metode detach.
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
    // Ovo je greska jer ce napraviti dve niti koje ce jedna drugoj smetati
    thread(f).detach();     // zavrsava program, ali ostavlja nit da radi
    thread(f).join();       // ceka da se niz zavrsi

    // Ovo je manja greska, ali je opet greska
    thread(f).join();
    thread(f).detach();
    */

    // Ovo je najpravilnije
    thread t(f);
    t.join();

    return 0;
}
