#ifndef NEMACKI_PRODAJNI_LANAC_H_INCLUDED
#define NEMACKI_PRODAJNI_LANAC_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <random>

#include "kupac.h"

using namespace std;
using namespace chrono;


class Eksamarket 
{
    private:
        // Dodaj B
		mutex m;
        Kupac &kupac;
        int br_kasa;
        condition_variable *red_za_kase;
        bool *kasa_radi;
        bool *zauzetost_kase;
        int *zauzetost_reda;
        uniform_int_distribution<int> smena_kasa_rand;
        default_random_engine gen;
    public:
		// Dopuni B po potrebi
        Eksamarket(Kupac &k, int broj_kasa) 
		{
            red_za_kase = new condition_variable[br_kasa];
            kasa_radi = new bool[br_kasa];
            zauzetost_kase = new bool[br_kasa];
            zauzetost_reda = new int[br_kasa];

            for (int i = 0; i < br_kasa; i++) 
			{
                if (i < 2)
                    kasa_radi[i] = true;
                else
                    kasa_radi[i] = false;

                zauzetost_kase[i] = false;
                zauzetost_reda[i] = 0;
            }
			
            smena_kasa_rand = uniform_int_distribution<int>(0, br_kasa-1);
        }
		// Dopuni B po potrebi
        ~Eksamarket() 
		{
            delete[] red_za_kase;
            delete[] kasa_radi;
            delete[] zauzetost_reda;
            delete[] zauzetost_kase;
        }

	/**
	Kupac dolazi na kase i zauzima prvu slobodnu kasu koja je aktivna. Ako ne postoje takve kase odlazi na cekanje u najkraci red cekanja aktivne kase.
	
	pozvati Kupac.prilazi_kasama na pocetku
	pozvati Kupac.ceka ako kupac odlazi na cekanje na nekoj kasi
	pozvati Kupac.kupuje kada kupac zauzme kasu i pocne sa kupovinom
	pozvati Kupac.odlazi kada kupac zavrsi kupovinu
	@param id - r. br. kupca
	*/

	// Implementiraj B kupac
    void kupovina(int id) 
	{
		kupac.prilazi_kasama(id);
		unique_lock<mutex> l(m);

		int kasa = -1;
		int red_min = -1;

		for (int i = 0; i < br_kasa; i++) 
		{
			if (kasa_radi[i]) 
			{
				if (kasa == -1) 
				{
					kasa = i;
					red_min = zauzetost_reda[i];
				} 
				else if (!zauzetost_kase[i]) 
				{
					kasa = i;
					red_min = 0;
				} 
				else if (zauzetost_reda[i] < red_min) 
				{
					kasa = i;
					red_min = zauzetost_reda[i];
				}
			}
		}

		while (zauzetost_kase[kasa]) 
		{
			zauzetost_reda[kasa]++;
			kupac.ceka(id, kasa, zauzetost_reda[kasa]);
			red_za_kase[kasa].wait(l);
			zauzetost_reda[kasa]--;
		}

		zauzetost_kase[kasa] = true;

		l.unlock();
		kupac.kupuje(id, kasa);
		this_thread::sleep_for(seconds(1));
		l.lock();

		zauzetost_kase[kasa] = false;

		red_za_kase[kasa].notify_one();
		kupac.odlazi(id, kasa);
    }

	/**
	Na svakih 1s treba jednu aktivnu kasu deaktivirati i jednu neaktivnu kasu aktivirati.
	
	pozvati Kupac.smena_kasa nakon sto se po jedna kasa aktivira i deaktivira
	*/
	
	// Implementiraj B smena
	void smena_kasa() 
	{
		int zatvorena, otvorena;

		while (true) 
		{
			this_thread::sleep_for(seconds(1));
			unique_lock<mutex> l(m);

			do {
				otvorena = smena_kasa_rand(gen);
			} while (!kasa_radi[otvorena]);
			
			do {
				zatvorena = smena_kasa_rand(gen);
			} while (kasa_radi[zatvorena]);

			kasa_radi[otvorena] = false;
			kasa_radi[zatvorena] = true;

			kupac.smena_kasa(otvorena, zatvorena);
		}
	}
};


#endif // NEMACKI_PRODAJNI_LANAC_H_INCLUDED
