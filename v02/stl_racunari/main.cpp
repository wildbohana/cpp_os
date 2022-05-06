/* 
Napisati program koji omogućava evidentiranje fakultetskih učionica.

Za svaku učionicu potrebno je evidentirati naziv učionice, kvadraturu i sve računare koji se nalaze u učionici. 
Za svaki računar evidentiraju se podaci o hard disku i procesoru.

U okviru main funkcije, definisati jednu učionicu koja sadrži dva računara.
Zatim je potrebno na konzolu ispisati hard disk i procesor svakog računara u učionici.
*/

#include <iostream>
#include <list>
#include <string>

#define maks 100

using namespace std;

class Racunar 
{
	private:
		string hdd;
		string cpu;
	public:
		void set_hdd(string h) {hdd = h;}
		string get_hdd() {return hdd;}

		void set_cpu(string c) {cpu = c;}
		string get_cpu() {return cpu;}
};

class Ucionica 
{
	private:
		string naziv;
		double kvadratura;
		list<Racunar> racunari;
	public:
		void set_naziv(string n) {naziv = n;}
		string get_naziv() {return naziv;}

		void set_kvadratura(double k) {kvadratura = k;}
		double get_kvadratura() {return kvadratura;}

		void dodaj_racunar(Racunar r) {racunari.push_back(r);}
		list<Racunar> get_racunari() {return racunari;}
};

int main()
{
	Ucionica u;
	u.set_naziv("NTP-316");
	u.set_kvadratura(25.6);

	Racunar r1;
	r1.set_cpu("Intel");
	r1.set_hdd("Samsung");

	Racunar r2;
	r2.set_cpu("AMD");
	r2.set_hdd("SanDisk");

	u.dodaj_racunar(r1);
	u.dodaj_racunar(r2);

	list<Racunar> racunari = u.get_racunari();

	list<Racunar>::iterator it;

	int i = 0;

	for (it = racunari.begin(); it != racunari.end(); it++, i++)
	{
		cout << "CPU racunara [" << i << "]: " << it->get_cpu() << endl;
		cout << "HDD racunara [" << i << "]: " << it->get_hdd() << endl;
	}

    return 0;
}
