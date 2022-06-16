#include <iostream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <list>
#include <random>
#include <chrono>
#include <map>
#include <cstring>
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <csignal>

using namespace std;

// Za seed PRNG-a i za kompaktnost pisanja
typedef chrono::high_resolution_clock hrc_t;
typedef uint32_t u32;

// 4MiB
const size_t MEMORY_CAPACITY = (1 << 22);
const hrc_t::time_point start = hrc_t::now();
const chrono::milliseconds stepInterval(100);
const int PROCESSES = 4;

const char* outputName = "stateFifo";
const char* inputName = "commandFifo";

const int EF_FIRSTFIT = 0;
const int EF_LASTFIT = 1;
const int EF_BESTFIT = 2;
const int EF_WORSTFIT = 3;
const int EF_LENGTH = 4;

const int F_ALLOCATING = 1;
const int F_COMPACTING = 2;
const int F_CULLING = 4;

bool allocationEnabled = true;
bool compactingActive = false;
bool lruActive = false;

int type = EF_FIRSTFIT;

void onAllocationChanged();
void onCompactionChanged();
void onLRUChanged();
void onTypeChanged();

void intHandler(int sig);

mutex mCompaction;
condition_variable cvCompaction;

// Jedan fragment slobodne memorije
struct Fragment
{ 
    u32 loc;		// Pozicija u memoriji
    u32 len;		// Veličina
};

// Pomoćna klasa, slobodno je proširiti
class Diagnostics
{ 
    private:
        mutex m;
        int fdOutput;			// Izlaz
        int fdInput;			// Ulaz
        int waiting; 			// Koliko procesa čeka
        char outBuffer[8192];	// Izlazni bafer
        bool visual;			// Uključen vizualizator
        char unit;				// Jedinica za ispis
        u32 pages;				// Broj stranica
        u32 size;				// Veličina memorije
        u32 contiguous;			// Broj segmenata za redom
		
    public:
        Diagnostics(bool v) : waiting(0), visual(v) 
		{
            if (visual)
			{
                fdInput = open(inputName, O_RDONLY);
                fdOutput = open(outputName, O_WRONLY);
                *((int*)(outBuffer)) = 3072 + 16;

                outBuffer[4] = type;
                outBuffer[5] = waiting;
                outBuffer[6] = F_ALLOCATING;
                outBuffer[7] = unit;

                *((u32*)(outBuffer + 8)) = pages;
                *((u32*)(outBuffer + 12)) = size;
                *((u32*)(outBuffer + 16)) = contiguous;

                for (int i = 20; i < 3092; i++)
                    outBuffer[i] = 0;

				unit = 'b';
                pages = 0;
                size = 0;
                contiguous = 0;
            }
        }

		// SOV-B: VRAĆA BROJ PROCESA KOJI SU U STANJU ČEKANJA
		int koliko_ceka()
		{
			return waiting;
		}

		// Ovoj metodi šaljemo podatke za ispis u gornju liniju
        void reportFreeSpace(u32 newPages, u32 newSize, char newUnit, u32 newContiguous)
		{
            pages = newPages;
            size = newSize;
            unit = newUnit;
            contiguous = newContiguous;
        }

		// Povećava se broj procesa koji čekaju
        void incWaiting() 
		{
            unique_lock<mutex> l(m);
            waiting++;
        }

		// Smanjuje se broj procesa koji čekaju
        void decWaiting() 
		{
            unique_lock<mutex> l(m);
            waiting--;
        }

		// SOV-B: Dodat ispis za dealokaciju pri LRU procesu
		void lruMessage(int id)
		{
			cout << "LRU has deallocated memory from process " << id << "." << endl;
		}

		// Poruka za alokaciju
        void allocateMessage(int pid, u32 loc, u32 len, u32 seg)
		{
            unique_lock<mutex> l(m);
            cout << "Process " << pid << " allocating from " << loc << " to " << loc + len << endl;
            
			if (visual)
			{
                int x = loc / 4096; 
                int ll = len / 4096; 

                for (int i = 0; i < ll; i++) 
				{
                    outBuffer[20 + (x + i)*3 + 0] = 1;
                    outBuffer[20 + (x + i)*3 + 1] = (char)pid;
                    outBuffer[20 + (x + i)*3 + 2] = (char)seg;
                }
            }
        }

		// Poruka za dealokaciju
        void deallocateMessage(int pid, u32 oLoc, u32 oLen, u32 loc, u32 len)
		{
            unique_lock<mutex> l(m);
            cout << "Process " << pid << " asked to deallocate from " << oLoc << " to " << oLoc + oLen << endl;
            cout << "Process " << pid << " deallocated from " << loc << " to " << loc + len << endl;

            if (visual)
			{
                int x = loc / 4096; 
                int ll = len / 4096; 

                for (int i = 0; i < ll; i++)
				{
                    outBuffer[20 + (x + i)*3 + 0] = 0;
                    outBuffer[20 + (x + i)*3 + 1] = 0;
                    outBuffer[20 + (x + i)*3 + 2] = 0;
                }
            }
        }

		// Poruka za dealokaciju pri kompakciji
        void compactionDeallocateMessage(u32 oLoc, u32 oLen, u32 loc, u32 len)
		{
            unique_lock<mutex> l(m);
            cout << "The compacter asked to deallocate from " << oLoc << " to " << oLoc + oLen << endl;
            cout << "The compacter deallocated from " << loc << " to " << loc + len << endl;

            if (visual)
			{
                int x = loc / 4096; 
                int ll = len / 4096; 

                for (int i = 0; i < ll; i++)
				{
                    outBuffer[20 + (x + i)*3 + 0] = 0;
                    outBuffer[20 + (x + i)*3 + 1] = 0;
                    outBuffer[20 + (x + i)*3 + 2] = 0;
                }
            }
        }

