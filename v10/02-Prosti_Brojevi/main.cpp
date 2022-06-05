/*
Napisati konkurentni program koji koristi vise niti da generise i smesti u memoriju proste brojeve.
Program se sastoji od tri faze, a, b, i c. Svaku od tih faza testira po jedna funkcija koja je 
makar parcijalno data.

a) 
Napisati funkciju koja racuna proste brojeve pocevsi od neke vrednosti i koja se izvrsava dok ne
generise specificiran broj prostih brojeva. Test pokrece ovu funkciju vise puta, paralelno i ispisuje
rezultat. Prosti brojevi su oni koji su deljivi samo sa jedan i samim sobom, tj. 23 je prost broj
zato sto je deljiv samo sa 1 i 23. 49 nije prost broj posto je osim sa 1 i 49 deljiv i sa 7. 

b) 
Dovrsiti klasu Memorija koja sluzi da alocira i dealocira regione memorije specificirane duzine.
Memorijom se upravlja u jedinicama koje u velicini odgovaraju 1 int broju. Metod alokacije je
first-fit. Klasa mora biti thread-safe i blokirati ako memorije nema. Kada se memorija dealocira
samo markirati odsecak kao slobodan, bez komprimovanja. Na pocetku evidencije, sva memorije je u
jednom velikom, slobodnom odsecku. Koliko memorije ima se specificira prilikom stvaranja promenljive
tipa 'Memorija'.

c) 
Napisati funkciju koja se izvrsava kao detach-ovana, daemon-nit koja, kada se to od nje
zatrazi, preko metode u klasi Memorija, vrsi kompakciju memorije, tj. susedne odsecke memorije
izmedju kojih nema alociranog prostora pretvara u jedan veliki odsecak. Obezbediti ovoj niti
da se moze na zahtev korisnika ugasiti. Komprimovanje ovde ne radi realokaciju nicega, sve sto radi
jeste detektuje situacije gde imamo susedne odsecke koji su slobodni i spaja ih. Napisati tajmer
koji poziva kompakciju memorije svakih deset sekundi. 

Detalje zadataka pod a, b, i c mozete videti u daljem tekstu koda gde su uvek oznaceni komentarima
oblika 
	//TODO X Tekst 
Gde je X ili A ili B ili C (u zavisnosti toga kom zadatku pripada), a tekst su instrukcije. 
*/

// I/O
#include <iostream>
#include <iomanip>

// STL
#include <list>

// THREADING
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace chrono;

struct Odsecak
{
    int* m; 		// Pokazivac na memoriju odsceka
    int n; 			// Velicina koja je zauzeta
    bool free;		// Da li je odsecak slobodan ili ne
};

class Dijagnostika
{
    private:
        mutex m;

    public:
        Dijagnostika() {}
        
		// Stampa rezultate racunanja prostih brojeva na lep nacin
        void stampajRezultate(const char* tag, int n, int* mem)
		{
            unique_lock<mutex> l(m);

            cout << "[" << tag << "] Rezultati: " << endl;
            for (int i = 1; i <= n; i++)
			{
                cout << setfill(' ') << setw(8) << mem[i-1] << " ";
                if (i % 9 == 0) cout << endl;
            }
            cout << endl;
        }

        /*
		stampa memoriju na pregledan nacin gde se sukcesivni odsceci oznacavaju sa a, b, c, d itd.
        Mala slova oznacavaju zauzete odsecke, Velika slova oznacavaju slobodne odsecke
        */
	   	void stampajMapuMemorije(list<Odsecak>::iterator begin, list<Odsecak>::iterator end)
		{
            unique_lock<mutex> l(m);

            int j = 1;
            char c = 'a';
            const char offset = 'A' - 'a';

            for (auto it = begin; it != end; it++)
			{
                for (int i = 0; i < it->n; i++)
				{
                    if (it->free) cout << (char)(c + offset);
                    else cout << c;
                    
					if (j % 8 == 0) cout << " ";

                    if (j >= 72) 
					{
                        cout << endl;
                        j = 0;
                    }
                    j++;
                }
                c++;
            }
            cout << endl;
        }

        // Opsta poruka za debug
        void stampajPoruku(const char* tag, const char* msg)
		{
            unique_lock<mutex> l(m);
            cout << "[" << tag << "] " << msg << endl;
        }
};

class Memorija
{
    private:
        Dijagnostika& d;
        list<Odsecak> evidencija;
		int kapacitet;
        int* mem;
        mutex m;
        condition_variable c;
        condition_variable compactCommand; 
        bool zavrseno;
        bool doCompaction;
        
		/*
		First fit algoritam, dat kao ozbiljna olaksica. Ova verzija NIJE thread-safe.
        Namenjena je samo da olaksa implementaciju zauzimanja, ne moze da se koristi van klase. 
        */
		list<Odsecak>::iterator firstFit(int n)
		{
            for (auto it = evidencija.begin();it != evidencija.end();it++)
			{
                if (it->free && it->n == n)
				{
                    it->free = false; 
                    return it;
                } 
				else if(it->free && it->n > n)
				{
                    int n2 = it->n;

                    it->n = n;
                    it->free = false; 

                    Odsecak o = {m: it->m + n, n: n2 - n, free: true};

                    it = evidencija.insert(++it, o);
                    it--;

                    return it;
                }
            }
            return evidencija.end();
        }

