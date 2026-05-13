#include "MPSC/MPSC.hpp"
#include "MarketData/MarketData.hpp"
#include "OrderBook/ExchangeQuote.hpp"
#include "OrderBook/OrderBook.hpp"

#include <iostream>
#include <queue>
#include <chrono>
#include <cstdlib>

std::atomic<bool> running{true};

inline uint64_t getCurrentNanos() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

int main() {
    std::cout << "[Main] Program started (Single-threaded)." << std::endl;

    // 1. Initialize shared infrastructure
    std::queue<Arbitrage::MarketItem> queue;
    Arbitrage::OrderBook<Arbitrage::TopOfBook> book;

    // Initialize bestBidPrice for both exchanges
    book.array[0].bestBidPrice = 10000;
    book.array[1].bestBidPrice = 10000;

    // 2. Single-threaded loop: generate data and consume
    size_t popCount = 0;
    size_t pushCount = 0;
    int currentExchange = 0;  // Toggle between exchanges 0 and 1

    auto startTime = std::chrono::high_resolution_clock::now();

    while (running.load(std::memory_order_relaxed)) {
        // Produce: Add market data to queue
        Arbitrage::MarketItem item;
        item.itemId = 12345;
        item.exchangeId = currentExchange;
        item.priceCents = rand() % 100000000000000;
        item.timestamp = getCurrentNanos();

        queue.push(item);
        pushCount++;

        // Toggle between exchanges
        currentExchange = (currentExchange + 1) % 2;

        // Consume: Process items from queue
        if (!queue.empty()) {
            Arbitrage::MarketItem frontItem = queue.front();
            queue.pop();

            // Update the order book with new market data
            book.updateData(frontItem, frontItem.exchangeId);

            // Check for arbitrage opportunity
            if (book.checkArbitrage()) {
                std::cout << ">>> ARBITRAGE DETECTED! <<<\n";
            }
            popCount++;
        }

        // Log progress
        if (popCount % 100000 == 0) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startTime).count();
            double throughput = (popCount / (elapsed / 1000.0)) / 1e6;  // Million items/sec
            std::cout << "[Consumer] Still running, popCount=" << popCount 
                      << " | Throughput: " << throughput << " M items/sec" << std::endl;
        }

        // Stop after 10 million items
        if (popCount >= 10000000) {
            running = false;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    double totalThroughput = (popCount / (totalTime / 1000.0)) / 1e6;

    std::cout << "[Main] Program exiting.\n";
    std::cout << "  Total Time: " << totalTime << " ms\n";
    std::cout << "  Items Pushed: " << pushCount << "\n";
    std::cout << "  Items Popped: " << popCount << "\n";
    std::cout << "  Throughput: " << totalThroughput << " M items/sec" << std::endl;
    return 0;
}