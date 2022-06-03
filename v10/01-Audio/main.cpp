/*
Napisati konkurentni program koji simulira audio podsistem racunara. Sistem se sastoji primarno
od centralnog audio bafera koji sadrzi odredjeni broj semplova. Radi jednostavnosti, smatra se
kao dato da je stopa semplovanja 32-bitna sa 8Khz. To znaci da 1 sekunda audio zapisa u ovom
formatu je 8000 32-bitnih float vrednosti, odn. 32 kilobajta. Svaki sempl predstavlja 25ms
odnosno 200 sempl vrednosti. Bafer audio uredjaja je dugacak 2 sekunde, odn. 16000 sempl vrednosti, 
odn. 80 semplova, ali ovo treba da je podesivo. 

Sempl sadrzi ne samo podatke, nego i trenutak u vremenu kada je namenjen da se 
pusti. Radi jednostavnosti, predpostavimo da su ti trenuci uvek specificirani u jedinicama od 25ms
pocevsi od nekog zamisljenog nultog momenta. 

a) 
Dovrsiti funkciju koja generise sinusni ton date frekvencije, sa datom stopom semplovanja,
za dat vremenski trenutak i smesta je u prosledjen bafer. Zatim pokrenuti vise niti koje paralelno
u nekoliko bafera generisu tonove frekvencija 440Hz, 554.356Hz, 660Hz (pravimo akord). 

b) 
Modifikovati i dovrsiti klasu Bafer tako da vodi racuna o:
1) Popunjenosti, tako da ako se iz nje cita a prazna je ili se u nju pise a puna je, se blokira. 
2) Redosledu, tako da se ubaceni semplovi uvek sortiraju po svom vremenu. 
3) Sumiranju: Semplovi namenjeni za isto vreme se sabiraju, vodeci racuna da je 1.0 maksimalna vrednost.
4) Prioritetu: Semplovi koji 'kasne' odnosno nalaze se u proslosti u odnosu na najstarije se odbacuju. 

c) 
Napisati pozadinsku nit koja se aktivira na tajmer (koji okida svakih 25ms) i cita u vremenu
najskoriji sempl koji se 'pusta' (ispisuje na ekran). Pozadinska nit, i pozadinska nit tajmera se
moraju zavrsiti na zahtev glavne niti. 


Detalje zadataka pod a, b, i c mozete videti u daljem tekstu koda gde su uvek oznaceni komentarima
oblika 
	//TODO X Tekst 
Gde je X ili A ili B ili C (u zavisnosti toga kom zadatku pripada), a tekst su instrukcije. 

BONUS ZADATAK 1: 
Tako napraviti kod da se *sva* neophodna memorija za cuvanje uzoraka u audio baferu alocira na 
samom pocetku izvrsavanja, i da nema dalje dealokacije i alokacije tokom izvrsavanja.

BONUS ZADATAK 2: 
Generisati audio fajl tona koji ovaj program generise uz opis toga kako ste ga napravili. 
*/

// I/O
#include <iostream>
#include <iomanip>

// STL
#include <list>

// Multithreading
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable> 

// Other
#include <cmath>
#include <cstring>

using namespace std;
using namespace chrono;

struct Sample
{
    float data[200];
    int time;
};

class Dijagnostika
{
    private:
        mutex m;
    public:
        Dijagnostika() {}

        /*
		stampajSample uzme jedan uzorak audio signala i prikaze ga na ekranu "graficki"
		Uzme srednju vrednost delica sample-a (njih 20) i sracuna kolika je velicina proseka
		u skali od 0 do 15. 

		Onda ga prikaze na 5 linija teksta u zavisnosti od jacine signala na sledeci nacin
		13-15|    | Prazno znaci da signal nije stigao dovde u jacini
		10-12| -  | Znak - znaci da je u ovoj kategoriji signal na poziciji 2, tj. 11
		07-09| #  | Znak # znaci da je signal stigao da i potpuno popunio ovaj nivo jacine
		04-06| #  | Znak # znaci da je signal stigao da i potpuno popunio ovaj nivo jacine
		01-03| #  | Znak # znaci da je signal stigao da i potpuno popunio ovaj nivo jacine
        */