		// Poruka za čitanje
        void readMessage(int pid, u32 loc, int seg)
		{
            unique_lock<mutex> l(m);
            cout << "Process " << pid << " reading from location " << loc << " in segment " << seg << endl;
        }

		// Poruka za pisanje
        void writeMessage(int pid, u32 loc, int seg)
		{
            unique_lock<mutex> l(m);
            cout << "Process " << pid << " writing to location " << loc << " in segment " << seg << endl;
        }

		// Poruka za stanje segmenta
        void processStatusMessage(int pid, int seg)
		{
            unique_lock<mutex> l(m);
            cout << "Process " << pid << " has " << seg << " segments." << endl;
        }

		// Poruka za prikaz slobodne memorije
        void printFreeMemoryMap(int pid, u32 amount, list<Fragment>& fm)
		{
            unique_lock<mutex> l(m);
            cout << "Process " << pid << " is waiting for " << amount << " bytes, which is " << amount / 4096 << " 4K pages." << endl;
        }

		// Poruka za kompakciju
        void compactionMessage(int pid, int seg, u32 oBase, u32 len, u32 nBase)
		{
            unique_lock<mutex> l(m);
            cout << "Process " << pid << " and segment " << seg << "of length " << len;
			cout << " bytes is a compaction candidate. To be moved from " << oBase << " to " << nBase << endl;

            if (visual)
			{
                int x = nBase / 4096; 
                int ll = len / 4096; 

                for (int i = 0; i < ll; i++)
				{
                    outBuffer[20 + (x + i)*3 + 0] = 1;
                    outBuffer[20 + (x + i)*3 + 1] = (char)pid;
                    outBuffer[20 + (x + i)*3 + 2] = (char)seg;
                }
            }
        }

		// Ispis na vizualizatoru
        void runOutput()
		{
            while (1 && visual)
			{
                this_thread::sleep_for(stepInterval / 2);

                outBuffer[4] = type;
                outBuffer[5] = waiting;
                outBuffer[6] = 0;
                outBuffer[7] = unit;

                *((u32*)(outBuffer + 8)) = pages;
                *((u32*)(outBuffer + 12)) = size;
                *((u32*)(outBuffer + 16)) = contiguous;

                if (allocationEnabled) outBuffer[6] |= F_ALLOCATING;
                if (compactingActive) outBuffer[6] |= F_COMPACTING; 
                if (lruActive) outBuffer[6] |= F_CULLING; 

                write(fdOutput, outBuffer, 3092);
            }
        }

		// Unos sa konzole
        void runInput()
		{
			// Ako koristimo vizualizator
            if (visual)
			{
                while (1)
				{
                    char inbuf[2];
                    read(fdInput, inbuf, 2);

                    if (inbuf[0] == 'q' && inbuf[1] == 'q')
					{
                        intHandler(0);
                        return;
                    }
					else if (inbuf[0] == 'c')
					{
                        type = (int)inbuf[1];
                        onTypeChanged();
                    } 
					else if (inbuf[0] == 'a')
					{
                        if (inbuf[1] == 0)
                            allocationEnabled = false;
                        else if(inbuf[2] == 1)
                            allocationEnabled = true;
                        else
                            allocationEnabled = !allocationEnabled;
                        
                        onAllocationChanged();
                    }
					else if(inbuf[0] == 'd')
					{
                        if (inbuf[1] == 0)
                            compactingActive = false;
                        else if (inbuf[2] == 1)
                            compactingActive = true;
                        else
                            compactingActive = !compactingActive;
                        
                        onCompactionChanged();
                    }
					else if (inbuf[0] == 'n')
					{
                        if (inbuf[1] == 0)
                            lruActive = false;
                        else if (inbuf[2] == 1)
                            lruActive = true;
                        else
                            lruActive = !lruActive;
                        
                        onLRUChanged();
                    }
                }       
            }
			// Ako ne koristimo vizualizator
			else
			{
                while (1)
				{
                    char c;
                    cin >> c;

                    if (c == 'q')
					{
                        intHandler(0);
                        return;
                    }
					else if (c == 'c')
					{
                        type = (type + 1) % EF_LENGTH;
                        {
                            unique_lock<mutex> l(m);
                            cout << "Mod je sada: " << type << endl;
                        }
                        onTypeChanged();
                    }
					else if (c == 'a')
					{
                        allocationEnabled = !allocationEnabled;
                        {
                            unique_lock<mutex> l(m);
                            cout << "Alokacije je sada: " << ((allocationEnabled) ? "ON" : "OFF") << endl;
                        }
                        onAllocationChanged();
                    }
					else if (c == 'd')
					{
                        compactingActive = !compactingActive;
                        {
                            unique_lock<mutex> l(m);
                            cout << "Kompakcija je sada: " << ((compactingActive) ? "ON" : "OFF") << endl;
                        }
                        onCompactionChanged();
                    }
					else if (c == 'n')
					{
                        lruActive = !lruActive;
                        {
                            unique_lock<mutex> l(m);
                            cout << "LRU je sada: " << ((compactingActive) ? "ON" : "OFF") << endl;
                        }
                        onLRUChanged();
                    }
                }
            }
        }
};

// STRUKTURA ZA SOV-B 
typedef chrono::time_point<chrono::system_clock> poslednji_pristup; 

struct LeastRecentlyUsed
{
	mutex m;
	condition_variable cv;

	poslednji_pristup pp = chrono::system_clock::now();

