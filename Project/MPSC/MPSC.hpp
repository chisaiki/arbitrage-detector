/*Lock-Free Queue Implementation*/
#ifndef MSPC_QUEUE
#define MSPC_QUEUE

struct Node{
    int data;
    Node* next;

    Node(const int value, Node* next = nullptr) : data(value), next(next){}
};

template <typename DataType>
class MPSCQueue{
    private:
        //Maybe the data nodes?
        std::atomic<DataType*> head_;
        std::atomic<DataType*> tail_;
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
    TypeData front();

    /*Rear*/
    TypeData rear();

    /*Checks if queue has any elements in it: need it for consumer thread*/
    bool isEmpty();

    /*Checks if buffer is full, but honestly I don't think I will make it bounded*/

};

#endif