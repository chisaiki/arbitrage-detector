    #include "MPSC.hpp"
   
    
    namespace MQueue{

        template <typename DataType, size_t Capacity> /*WHY DO I NEED THIS??*/
        /*Enqueue Atomic Functions is a must?*/
        void MPSCQueue<DataType, Capacity>::push(const DataType Data){
            // if(int_tail - int_head < capacity_){
            //     std::cout << "push" << std::endl;
            //     int_buffer[int_tail & (capacity_ - 1)] = Data;
            //     int_tail = int_tail + 1;
            // }
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
                
                bool arbFound = false;

                /*Find the website data of the other website*/
                int otherID = 1 - slot_data.exchangeId;

                /*Find the appropriate order book location for the other website's data*/
                auto& otherExchangeData = localBook.array[otherID];

                /*See if the current sell price is lower than what the other website is asking for*/
                if (slot_data.priceCents < otherExchangeData.bestBidPrice){
                    arbFound = true; /*I have found a price thats lower than what I was trying to buy it for*/
                }
               
                localBook.updateData(slot_data, slot_data.exchangeId);

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
            // return int_buffer[int_head];
            return DataType{};
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