	// Polja potrebna za poziv dealokacije
	u32 identfikator = 0;
	u32 proces = 0;
};

// Globalna promenljiva u koju se smešta najstariji segment koji će se dealocirati
LeastRecentlyUsed LRU;

void setLRU(poslednji_pristup novi, u32 id, u32 p) 
{
	LRU.pp = novi;
	LRU.identfikator = id;
	LRU.proces = p;
}

// Jedna stavka u tabeli segmenta
// SOV-B: struktura je proširena sa poljem pp
struct TableEntry
{ 
    u32 base = 0;			// Prvi segment
    u32 len = 0;			// Veličina
    int m = -1;				// Koji je u listi

	// Polje pamti kada je poslednji put pristupljeno segmentu
	poslednji_pristup pp = chrono::system_clock::now();
};

// Struktura koja se koristi u memoriji
struct ReusableMutex
{
    bool taken = false;
    mutex m;
};

// Tabela segmenata za neki proces
class SegmentTable
{ 
	private:
		// Tabela segmenata za proces 
        map<int, TableEntry> table;

		// setLRU(segmentTables[i].table.begin()->second.pp, segmentTables[i].table.begin()->first, i);
        
    	// Kad nađe najveći ID, vrati tu vrednost uvećanu za 1. To garantuje jedinstven ID. 
        int getNewID()
		{
            if (table.empty()) return 0;

            int id = 0;
            for (pair<int, TableEntry> p : table)
                if (p.first > id) id = p.first;
            
            return ++id;
        }

        friend class SystemMemory;

    public:
        SegmentTable() {}

		// Ubacivanje segmenta
        int insertSegment(TableEntry tableEntry)
		{
            int id = getNewID();
            table.insert(pair<int, TableEntry>(id, tableEntry));
            return id;
        }

		// Brisanje segmenta
        void deleteSegment(int id)
		{
            table.erase(id);
        }

		// Informacije o traženom segmentu procesa
        TableEntry& getEntry(int id)
		{
            return table[id];
        }

        /* 
		Vraća nasumični validni ID iz date tabele. Nije ključno za rešenje, ali čini programiranje procesa lakšim. 
		Prosleđujemo random_engine zato što je njegovo generisanje jako sporo, i stoga ne treba da se pravi tokom izvršavanja programa. 
		Naša arhitektura ovde ima poseban generator za svaki proces budući da generisanje slučajnih brojeva predstavlja 
		operaciju pisanja (menja interno stanje) te stoga se mora ili sinhronizovati ili obezbediti jedan takav po niti. 
		Mi smo uradili ovo drugo, zbog brzine. 
        */
        int getRandomID(default_random_engine& gen)
		{
            uniform_int_distribution<int> d(0, table.size() - 1);
            int x = d(gen);
            int i = 0;

            for (pair<int, TableEntry> p : table)
			{
                if (i == x) return p.first;
                i++;
            }
            cerr << "Random segment generation failed" << endl;

            return 0;
        }
};

// Klasa koja simulira OS
class SystemMemory
{
	 private:
        char *mem;								// Memorija (delić memorije)
        bool terminal;							// Gašenje procesa
        Diagnostics& cadh;						// Dijagnostika za ispise
        map<int, SegmentTable> segmentTables;	// Mapa tabela segmenata
        list<Fragment> freeMemory;				// Lista slobodnih segmenata
        mutex mAllocate;
        condition_variable cvFree;
        ReusableMutex mSegments[1024];			// 1024 dostupna segmenta
        
		// setLRU(segmentTables[i].table.begin()->second.pp, segmentTables[i].table.begin()->first, i);

		// Pronalazimo slobodan segment
        int getFreeMutex()
		{
            for (int i = 0; i < 1024; i++)
                if (!mSegments[i].taken) return i;
            
            return -1;
        }

    public:
        SystemMemory(size_t capacity, Diagnostics& d) : terminal(false), cadh(d) 
		{
            mem = (char*)malloc(capacity);

            if (mem == NULL)
			{
                perror("Could not initialize memory.");
                exit(1);
            }

            // Na početku je sva memorija slobodna što znači da je naša evidencija slobodne memorije
			// od 1 odsečka koji počinje od 0 i veliki je koliko uopšte ima memorije. 
            freeMemory.push_back((Fragment) {.loc = 0, .len = (u32)capacity});
        }
		
		// Moramo osloboditi zauzeto
        ~SystemMemory()
		{	
            free(mem);
        }

        // Operacija čitanja. Primetite adresu koja je segment + logička adresa
        char read(int processID, int segmentID, u32 logicalAddress)
		{
            unique_lock<mutex> l(mSegments[segmentTables[processID].getEntry(segmentID).m].m);

			// Ispisuje na ekran
            cadh.readMessage(processID, logicalAddress, segmentID);

            int loc = segmentTables[processID].getEntry(segmentID).base;

			// SOV-B: PRISTUPA SE MEMORIJI -> ZAPISUJEMO U SEGMENT
			segmentTables[processID].getEntry(segmentID).pp = chrono::system_clock::now();

			//Provera prava pristupa
            if (logicalAddress >= segmentTables[processID].getEntry(segmentID).len)
			{ 
                cerr << "Internal segmentation violation." << endl;
                exit(2);
            }

			// Očitavanje vrednosti uz translaciju
            return mem[loc + logicalAddress]; 
        }

