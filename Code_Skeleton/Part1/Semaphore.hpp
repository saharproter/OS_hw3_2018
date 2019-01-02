#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H
#include "Headers.hpp"


// Synchronization Warm up 
class Semaphore {
public:
	Semaphore(); // Constructs a new semaphore with a counter of 0
	Semaphore(unsigned val);// Constructs a new semaphore with a counter of val

	//up = post, down = wait
	void up(); // Mark: 1 Thread has left the critical section
	void down();// Block untill counter >0, and mark - One thread has
		// entered the critical section.

    ~Semaphore();

private:
	int count = 0;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
// TODO
};

#endif
