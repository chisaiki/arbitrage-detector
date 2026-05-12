#ifndef ORDER_BOOK
#define ORDER_BOOK

#include <iostream>

namespace Arbitrage{

    template <typename BookData>
    class OrderBook{

        private:
        
            BookData array[2] = {};

        public:

            OrderBook() = default;
            ~OrderBook() = default;

            void print();


    };


}

#include "OrderBook.cpp"
#endif