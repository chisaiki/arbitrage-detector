#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <random>

#include <iomanip>
#include <cmath>

#define THRESHOLD (80) /*the parentheses prevent unexpected behavior*/

//Shared Memory

struct Prices{
    double prices[2]; 
};

std::mutex price_data_access_lock;

void update_price(Prices& price_ds, int i_value, std::atomic<bool>& continue_running){
    
    while(continue_running){
        /*Generate Random Number*/
        std::random_device rd;  // Non-deterministic seed
        std::mt19937 gen(rd()); // Mersenne Twister engine
        std::uniform_real_distribution<double> distrib(1.0, 100.0); // Range [1.0, 100.0]

        /*Update the respective price*/
        price_data_access_lock.lock();
            price_ds.prices[i_value] = distrib(gen); 
        price_data_access_lock.unlock();
       
    }
}

void find_difference(Prices& price_ds, std::atomic<bool>& continue_running){
    double profit = 0.0;

    std::cout << std::fixed << std::setprecision(2);

    while(continue_running){
        /*Set the mutex*/
        price_data_access_lock.lock();
            profit = std::abs(price_ds.prices[0] - price_ds.prices[1]);
        price_data_access_lock.unlock();

        if (profit > THRESHOLD){
            std::cout << "Profit: " << profit << std::endl;
        }
    }

}

int main(){

    Prices price_tracker{};
    std::atomic<bool> keep_running(true);

    //create threads
    std::thread get_price_threads[2];
    /*It should be fine to have this thread running since I reset the values of prices to 0*/
    /*If anything I need it running first so it can calculate ALL price difference*/
    std::thread difference_compute(find_difference, std::ref(price_tracker), std::ref(keep_running));

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

    difference_compute.join();


    std::cout << "Finished running thread" << std::endl;
    std::cout << "["<< price_tracker.prices[0] << "] [" << price_tracker.prices[1] << "]" << std::endl;
    
    return 0;
    
}   

// Each thread should be generating a random number and
// updating the price variables after the generate the number

// a third thread will be reading the data as soon as it updates 
// can this third thread be the main or a seperate?
// and calculates the difference to display

// who should display it? should i focus on optimization?