		// Operacija pisanja
        void write(int processID, int segmentID, u32 logicalAddress, char value)
		{
            unique_lock<mutex> l(mSegments[segmentTables[processID].getEntry(segmentID).m].m);

			// Ispisuje na ekran
            cadh.writeMessage(processID, logicalAddress, segmentID);

            int loc = segmentTables[processID].getEntry(segmentID).base;

			// SOV-B: PRISTUPA SE MEMORIJI -> ZAPISUJEMO U SEGMENT
			segmentTables[processID].getEntry(segmentID).pp = chrono::system_clock::now();

            if (logicalAddress >= segmentTables[processID].getEntry(segmentID).len)
			{
                cerr << "Internal segmentation violation." << endl;
                exit(2);
            }

            mem[loc + logicalAddress] = value;
        }
        
		// Alokacija memorije za proces
        int allocate(int processID, u32 amount)
		{
            if (!segmentTables.count(processID))
			{ 
				// U slučaju da ovo zovemo prvi put za dati proces, ubacujemo novu tabelu
                {
                    unique_lock<mutex> l(mAllocate);
                    segmentTables.insert(pair<int, SegmentTable>(processID, SegmentTable()));
                }

				// Ubacivanje menja deljenu klasu te je stavljamo u isključiv region.
                int loc = -1;

                {
					// I ovde zaključavamo: rad sa evidencijom slobodne memorije je takođe rad sa deljenim resursom
                    unique_lock<mutex> l(mAllocate);

					// U slučaju čekanja pišemo kako trenutno izgleda memorija.
                    while ((loc = findFree(amount)) < 0)
					{
                        cadh.printFreeMemoryMap(processID, amount, freeMemory); 
                        cadh.incWaiting();
                        cvFree.wait(l);
                        cadh.decWaiting();

						// Proveravamo da li nas je probudio neuspeh u alokaciji, no proces gašenja.
                        if (terminal) return -1; 
                    }
                }

                // Sada kada imamo odakle počinje naša slobodna memorija (u loc) napravimo segment i stavimo ga u evidenciju
                int ret = segmentTables[processID].insertSegment((TableEntry) {.base = (u32)loc, .len  = amount, .m = getFreeMutex(), .pp=chrono::system_clock::now()});
                cadh.allocateMessage(processID, (u32)loc, amount, ret);

				// SOV-A: AŽURIRANJE ISPISA O STANJU MEMORIJE
				gornja_linija();

				// Vraćamo indeks segmenta koji smo alocirali
                return ret; 
            }
			else
			{
                // Isto kao gore, ali bez potrebe da ubacijemo tabelu budući da već postoji
                int loc = -1;

                {
                    unique_lock<mutex> l(mAllocate);

                    while ((loc = findFree(amount)) < 0)
					{
                        cadh.printFreeMemoryMap(processID, amount, freeMemory);
                        cadh.incWaiting();
                        cvFree.wait(l);
                        cadh.decWaiting();
                        if (terminal) return -1;
                    }                    
                }

                int ret = segmentTables[processID].insertSegment((TableEntry) {.base = (u32)loc, .len  = amount, .m = getFreeMutex(), .pp=chrono::system_clock::now()});
                cadh.allocateMessage(processID, (u32)loc, amount, ret);

				// SOV-A: AŽURIRANJE ISPISA O STANJU MEMORIJE
				gornja_linija();

                return ret;
            }
        }

		// Dealokacija segmenta memorije za proces
        void deallocate(int processID, int segmentID)
		{
            int loc = segmentTables[processID].getEntry(segmentID).base;
            int len = segmentTables[processID].getEntry(segmentID).len;

            {
                unique_lock<mutex> l(mAllocate);
                unique_lock<mutex> ll(mSegments[segmentTables[processID].getEntry(segmentID).m].m);
                list<Fragment>::iterator it;
                
				/*
				Ovde se implementira algoritam koji smo diskutovali na času. 
				Ako oslobađamo regione fizičke memorije bitno je detektovati situaciju 
				gde imamo fizičke regione koji se dodiruju, budući da njih treba spojiti. 
				Ako je novoslobođeni segment *, a stari slobodni segmenti #, dok su 
				zauzeti delovi predstavljeni praznim prostorom naše opcije su:
				
				###### **** ######## prelazi u ###### **** ########
				######****  ######## prelazi u ##########  ########
				######  ****######## prelazi u ######  ************
				######******######## prelazi u ####################
                */

                for (it = freeMemory.begin(); it != freeMemory.end(); it++)
                    if (it->loc > loc) break;
                
                if (it == freeMemory.begin())
				{
                    freeMemory.push_front((Fragment) {.loc = (u32)loc, .len = (u32)len});
                    it = freeMemory.begin();

                    list<Fragment>::iterator next = it;
                    next++;

                    if (next != freeMemory.end())
					{
                        if (it->loc + it->len == next->loc)
						{
                            it->len = it->len + next->len;
                            freeMemory.erase(next);
                            cadh.deallocateMessage(processID, (u32)loc, len, (u32)loc, it->len);
                            cvFree.notify_all();                                   
                        }
						else
						{
                            cadh.deallocateMessage(processID, (u32)loc, len, (u32)loc, len);
                            cvFree.notify_all();                                   
                        }
                    }
					else
					{
                        cadh.deallocateMessage(processID, (u32)loc, len, (u32)loc, len);
                        cvFree.notify_all();
                    }
                }
				else
				{
                    it--;
                    if ((it->loc + it->len) == loc)
					{
                        it->len = it->len + len;
                        list<Fragment>::iterator next = it;
                        next++;

                        if (next != freeMemory.end())
						{
                            if (it->loc + it->len == next->loc)
							{
                                it->len = it->len + next->len;
                                freeMemory.erase(next);
                                cadh.deallocateMessage(processID, loc, len, it->loc, it->len);
                                cvFree.notify_all();
                            }
							else
							{
                                cadh.deallocateMessage(processID, loc, len, it->loc, it->len);
                                cvFree.notify_all();
                            }
                        }
						else
						{
                            cadh.deallocateMessage(processID, loc, len, it->loc, it->len);
                            cvFree.notify_all();
                        }
                    }
					else
					{
                        it++;
                        it = freeMemory.insert(it, (Fragment) {.loc = (u32)loc, .len = (u32)len});

                        list<Fragment>::iterator next = it;
                        next++;

                        if (next != freeMemory.end())
						{
                            if (it->loc + it->len == next->loc)
							{
                                it->len = it->len + next->len;
                                freeMemory.erase(next);
                                cadh.deallocateMessage(processID, loc, len, it->loc, it->len);
                                cvFree.notify_all();                                    
                            }
							else
							{
                                cadh.deallocateMessage(processID, loc, len, it->loc, it->len);
                                cvFree.notify_all();
                            }
                        }
						else
						{
                            cadh.deallocateMessage(processID, loc, len, it->loc, it->len);
                            cvFree.notify_all();
                        }
                    }
                }
                mSegments[segmentTables[processID].getEntry(segmentID).m].taken = false;
                segmentTables[processID].deleteSegment(segmentID);	
            }

			// SOV-A: AŽURIRANJE ISPISA O STANJU MEMORIJE
			gornja_linija();
        }

