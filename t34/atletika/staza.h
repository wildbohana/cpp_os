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
		// Dodato:
		enum ATHLETE {SKAKAC, BACAC};
		ATHLETE trenutni;
		bool slobodno;
		int skakacaCeka;
		int bacacaCeka;
		mutex m;
		condition_variable skakaci;
		condition_variable bacaci;
	public:
		// Prosiriti po potrebi ...
		Atletska_staza(Takmicar& tak) : takmicar(tak) 
		{
			trenutni = SKAKAC;
			skakacaCeka = 0;
			bacacaCeka  = 0;
			slobodno = true;
		}

		/*
		Metoda koju poziva nit koja simulira skakaca kako bi takmicar obavio skok.
		Ukoliko je zaletiste zauzeto, ova metoda ce stajati blokirana dok se ono ne oslobodi i takmicar obavi nastup.
		
		rbr - Redni broj takmicara
		
		Potrebno je pozvati metodu takmicar.skakac_ceka kada skakac ne moze da stupi na zaletiste.
		Potrebno je pozvati metodu takmicar.skakac_skace kada skakac stupi na zaletiste, pre skoka.
		Potrebno je pozvati metodu takmicar.skakac_zavrsio kada je skakac zavrsio i kada treba prikazati njegov rezultat.
		*/

		// Implementirati ...
		Povratna_vrednost skaci(int rbr) 
		{
			// dolazak na stazu
			system_clock::time_point dosao = system_clock::now();

			unique_lock<mutex> l(m);

			// da li je skakac dosao na red?
			while(!slobodno || trenutni != SKAKAC)
			{
				skakacaCeka++;
				takmicar.skakac_ceka(rbr);
				skakaci.wait(l);
				skakacaCeka--; 
			}
			// zauzimamo stazu
			slobodno = false; 

			l.unlock();
			takmicar.skakac_skace(rbr);
			// skok traje 2 sekunde
			this_thread::sleep_for(seconds(2));
			system_clock::time_point zavrsio = system_clock::now(); 
			l.lock();

			// oslobadjamo stazu
			slobodno = true; 

			// posle skakaca na stazu ide bacac (ako ih jos uvek ima u redu)
			// ako ih nema, sledeci na redu ce ponovo biti skakac
			if (bacacaCeka > 0)
			{
				trenutni = BACAC;
				bacaci.notify_one();
			}
			else
			{
				trenutni = SKAKAC;
				skakaci.notify_one();
			}

			// skakac moze skociti izmedju 0 i 9 metara
			Povratna_vrednost pv;
			pv.rezultat = rand() % 10;
			pv.trajanje = zavrsio - dosao;

			takmicar.skakac_zavrsio(rbr, pv);

			return pv;
		}

		/*
		Metoda koju poziva nit koja simulira bacaca kako bi takmicar obavio bacanje.
		Ukoliko je zaletiste zauzeto, ova metoda ce stajati blokirana dok se ono ne oslobodi i takmicar obavi nastup.
		
		rbr - Redni broj takmicara
		
		Potrebno je pozvati metodu takmicar.bacac_ceka kada bacac ne moze da stupi na zaletiste.
		Potrebno je pozvati metodu takmicar.bacac_skace kada bacac stupi na zaletiste, pre skoka.
		Potrebno je pozvati metodu takmicar.bacac_zavrsio kada je bacac zavrsio i kada treba prikazati njegov rezultat.
		*/

		// Implementirati ...
		Povratna_vrednost baciKoplje(int rbr) 
		{
			// dolazak na stazu
			system_clock::time_point dosao = system_clock::now();

			unique_lock<mutex> l(m);

			// da li je bacac dosao na red?
			while(!slobodno || trenutni != BACAC)
			{
				bacacaCeka++; 
				takmicar.bacac_ceka(rbr);
				bacaci.wait(l);
				bacacaCeka--; 
			}
			// zauzimamo stazu
			slobodno = false;

			l.unlock();
			takmicar.bacac_baca(rbr);
			// bacanje koplja traje 1 sekundu
			this_thread::sleep_for(seconds(1));
			system_clock::time_point zavrsio = system_clock::now();
			l.lock();

			// oslobadjamo stazu
			slobodno = true; 

			// posle bacaca na stazu ide skakac (ako ih jos uvek ima u redu)
			// ako ih nema, sledeci na redu ce ponovo biti bacac
			if(skakacaCeka > 0)
			{
				trenutni = SKAKAC;
				skakaci.notify_one();
			}
			else
			{
				trenutni = BACAC;
				bacaci.notify_one();
			}

			// bacac moze baciti koplje izmedju 0 i 100 metara
			Povratna_vrednost pv;
			pv.rezultat = rand() % 100;
			pv.trajanje = zavrsio - dosao;

			takmicar.bacac_zavrsio(rbr, pv);

			return pv;
		}
};

#endif // STAZA_H_INCLUDED