        void stampajSample(Sample *s)
		{
            unique_lock<mutex> l(m);
			
            cout << "Playing sample at " << s->time << endl;

            int p[20] = {0};
            for (int i = 0; i < 20; i++)
			{
                float avg = 0;
                for (int j = 0; j < 10; j++)
                    avg += s->data[i * 10 + j];
                
                avg = avg / 10.0f;

                int x = round((avg * 15.0f));
                p[i] = x;
            }

            for (int i = 4; i >= 0; i--)
			{
                for (int j = 0; j < 20; j++)
				{
                    if (p[j] > i * 3)
					{
                        if (p[j] > i * 3 + 2) cout << "#";
						else if (p[j] > i * 3 + 1) cout << "-";
						else cout << "_";
                    }
					else
					{
                        cout << " ";
                    }
                }
                cout << "\n";
            }

            cout << endl;
        }

        // Stampanje uzorka za dijagnostiku u sirovoj formi, tj. kao brojevi.
        // Nema sinhronizaciju zato sto je tu samo za debugovanje
        void stampajSampleRaw(Sample *s)
		{
            for (int i = 0; i < 200; i++)
                cout << s->data[i] << endl;
        }

        // Služi za debagovanje
        void stampajPoruku(const char* tag, const char* msg)
		{
            unique_lock<mutex> l(m);
            cout << "[" << tag << "] " << msg << endl;
        }
};

class Bafer 
{
    private:
		// TODO B,C Dodati po potrebi prosirenja
        Dijagnostika& d;

		Sample* data;
        bool* freeMap;
        list<int> index; 
        int capacity;
        int n;
        mutex m;
        condition_variable full;
        condition_variable empty; 
        condition_variable cplay;
        bool doPlay;
        bool doZavrsi;

        void addSample(float* dest, float* src)
		{
            for(int i = 0; i < 200; i++)
                dest[i] = fmin(1.0f, dest[i] + src[i]);
        }

        int allocate()
		{
            int i = 0;

            while (i < capacity && !freeMap[i]) i++;
            if (i >= capacity) exit(1);

            freeMap[i] = false;

            return i;
        }

        void deallocate(int i)
		{
            freeMap[i] = true; 
        }
		
		// Nije thread-safe
        void loadInternal(Sample& out)
		{
            int i = index.front();
            index.pop_front();
			
            out.time = data[i].time;
            memcpy(out.data, data[i].data, 200 * sizeof(float));

            deallocate(i);
            n--;
        }
        
    public:
		// TODO B,C Dodati po potrebi prosirenja
        Bafer(Dijagnostika& dd, int cap) : d(dd), n(0), capacity(cap), doPlay(false), doZavrsi(false)
		{
            data = new Sample[cap];
            freeMap = new bool[cap];

            for (int i = 0; i < cap;i++)
                freeMap[i] = true;
        }

         // TODO B,C Dodati po potrebi prosirenja
        ~Bafer()
		{
			delete[] data;
            delete[] freeMap;
        }

        Dijagnostika& getDijagnostika()
		{
            return d;
        }

