#ifndef PODACI_H_INCLUDED
#define PODACI_H_INCLUDED

using namespace std;
using namespace chrono;

struct Podaci 
{
    duration<double, milli> trajanje;
    system_clock::time_point dosao;
    system_clock::time_point pocetak;
};

#endif // PODACI_H_INCLUDED
