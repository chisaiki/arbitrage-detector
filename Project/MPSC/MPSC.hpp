/*Lock-Free Queue Implementation*/
#ifndef MSPC_QUEUE
#define MSPC_QUEUE

#include <iostream>
#include <atomic>
#include <cstddef>
#include <new> // Required for hardware cache line definitions

namespace MQueue{

template <typename DataType, size_t Capacity = 1024>
    class MPSCQueue{
        private: 

            /*Set standard L1 cache line size (typically 64 bytes on x86_64)*/ 
            /*Guarantee ABI stability by hardcoding target-specific cache line sizes,
            ignoring transient compiler tuning flags.*/
            #if defined(__s390x__) || defined(__powerpc__) || defined(__POWERPC__)
                static constexpr size_t CacheLineSize = 128;
            #else
                // 64 bytes covers x86, x86_64, standard ARM, and ARM64.
                static constexpr size_t CacheLineSize = 64;
            #endif

            /*Alienate cache lines for head and tail to avoid false sharing*/
            alignas(CacheLineSize) std::atomic<size_t> tail_{0};
            alignas(CacheLineSize) std::atomic<size_t> head_{0};
            /*Ensures element 0 starts cleanly on its own cache boundary*/
            alignas(CacheLineSize) DataType buffer[1024] = {};

        
        public:

        /*Constructor*/
        MPSCQueue() = default;
       

        MPSCQueue(const MPSCQueue&) = delete;
        MPSCQueue& operator=(const MPSCQueue&) = delete;
        MPSCQueue(MPSCQueue&&) = delete;
        MPSCQueue& operator=(MPSCQueue&&) = delete;

        /*Destructor*/
        ~MPSCQueue() = default;

        /*Enqueue Atomic Functions is a must?*/
        void push(const DataType Data);

        /*Dequeue (Only done by one thread)*/
        DataType pop(); 

        /*Peek*/
        DataType front();

        /*Rear*/
        DataType rear();

        /*Checks if queue has any elements in it: need it for consumer thread*/
        bool isEmpty() const;

        /*Checks if buffer is full*/
        bool isFull();

        void print();

    };
}
#include "MPSC.cpp"
#endif