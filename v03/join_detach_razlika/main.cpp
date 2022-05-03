/*
Napraviti konkurentni program koji stvara nit iz koje pita korisnika za visinu u centimetrima i potom ispisuje: “Vasa visina je <uneta_visina> cm.”

Testirati program tako što će main nit, koristeći objekat koji predstavlja novu nit, da pozove metodu join. Potom promeniti taj poziv u poziv metode detach.
*/

#include <iostream>
#include <thread>

using namespace std;

void ispis()
{
	int visina;
	cout << "Unesite vasu visinu u cm:" << endl;
	cin >> visina;
	cout << "Vasa visina je: " << visina << " cm." << endl;

}

int main()
{
	thread t(ispis);

	//t.detach();	// ispis() se nece ni izvrsiti
	t.join();		// izvrsice se ispis()

    return 0;
}



