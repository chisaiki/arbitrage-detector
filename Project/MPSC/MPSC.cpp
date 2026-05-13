#include "MPSC.hpp"


namespace MQueue{

    template <typename DataType, size_t Capacity> /*WHY DO I NEED THIS??*/
    /*Enqueue*/
    bool MPSCQueue<DataType, Capacity>::push(const DataType& Data){
        
        uint32_t index = tail_.load(std::memory_order_relaxed);

        while(true){
            /*Check if the index I'm working with is actually the correct tail, 
              otherwise get me the right tail value*/

            /*Find the location of the tail in the buffer*/
            uint32_t buffer_slot = index & (Capacity - 1);
            DataType& slot_data = buffer[buffer_slot];

            /*Check if slot has already been processed by consumer*/
            if(!slot_data.can_overwrite.load(std::memory_order_acquire)){ 
                /*Buffer is full --> DROP the data packet (Need to change this later)*/

                uint32_t actual_tail = tail_.load(std::memory_order_relaxed);
                if (index != actual_tail) {
                    // We were looking at an old tail position! 
                    // Update our index guess and spin the loop to try the fresh tail.
                    index = actual_tail;
                    continue; 
                }
                
                return false;
            }

            if (tail_.compare_exchange_weak(index, index + 1, 
                                    std::memory_order_release, 
                                    std::memory_order_relaxed)) {

                /*Correct tail has been acquired, I can safely update the data*/
                slot_data.timestamp = Data.timestamp;      
                slot_data.priceCents = Data.priceCents;

                /*Let consumer know the slot has new data to check*/
                slot_data.can_overwrite.store(false, std::memory_order_release);
                return true;
            }
        } /*Current implementation of push is redunant, look into sequencing*/
    }
    
    
    template <typename DataType, size_t Capacity>
    /*Dequeue (Only done by one thread)*/
    bool MPSCQueue<DataType, Capacity>::pop(Arbitrage::OrderBook<Arbitrage::TopOfBook>& localBook){
        uint32_t index = head_.load(std::memory_order_relaxed);

        /*Find the location of the head in the buffer*/
        uint32_t buffer_slot = index & (Capacity - 1);
        
        /*Get access to the current MarketData*/
        DataType& slot_data = buffer[buffer_slot];

        /*If the queue is NOT empty*/
        if(slot_data.can_overwrite.load(std::memory_order_acquire) == false){ 
            
            /*I might need to move this entire logic to an order book function*/
            bool arbFound = false;

            /*Find the website data of the other website*/
            int otherID = 1 - slot_data.exchangeId;

            /*Find the appropriate order book location for the other website's data*/
            auto& otherExchangeData = localBook.array[otherID];
            
            /*Check if the data is newer*/
            if (slot_data.timestamp > otherExchangeData.lastUpdateTimestamp) {
                
                /*See if the current sell price is lower than what the other website is asking for*/
                if (slot_data.priceCents < otherExchangeData.bestBidPrice) {
                    arbFound = true;
                }

                /*Update book with the fresh data*/
                localBook.updateData(slot_data, slot_data.exchangeId);
            }

            /*Let producers know the slot is now available*/
            slot_data.can_overwrite.store(true, std::memory_order_release);
            head_.store(index + 1, std::memory_order_release);
            
            return arbFound;
        }
        
        return false;
    }


    template <typename DataType, size_t Capacity>        
    /*Peek*/
    DataType MPSCQueue<DataType, Capacity>::front(){
        uint32_t index = head_.load(std::memory_order_relaxed);
        return buffer[index];
    }

    template <typename DataType, size_t Capacity>
    /*Rear*/
    DataType MPSCQueue<DataType, Capacity>::rear(){
        // return int_buffer[(int_tail - 1) & (capacity_ - 1) ];
        return DataType{};
    }

    template <typename DataType, size_t Capacity>
    /*Checks if queue has any elements in it: need it for consumer thread*/
    bool MPSCQueue<DataType, Capacity>::isEmpty() const{
        /*Explicitly load both atomic values with zero synchronization overhead*/
        return head_.load(std::memory_order_relaxed) == tail_.load(std::memory_order_acquire);
    }

    template <typename DataType, size_t Capacity>
    /*Checks if buffer is full*/
    bool MPSCQueue<DataType, Capacity>::isFull(){
        // if (int_tail - int_head == capacity_){
        //     return true;
        // }
        return false;
    }


    template<typename DataType, size_t Capacity>
    void MPSCQueue<DataType, Capacity>::print(){                
        for (int i = 0; i < Capacity; i++){
            std::cout << "Data: " << i << std::endl;
            std::cout << "[" << buffer[i].itemId << "]" << std::endl;
            std::cout << "[" << buffer[i].timestamp << "]" << std::endl;
            std::cout << "[" << buffer[i].priceCents << "]" << std::endl;
        }
            std::cout << "Cache Line Size: " << CacheLineSize << std::endl;
        
    }

}