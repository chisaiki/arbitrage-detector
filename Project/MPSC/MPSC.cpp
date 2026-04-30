        #include "MPSC.hpp"
    
    namespace MQueue{

        template <typename DataType> /*WHY DO I NEED THIS??*/
        /*Enqueue Atomic Functions is a must?*/
        void MPSCQueue<DataType>::push(DataType Data){
            if(!isFull()){
                std::cout << "push" << std::endl;
                int_buffer[int_tail & (capacity_ - 1)] = Data;
                int_tail = int_tail + 1;
            }
            //std::cout << "size: " << (int_tail) << std::endl;
        }
        
        template <typename DataType>
        /*Dequeue (Only done by one thread)*/
        DataType MPSCQueue<DataType>::pop(){
            if (!isEmpty()){
                std::cout << "pop" << std::endl;
                DataType popped = int_buffer[int_head & (capacity_ - 1)];
                int_buffer[int_head & (capacity_ - 1)] = 0;
                int_head++;

                return popped;
            }
            
            return DataType{};
        }

        template <typename DataType>
        /*Peek*/
        DataType MPSCQueue<DataType>::front(){
        }

        template <typename DataType>
        /*Rear*/
        DataType MPSCQueue<DataType>::rear(){
        }

        template <typename DataType>
        /*Checks if queue has any elements in it: need it for consumer thread*/
        bool MPSCQueue<DataType>::isEmpty(){
            if (int_head == int_tail){
                return true;
            }

            return false;
        }

          template <typename DataType>
        /*Checks if buffer is full*/
        bool MPSCQueue<DataType>::isFull(){
            if (int_tail - int_head == capacity_){
                return true;
            }
            return false;
        }

        /*Constructors*/
        template <typename DataType>
        MPSCQueue<DataType>::MPSCQueue(){
            int_head = 0;
            int_tail = 0;  
        }
        
        template <typename DataType>
        MPSCQueue<DataType>::MPSCQueue(const DataType data){
            std::cout << "Constructor called" << std::endl;
            int_head = 0;
            int_tail = 0;            
        }

        /*Destructor*/
        template <typename DataType>
        MPSCQueue<DataType>::~MPSCQueue(){};

    }