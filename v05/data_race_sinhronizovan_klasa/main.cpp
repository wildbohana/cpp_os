/*		
Napraviti konkurentni program koji modeluje klasu brojača. Interfejs klase sadrži sledeće metode: 

class Brojac {
   public:
      void inc();
      void dec();
      friend ostream& operator<<(ostream& , Brojac& );
};

Metode inc i dec povećavaću i smanjuju vrednost brojača respektivno. Operator << služi za ispis brojača na ekran.

Klasa treba da zaštiti konzistentnost brojača u datim metodama uz pomoć klasa mutex i unique_lock. 

Kreirati 1 globalni objekat brojača kome će pristupati 2 niti. 

Kreirati 2 niti pri čemu jedna nit poziva metodu uvećavanja brojača 1000000 puta a druga metodu smanjivanja brojača 1000000 puta. Na kraju programa ispisati konačnu vrednost brojača nakon uvećavanja i smanjivanja.
*/

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

const int ITERATIONS = 100000000;

// NE ZABORAVI <MUTEX> KOD unique_lock<mutex> 

class Brojac {
	private:
		mutex m;
		int broj;
	public:
		Brojac() : broj(0) {}

		void inc() 
		{
			unique_lock<mutex> l(m);
			broj++;
		}

		void dec()
		{
			unique_lock<mutex> l(m);
			broj--;
		}

		friend ostream& operator<<(ostream& os, Brojac& b)
		{
			// OBRATI PAZNJU: ne mozes samo m, nego moras b.m
			unique_lock<mutex> l(b.m);
			os << b.broj << endl;
			return os;
		}
};

Brojac br;

void inkrement()
{
	for (int i = 0; i < ITERATIONS; i++)
		br.inc();
}

void dekrement()
{
	for (int i = 0; i < ITERATIONS; i++)
		br.dec();
}

int main()
{
	thread t1(inkrement);
	thread t2(dekrement);

	t1.join();
	t2.join();

	cout << br;

	return 0;
}

