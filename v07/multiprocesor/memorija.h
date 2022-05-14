#ifndef MEMORIJA_H_INCLUDED
#define MEMORIJA_H_INCLUDED

#include <mutex>
#include <condition_variable>

#include "dijagnostika.h"

using namespace std;
using namespace chrono;

enum Stanje {SLOBODAN, CITANJE, UPIS };

class Memorija 
{
	private:
		Dijagnostika& dijagnostika;
		mutex m;
		vector<char> mem;

		const int VELICINA_RECI = 4;

		struct Pristup
		{
			Stanje stanje;
			int aktivnih_citaca;
			int citaca_ceka;
			condition_variable citanje;
			condition_variable upis;
			Pristup(): stanje(SLOBODAN), aktivnih_citaca(0), citaca_ceka(0) {}
		};

		vector<Pristup*> pristupi;

	public:
		// Proširiti po potrebi ...
		Memorija(Dijagnostika& d, int bajtova) : dijagnostika(d), mem(bajtova) 
		{
			int reci = (bajtova + (VELICINA_RECI - 1)) / VELICINA_RECI;
			for (int i = 0; i < reci; i++)
			{
				pristupi.push_back(new Pristup);
			}
		}

		~Memorija()
		{
			for (auto it = pristupi.begin(); it != pristupi.end(); it++)
				delete *it;
		}

		/*
		Metoda koju poziva nit koja simulira proces koji pristupa memoriji kako bi obavila čitanje iz nje
		
		rbp    - Redni broj procesa
		adresa - Lokacija sa koje se čita vrednost prethodno upisana u memoriju
		
		Potrebno je pozvati dijagnostika.proces_ceka_citanje kada je memorijska reč zaključana i proces mora da čeka.
		Potrebno je pozvati dijagnostika.proces_procitao onda kada je ostvaren pristup memoriji i kada je vrednost učitana iz nje.
		*/
		
		// Implementirati ...
		char citaj(int rbp, int adresa) 
		{
			unique_lock<mutex> l(m);
			int rec = adresa / VELICINA_RECI;
			
			while (pristupi[rec]->stanje == UPIS) 
			{
				(pristupi[rec]->citaca_ceka)++;
				dijagnostika.proces_ceka_citanje(rbp, adresa);
				pristupi[rec]->citanje.wait(l);
			}
			
			(pristupi[rec]->citaca_ceka)--;
			
			(pristupi[rec]->aktivnih_citaca)++;
			pristupi[rec]->stanje = CITANJE; 

			l.unlock();
			this_thread::sleep_for(seconds(1));
			l.lock();

			(pristupi[rec]->aktivnih_citaca)--;
			if ((pristupi[rec]->aktivnih_citaca) == 0) 
			{ 
				pristupi[rec]->upis.notify_one(); 
				pristupi[rec]->stanje = SLOBODAN; 
			}

			dijagnostika.proces_procitao(rbp, adresa, mem[adresa]);

			return mem[adresa];
		}

		/*
		Metoda koju poziva nit koja simulira proces koji pristupa memoriji kako bi obavila upis u nju
		
		rbp      - Redni broj procesa
		adresa   - Lokacija u koju se upisuje nova vrednost
		vrednost - Vrednost koja se upisuje u memoriju
		
		Potrebno je pozvati dijagnostika.proces_ceka_upis kada je memorijska reč zaključana i proces mora da čeka.
		Potrebno je pozvati dijagnostika.proces_upisao onda kada je ostvaren pristup memoriji i kada je vrednost upisana u nju.
		*/

		// Implementirati ...
		void pisi(int rbp, int adresa, char vrednost) 
		{
			unique_lock<mutex> l(m);
			
			int rec = adresa / VELICINA_RECI;
			
			while (pristupi[rec]->stanje != SLOBODAN) 
			{
				dijagnostika.proces_ceka_upis(rbp, adresa);
				pristupi[rec]->upis.wait(l);
			}

			pristupi[rec]->stanje = UPIS;

			l.unlock();
			this_thread::sleep_for(seconds(1));
			l.lock();

			pristupi[rec]->stanje = SLOBODAN; 
			mem[adresa] = vrednost;

			dijagnostika.proces_upisao(rbp, adresa, vrednost);

			if (pristupi[rec]->citaca_ceka != 0) 
				pristupi[rec]->citanje.notify_all(); 
			else
				pristupi[rec]->upis.notify_one();
		}
};

#endif // MEMORIJA_H_INCLUDED
