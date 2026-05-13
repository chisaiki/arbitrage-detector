#include "MPSC/MPSC.hpp"
#include "MarketData/MarketData.hpp"
#include "OrderBook/ExchangeQuote.hpp"
#include "OrderBook/OrderBook.hpp"

int main(){
    MQueue::MPSCQueue<Arbitrage::MarketItem> Object;
    Arbitrage::OrderBook<Arbitrage::TopOfBook> Book;

    std::cout << "Object made" << std::endl;
    Object.print();

}