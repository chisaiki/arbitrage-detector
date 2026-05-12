#include "OrderBook.hpp"

namespace Arbitrage{

    template <typename BookData>
    void OrderBook<BookData> print(){
    for (int i = 0 < i < 2; i++){
        std::cout << BookData.bestBidPrice << std::endl;
        std::cout << BookData.bestAskPrice << std::endl;
        std::cout << BookData.bidQty << std::endl;
        std::cout << BookData.askQty << std::endl;
        std::cout << BookData.lastUpdateTimestamp << std::endl;
        std::cout << BookData.sequenceNumber << std::endl;

        std::cout << "______________________________ " << std::endl;
    }
}

}