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
        bool MPSCQueue<DataType, Capacity>::pop(){
            uint32_t index = head_.load(std::memory_order_relaxed);

            /*Find the location of the head in the buffer*/
            uint32_t buffer_slot = current_read_idx & (Capacity - 1);
            
            /*Get access to the current MarketData*/
            DataType& slot_data = buffer[slot_idx];
            
            /*If the queue is NOT empty*/
            if(slot_data.can_overwrite == false){
                /*Do the arbitrage operations in here instead of the buffer copy*/
               
                /*Let producers know the slot is now available*/
                slot_data.can_overwrite.store(true, std::memory_order_release);
                head_.store(index + 1, std::memory_order_release);
                
                return true;
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
            return head_.load(std::memory_order_relaxed) == tail_.load(std::memory_order_aquire);
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
                std::cout << "[" << buffer[i] << "]" << std::endl;
            }

            std::cout << "Cache Line Size: " << CacheLineSize << std::endl;

        }

    }