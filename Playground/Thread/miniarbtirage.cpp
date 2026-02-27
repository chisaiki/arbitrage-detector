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

// a third thread will be reading the data as soon as it updates 
// and calculates the difference to display

// who should display it? should i focus on optimization?