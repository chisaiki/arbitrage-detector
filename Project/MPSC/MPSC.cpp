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
        DataType MPSCQueue<DataType, Capacity>::pop(){
            // if (int_tail != int_head){
            //     std::cout << "pop" << std::endl;
            //     DataType popped = int_buffer[int_head & (capacity_ - 1)];
            //     int_buffer[int_head & (capacity_ - 1)] = 0;
            //     int_head++;

            //     return popped;
            // }
            
            return DataType{};
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
        bool MPSCQueue<DataType, Capacity>::isEmpty(){
            // if (int_head == int_tail){
            //     return true;
            // }

            return false;
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