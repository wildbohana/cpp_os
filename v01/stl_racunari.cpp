/* 
Napisati program koji omogućava evidentiranje fakultetskih učionica.

Za svaku učionicu potrebno je evidentirati naziv učionice, kvadraturu i sve računare koji se nalaze u učionici. 
Za svaki računar evidentiraju se podaci o hard disku i procesoru.

U okviru main funkcije, definisati jednu učionicu koja sadrži dva računara.
Zatim je potrebno na konzolu ispisati hard disk i procesor svakog računara u učionici.
*/

#include <iostream>
#include <string>
#include <list>

#define MAX 100

using namespace std;


class Racunar {
    private:
        string disk;
        string procesor;
    public:
        Racunar(string hdd, string cpu) {
			disk = hdd;
			procesor = cpu;
		}

        string getHdd() {
			return disk;
		}

        string getCpu() {
			return procesor;
		}
};

class Ucionica {
    private:
        string naziv;
        double kvadratura;
        vector<Racunar> racunari;
    public:
		Ucionica(string name, int kvad) {
			naziv = name;
			kvadratura = kvad;
		}

        void dodajRacunar(Racunar &r) {
			racunari.push_back(r);
		}

        string getNaziv() {
			return naziv;
		}

        int getKvadratura() {
			return kvadratura;
		}

        vector<Racunar> getRacunari() {
			return racunari;
		}

};


int main()
{
    Ucionica u("NTP 333", 50);

    Racunar r1("Seagate", "Intel");

    Racunar r2("WD", "AMD");

    u.dodajRacunar(r1);
    u.dodajRacunar(r2);

    // Ispis učionice
    cout << "--------UCIONICA--------" << endl;
    cout << "Naziv: " << u.getNaziv() << endl;
    cout << "Kvadratura: " << u.getKvadratura() << endl;
	
    cout << "--------RACUNARI--------" << endl;
    for (int i = 0; i < 2; i++) {
        cout << "HDD:"  << u.getRacunari()[i].getHdd() << " ";
        cout << "CPU:"  << u.getRacunari()[i].getCpu() << endl;
    }

    return 0;
}
