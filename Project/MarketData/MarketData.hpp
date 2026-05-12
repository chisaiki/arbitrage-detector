#include <cstdint>
#include <atomic>

namespace Arbitrage{
    
    struct alignas(8) MarketItem{
        uint64_t itemId;                 // Bytes 0 - 7
        uint64_t timestamp;              // Bytes 8 - 15
        uint32_t priceCents;             // Bytes 16 - 19
        std::atomic<bool> can_overwrite; // Byte 20 (Exactly 1 byte)
    };
}