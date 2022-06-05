#ifndef RED_H_INCLUDED
#define RED_H_INCLUDED

#include <deque>
#include <algorithm>

#include "cv_hrono.h"

using namespace std;

// Specijalna klasa koja služi za modelovanje jednog reda čekanja, u jednom nivou prioriteta
class Red 
{
	private:
		int prioritet;          // Broj prioriteta
		cv_hrono* cv;           // Hronološka CV koja služi za FIFO signaliziranje
		deque<int> spremni;     // Red ID-jeva procesa koji se vezuju u čekanje na hronološku CV

	public:
		Red(int p): prioritet(p) 
		{
			// Konstruktor stvara novu CV dinamički
			cv = new cv_hrono;
		}

		// Metoda za dodavanje u red čekanja FIFO tipa
		void dodaj_u_red(int id_procesa, unique_lock<mutex>& l) 
		{
			// Provera ako dođe do lažnog buđenja
			auto it = find(spremni.begin(), spremni.end(), id_procesa); 
			
			// Stavljanje na kraj spremnih
			if (it == spremni.end())
				spremni.push_back(id_procesa);      
			
			// Čekanje na hronološkoj CV
			cv->wait(l);                            
		}

		int izbaci_iz_reda() 
		{
			// Notifikacija će na hronološkoj CV probuditi PRVOG koji je ušao u čekanje
			cv->notify_one();

			// Skidanje i brisanje ID-a procesa koji je izašao iz čekanja
			int id_procesa = spremni.front();
			spremni.pop_front();
			
			return id_procesa;
		}

		// Provera da li ima procesa u redu vezanom za ovaj prioritet
		bool prazan() 
		{
			return spremni.empty();
		}

		// Metoda za vraćanje prioriteta ovog reda
		int preuzmi_prioritet() 
		{
			return prioritet;
		}

		// Oslobađanje dinamički zauzete memorije za objekat klase cv_hrono
		void pocisti() 
		{
			delete cv;
		}
};

#endif // RED_H_INCLUDED