#ifndef _QUEUEL_H
#define _QUEUEL_H
#include "Headers.hpp"
// Single Producer - Multiple Consumer queue
template <typename T>class PCQueue
{

public:
    PCQueue() : queue(){
        //this->queue = new std::queue<T>;
        pthread_mutex_init(&(this->mutex), nullptr);
        pthread_cond_init(&(this->cond), nullptr);
    }
	// Blocks while queue is empty. When queue holds items, allows for a single
	// thread to enter and remove an item from the front of the queue and return it. 
	// Assumes multiple consumers.
	T pop(){
        pthread_mutex_lock(&(this->mutex));
        while(this->queue.empty()){
            pthread_cond_wait(&(this->cond), &(this->mutex));
        }
        T item = this->queue.front();
        this->queue.pop();
        pthread_mutex_unlock(&(this->mutex));
        return item;
    }

	// Allows for producer to enter with *minimal delay* and push items to back of the queue.
	// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.  
	// Assumes single producer 
	void push(const T& item){
        pthread_mutex_lock(&(this->mutex));
        this->queue.push(item);
        pthread_mutex_unlock(&(this->mutex));
        pthread_cond_broadcast(&(this->cond));
    }

    ~PCQueue(){
        pthread_mutex_destroy(&(this->mutex));
        pthread_cond_destroy(&(this->cond));
        //delete(this->queue);
    }


private:
	//private:
	std::queue<T> queue;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	// Add your class memebers here
};
// Recommendation: Use the implementation of the std::queue for this exercise
#endif