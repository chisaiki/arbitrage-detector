#include "MPSC/MPSC.hpp"

void print(int array[]){
    for (int i = 0; i < 4; i++){
        std::cout << array[i] << " ";
    }
}

int main(){
    MQueue::MPSCQueue<int> Object;

    Object.push();
    std::cout << Object.isFull() << std::endl;

    print(Object.int_buffer);
}