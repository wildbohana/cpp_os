#ifndef RASPOREDJIVAC_H_INCLUDED
#define RASPOREDJIVAC_H_INCLUDED

#include <vector>
#include <mutex>

#include "dijagnostika.h"
#include "red.h"

using namespace std;
using namespace chrono;

class Rasporedjivac 
{
	private:
		Dijagnostika& dijagnostika;
		int aktivni_proces;
		// Vektor redova čekanja pri čemu je svaki red vezan za određeni nivo prioriteta
		vector<Red> redovi;
		mutex m;

	public:
		Rasporedjivac(Dijagnostika& d, int broj_nivoa_prioriteta) : dijagnostika(d) 
		{
			aktivni_proces = -1;
			
			// Stvaranje vektora redova prioriteta
			for (int i = 0; i < broj_nivoa_prioriteta; i++) 
				redovi.push_back(Red(i));
		}

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
		prioritet    - prioritet procesa
		
		Ukoliko je procesor već zauzet i ne mogu se izvršavati naredbe procesa, potrebno je pozvati 
		metodu dijagnostika.proces_ceka, a nakon toga proces treba da pređe u stanje čekanja.
		Nakon što je proces izvršio naredbu, potrebno je pozvati dijagnostika.izvrsio_naredbu.
		*/
		void izvrsi(int id_procesa, int broj_naredbi, int prioritet) 
		{
			for (int i = 0; i < broj_naredbi; ++i) 
			{
				unique_lock<mutex> l(m);
				
				// Ako niko ne drži procesor, zauzima se
				if (aktivni_proces == -1) 
					aktivni_proces = id_procesa;
				
				// Ako neko već drži procesor, čeka se u redu svog prioriteta
				while (aktivni_proces != id_procesa) 
				{
					dijagnostika.proces_ceka(id_procesa);
					redovi[prioritet].dodaj_u_red(id_procesa, l);
				}
				
				// Simulacija izvršavanja naredbe
				l.unlock();
				this_thread::sleep_for(milliseconds(300));
				l.lock();

				// Nađe se prvi red u kojem neki proces čeka, počevši od najvišeg prioriteta pa idući na dole
				int obavesti_red = -1;
				for (auto it = redovi.begin(); it != redovi.end(); it++) 
				{
					if (!it->prazan()) 
					{
						obavesti_red = it->preuzmi_prioritet();
						break;
					}
				}

				// Ako je nađeni red nižeg prioriteta od prioriteta trenutnog procesa, treba da nastavi trenutni proces ako ima još naredbi.
				// Ova situacija će se desiti kada je proces jedini u svom nivou prioriteta.
				if (obavesti_red != -1) 
				{
					int sledeci;
					
					// Ako nema niko prioritetniji i ako proces ima još naredbi nastavlja
					if (obavesti_red > prioritet &&	i < broj_naredbi-1)
						sledeci = id_procesa;
					// U suprotnom, obaveštava se taj prioritetniji ili sledeći manje prioritetan ako trenutni proces nema više naredbi
					else
						sledeci = redovi[obavesti_red].izbaci_iz_reda();

					aktivni_proces = sledeci;
				}
				// Ako ništa od ovog nije slučaj to znači da niko ne čeka na procesor i on se oslobađa
				else
				{
					aktivni_proces = -1;    
				}

				dijagnostika.izvrsio_naredbu(id_procesa, i);
			}
		}
};

#endif // RASPOREDJIVAC_H_INCLUDED