        // Ovo je ono što u stvari zove proces, ovo ga samo usmeri gde treba
        int getRandomID(int processID, default_random_engine& gen)
		{
            return segmentTables[processID].getRandomID(gen);
        }

        // Ovim zaustavljamo niti koje čekaju u okviru OS. 
        void terminate()
		{
            terminal = true;
            cvFree.notify_all();
            cvCompaction.notify_all();
        }

		// Kompakcija segmenata memorije procesa
        void compactionProcess()
		{
            bool foundCandidate = false;
            int cProcess = -1;
            int cSegment = -1;
            int cSegmentM = -1;

            Fragment target;
            {
                unique_lock<mutex> l(mAllocate);

                for (pair<int, SegmentTable> p : segmentTables)
				{
                    for (pair<int, TableEntry> pp : p.second.table)
					{
                        int inefficiency = 0;
                        int gapBack = 0;
                        int gapForward = 0;
                        int targetInefficiency = MEMORY_CAPACITY;

                        list<Fragment>::iterator it = freeMemory.begin();
                        list<Fragment>::iterator targetFragment = freeMemory.begin(); 

                        for (Fragment f : freeMemory)
						{
                            if (((int)f.len - (int)pp.second.len) < targetInefficiency && ((int)f.len - (int)pp.second.len) > 0)
							{
                                targetInefficiency = (int)f.len - (int)pp.second.len;
                                targetFragment = it;
                            }

                            if (f.loc + f.len == pp.second.base)
                                gapBack = f.len;
                            else if (f.loc + f.len == pp.second.base + pp.second.len)
                                gapForward = f.len;
                            
                            it++;
                        }

                        inefficiency = gapBack + gapForward;

                        if (inefficiency > targetInefficiency + 8192)
						{
                            if (targetFragment->len == pp.second.len)
							{
                                target = *targetFragment;
                                freeMemory.erase(targetFragment);
                                foundCandidate = true;
                                cProcess = p.first;
                                cSegment = pp.first;
                                cSegmentM = pp.second.m;
                                mSegments[pp.second.m].m.lock();
                                break;
                            }
							else
							{
                                target = *targetFragment;
                                target.len = pp.second.len;
                                targetFragment->len -= pp.second.len;
                                targetFragment->loc += pp.second.len;
                                foundCandidate = true;
                                cProcess = p.first;
                                cSegment = pp.first;
                                cSegmentM = pp.second.m;
                                mSegments[pp.second.m].m.lock();
                                break;
                            }
                        }
                        if (foundCandidate) break;
                    }
                    if (foundCandidate) break;
                }
            }

            if (foundCandidate)
			{
                this_thread::sleep_for(chrono::milliseconds(50)); 

                TableEntry* te = &((segmentTables[cProcess]).table[cSegment]);
                cadh.compactionMessage(cProcess, cSegment, te->base, te->len, target.loc);

                for (int i = 0; i < te->len; i++)
                    mem[target.loc + i] = mem[te->base + i];

                int loc = te->base;
                int len = te->len;

                te->base = target.loc;
                mSegments[cSegmentM].m.unlock();
                {
                    unique_lock<mutex> l(mAllocate);
                    list<Fragment>::iterator it;

                	/*
					Ovde se implementira algoritam koji smo diskutovali na času. 
					Ako oslobađamo regione fizičke memorije bitno je detektovati situaciju 
					gde imamo fizičke regione koji se dodiruju, budući da njih treba spojiti. 
					Ako je novoslobođeni segment *, a stari slobodni segmenti #, dok su zauzeti 
					delovi predstavljeni praznim prostorom naše opcije su:

					###### **** ######## prelazi u ###### **** ########
					######****  ######## prelazi u ##########  ########
					######  ****######## prelazi u ######  ************
					######******######## prelazi u ####################
                	*/

                    for (it = freeMemory.begin(); it != freeMemory.end(); it++)
                        if (it->loc > loc) break;
                    
                    if (it == freeMemory.begin())
					{
                        freeMemory.push_front((Fragment) {.loc = (u32)loc, .len = (u32)len});
                        it = freeMemory.begin();

                        list<Fragment>::iterator next = it;
                        next++;
                        
						if (next != freeMemory.end())
						{
                            if (it->loc + it->len == next->loc)
							{
                                it->len = it->len + next->len;
                                freeMemory.erase(next);
                                cadh.compactionDeallocateMessage((u32)loc, len, (u32)loc, it->len);
                                cvFree.notify_all();                                   
                            }
							else
							{
                                cadh.compactionDeallocateMessage((u32)loc, len, (u32)loc, len);
                                cvFree.notify_all();                                   
                            }
                        }
						else
						{
                            cadh.compactionDeallocateMessage((u32)loc, len, (u32)loc, len);
                            cvFree.notify_all();
                        }
                    }
					else
					{
                        it--;
                        if ((it->loc + it->len) == loc)
						{
                            it->len = it->len + len;
                            list<Fragment>::iterator next = it;
                            next++;

                            if (next != freeMemory.end())
							{
                                if (it->loc + it->len == next->loc)
								{
                                    it->len = it->len + next->len;
                                    freeMemory.erase(next);
                                    cadh.compactionDeallocateMessage(loc, len, it->loc, it->len);
                                    cvFree.notify_all();
                                }
								else
								{
                                    cadh.compactionDeallocateMessage(loc, len, it->loc, it->len);
                                    cvFree.notify_all();
                                }
                            }
							else
							{
                                cadh.compactionDeallocateMessage(loc, len, it->loc, it->len);
                                cvFree.notify_all();
                            }
                        }
						else
						{
                            it++;
                            it = freeMemory.insert(it, (Fragment) {.loc = (u32)loc, .len = (u32)len});

                            list<Fragment>::iterator next = it;
                            next++;

                            if (next != freeMemory.end())
							{
                                if (it->loc + it->len == next->loc)
								{
                                    it->len = it->len + next->len;
                                    freeMemory.erase(next);
                                    cadh.compactionDeallocateMessage(loc, len, it->loc, it->len);
                                    cvFree.notify_all();                                    
                                }
								else
								{
                                    cadh.compactionDeallocateMessage(loc, len, it->loc, it->len);
                                    cvFree.notify_all();
                                }
                            }
							else
							{
                                cadh.compactionDeallocateMessage(loc, len, it->loc, it->len);
                                cvFree.notify_all();
                            }
                        }
                    }
                }
            }
        }

