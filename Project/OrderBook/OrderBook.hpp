#ifndef ORDER_BOOK
#define ORDER_BOOK

#include <iostream>
#include "../MarketData/MarketData.hpp"

namespace Arbitrage{

    template <typename BookData>
    class OrderBook{        
        public:

            BookData array[2] = {};
            OrderBook() = default;
            ~OrderBook() = default;

            void updateData(Arbitrage::MarketItem& Item, int id);
            
            
            /*ONLY FOR TESTING USAGE*/
            bool checkArbitrage();
            void print();


    };


}

#include "OrderBook.cpp"
#endif