//
// Created by danielle on 23/12/2018.
//

#include "Headers.h"
#include "PCQueue.hpp"

// Single Producer - Multiple Consumer queue
// Blocks while queue is empty. When queue holds items, allows for a single
// thread to enter and remove an item from the front of the queue and return it.
// Assumes multiple consumers.
T PCQueue::pop(){
    pthread_mutex_lock(this->mutex);
    while(queue.empty()){
        pthread_cond_wait(this->cond, this->mutex);
    }
    T item = queue.front();
    queue.pop();
    pthread_mutex_unlock(this->mutex);
    return item;
}

// Allows for producer to enter with *minimal delay* and push items to back of the queue.
// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.
// Assumes single producer
void PCQueue::push(const T& item){
    pthread_mutex_lock(this->mutex);
    queue.push(item);
    pthread_mutex_unlock(this->mutex);
    pthread_cond_brodcast(this->cond);
}