		// Dodate metode
		// Nije thread-safe
		void free(int* p)
		{
            for (auto it = evidencija.begin(); it != evidencija.end(); it++)
			{
                if (it->m == p)
				{
                    it->free = true;
                    break;
                }
            }
        }

        // Nije thread-safe
		void compact()
		{
            for (auto it = evidencija.begin(); it!= evidencija.end();)
			{
				//Dobijemo iterator na sledeću stvar
				//Zato što ne možemo da napišemo it + 1, morali smo da it povećamo, pa ga posle smanjujemo
                auto next = ++it; 
                it--;
				
				// Stigli smo do kraja
                if (next == evidencija.end())
				{
                    it++;
                    continue;
                }

				// Ako barem jedan od odsečaka nije slobodan, onda nema šta da kompaktiramo
                if ((!it->free) || (!next->free))
				{
                    it++;
                    continue;
                }

				// Uništimo jedan odsečak i dodamo ga na prethodni
                while (it->m + it->n == next->m && next->free)
				{
                    it->n = it->n + next->n; 
                    next = evidencija.erase(next);
                }

				// Skok na sledeći 
                it = next;
            }
        }

    public:
		// TODO B Dopuniti po potrebi
        Memorija(int k, Dijagnostika& dd) : d(dd), kapacitet(k), zavrseno(false), doCompaction(false)
		{
            // Ma pocetku to je memorija samo jedan odsecak gde je sve slobodno
            mem = new int[k];
			Odsecak o = {m: mem, n: k, free: true}; 
		    evidencija.push_front(o);
        }
		
		// TODO B Dopuniti po potrebi
        ~Memorija()
		{
            evidencija.clear();
            delete [] mem;
        }
		
        Dijagnostika& getDijagnostika() const
		{
            return d;
        }

		// TODO C Omoguciti da se ovim terminira nit za kompakciju
        void zavrsi()
		{
            zavrseno = true;
            compactCommand.notify_one(); 
        }

		// TODO C Omoguciti da se ovim zatrazi akcija od niti za kompakciju
        void initiateCompaction()
		{
            doCompaction = true;
            compactCommand.notify_one();
        }

        void stampajMapuMemorije()
		{
            d.stampajMapuMemorije(evidencija.begin(), evidencija.end());
        }

        /*TODO B Napisati thread-safe, sinhronizovan kod koji eliminse sansu stetnih preplitanja
        i omogucava da se izvrsavanje blokira (tj. predje u WAIT stanje) ako nema dovoljno
        memorije slobodne. N je koliko se memorije trazi. tag nam kaze koja nit trazi memoriju
    	Sluzi da bi se mogle stampati lepse poruke za dijagnostiku. 
		*/

		// Alocira n jedinica za int na thread-safe nacin		
		int* alociraj(int n, const char* tag = "Unknown")
		{
            unique_lock<mutex> l(m);
            list<Odsecak>::iterator it;
			
			// Nit čeka da bude oslobođena
            while ((it = firstFit(n)) == evidencija.end())
			{
                d.stampajPoruku(tag, "Nit ceka..."); 
                c.wait(l);
            }

            d.stampajPoruku(tag, "Nit zauzela memoriju");
            return it->m;
        }

		/*
		TODO B Napisati thread-safe, sinhronizovan kod koji eliminse sansu stetnih preplitanja
		Koji oslobadja memoriju na koju pokazuje p. Tag nam kaze koja nit trazi memoriju
		Sluzi da bi se mogle stampati lepse poruke za dijagnostiku.
		*/

        //Oslobadja n jedinica za int na thread-safe nacin
        void dealociraj(int* p, const char* tag = "Unknown")
		{
			unique_lock<mutex> l(m);

            d.stampajPoruku(tag, "Nit dealocira memoriju");
            free(p);

            c.notify_all();
        }

		/*
		TODO C Napisati thread-safe sinhronizovan kod koji se izvrsava u beskonacnoj petlji koja
        ceka da se otkoci zbog gasenja ili trazenja kompakcije. 
        */
	   	void kompaktiraj()
		{
        	while (true)
			{
                unique_lock<mutex> l(m);

                while (!doCompaction && !zavrseno) compactCommand.wait(l);
				if (zavrseno) return;
                
				doCompaction = false;

                d.stampajPoruku("COMPACTION DEAMON", "Running compaction...");
                stampajMapuMemorije();
                compact();

                d.stampajPoruku("COMPACTION DEAMON", "After compaction:");
                stampajMapuMemorije();
                c.notify_all();
            }
        }
};

// Racuna proste brojeve pocevsi od vrednosti 'od', pa sve dok ne nadje n prostih brojeva. 
// rezultat smesta tamo gde pokazuje 'gde.' 

