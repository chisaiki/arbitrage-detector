#include "MPSC/MPSC.hpp"
#include "MarketData/MarketData.hpp"
#include "OrderBook/ExchangeQuote.hpp"

int main(){
    MQueue::MPSCQueue<int> Object;
    Arbitrage::OrderBook<Arbitrage::TopOfBook> Book;

    std::cout << "Object made" << std::endl;
    Object.print();

    Book.print();
}