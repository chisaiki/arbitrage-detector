        #include "MPSC.hpp"
    
    namespace MQueue{

        template <typename DataType> /*WHY DO I NEED THIS??*/
        /*Enqueue Atomic Functions is a must?*/
        void MPSCQueue<DataType>::push(){
            if(size_ < buffer_size_){
                std::cout << "PUSH" << std::endl;
                size_++;
            }
            std::cout << "size: " << size_ << std::endl;
        }
        
        template <typename DataType>
        /*Dequeue (Only done by one thread)*/
        DataType MPSCQueue<DataType>::pop(){
            size_--;
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
            std::cout << "Empty Called" << std::endl;

            if (head_ == tail_ && size_ == 0){
                return true;
            }

            return false;
        }

          template <typename DataType>
        /*Checks if buffer is full*/
        bool MPSCQueue<DataType>::isFull(){
            std::cout << "Full?" << std::endl;
            if (head_ == tail_ && buffer_size_ == size_){
                return true;
            }
            return false;
        }

        /*Constructors*/
        template <typename DataType>
        MPSCQueue<DataType>::MPSCQueue(){}
        
        template <typename DataType>
        MPSCQueue<DataType>::MPSCQueue(const DataType data){
            std::cout << "Constructor called" << std::endl;
            head_.store(tail_.load());            
            size_ = 0;
        }

        /*Destructor*/
        template <typename DataType>
        MPSCQueue<DataType>::~MPSCQueue(){};

    }