        // Pokretanje kompakcije segmenata memorije procesa
		void compactionRunner()
		{
            while (1)
			{
                {
                    unique_lock<mutex> l(mCompaction);

                    while (!compactingActive)
                        cvCompaction.wait(l);
                    
                    if (terminal) return;
                }

                compactionProcess();
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }

		/*
		Implementacija first fit algoritma. Prvi odsečak koji je dovoljno veliki se odabira. 
		Ako je taman veličine ceo se izvozi. Ako je veći, mrvi se tako što se početak odsečka 
		pomera unapred za zauzet prostor i dužina se adekvatno smanjuje, a kao povratna vrednost 
		se daje pređašnja adresa početka tog odsečka. 
        */
        int findFree(u32 amount)
		{
            if (type == EF_FIRSTFIT)
			{
                for (list<Fragment>::iterator it = freeMemory.begin();it != freeMemory.end();it++)
				{
                    if (it->len == amount)
					{
                        int ret = (int)it->loc;
                        freeMemory.erase(it);
                        return ret;
                    } 
					else if (it->len > amount)
					{
                        int ret = (int)it->loc;
                        it->loc = it->loc + amount; 
                        it->len = it->len - amount;
                        return ret; 
                    }
                }
            }
			else if (type == EF_LASTFIT)
			{
                for (list<Fragment>::reverse_iterator it = freeMemory.rbegin();it != freeMemory.rend();it++)
				{
                    if (it->len == amount)
					{
                        int ret = (int)it->loc;
                        freeMemory.erase(std::next(it).base());
                        return ret;
                    }
					else if (it->len > amount)
					{
                        int ret = (int)it->loc;
                        it->loc = it->loc + amount; 
                        it->len = it->len - amount;
                        return ret; 
                    }
                }
            }
			else if (type == EF_BESTFIT)
			{
                list<Fragment>::iterator best = freeMemory.begin();

                for (list<Fragment>::iterator it = freeMemory.begin(); it != freeMemory.end(); it++)
				{
                    long odiff = (long)best->len - (long)amount;
                    long diff = (long)it->len - (long)amount;

                    if ((diff < odiff) && (diff >= 0))
                        best = it;
					else if ((odiff < 0) && (diff >= 0))
                        best = it;
                    
                }
                if (best->len == amount)
				{
                    int ret = (int)best->loc;
                    freeMemory.erase(best);
                    return ret;
                }
				else if (best->len > amount)
				{
                    int ret = (int)best->loc;
                    best->loc = best->loc + amount; 
                    best->len = best->len - amount;
                    return ret; 
                }
            }
			else if (type == EF_WORSTFIT)
			{
                list<Fragment>::iterator best = freeMemory.begin();
                for (list<Fragment>::iterator it = freeMemory.begin();it != freeMemory.end();it++)
				{
                    long odiff = (long)best->len - (long)amount;
                    long diff = (long)it->len - (long)amount;

                    if ((diff > odiff) && (diff >= 0))
                        best = it;
                }

                if (best->len == amount)
				{
                    int ret = (int)best->loc;
                    freeMemory.erase(best);
                    return ret;
                }
				else if (best->len > amount)
				{
                    int ret = (int)best->loc;
                    best->loc = best->loc + amount; 
                    best->len = best->len - amount;
                    return ret; 
                }
            }

            return -1;
        }

		// SOV-A: FUNKCIONALNOST ZA GORNJU LINIJU VIZUALIZATORA
		/*
		- Obezbediti da se u vizuelizatoru u gornjoj liniji stalno ispisuje koliko ima ukupno 
		slobodne memorije, i to u 4K stranicama i u jedinici koja odgovara veličini memorije (B, KB, MB)
		(jedinica se bira tako da se uzme najveća jedinica gde je vrednost i dalje barem jedan)
		- Treba da se ispiše veličina (u stranicama od 4K) najvećeg kontinualnog slobodnog regiona u memoriji
		*/
		// Funkcija se poziva na kraju metoda za alokaciju i dealokaciju i prikazuje novo stanje u memoriji
		void gornja_linija()
		{
			// Konstantan iterator - sa njim možemo samo da čitamo podatke
			list<Fragment>::const_iterator iterator = freeMemory.cbegin();

			u32 noveStranice = 0;
			u32 novaVelicina = 0;
			char novaJedinica;
			u32 novaNajduzaCelina = iterator->len;

			// Prolazimo kroz celu slobodnu memoriju i:
			// - sabiramo dužine svakog segmenta kako bi dobili ukupnu dužinu svih slobodnih segmenata
			// - pronalazimo koji segment slobodne je najduži
			for (; iterator != freeMemory.cend(); iterator++)
			{
				novaVelicina += iterator->len;

				if (iterator->len > novaNajduzaCelina)
					novaNajduzaCelina = iterator->len;
			}

			// Delimo sa 4096 da bi smo od ukupnog broj bajtova dobili stranice od 4 kilobajta
			noveStranice = novaVelicina / 4096;
			novaNajduzaCelina /= 4096;

			if (novaVelicina < 1024)
			{
				novaJedinica = 'b';
			}
			else if (novaVelicina >= 1024 && novaVelicina < 1024*1024)
			{
				novaJedinica = 'k';
				novaVelicina /= 1024;
			}
			else if (novaVelicina >= 1024*1024)
			{
				novaJedinica = 'm';
				novaVelicina /= 1024*1024;
			}

			// Dijagnostika prikazuje ažurirane podatke o memoriji 
			cadh.reportFreeSpace(noveStranice, novaVelicina, novaJedinica, novaNajduzaCelina);
		}

		// SOV-B: IMPLEMENTIRATI MEHANIZAM KOJI ODRŽAVA GLOBALNU TABELU SEGMENATA SVIH PROCESA
		/*
		- U slučaju da je mehanizam LRU-a aktiviran preko vizuelizatora (što će pokazati globalni flag lruActive), 
		i da su svi procesi u stanju čekanja, ovaj proces dealocira onaj segment sa najstarijom referencom 
		(tj. iz koga je najdavnije čitano odnosno u koji je najdavnije pisano) u skladu sa LRU algoritmom
		- Onda obavesti procese koji čekaju na slobodnu memoriju, čeka 30ms, 
		i opet proverava (u beskonačnoj petlji) da li su svi procesi u stanju čekanja
		- Sme se koristiti ili LRU ili NFU u implementaciji, u skladu sa tim šta nam je lakše
		*/
		// Funkcija se pokreće kao detach nit
		void tabela_segmenata()
		{
			unique_lock<mutex> l(LRU.m);

			while (1)
			{
				// Ako LRU nije aktivan, cekaj
				while (!lruActive) LRU.cv.wait(l);

				// Ukoliko je LRU aktivan i ukoliko svi procesi cekaju
				if (cadh.koliko_ceka() == PROCESSES)
				{
					// Postavi da je LRU nulti segment nultog procesa
					setLRU(segmentTables[0].table.begin()->second.pp, segmentTables[0].table.begin()->first, 0);

					// Prolazimo kroz sve segmente svih procesa kako bi pronasli onaj sa najstarijim vremenom poslednjeg pristupa
					for (int i = 0; i < PROCESSES; i++)
					{
						for (auto it = segmentTables[i].table.begin(); it != segmentTables[i].table.end(); it++)
						{
							// Ako sat vrati vece vreme, to znaci da je proces mlađi
							if (LRU.pp > (it->second.pp))
								setLRU(segmentTables[i].table.begin()->second.pp, segmentTables[i].table.begin()->first, i);
						}
					}
					
					deallocate(LRU.proces, LRU.identfikator);
					cadh.lruMessage(LRU.proces);
					cvFree.notify_one();
				}

				// Posle 30 ms ponovo proveri da li su svi procesi u stanju cekanja
				this_thread::sleep_for(chrono::milliseconds(30));
			}
		}
};

// Sinhronizovani brojač da bi procesi dobili ID brojeve koji su jedinstveni
class IDManager
{
    private:
        int process;
        mutex m;

