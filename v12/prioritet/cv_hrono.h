#ifndef CV_HRONO_H_INCLUDED
#define CV_HRONO_H_INCLUDED

#include <deque>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;
using namespace chrono;

// Klasa cv_hrono simulira hronolosku condition_variablu tj. fifo CV
class cv_hrono 
{
	private:
		// Da bi se ovo omogucilo stvaraju se zasebne CV za svako novo cekanje ove CV se nalaze u cv_pair strukturi
		struct cv_pair 
		{					
			condition_variable cv;
			bool fulfilled;
			cv_pair() : fulfilled(false) {}
		};

		// Double ended queue - deque prethodno opisanih struktura
		// kako bi se moglo stavljati na kraj i brisati sa pocetka (FIFO)
		deque<cv_pair*> ccv;				

	public:
		void wait(unique_lock<mutex>& lock);
		cv_status wait_for(unique_lock<mutex>& lock, int sekundi);
		bool notify_one();
		void notify_all();
};

#endif // CV_HRONO_H_INCLUDED
