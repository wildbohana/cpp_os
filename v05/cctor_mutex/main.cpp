/*
Data je klasa Brojac.
U main funkciji napraviti objekat klase Brojac. Ispis vrednosti Brojaca radi posebna nit.
Napraviti nit koja vrši ispis i proslediti joj Brojac.
*/

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

class Brojac 
{
	private:
		mutex m; 		//propusnica
		int broj; 		//vrednost Brojaca
	public:
		Brojac() : broj(0) {} 

		void inc() { 
			unique_lock<mutex> l(m); 
			++broj; 
		} 

		void dec() 
		{ 
			unique_lock<mutex> l(m); 
			--broj; 
		}

		friend ostream& operator<<(ostream& os, Brojac& b) 
		{ 
			unique_lock<mutex> l(b.m);
			os << b.broj << endl;
			return os;
		}
};

void ispis(Brojac& b) 
{
    cout << b;
}

int main() 
{
	Brojac b;

	b.inc();
	
	// greska jer se ne moze kopirati mutex ili objekat koji sadrzi mutex
	// error: redeclaration of ‘std::thread t’
	// thread t(ispis, b); 

	// moras poslati po referenci (referenca ne kopira objekat)
	thread t(ispis, ref(b));
	t.join();
}
