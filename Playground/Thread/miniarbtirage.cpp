#include <iostream>
#include <thread>

//Shared Memory

struct prices{
    int price_A;
    int price_B;
};



int main(){

    prices Tracker{};

    return 0;
    
}   

// Each thread should be generating a random number and
// updating the price variables after the generate the number