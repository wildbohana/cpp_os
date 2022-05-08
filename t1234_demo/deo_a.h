#ifndef _1234_DEO_A_H
#define _1234_DEO_A_H

#include <random>
#include <thread>
#include <vector>

using namespace std;

typedef vector<float>::const_iterator ci;
typedef vector<float>::const_reverse_iterator cri;

/**
 * Provera da li je redosled elemenata u segmentu prvog vektora brojeva obrnut redosled segmenta drugog vektora brojeva.
 *
 * @param a_pocetak pocetak segmenta prvog vektora brojeva
 * @param a_kraj kraj segmenta prvog vektora brojeva
 * @param b_pocetak pocetak segmenta drugog vektora brojeva
 * @return da li je drugi vektor brojeva obrnutog redosleda u odnosu na prvi
 */

// Implementiraj A deo 1
bool jesu_li_isti_elementi(ci a_pocetak, ci a_kraj, cri b_pocetak, bool &rez) 
{
	// a_pocetak je CI, b_pocetak je CRI
	for (;a_pocetak != a_kraj; a_pocetak++, b_pocetak++)
        if (*a_pocetak != *b_pocetak) 
			return false;

    return true;
}

/**
 * Paralelna/konkurentna provera da li je redosled brojeva u drugom vektoru brojeva obrnut u odnosu na prvi.
 * Broj niti zavisi od slucaja jer se za svaku nit nasumicno odredjuje velicina segmenta vektora za koji se vrsi provera.
 * Velicina segmenta koju obradjuje jedna nit moze biti bilo sta izmedju jednog i pet elemenata
 *
 * @param a prvi vektor brojeva
 * @param b drugi vektor brojeva
 * @return da li je drugi vektor brojeva obrnutog redosleda u odnosu na prvi
 */

// Implementiraj A deo 2
bool proveri_obrnutost(vector<float> a, vector<float> b) 
{
    default_random_engine rand;
    uniform_int_distribution<int> gen_br_el(1, 5);

    vector<int> podela_posla;

    ci a_p = a.cbegin();
    cri b_p = b.crbegin();

    int n = a.size();
	int brn = 0;

    while (brn < n) 
	{
        int br = gen_br_el(rand);
        podela_posla.push_back(br);
        brn += br;
    }

    thread niti[podela_posla.size()];
    bool rezultati[podela_posla.size()];

    int i = 0;
    brn = 0;

    for (int br_elem: podela_posla) 
	{
        brn += br_elem;
        niti[i] = thread(jesu_li_isti_elementi, a_p, (brn < n) ? a_p+br_elem : a.cend(), b_p, ref(rezultati[i]));
        i++;
        a_p += br_elem;
        b_p += br_elem;
    }

    for (thread &nit : niti) nit.join();

    bool isti_su = true;
    for (bool rez: rezultati) 
		isti_su = isti_su && rez;

    return isti_su;
}

#endif //_1234_DEO_A_H
