        #include "MPSC.hpp"
    
    namespace MQueue{

        template <typename DataType> /*WHY DO I NEED THIS??*/
        /*Enqueue Atomic Functions is a must?*/
        void MPSCQueue<DataType>::push(){
            std::cout << "PUSH" << std::endl;
        }
        
        template <typename DataType>
        /*Dequeue (Only done by one thread)*/
        DataType MPSCQueue<DataType>::pop(){
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
            std::cout << "PUSH" << std::endl;
            return false;
        }

          template <typename DataType>
        /*Checks if buffer is full*/
        bool MPSCQueue<DataType>::isFull(){
            std::cout << "PUSH" << std::endl;
            return false;
        }

        /*Constructors*/
        template <typename DataType>
        MPSCQueue<DataType>::MPSCQueue(){}
        
        template <typename DataType>
        MPSCQueue<DataType>::MPSCQueue(const DataType data){
            std::cout << "Constructor called" << std::endl;
        }

        /*Destructor*/
        template <typename DataType>
        MPSCQueue<DataType>::~MPSCQueue(){};

    }