        // TODO B Napraviti thread-safe cuvanje uzorka ovde u skladu sa zadatkom
		void store(Sample& in)
		{
			unique_lock<mutex> l(m);

            while (n >= capacity)
                full.wait(l);
            
            if (n == 0)
			{
                int i = allocate();

                data[i].time = in.time;
                memcpy(data[i].data, in.data, 200 * sizeof(float));

                n++;

                index.push_front(i);
                empty.notify_one();
            }
			else
			{
				// Prekasno poslat
                if (data[index.front()].time > in.time) return;

                bool flag = false;

                for (auto it = index.begin();it != index.end();it++)
				{
                    if (data[*it].time == in.time)
					{
                        addSample(data[*it].data, in.data);
                        flag = true; 
                        break;
                    }
					else if (data[*it].time > in.time)
					{
                        int i = allocate();

                        data[i].time = in.time;
                        memcpy(data[i].data, in.data, 200 * sizeof(float));
						
                        index.insert(it, i);
                        flag = true;

                        n++;
                        break; 
                    }
                } 

                if (!flag)
				{
                    int i = allocate();

                    data[i].time = in.time;
                    memcpy(data[i].data, in.data, 200 * sizeof(float));

                    index.push_back(i);

                    n++;
                }
            }
        }

	    // TODO B Napraviti thread-safe ucitavanje uzorka ovde u sladu sa zadatkom
        void load(Sample& out)
		{
			unique_lock<mutex> l(m);

            while (n == 0) 
                empty.wait(l);

            loadInternal(out);

            full.notify_one();
        }

        // TODO C napraviti beskonacnu petlju ovde koja se izvrsava pozadniski i inicira iz tajmer
        // niti, a 'pusta' zvuk, tj. ispisuje tekuci sample na ekran.  
		void play()
		{
            while(1)
			{
                unique_lock<mutex> l(m);

                while(!doPlay && !doZavrsi)
                    cplay.wait(l);                
                
                if (doZavrsi) return;

                doPlay = false;

                if (n == 0) continue;

                Sample s;
                loadInternal(s);
                d.stampajSample(&s);

                full.notify_one();
            }
        }

        // TODO C mehanizam koji se poziva iz tajmer niti i 'otkoci' nit play 
		void initiatePlay()
		{
            unique_lock<mutex> l(m);

            doPlay = true;
            cplay.notify_one();
        }

		// TODO B Ispisuje ceo bafer na ekran 
        void dumpBuffer()
		{
           while (n != 0)
		   {
                Sample s; 
                load(s);
                d.stampajSample(&s);
            }
        }

		// TODO OPCIONO Ispisuje ceo bafer na ekran u sirovoj formi
        void dumpBufferRaw()
		{
            while (n != 0)
			{
                Sample s;
                load(s);
                d.stampajSampleRaw(&s);
            }
        }

		// Naknadno dodato
		void zavrsi()
		{
            unique_lock<mutex> l(m);

            doZavrsi = true;
            cplay.notify_one(); 
        }
};

/*
Funkcija koja radi odgovarajucu matematiku koja racuna uzorak sinusnog tona datih parametara
frequency - frekvencija koja se generise, u Hz
time - Koliko se vec emituje zvuk (neophodno za racunanje faze tona) u sekundama
length - Duzina uzorka koji se generise, u sekundama
sampleRate - stopa uzorkovanja, u Hz
volume - Jacina zvuka od 0 do 1.0
data - gde da se smeste podaci
*/

void computeSample(double frequency, double time, double length, double sampleRate, double volume, float* data)
{
    // Računanje faze
    double phase = (time * frequency);
    phase = phase - floor(phase);
    phase = phase * 2 * M_PI; 

    int n = ceil(length * sampleRate); 
    double incr = (2 * M_PI * frequency) / sampleRate;
    double t = phase;

    for (int i = 0; i < n; i++)
	{
		// Očuvanje raspona od 0 do 1. 
        *(data + i) = (float)(volume * ((sin(t) / 2) + 0.5));
        t+= incr;
    }
}

/*
TODO A Izracunati tri sample-a duzine od 0.025s frekvencija 440, 554.356, 660
sa stopom uzorkovanja od 8kHz i jacinom zvuka od 100% u paraleli u tri nezavisne niti. 
Zatim ispisati sva tri uzorka. 
*/
void testirajA(Dijagnostika& d)
{
    Sample s1, s2, s3;

    s1.time = 0;
    s2.time = 0;
    s3.time = 0;

    thread t1(computeSample, 440, 0, 0.025, 8000, 1.0, s1.data);
    thread t2(computeSample, 554.356, 0, 0.025, 8000, 1.0, s2.data);
    thread t3(computeSample, 660, 0, 0.025, 8000, 1.0, s3.data);

    t1.join();
    t2.join();
    t3.join();

    d.stampajSample(&s1);
    d.stampajSample(&s2);
    d.stampajSample(&s3);
}

