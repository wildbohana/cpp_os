#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include "dijagnostika.h"
#include "naredba.h"
#include "povratna_vrednost.h"

using namespace std;

class Program 
{
	private:
		Dijagnostika& dijagnostika;
		mutex m;
		condition_variable cv_r;
		condition_variable cv_v;
		int kolicina;
		int slobodna;
		int virtualna;
		bool zauzmi_vm;

		void zauzmi(int koliko) 
		{
			unique_lock<mutex> l(m);

			if (slobodna >= koliko) 
				slobodna -= koliko;
			else 
			{
				zauzmi_vm = true;
				kolicina = koliko;
				cv_v.notify_one();

				while (zauzmi_vm)
					cv_r.wait(l);
			}

			// lock i unlock ne smeju da postoje
			this_thread::sleep_for(seconds(1)); 
		}

	public:
		// Proširiti po potrebi ...
		Program(Dijagnostika& d, int kapacitet) : dijagnostika(d), slobodna(kapacitet) 
		{
			virtualna = 0;
			kolicina = 0;
		}

		Dijagnostika& getDijagnostika() 
		{
			return dijagnostika;
		}

		/*
		Metoda koju poziva nit koja simulira izvršenje programa, kako bi zauzela potrebnu memoriju
		
		naredba - naredba koja se izvršava (naredba kojom se zauzima memorija)
		Vraća instancu klase Povratna_vrednost koja sadrži opis zauzeća memorije NAKON izvršenja naredbe.
		*/

		// Implementirati ...
		Povratna_vrednost izvrsi_naredbu(Naredba naredba) 
		{
			if (naredba.tip == "repeat") 
				for (int i = 0; i < naredba.ponavljanja; i++)
					zauzmi(naredba.kolicina_memorije);
			else
				zauzmi(naredba.kolicina_memorije);

			// može i ovako da se vrati
			return {slobodna, virtualna};
		}

		/*
		Metoda koju poziva nit koja simulira deo operativnog sistema koji se bavi zauzimanjem 
		virtuelne memorije kako bi se zauzela određena količina VM
		Potrebnu količinu VM (kao i bilo koju drugu neophodnu informaciju) preneti u nekom izabranom polju klase.
		*/

		// Implementirati ...
		int zauzmi_virtuelnu_memoriju() 
		{
			unique_lock<mutex> l(m);

			while (!zauzmi_vm)
				cv_v.wait(l);
			
			// lock i unlock ne smeju da postoje
			this_thread::sleep_for(seconds(1)); 
			
			virtualna += kolicina;
			zauzmi_vm = false;

			cv_r.notify_one();
			return virtualna;
		}
};

#endif // PROGRAM_H_INCLUDED
