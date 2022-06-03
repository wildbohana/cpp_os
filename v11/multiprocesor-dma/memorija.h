#ifndef MEMORIJA_H_INCLUDED
#define MEMORIJA_H_INCLUDED

#include <vector>
#include <mutex>

using namespace std;

class Memorija {
	// Dodato:
	private:
		// Vektor memorijskih lokacija. Svaka lokacija jedan char.
		vector<char> mem;
		mutex m;

	public:
		// Implementirati ...
		Memorija(int bajtova) 
		{
	        mem.resize(bajtova, 0);
		}

		// Implementirati ...
		char citaj(int adresa) 
		{
			unique_lock<mutex> l(m);
	        this_thread::sleep_for(chrono::milliseconds(300));
    	    return mem[adresa];
		}

		// Implementirati ...
		void pisi(int adresa, char vrednost) 
		{
			// Radi očuvanja konzistentnosti memorijske lokacije. Obratiti pažnju na razliku u odnosu na rešenje u zadatku "multiprocesor"
			// Ovde nema provere stanja, brojača... Sva sinhronizacija se vrši na nivou Magistrala-a (magistrale tj. sabirnice).
			unique_lock<mutex> l(m);
			this_thread::sleep_for(chrono::milliseconds(300));
			mem[adresa] = vrednost;
		}
};

#endif // MEMORIJA_H_INCLUDED
