#ifndef RASPOREDJIVAC_H_INCLUDED
#define RASPOREDJIVAC_H_INCLUDED

#include "dijagnostika.h"
#include "red.h"

#include <mutex>
#include <condition_variable>
#include <vector>

using namespace std;

class Rasporedjivac 
{
	private:
		Dijagnostika& dijagnostika;
		int aktivni_proces;     // ID aktivnog procesa
		vector<Red> redovi;     // Vektor redova čekanja pri čemu je svaki red vezan za određeni nivo prioriteta
		mutex m;

	public:
		// Proširiti po potrebi ...
		Rasporedjivac(Dijagnostika& d, int broj_nivoa_prioriteta) : dijagnostika(d) 
		{
			aktivni_proces = -1;
			
			// Stvaranje vektora redova prioriteta
			for (int i = 0; i < broj_nivoa_prioriteta; i++)   
				redovi.push_back(Red(i));
		}

		// Dodat destruktor
		// Desktruktor inicira oslobađanje dinamički zauzete memorije u redovima čekanja
		~Rasporedjivac() 
		{
			for (auto it = redovi.begin(); it != redovi.end(); ++it)
				it->pocisti();
		}

		Dijagnostika& getDijagnostika() 
		{
			return dijagnostika;
		}

		/*
		Metoda koju poziva nit koja simulira izvršenje procesa, kako bi se proces izvršio
		
		id_procesa   - ID procesa
		broj_naredbi - ukupan broj naredbi koje proces treba da izvrši
		
		Ukoliko je procesor već zauzet i ne mogu se izvršavati naredbe procesa, potrebno je 
		pozvati metodu dijagnostika.proces_ceka, a nakon toga proces treba da pređe u stanje čekanja.
		Nakon što je proces izvršio naredbu, potrebno je pozvati dijagnostika.izvrsio_naredbu.
		*/

		// Implementirati ...
		void izvrsi(int id_procesa, int broj_naredbi) 
		{
			// Prioritet će se menjati tokom vremena, dinamički, kako bude opadao prioritet procesa
			int prioritet = 0;  

			for (int i = 0; i < broj_naredbi; i++) 
			{
				unique_lock<mutex> l(m);

				// Ako je procesor slobodan, potrebno ga je zauzeti:
				if (aktivni_proces == -1)
					aktivni_proces = id_procesa;

				// Ako je procesor zauzet, proces staje u redu čekanja svog prioriteta
				while (aktivni_proces != id_procesa) 
				{
					dijagnostika.proces_ceka(id_procesa);
					redovi[prioritet].dodaj_u_red(id_procesa, l);
				}
				
				// Simulacija izvršenja naredbe
				l.unlock();
				this_thread::sleep_for(chrono::milliseconds(300));	
				l.lock();

				// Nalaženje prvog reda u kojem čeka neki proces, počevši od najvišeg prioriteta pa idući na dole
				int obavesti_red = -1;
				for (auto it = redovi.begin(); it != redovi.end(); it++) 
				{
					if (!it->prazan()) 
					{
						obavesti_red = it->preuzmi_prioritet();
						break;
					}
				}

				// Prolazak kroz redove prioriteta i aktivacija procesa koji je prvi u redu najvećeg prioriteta
				if (obavesti_red != -1) 
				{
					int sledeci = redovi[obavesti_red].izbaci_iz_reda();
					aktivni_proces = sledeci;
				}
				else
				{
					aktivni_proces = -1;
				}

				// Smanjivanje prioriteta trenutnog procesa (niži prioritet je ujedno veća numerička vrednost)
				if (prioritet < redovi.size() - 1) 
					prioritet++;

				dijagnostika.izvrsio_naredbu(id_procesa, i, prioritet);
			}
		}
};

#endif // RASPOREDJIVAC_H_INCLUDED
