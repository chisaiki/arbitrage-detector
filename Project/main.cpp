#include "MPSC/MPSC.hpp"
#include "MarketData/MarketData.hpp"
#include "OrderBook/ExchangeQuote.hpp"
#include "OrderBook/OrderBook.hpp"

#include <thread>
#include <vector>
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
        item.priceCents = rand() % 1000; // Random price between 1000
        item.timestamp = getCurrentNanos();
        
        queue.push(item); 
        // Sleep for a few microseconds to simulate high-frequency bursts
    }
}



int main() {
    // 1. Initialize your shared infrastructure
    MQueue::MPSCQueue<Arbitrage::MarketItem, 1024> queue;
    Arbitrage::OrderBook<Arbitrage::TopOfBook> book;

    // 2. Launch Producer Threads (WebSockets or Mocks)
    // We pass the queue by reference and an ID (0 or 1)
    std::thread producerA(mock_producer, std::ref(queue), 0);
    std::thread producerB(mock_producer, std::ref(queue), 1);

    // 3. The Consumer Loop (The "Hot Path")
    // This thread should ideally be "pinned" to a core to avoid context switching
    while (running.load(std::memory_order_relaxed)) {
        
        // Your optimized pop function we just finished
        if (queue.pop(book)) {
            // Because pop() returns arbFound, this only triggers on profit!
            std::cout << ">>> ARBITRAGE DETECTED! <<<\n";
        }
        
        // Note: No 'sleep' or 'yield' here. In HFT, we 'busy-wait' 
        // to ensure 0ns reaction time to new data.
    }

    // 4. Cleanup
    producerA.join();
    producerB.join();

    return 0;
}



// int main(){
//     MQueue::MPSCQueue<Arbitrage::MarketItem, 1024> Object;
//     Arbitrage::OrderBook<Arbitrage::TopOfBook> Book;

//     std::cout << "Object made" << std::endl;
//     Object.print();

// }