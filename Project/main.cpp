#include "MPSC/MPSC.hpp"

int main(){
    MQueue::MPSCQueue Object(1);

    Object.push();
    std::cout << Object.isFull() << std::endl;
}