    public:
        IDManager() : process(0) {}

        int getProcessID()
		{
            unique_lock<mutex> l(m);
            return process++; 
        }
};

class Process
{
	private:
        int id;
        Diagnostics& cadh;
        default_random_engine generator;
        SystemMemory& systemMemory;
        map<int, u32> localTable;
        uniform_int_distribution<int> segmentSizeDistribution; 
        uniform_int_distribution<int> stepDistribution;
        bool terminate; 

    public:
        Process(IDManager& idm, SystemMemory& sm, Diagnostics& dd) : 
		systemMemory(sm), segmentSizeDistribution(1,25), stepDistribution(1,100), terminate(false), cadh(dd)
		{
               id = idm.getProcessID();
               hrc_t::duration d = hrc_t::now() - start;
               auto x = d.count();
               x = x ^ (id << 7);	
			   // Dozvoljavamo bitovima id-a da utiču na seed vrednost generatora slučajnih brojeva ovog procesa. 
			   // Ovo omogućava da slučajne vrednosti budu maksimalno različite čak i ako su početna vremena veoma bliska. 
               generator.seed(x);
        }
        
		void run()
		{
            cout << "Running process with ID" << id << endl;
            // Pravljenje početnog, permanentnog segmenta. 
            u32 amount = segmentSizeDistribution(generator) * 4096;
            int permanentSegment = systemMemory.allocate(id, amount);
            localTable.insert(pair<int, u32>(permanentSegment, amount));

            while (1)
			{
                if (terminate) return;

				// Vrednost od 1 do 100 što nam omogućava da precizno definišemo
				// šanse za različite korake simulacije
                int step = stepDistribution(generator);

                if (step < 7)
				{
                    if (!allocationEnabled) continue;

                    // Veličina za alokaciju koja je umnožak 4096. 
                    u32 amount = segmentSizeDistribution(generator) * 4096;
                    int seg = systemMemory.allocate(id, amount);

					//Dobijamo -1 samo ako je neko pozvao terminate dok smo mi čekali. 
                    if (seg < 0)
					{ 
                        terminate = true;
                        continue;
                    }

                    localTable.insert(pair<int, u32>(seg, amount));
                }
				else if (step >= 7 && step < 10)
				{
                    int seg = systemMemory.getRandomID(id, generator);

					// Ne dozvoljavamo da se oslobodi permanentni segment
                    if (seg == permanentSegment) continue; 

                    systemMemory.deallocate(id, seg);
                    localTable.erase(seg);                    
                }
				else if (step >= 10 && step < 80)
				{
                    int seg = systemMemory.getRandomID(id, generator);
                    uniform_int_distribution<u32> sd(0, localTable[seg] - 1);
                    u32 logicalAddress = sd(generator);
                    systemMemory.read(id, seg, logicalAddress);
                }
				else if (step >= 80 && step < 101)
				{
                    int seg = systemMemory.getRandomID(id, generator);
                    uniform_int_distribution<u32> sd(0, localTable[seg] - 1);
                    uniform_int_distribution<int> dd(-1000000, 1000000);
                    u32 logicalAddress = sd(generator);
                    int data = dd(generator);
                    systemMemory.write(id, seg, logicalAddress, data);
                }
                else
				{
                    cerr << "Impossible step value" << endl;
                    exit(3);
                }

                cadh.processStatusMessage(id, localTable.size());
                this_thread::sleep_for(stepInterval);
            }
        }

