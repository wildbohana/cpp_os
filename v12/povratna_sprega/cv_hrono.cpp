#include "cv_hrono.h"

// Metoda cekanja na cv_hrono, mora da se prosledi referenca (ne moze kopija) na lock
void cv_hrono::wait(unique_lock<mutex>& lock) 
{
	// Stvara se nova struktura cv_pair kada se ulazi u wait
	cv_pair* pair = new cv_pair;

	// Ova struktura se stavlja na kraj reda ccv (FIFO)
	ccv.push_back(pair);

	// Na ovoj strukturi se ceka dok se bool fulfilled ne postavi na true i tek tada se izlazi iz cekanja
	while (!pair->fulfilled)
		pair->cv.wait(lock);
	
	// Semanticka provera - da li je zaista cv koja je izasla iz cekanja ona prva is reda (front),
	// ukoliko slucajno iz nekog razloga nije prepusta se procesor drugoj niti (yield) i nakon toga se ponovo proverava
	while (ccv.front()!=pair) 
	{		
		lock.unlock();
		this_thread::yield();
		lock.lock();
	}

	// Skidanje strukture sa vrha reda (FIFO), i brisanje strukture iz memorije
	ccv.pop_front();					
	delete pair;
}      

// Metoda ogranicenog cekanja na cv_hrono 
cv_status cv_hrono::wait_for(unique_lock<mutex>& lock, int sekundi) 
{		
	//funkcionise veoma slicno kao metoda wait, sa tim sto ako se desi timeout, prekida se cekanje
	cv_pair* pair = new cv_pair;
	ccv.push_back(pair);

	cv_status status;
	while (!pair->fulfilled) 
	{
		status = pair->cv.wait_for(lock, seconds(sekundi));
		if (status == cv_status::timeout) break;
	}

	ccv.pop_front();    

	delete pair;   
	return status;
}

// Metoda notifikacije na cv_hrono
bool cv_hrono::notify_one() 
{
	// Ukoliko nema nikoga u cekanju vraca se false
	if (ccv.empty()) return false;		
	
	// Ukoliko ima nekog, uzima se prvi iz reda cekanja (FIFO) i obavestava se
	ccv.front()->fulfilled = true;				
	ccv.front()->cv.notify_one();

	return true;
}

// Metoda notifikacije svih na cv_hrono
void cv_hrono::notify_all() 
{
	// Prolazi se kroz citav red cekanja i obavestavaju se svi - jedan po jedan	
	for (auto i = ccv.begin(); i != ccv.end(); ++i) 
	{	
		(*i)->fulfilled = true;
		(*i)->cv.notify_one();
	}
}
