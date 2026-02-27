#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

#include <random>

//Shared Memory

struct Prices{
    double prices[2]; 
};

std::mutex price_data_access_lock;

void update_price(Prices& price_ds, const int& i_value, std::atomic<bool>& continue_running){
    
    while(continue_running){
        /*Generate Random Number*/
        std::random_device rd;  // Non-deterministic seed
        std::mt19937 gen(rd()); // Mersenne Twister engine
        std::uniform_real_distribution<double> distrib(1.0, 100.0); // Range [1.0, 100.0]

        /*Update the respective price*/
        price_ds.prices[i_value] = distrib(gen);
        
        std::cout << price_ds.prices[i_value] << std::endl;
        
    }
    
}

int main(){

    Prices price_tracker{};
    std::atomic<bool> keep_running = true;

    //create threads
    std::thread get_price_threads[2];

    for (int i = 0; i < 2; i++) {
        get_price_threads[i] = std::thread(update_price, std::ref(price_tracker), i, std::ref(keep_running));
    }

    /*Threds will continue to run until a user presses enter on the keyboard*/
    std::cin.get();
    keep_running = false;

    /*Close Threads*/
    for(int i = 0; i < 2; i++){
        get_price_threads[i].join();
    }


    std::cout << "Finished running thread" << std::endl;
    std::cout << "["<< price_tracker.prices[0] << "] [" << price_tracker.prices[1] << "]" << std::endl;
    
    return 0;
    
}   

// Each thread should be generating a random number and
// updating the price variables after the generate the number

// a third thread will be reading the data as soon as it updates 
// and calculates the difference to display

// who should display it? should i focus on optimization?