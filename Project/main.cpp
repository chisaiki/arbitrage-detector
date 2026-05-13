#include "MPSC/MPSC.hpp"
#include "MarketData/MarketData.hpp"
#include "OrderBook/ExchangeQuote.hpp"
#include "OrderBook/OrderBook.hpp"

#include <thread>
#include <vector>
#include <iostream>
#include <chrono>


// Global or shared control
std::atomic<bool> running{true};

inline uint64_t getCurrentNanos() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

void mock_producer(MQueue::MPSCQueue<Arbitrage::MarketItem, 1024>& queue, int id) {
    while(running) {
        Arbitrage::MarketItem item;
        item.itemId = 12345;
        item.exchangeId = id;
        // Switch back to random price generation for both producers
        item.priceCents = rand() % 100000000000000; // Random price between 0 and 999
        item.timestamp = getCurrentNanos();

        queue.push(item);
        // if (queue.push(item)) {
        //     std::cout << "[Producer " << id << "] Pushed item: price=" << item.priceCents << ", timestamp=" << item.timestamp << std::endl;
        // } else {
        //     std::cout << "[Producer " << id << "] Queue full, item dropped." << std::endl;
        // }
        // Sleep for a few microseconds to simulate high-frequency bursts
    }
}



int main() {
    std::cout << "[Main] Program started." << std::endl;

    // 1. Initialize your shared infrastructure
    MQueue::MPSCQueue<Arbitrage::MarketItem, 1024> queue;
    Arbitrage::OrderBook<Arbitrage::TopOfBook> book;

    // Initialize bestBidPrice for both exchanges to a high value for testing
    book.array[0].bestBidPrice = 10000;
    book.array[1].bestBidPrice = 10000;

    // 2. Launch Producer Threads (WebSockets or Mocks)
    std::thread producerA(mock_producer, std::ref(queue), 0);
    std::thread producerB(mock_producer, std::ref(queue), 1);

    // 3. The Consumer Loop (The "Hot Path")
    size_t popCount = 0;
    while (running.load(std::memory_order_relaxed)) {
        if (queue.pop(book)) {
            std::cout << ">>> ARBITRAGE DETECTED! <<<\n";
        }
        if (++popCount % 100000 == 0) {
            std::cout << "[Consumer] Still running, popCount=" << popCount << std::endl;
        }
        // Note: No 'sleep' or 'yield' here. In HFT, we 'busy-wait' 
        // to ensure 0ns reaction time to new data.
    }

    // 4. Cleanup
    producerA.join();
    producerB.join();

    std::cout << "[Main] Program exiting." << std::endl;
    return 0;
}



// int main(){
//     MQueue::MPSCQueue<Arbitrage::MarketItem, 1024> Object;
//     Arbitrage::OrderBook<Arbitrage::TopOfBook> Book;

//     std::cout << "Object made" << std::endl;
//     Object.print();

// }