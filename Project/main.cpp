#include "MPSC/MPSC.hpp"
#include "MarketData/MarketData.hpp"

int main(){
    MQueue::MPSCQueue<int> Object;

    std::cout << "Object made" << std::endl;
    Object.print();
}