#ifndef POVRATNA_H_INCLUDED
#define POVRATNA_H_INCLUDED

using namespace std;
using namespace chrono;

/*
Struktura koja opisuje povratnu vrednost, duzinu skoka 
ili bacanja i trajanje skoka i bacanja u milisekundama.
*/

struct Povratna_vrednost 
{                      
    int rezultat;                               
    duration<double, milli> trajanje;
};

#endif // POVRATNA_H_INCLUDED