/*
TODO B Funkcija koja cini telo niti za proizvodnju tona koja sluzi da generise uzorke 
prave duzine (25ms) i smesta ih sa odgovarajucim parametrom vremena u Bafer koristeci 
metodu store
POMOC: Da bi za datu duzinu tona saznali broj uzoraka moze vam pomoci sledeci kod:
int n = ceil(length / 0.025);
*/
void toneGenerator(double freq, double length, double volume, Bafer& b)
{
    int n = ceil(length / 0.025);

    for (int i = 0; i < n; i++)
	{
        Sample s;
        s.time = i;

        computeSample(freq, i * 0.025, 0.025, 8000, volume, s.data);
        b.store(s);
    }
}

/*
TODO B Pokrenuti tri niti koje izvrsavaju toneGenerator sa parametrima 440Hz, 554.356Hz
660Hz, duzinama od po 1s, i jacinom zvuka od 30%. 
Kada niti zavrse, ispisati ceo bafer. 
*/
void testirajB(Bafer& b)
{
    thread t1(toneGenerator, 440.0, 1.0, 0.3, ref(b));
    thread t2(toneGenerator, 554.356, 1.0, 0.3, ref(b));
    thread t3(toneGenerator, 660.0, 1.0, 0.3, ref(b));

    t1.join();
    t2.join();
    t3.join();

    b.dumpBuffer();
}


/*
TODO C Napraviti ovde telo niti koje sluzi da automatski, tempirano 'pusta' (ispisuje na ekran)
tekuci uzorak iz bafera svakih 25ms kroz initiatePlay. 
*/
void timerThread(Bafer& b, bool& active)
{
    while (active)
	{
        this_thread::sleep_for(milliseconds(25));
        b.initiatePlay();
    }
}

//Telo niti za pustanje zvuka
void daemonThread(Bafer& b)
{
    b.play();
}

/*
TODO C pokrenuti 5 niti
Tri su generatori tona sa parametrima: 440.0Hz, 554.356Hz, 660.0Hz frekvencije
5.0s duzina
30% jacina zvuka
jedna je tajmerska nit
i konacna je nit za pustanje zvuka tj. ispis na ekran cije je telo 'daemonThread'
Nit za pustanje zvuka i nit za tajmer su pozadinske
TODO C Pustiti korisnika da prekine program na zahtev tako sto otkuca 'q'
TODO C Pred kraj izvrsavanja, zaustaviti pozadinske niti tako sto im se izda komanda. 
*/
void testirajC(Bafer& b)
{
    bool running = true; 

    thread t1(toneGenerator, 440.0, 5.0, 0.3, ref(b));
    thread t2(toneGenerator, 554.356, 5.0, 0.3, ref(b));
    thread t3(toneGenerator, 660.0, 5.0, 0.3, ref(b));

    thread daemon(daemonThread, ref(b));
    thread timer(timerThread, ref(b), ref(running));

    daemon.detach();
    timer.detach();

    t1.join();
    t2.join();
    t3.join();

    char x;

    do {
        cin >> x;
    } while (x != 'q');

    running = false; 
    b.zavrsi();
}

/*
TODO A Pokrenuti 'testirajA'
TODO B Pokrenuti 'testirajB'
TODO C Pokrenuti 'testirajC'
*/
void testirajSve()
{
    Dijagnostika d;
    Bafer b(d, 80); 

    //testirajA(d);
    //testirajB(b);
    testirajC(b);
}

int main()
{
    // Dopuni
	testirajSve();

    return 0;
}
