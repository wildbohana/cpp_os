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

	public:
		// Proširiti po potrebi ...
		Program(Dijagnostika& d, int kapacitet) : dijagnostika(d) 
		{
			
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
			
		}

		/*
		Metoda koju poziva nit koja simulira deo operativnog sistema koji se bavi zauzimanjem 
		virtuelne memorije kako bi se zauzela određena količina VM
		Potrebnu količinu VM (kao i bilo koju drugu neophodnu informaciju) preneti u nekom izabranom polju klase.
		*/

		// Implementirati ...
		int zauzmi_virtuelnu_memoriju() 
		{
			
		}
};

#endif // PROGRAM_H_INCLUDED
