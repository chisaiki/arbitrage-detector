namespace Arbitrage{

    struct alignas(32) TopOfBook {
        uint64_t bookItemId{0}; 
        uint64_t bestBidPrice{0};
        uint64_t bestAskPrice{0};
        uint64_t bidQty{0};
        uint64_t askQty{0};
        uint64_t lastUpdateTimestamp{0};
        uint64_t sequenceNumber{0};
    };

}