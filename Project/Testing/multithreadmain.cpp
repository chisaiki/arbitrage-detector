#include "../MPSC/MPSC.hpp"
#include "../MarketData/MarketData.hpp"
#include "../OrderBook/ExchangeQuote.hpp"
#include "../OrderBook/OrderBook.hpp"

#include <thread>
#include <vector>
#include <iostream>
#include <chrono>
#include <atomic>


std::atomic<bool> running{true};
std::atomic<size_t> globalPopCount{0};

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
        
        /*Random price generation for producers*/
        item.priceCents = rand() % 100000000000000; // Random price between 0 and 999
        item.timestamp = getCurrentNanos();

        queue.push(item);
    }

    /*the producers have no sleep or delay. 
    They're running in tight infinite loops, 
    pushing items to the queue as fast as possible, to show maximum throughput*/
}

int main() {
    std::cout << "[Main] Program started." << std::endl;

    /*Initalize necessary structures*/
    MQueue::MPSCQueue<Arbitrage::MarketItem, 1024> queue;
    Arbitrage::OrderBook<Arbitrage::TopOfBook> book;

    /*Default values for order book is too low, so I need to change it*/
    book.array[0].bestBidPrice = 10000;
    book.array[1].bestBidPrice = 10000;

    /*Create threads pushing fake data */
    std::thread producerA(mock_producer, std::ref(queue), 0);
    std::thread producerB(mock_producer, std::ref(queue), 1);

    auto startTime = std::chrono::high_resolution_clock::now();

    /*The Consumer Loop (The "Hot Path")*/
    size_t popCount = 0;
    while (running.load(std::memory_order_relaxed)) {
        if (queue.pop(book)) {
            std::cout << ">>> ARBITRAGE DETECTED! <<<\n";
        }
        popCount++;
        globalPopCount = popCount;
        
        if (popCount % 100000 == 0) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startTime).count();
            double throughput = (popCount / (elapsed / 1000.0)) / 1e6;  // Million items/sec
            std::cout << "[Consumer] Still running, popCount=" << popCount 
                      << " | Throughput: " << throughput << " M items/sec" << std::endl;
        }

        /*Stop after 10 million items*/
        if (popCount >= 10000000) {
            running = false;
        }

        /*Dont push consumer loop to sleep, should be constantly waiting
        "busy-wait" to create 0ns reaction time*/
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    double totalThroughput = (popCount / (totalTime / 1000.0)) / 1e6;

    producerA.join();
    producerB.join();

    std::cout << "[Main] Program exiting.\n";
    std::cout << "  Total Time: " << totalTime << " ms\n";
    std::cout << "  Items Popped: " << popCount << "\n";
    std::cout << "  Throughput: " << totalThroughput << " M items/sec" << std::endl;
    return 0;
}