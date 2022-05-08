/*
Deo A:
Napisati program koji konkurentno vrsi proveru da li je redosled elemenata jednog vektora obrnut redosled drugog vektora.
U zaglavlju deo_a.h treba implementirati metode jesu_li_isti_elementi i proveri_obrnutost.
jesu_li_isti_elementi:
    proverava nad jednim segmentom da li je redosled jednog vektora obrnut drugi.
proveri_obrnutost:
    deli posao provere obrnutosti elemenata na vise niti i to tako da se nitima dodeljuju delovi posla na slucajan nacin,
    od 1-og do 5 elemenata, tako da ukupan broj niti zavisi od slucaja do slucaja.
###########################################################################################################
Deo B:
Modeluje se dolazak kupaca na kase u market. Market ima N kasa i pritom radi samo M (M<N).
Kupci pri dolasku zauzimaju prvu slobodnu kasu koja radi i slobodna je. Ako nema kase koja radi i slobodna je, staju u red
na kasu koja radi a ima najmanji red cekanja. Nema menjanja redova. Kupovina traje 1s.
Na svakih 1s nasumicno jedna od aktivnih kasa postaje neaktivna i jedna od neaktivnih kasa postaje aktivna. Deaktivirana kasa
opsluzuje sve korisnike koji cekaju u redu, samo ne prima nove.
U zaglavlju nemacki_prodajni_lanac.h treba implementirati metode kupovina i smena_kasa klase Eksamarket kao i dodati neophodna
polja i dopuniti konstruktor i destruktor po potrebi
kupovina:
    modeluje ponasanje kupca; kupac dolazi na kase; ako ima slobodna kasa koja radi, odmah je zauzima; ako nema slobodnih kasa
    koje rade onda ceka u redu na kasi koja radi i ciji je red cekanja najkraci; kupovina traje 1s; obavestava narednog u redu
    koji ceka da plati na kasi koju je oslobodio
smena_kasa:
    na svakih 1s nasumicno jednu od aktivnih kasa deaktivira a jednu neaktivnih kasa aktivira
*/
#include <iostream>

#include "deo_a.h"
#include "kupac.h"
#include "nemacki_prodajni_lanac.h"

void testirajA_niti() 
{
    vector<float> a = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 3, 6, 5, 7, 6}, b(a.size());

    for (auto br = a.rbegin(); br != a.rend(); br++) b.push_back(*br);
    //b[5] = -4.3f;

    bool isti_su;
    jesu_li_isti_elementi(a.cbegin(), a.cend(), b.crbegin(), isti_su);

    if (isti_su) cout << "Drugi niz je obrnut prvi." << endl;
    else cout << "Drugi niz nije obrnut prvi." << endl;
}

void testirajA_obrada() 
{
    vector<float> a = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 3, 6, 5, 7, 6}, b(a.size());

    for (auto br = a.rbegin(); br != a.rend(); br++) b.push_back(*br);

    b[5] = -4.3f;

    bool isti_su = proveri_obrnutost(a, b);

    if (isti_su) cout << "Drugi niz je obrnut prvi." << endl;
    else cout << "Drugi niz nije obrnut prvi." << endl;
}

void smena(Eksamarket &eksa) 
{
    eksa.smena_kasa();
}

void kupac(Eksamarket &eksa, int id, int cekanje) 
{
    this_thread::sleep_for(milliseconds(cekanje));
    eksa.kupovina(id);
}

void testirajB() 
{
    const int br_kasa = 5, br_kupaca = 15;

    static Kupac k;
    static Eksamarket eksa(k, br_kasa);

    thread smenjanje(smena, ref(eksa));
    smenjanje.detach();

    thread *kupci = new thread[br_kupaca]{};

    for (int i = 0; i < br_kupaca; i++)
        kupci[i] = thread(kupac, ref(eksa), i+1, i*200);
    
    for (int i = 0; i < br_kupaca; i++) 
        kupci[i].join();
    
    delete[] kupci;
}

int main() 
{
    testirajA_obrada();
    testirajA_niti();
    testirajB();
    _Exit(0);
}
