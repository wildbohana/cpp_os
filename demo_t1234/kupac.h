#ifndef KUPAC_H_INCLUDED
#define KUPAC_H_INCLUDED

#include <mutex>

class Kupac 
{
    protected:
        mutex term;
    public:
        /*
        @param id - r. br. kupca
        */
        virtual void prilazi_kasama(int id) 
		{
            unique_lock<mutex> l(term);
            cout << "Kupac " << id << " dosao u radnju" << endl;
        }

        /*
        @param id - r. br. kupca
        @param kasa - indeks kase
        @param u_redu - r. br. kupca u redu cekanja
        */
        virtual void ceka(int id, int kasa, int u_redu) 
		{
            unique_lock<mutex> l(term);
            cout << "Kupac " << id << " ceka na kasi " << kasa << " kao " << u_redu << ". u redu" << endl;
        }

        /*
        @param id - r. br. kupca
        @param kasa - indeks kase
        */
        virtual void kupuje(int id, int kasa) 
		{
            unique_lock<mutex> l(term);
            cout << "Kupac " << id << " zapocinje kupovinu na kasi " << kasa << endl;
        }

        /*
        @param id - r. br. kupca
        @param kasa - indeks kase
        */
        virtual void odlazi(int id, int kasa) 
		{
            unique_lock<mutex> l(term);
            cout << "Kupac " << id << " je kupio sta je hteo i odlazi" << endl;
        }

        /*
        @param kasa1 - indeks kase koja se deaktivira
        @param kasa2 - indeks kase koja se aktivira
        */
        virtual void smena_kasa(int kasa1, int kasa2) 
		{
            unique_lock<mutex> l(term);
            cout << "[EKSA] Kasa " << kasa1 << " prestaje sa radom, otvara se kasa " << kasa2 << endl;
        }
};

#endif // KUPAC_H_INCLUDED
