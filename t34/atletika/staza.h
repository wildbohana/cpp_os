#ifndef STAZA_H_INCLUDED
#define STAZA_H_INCLUDED

#include "povratna.h"
#include "takmicar.h"

#include <mutex>
#include <condition_variable>

using namespace std;
using namespace chrono;

class Atletska_staza 
{
	private:
		Takmicar& takmicar;
		mutex m;
		condition_variable cv_skakac;
		condition_variable cv_bacac;
		int skakaca_ceka;
		int bacaca_ceka;
		enum ATHLETE {bacac, skakac};
		ATHLETE trenutni;
		bool zauzeto;
	public:
		// Prosiriti po potrebi ...
		Atletska_staza(Takmicar& tak) : takmicar(tak) 
		{
			zauzeto = false;
			trenutni = skakac;
			skakaca_ceka = 0;
			bacaca_ceka = 0;
		}

		// Metoda koju poziva nit koja simulira skakaca kako bi takmicar obavio skok.
		// Ukoliko je zaletiste zauzeto, ova metoda ce stajati blokirana dok se ono ne oslobodi i takmicar obavi nastup.
		//
		// rbr - Redni broj takmicara
		//
		// Potrebno je pozvati metodu takmicar.skakac_ceka kada skakac ne moze da stupi na zaletiste.
		// Potrebno je pozvati metodu takmicar.skakac_skace kada skakac stupi na zaletiste, pre skoka.
		// Potrebno je pozvati metodu takmicar.skakac_zavrsio kada je skakac zavrsio i kada treba prikazati njegov rezultat.
		
		// Implementirati ...
		Povratna_vrednost skaci(int rbr) 
		{
			unique_lock<mutex> l(m);

			while (zauzeto|| trenutni != skakac)
			{
				skakaca_ceka++;
				takmicar.skakac_ceka(rbr);
				cv_skakac.wait(l);
			}

			skakaca_ceka--;
			zauzeto = true;
			
			system_clock::time_point poceo = system_clock::now();
			takmicar.skakac_skace(rbr);

			l.unlock();
			this_thread::sleep_for(chrono::seconds(1));
			l.lock();

			system_clock::time_point zavrsio = system_clock::now();

			zauzeto = false;

			Povratna_vrednost pv;

			pv.rezultat = rand()%10;
			pv.trajanje = zavrsio - poceo;

			takmicar.skakac_zavrsio(rbr, pv);

			if (bacaca_ceka)
			{
				trenutni = bacac;
				cv_bacac.notify_one();
			}
			else
			{
				trenutni = skakac;
				cv_skakac.notify_one();
			}

			return pv;
		}

		// Metoda koju poziva nit koja simulira bacaca kako bi takmicar obavio bacanje.
		// Ukoliko je zaletiste zauzeto, ova metoda ce stajati blokirana dok se ono ne oslobodi i takmicar obavi nastup.
		//
		// rbr - Redni broj takmicara
		//
		// Potrebno je pozvati metodu takmicar.bacac_ceka kada bacac ne moze da stupi na zaletiste.
		// Potrebno je pozvati metodu takmicar.bacac_skace kada bacac stupi na zaletiste, pre skoka.
		// Potrebno je pozvati metodu takmicar.bacac_zavrsio kada je bacac zavrsio i kada treba prikazati njegov rezultat.
		
		// Implementirati ...
		Povratna_vrednost baciKoplje(int rbr) 
		{
			unique_lock<mutex> l(m);

			while (zauzeto || trenutni != bacac)
			{
				bacaca_ceka++;
				takmicar.bacac_ceka(rbr);
				cv_bacac.wait(l);
			}

			bacaca_ceka--;
			zauzeto = true;
			
			system_clock::time_point poceo = system_clock::now();
			takmicar.bacac_baca(rbr);

			l.unlock();
			this_thread::sleep_for(chrono::seconds(2));
			l.lock();

			system_clock::time_point zavrsio = system_clock::now();

			zauzeto = false;

			Povratna_vrednost pv;

			pv.rezultat = rand()%100;
			pv.trajanje = zavrsio - poceo;

			takmicar.bacac_zavrsio(rbr, pv);

			if (skakaca_ceka)
			{
				trenutni = skakac;
				cv_skakac.notify_one();
			}
			else
			{
				trenutni = bacac;
				cv_bacac.notify_one();
			}

			return pv;
		}
};

#endif // STAZA_H_INCLUDED