        void doTerminate()
		{
            terminate = true;
        }
};

// Nit koja omogućava da se proces izvršava
void processRunner(Process& p)
{
    p.run();
}

void inputRunner(Diagnostics& d)
{
    d.runInput();
}

void outputRunner(Diagnostics& d)
{
    d.runOutput();
}

// Globalni pokazivači da bi mogli da pristupimo ključnim strukturama programa iz obrađivača signala.
SystemMemory* pMem = NULL;
Process* processes = NULL;

// Obrađivač signala koji se poziva kada se pritisne CTRL+C
// Terminiše sve niti
void intHandler(int sig)
{
    cout << "Terminating all threads... " << endl;

    if (pMem == NULL) exit(10);
    if (processes == NULL) exit(11); 

    pMem->terminate();

    for(int i = 0; i < PROCESSES;i++)
        processes[i].doTerminate();
}

// F4 - uključuje/isključuje alokaciju (allocationEnabled)
void onAllocationChanged()
{
	
}

// F3 - signal za kompakciju memorije
void onCompactionChanged()
{
    unique_lock<mutex> l(mCompaction);
    cvCompaction.notify_one();
}

// F2 - promena algoritma alokacije
// tip alokacije (EF_FIRSTFIT, EF_LASTFIT, EF_BESTFIT, EF_WORSTFIT)
void onTypeChanged()
{
	
}

// F5 - uključuje/isključuje LRU mehanizam
// SOV-B: HENDLER ZA PROMENU FLEGA ZA LRU
void onLRUChanged()
{
	// Ako je LRU aktivan, obavesti LRU.cv o tome
	if (lruActive)
	{
		unique_lock<mutex> l(LRU.m);
		LRU.cv.notify_one();
	}
}

// SOV-B: NIT KOJA ĆE POKRENUTI LRU PROCES
void lruRunner(SystemMemory &sm)
{
	sm.tabela_segmenata();
}

void compactionRunner(SystemMemory& sysmem)
{
    sysmem.compactionRunner();
}

int main(int argc, char** argv)
{
    bool visual = true;

    if (argc == 2) if (argv[1][0] == 't') visual = false;
    
    Diagnostics d(visual);
    SystemMemory mem(MEMORY_CAPACITY, d);
    pMem = &mem;
    signal(SIGINT, intHandler);
    IDManager idm;

    thread inThread(inputRunner, ref(d));
    thread outThread(outputRunner, ref(d));
    thread compactionThread(compactionRunner, ref(mem));

    compactionThread.detach();
    inThread.detach();
    outThread.detach();

    Process processList[PROCESSES] {Process(idm, mem, d), Process(idm, mem, d), Process(idm, mem, d), Process(idm, mem, d)}; 
    thread threadList[PROCESSES];

	// SOV-B NIT ZA LRU PROCES
	thread lruNit(lruRunner, ref(mem));
	lruNit.detach();

    for (int i = 0; i < PROCESSES; i++)
        threadList[i] = thread(processRunner, ref(processList[i]));
    
    processes = processList;

    for (int i = 0; i < PROCESSES; i++)
        threadList[i].join();
    
    return 0;
}
