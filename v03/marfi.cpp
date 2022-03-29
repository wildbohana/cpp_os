/*
Napraviti program koji ispituje ispravnost jednog od Marfijevih zakona, koji glasi:
"red u kojem ste odlucili da cekate je najsporiji".

Program predstavlja simulaciju rada tri kase za naplatu robe.
Svaka kasa ispred sebe ima red kupaca i svaka kasa je simulirana radom jedne programske niti.
Potrebno je implementirati funkciju kasa, koja simulira rad jedne kase.

Program vec sadrži implementiranu funkciju main koja radi sledece:
- Pita korisnika u koji red zeli da stane da ceka (redovi su oznaceni brojevima 0, 1 i 2).
- Kreira tri niti, pri cemu svaka simulira funkcionisanje kase u odgovarajucem redu cekanja.
- Meri se ukupno vreme cekanje u svakom od redova (ispred svake od kasa).

Prilikom kreiranja, svaka nit dobija parametar koji predstavlja broj kupaca koje kasa treba da opsluzi.
Ovo je slucajan broj u opsegu 1..30, koji se generise u funkciji main.
Opsluzivanje svakog kupca simulirati uspavljivanjem niti na slucajan vremenski period izmedju 1 i 100 ms.
Takodje, funkcija kasa treba da meri ukupno vreme obrade svih kupaca i to vreme treba da vrati kroz parametar vreme_na_kasi.

Posto se implementira funkcija kasa, potrebno je jos implementirati u funkciji main:
- Naci koji je red (0, 1 ili 2) najduze obradjivan.
- Uporediti to sa redom koji je korisnik izabrao. Ako je korisnik izabrao najsporiji red,
ispisati da je Marifijev zakon potvrdjen, u suprotnom ispisati da je Marfi, u ovoj konkretnoj simulaciji, na žalost omanuo.
*/

#include <iostream>
#include <thread>
#include <random>
#include <algorithm>

using namespace std;
using namespace chrono;

// Sta je bolje po C kod - define ili const int za ove vrednosti ?
const int UKUPNO_KASA = 3;
const int MAX_MILISEKUNDI_PO_KUPCU = 100;

//// ovo mi i dalje nije jasno odakle ga povlači
default_random_engine generator;

void kasa(int broj_kupaca, duration<double, milli> &vreme_na_kasi) {
	// Svima dodeli vreme cekanja u redu
    uniform_int_distribution<int> dist(1, MAX_MILISEKUNDI_PO_KUPCU);

    // Uzimanje vremena pre pocetka izvrsavanja
    steady_clock::time_point pocetak = steady_clock::now();

    // Simuliranje cekanja
	for(int i = 0; i < broj_kupaca; i++) {
		// Svaki kupac ce da bude u redu za kasu odredjeno vreme, i za to vreme ce njegova nit biti uspavana
        this_thread::sleep_for(milliseconds(dist(generator)));
    }

	// Uzimanje vremena nakon zavrsetka izvrsavanja
	// Moze i auto umesto steady_clock::time_point
    steady_clock::time_point kraj = steady_clock::now();
	
	// Ukupno cekanje = vreme zavrsetka - vreme pocetka
    vreme_na_kasi = kraj - pocetak;
}

int main() {
	// Inicijalizacija generatora slucajnih brojeva da pri svakom pokretanju daje razlicite brojeve
    generator.seed(system_clock::now().time_since_epoch().count());

    // Odredjivanje kase na koju klijent zeli da ode:
    int moja_kasa;
    cout << "Unesite kasu na kojoj cete stati u red (0, 1 ili 2):" << endl;
    cin >> moja_kasa;

    // Pravimo niz koji ce da sadrzi vremena cekanja na svakoj od kasa
    duration<double, milli> vremena[UKUPNO_KASA];
	// Svakoj kasi se dodeljuje od 1 do 30 kupaca
    uniform_int_distribution<int> dist(1, 30);

	// Somulacija cekanja (za sve 3 kase)
    thread niti[UKUPNO_KASA];
    for (int i = 0; i < UKUPNO_KASA; ++i) {
        niti[i] = thread(kasa, dist(generator), ref(vremena[i]));
    }

    for (int i = 0; i < UKUPNO_KASA; ++i) {
        niti[i].join();
    }

    // Pronalazenje reda u kojem se najduze cekalo
    int najsporija = max_element(vremena, &vremena[UKUPNO_KASA]) - vremena;

    // Provera zakona
    cout << "Marfijev zakon ";
    if(najsporija != moja_kasa)
        cout << "ni";			// lmao genijalno
    cout << "je potvrdjen.";
}
