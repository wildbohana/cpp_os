/*
Napisati program koji omogucuje evidenciju fakultetskih ucionica.
Za svaku ucionicu potrebno je evidentirati naziv ucionice, kvadraturu i racunare koji 
se nalaze u ucionici. Za svaki racunar evidentiraju se podaci o hard disku i procesoru.

U okviru main funkcije, definisati jednu ucionicu koja sadrzi dva racunara.
Zatim je potrebno na konzolu ispisati hard disk i procesor svakog racunara u ucionici .
*/

#include <iostream>
#include <vector>

using namespace std;

class Racunar {
	private:
		string harddisk;
		string procesor;
	public:        
        	Racunar(string hd, string cpu) : harddisk(hd), procesor(cpu) {}

        friend ostream &operator<<(ostream &os, const Racunar &r) {
        	return os << "Hard disk: " << r.harddisk << ", Procesor: " << r.procesor;
        }
};

class Ucionica {
	private:
        	string naziv;
        	float kvadratura;
        	vector<Racunar> racunari;
	public:
        	Ucionica(string n, float kv) : naziv(n), kvadratura(kv) {}

        	void ubaci(Racunar &r) {
        		racunari.push_back(r);
        	}

        	friend ostream &operator<<(ostream &os, const Ucionica &u) {
            		os << "Naziv: " << u.naziv << ", Kvadratura: " << u.kvadratura << endl;

            		os << endl;

            		int i = 1;
            		for (auto it = u.racunari.cbegin(); it != u.racunari.cend(); it++, i++) {
                	os << "Racunar" << i + 1 << ": " << *it << endl;
            	}

        	return os;
        }
};

int main()
{
	Racunar r1("Hitachi", "AMD");
	Racunar r2("Seagate", "Intel");

	Ucionica u1("NTP-316", 25);
	u1.ubaci(r1);
	u1.ubaci(r2);

	cout << u1;

	return 0;
}
