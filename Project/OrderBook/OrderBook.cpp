#include "OrderBook.hpp"

namespace Arbitrage{

    template <typename BookData>
    void OrderBook<BookData>::print(){
        for (int i = 0; i < 2; i++){
            std::cout << array[i].bookItemId << std::endl;
            std::cout << array[i].bestBidPrice << std::endl;
            std::cout << array[i].bestAskPrice << std::endl;
            std::cout << array[i].bidQty << std::endl;
            std::cout << array[i].askQty << std::endl;
            std::cout << array[i].lastUpdateTimestamp << std::endl;
            std::cout << array[i].sequenceNumber << std::endl;

            std::cout << "______________________________ " << std::endl;
        }
    }

    template <typename BookData>
    void OrderBook<BookData>::updateData(Arbitrage::MarketItem& Item, int id){
        array[id].bestAskPrice = Item.priceCents;
        array[id].lastUpdateTimestamp = Item.timestamp;
    }

}