#ifndef MARKET_DATA
#define MARKET_DATA 

#include <cstdint>
#include <atomic>

namespace Arbitrage{
    
    struct alignas(64) MarketItem{
        uint64_t itemId{0};                 
        uint64_t timestamp;              
        uint32_t priceCents{0}; 
        uint8_t exchangeId{0};           /*0 for WebSocket A, 1 for WebSocket B*/ 
        std::atomic<bool> can_overwrite{true}; // Byte 20 (Exactly 1 byte)
        /*This flag tells the producer if the consumer has already validated the data*/
    };
}

/*Changed to 64B from 24B padding to prevent cache misses via
false sharing between threads. Now each data item has its own 
cache line. Although it uses an unecessary amount of additional space
false sharing misses should improve performance.*/

#endif