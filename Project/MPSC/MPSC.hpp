/*Lock-Free Queue Implementation*/
#ifndef MSPC_QUEUE
#define MSPC_QUEUE

#include <atomic>
#include <cstddef>
#include <iostream>

struct Node{
    int data;
    Node* next;

    Node(const int value, Node* next = nullptr) : data(value), next(next){}
};

namespace MQueue{

template <typename DataType>
    class MPSCQueue{
        private:
            std::atomic<DataType*> head_;
            std::atomic<DataType*> tail_;
            DataType buffer[4];
            size_t size_ = 0;

        public:

        /*Constructors*/
        MPSCQueue();
        MPSCQueue(const DataType data);

        /*Destructor*/
        ~MPSCQueue();

        /*Enqueue Atomic Functions is a must?*/
        void push();

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