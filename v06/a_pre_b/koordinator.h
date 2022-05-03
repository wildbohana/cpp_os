#ifndef KOORDINATOR_H_INCLUDED
#define KOORDINATOR_H_INCLUDED

using namespace std;

#include <condition_variable>

class Koordinator 
{
	// Dodat ceo private deo
	private:
		enum redosled {PRVI, DRUGI};
		redosled na_redu_je;
		mutex m;
		condition_variable cv;
	public:
		// Dodat konstruktor za klasu Koordinator
		Koordinator() : na_redu_je(PRVI) {}

		// Funkcija se poziva nakon što je prva nit završila svoj posao, kako bi signalizirala pokretanje druge niti.
		// Implementirati ...
		void prvi_zavrsio() 
		{
			unique_lock<mutex> l(m);
			cv.notify_one();
			na_redu_je = DRUGI;
		}

		// Blokirajuća funkcija koja se poziva na početku druge niti kako bi nit čekala dok se prva nit ne završi.
		// Implementirati ...
		void drugi_ceka()
		{
			unique_lock<mutex> l(m);
			
			while(!(na_redu_je == DRUGI))
				cv.wait(l);
		}
	
};

// Kod provere uslova MORA stojati while (ne if) da bi se predupredila lazna budjenja (spurious wakeups)

#endif // KOORDINATOR_H_INCLUDED
