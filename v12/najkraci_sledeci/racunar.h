#ifndef RACUNAR_H_INCLUDED
#define RACUNAR_H_INCLUDED

#include "dijagnostika.h"

#include <mutex>
#include <vector>
#include <condition_variable>

using namespace std;
using namespace chrono;

//Struktura vezana za jedan proces. Sadrži običnu CV, trajanje procesa i id procesa:
struct Proces 
{
    condition_variable cv;
    int trajanje;
    int id;
    Proces(int t, int i) : trajanje(t), id(i) {}
};

class Racunar 
{
	private:
		Dijagnostika& dijagnostika;
		vector<Proces*> procesi;		// Vektor pokazivača na strukture Proces. Mora dinamički jer CV ne može da se kopira.
		int aktivan_proces;				// Broj aktivnog procesa
		mutex m;
		bool rasporedjivac_aktivan;		// Flag za aktiviranje raspoređivača
		condition_variable r_cv;		// CV na kojoj raspoređivač čeka
		bool kraj;						// Flag za kraj izvršavanja programa

	public:
		// Proširiti po potrebi ...
		Racunar(Dijagnostika& d) : dijagnostika(d) 
		{
			rasporedjivac_aktivan = false;
			aktivan_proces = -1;
			kraj = false;
		}

		Dijagnostika& getDijagnostika() 
		{
			return dijagnostika;
		}

		/*
		Metoda koju poziva nit koja simulira izvršenje procesa, kako bi se proces izvršio
		
		id_procesa   - ID procesa
		trajanje     - trajanje procesa (izraženo u stotinama milisekundi)
		
		Ukoliko je procesor već zauzet i ne mogu se izvršavati naredbe procesa, potrebno je 
		pozvati metodu dijagnostika.proces_ceka a nakon toga proces treba da pređe u stanje čekanja.
		Nakon što se proces izvršio, potrebno je pozvati dijagnostika.proces_zavrsio.
		*/
	
		// Implementirati ...
		void izvrsi(int id_procesa, int trajanje) 
		{
			// Čim krene da se izvršava, proces stvara strukturu Proces.
			Proces * p = new Proces(trajanje, id_procesa);  

			unique_lock<mutex> l(m);
			
			// Data struktura se pod lock-om dodaje u vektor ovih struktura.
			procesi.push_back(p);						   

			// Ako je procesor slobodan, zauzima se bezuslovno:
			if (aktivan_proces == -1)
				aktivan_proces = id_procesa;

			// Ako neki proces drži procesor, čeka se na CV u okviru strukture na koju pokazuje p.
			while (aktivan_proces != id_procesa) 
			{
				dijagnostika.proces_ceka(id_procesa);
				p->cv.wait(l);  // Treba imati u vidu da se pokazivač p nalazi takođe u vektoru procesi.
			}

			// Simulacija izvršenja procesa
			l.unlock();
			this_thread::sleep_for(milliseconds(100 * trajanje));   
			l.lock();

			dijagnostika.proces_zavrsio(id_procesa);
			
			// Nakon izvršavanja proces se briše iz vektora jer je završen
			for (auto it = procesi.begin(); it != procesi.end(); it++) 
			{
				if (*it == p) 
				{
					// Prvo erase, pa delete (it pokazuje na kopiju p koja doduše pokazuje na istu adresu kao i p)
					// tek nakon toga se briše memorija na koju pokazuje p
					procesi.erase(it);  
					delete p;			
					break;
				}
			}

			// Diže se flag za aktivaciju raspoređivača i šalje se signal raspoređivaču
			rasporedjivac_aktivan = true;
			r_cv.notify_one();
		}

		/*
		Metoda koju poziva nit koja simulira raspoređivač operativnog sistema, 
		kako bi se izabrao sledeći proces za izvršenje.
		
		Ukoliko još uvek ne postoji potreba da se raspoređivač aktivira, potrebno je pozvati 
		metodu dijagnostika.rasporedjivac_ceka a nakon toga nit treba da pređe u stanje čekanja.
		Nakon što se raspoređivač aktivirao i izabrao sledeći proces, potrebno je 
		pozvati dijagnostika.rasporedjivac_izvrsio.
		*/
	
		// Implementirati ...
		void rasporedjuj() 
		{
			unique_lock<mutex> l(m);

			while (true) 
			{
				// Raspoređivač čeka dok mu se ne postavi flag
				while (!rasporedjivac_aktivan && !kraj) 
				{ 
					dijagnostika.rasporedjivac_ceka();
					r_cv.wait(l);
				}

				if (kraj) return;
				
				// Prolazi kroz vektor procesa u čekanju
				if (procesi.size() > 0) 
				{
					auto najkraci = procesi.begin();
					
					// Traži se proces sa najkraćim trajanjem
					for (auto it = procesi.begin(); it != procesi.end(); it++) 
					{
						if ((*it)->trajanje < (*najkraci)->trajanje)
							najkraci = it;
					}
					
					// Sledeći proces je onaj sa najkraćim trajanjem
					aktivan_proces = (*najkraci)->id;   
					(*najkraci)->cv.notify_one();
				} 
				else 
				{
					aktivan_proces = -1;
				}

				// Spuštanje flag-a raspoređivača za novo raspoređivanje kasnije
				rasporedjivac_aktivan = false;  
				dijagnostika.rasporedjivac_izvrsio(aktivan_proces);
			}
		}

		/*
		Metoda koja postavlja signal za kraj izvršavanja i pokreće raspoređivač 
		iz čekanja kako bi program mogao da se završi
		*/

		// Implementirati ... 
		void zavrsi() 
		{
			kraj = true;
        	r_cv.notify_one();
		}
};

#endif // RACUNAR_H_INCLUDED
