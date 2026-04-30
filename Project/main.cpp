#include "MPSC/MPSC.hpp"

void print(int array[]){
    for (int i = 0; i < 4; i++){
        std::cout << array[i] << " ";
    }
}

int main(){
    MQueue::MPSCQueue<int> Object;

    Object.push(1);
    print(Object.int_buffer);
    std::cout << "Empty " << Object.isEmpty() << std::endl;

    Object.push(2);
    print(Object.int_buffer);
    Object.pop();
          print(Object.int_buffer);


    Object.push(3);
        print(Object.int_buffer);
        Object.pop();
              print(Object.int_buffer);


    Object.push(4);
                  print(Object.int_buffer);


    Object.push(5);
                  print(Object.int_buffer);
    std::cout << "Empty " << Object.isEmpty() << std::endl;


    Object.push(6);
        print(Object.int_buffer);
            Object.pop();
                  print(Object.int_buffer);


    Object.push(7);
        print(Object.int_buffer);
            Object.pop();
                  print(Object.int_buffer);


    Object.push(8);
        print(Object.int_buffer);
            Object.pop();
                  print(Object.int_buffer);


    Object.push(9);
        print(Object.int_buffer);

   Object.pop();
              print(Object.int_buffer);
                 Object.pop();
              print(Object.int_buffer);
                 Object.pop();
              print(Object.int_buffer);
                 Object.pop();
              print(Object.int_buffer);
    std::cout << "Empty " << Object.isEmpty() << std::endl;

        Object.pop();
              print(Object.int_buffer);
                 Object.pop();
              print(Object.int_buffer);
    std::cout << Object.isEmpty();
}