#ifndef KOORDINATOR_H_INCLUDED
#define KOORDINATOR_H_INCLUDED

using namespace std;

#include <condition_variable>

class Koordinator 
{
	private:
		// Konstanta koja govori koliko niti se čeka na završetak pre nego što se aktivira poslednja.
		const int cekam_niti;
		int zavrseno_niti;
		mutex m;
		condition_variable cv;
	public:
		// Proširiti po potrebi ... -> dodato zavrseno_niti(0)
		Koordinator(int cn) : cekam_niti(cn), zavrseno_niti(0) {}

		// Metoda koju na svom KRAJU zovu niti (a i b) koje ne čekaju (izvršavaju se odmah).
		// Implementirati ...
		void zavrsio() 
		{
			unique_lock<mutex> lock(m);

			if (++zavrseno_niti == cekam_niti)
				cv.notify_all();
		}

		// Metodu koju na svom POČETKU zovu niti (c) koje čekaju (ne izvršavaju se odmah).
		// Implementirati ...
		void cekam() 
		{
			unique_lock<mutex> lock(m);

			while (!(zavrseno_niti == cekam_niti)) 
				cv.wait(lock);		// MORA WHILE dok se ceka ispunjenje uslova
		}
};

#endif // KOORDINATOR_H_INCLUDED
