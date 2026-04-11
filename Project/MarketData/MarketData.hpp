#include <cstdint>

namespace Arbitrage{
    
    struct MarketDataStorage{
        uint64_t timestamp_ns;
        uint32_t exchange_data_id;
        double bid_price;
        double ask_price;
    };
}