/*Lock-Free Queue Implementation*/
#ifndef MSPC_QUEUE
#define MSPC_QUEUE

#include <atomic>
#include <cstddef>
#include <iostream>

namespace MQueue{

template <typename DataType>
    class MPSCQueue{
        private:
            std::atomic<size_t> head_;
            std::atomic<size_t> tail_; 
            DataType buffer[4];
            const size_t capacity_ = 4;

        
        public:

        /*TESTING*/
            int int_head;
            int int_tail;
            int int_buffer[4] = {0};

        /*Constructors*/
        MPSCQueue();
        MPSCQueue(const DataType data);

        /*Destructor*/
        ~MPSCQueue();

        /*Enqueue Atomic Functions is a must?*/
        void push(const DataType Data);

        /*Dequeue (Only done by one thread)*/
        DataType pop(); 

        /*Peek*/
        DataType front();

        /*Rear*/
        DataType rear();

        /*Checks if queue has any elements in it: need it for consumer thread*/
        bool isEmpty();

        /*Checks if buffer is full*/
        bool isFull();

    };
}
#include "MPSC.cpp"
#endif