/*
TODO A Napisati algoritam za trazenje prostih brojeva od neke vrednosti od pa dok ne nadje n
prostih brojeva i smestiti rezultate u niz na koji pokazuje 'gde.' 
Efikasnost algoritma se ne ocenjuje. 
*/
void racunaj(int od, int n, int* gde)
{
    int x = od;
    int i = 0;

    EXIT: while (i < n)
	{
		// Namerno neefikasan algoritam
        for (int j = 2; j < x; j++)
		{
            if(x % j == 0)
			{
                x++;
				// Jedan od par primera kada je GOTO prihvatljiv.
                goto EXIT;
            }
        }

        *gde++ = x;
        i++;
        x++;
    }
}

// Telo deattach-ovane niti za kompakciju u okviru zadatka C
void compactionDeamon(Memorija& mem)
{
    mem.kompaktiraj();
}

/*
Telo niti za racunanje koja se koristi za testiranje u zadacima B i C. 
n je broj blokova prostih brojeva koji se racunaju svaki od kojih je od 256 do 512 jedinica velik. 
*/
void calculationThread(const char* threadName, Memorija& mem, int n)
{
    for (int i = 0; i < n; i++)
	{
        int s = rand() % 257 + 256;
        int* x = mem.alociraj(s, threadName);

        racunaj(rand() % 10000, s, x);

        mem.getDijagnostika().stampajRezultate(threadName, s, x);
        mem.dealociraj(x, threadName);
    }
}

/*
TODO C Napisati telo timerThread funkcije koja predstavlja telo tajmer niti tako da
se svakih deset sekundi inicira kompakcija memorije i tako da se timerThread nit moze
ugasiti spolja kroz manipulaciju 'active' promenljivom. 
*/
void timerThread(Memorija& mem, bool& active)
{
    do {
        this_thread::sleep_for(seconds(10));
        mem.initiateCompaction();
    } while (active);
}

/*
TODO A napraviti niz od osam niti
TODO A Pokrenuti nit tako da izvrsava funkciju racunaj tako da racuna 128 prostih 
brojve pocevsi od 0 za prvu nit, 2000 za drugu, 4000 za trecu i tako dalje
i tako da smesti rezultate u memoriju na koju pokazuje p[i].
TODO A join-ovati sve niti koje su pokrenute u ovom testu. 
*/
void testirajA(Dijagnostika& d)
{
    int* mem = new int[1024];
    int* p[8];

	// Niz od osam niti
    thread t[8];

    for (int i = 0; i < 8; i++)
	{
        p[i] = mem + (i * 128);
		// Pokretanje tih osam niti
    	t[i] = thread(racunaj, i * 2000, 128, ref(p[i]));
    }

    // Join osam niti
	for (int i = 0; i < 8; i++) t[i].join();

    d.stampajRezultate("TEST1", 1024, mem);
    delete [] mem;
}


/*
Upozorenje: Test B (namerno) izaziva mrtvu petlju. 
TODO B Napraviti niz 10 niti. 
TODO B Pokrenuti nit koja ima ime smesteno u tags[i] i koja radi sa memorijom mem i racuna
n blokova prostih brojeva kroz calculationThread funkciju
TODO B join-ovati sve pokrenute niti. 
*/
void testirajB(Memorija& mem)
{
    char tags[10][10];

	// Niz od 10 niti
    thread t[10];

    for (int i = 0; i < 10; i++)
	{
        sprintf(tags[i], "BTEST_%d", i);
        int n = rand() % 6 + 1;
        
		// Pokretanje 10 niti
        t[i] = thread(calculationThread, ref(tags[i]), ref(mem), n);
    }
    // Join 10 niti
	for (int i = 0; i < 10; i++) t[i].join();
}

//TODO C Napraviti niz od 10 niti. 
//TODO C Pokrenuti nit koja ima ime smesteno u tags[i] i koja radi sa memorijom mem i racuna
//n blokova prostih brojeva kroz calculationThread funkciju
//TODO C pokrenuti kao detach-ovane niti koje pokrecu compactionDeamon i timerThread funkcije
//koristiti runTimer da se kontrolise da li se timer izvrsava ili ne. 
//TODO C join-ovati sve niti iz niza
void testirajC(Memorija& mem)
{
    char tags[10][10];
	
	// Niz od 10 niti
    thread t[10];

    for (int i = 0; i < 10; i++)
	{
        sprintf(tags[i], "CTEST_%d", i);
        int n = rand() % 6 + 1;
        
		// Pokretanje 10 niti
		t[i] = thread(calculationThread, ref(tags[i]), ref(mem), n);
    }

	// Pravljenje detach-ovane niti
	thread cd(compactionDeamon, ref(mem));
    cd.detach();

    bool runTimer = true;

	// Pravljenje detach-ovane niti
	thread timer(timerThread, ref(mem), ref(runTimer));
    timer.detach();

	// Join 10 niti
	for (int i = 0; i < 10; i++) t[i].join();
    
    mem.zavrsi();
    runTimer = false;
}

void testirajSve()
{
    Dijagnostika d;
    Memorija mem(1024, d);

    //testirajA(d);
    //testirajB(mem);
    testirajC(mem);
}

int main()
{
    testirajSve();
	
    return 0; 
}
