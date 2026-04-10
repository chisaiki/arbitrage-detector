#include <iostream>
#include <boost/asio.hpp>

void printfunc(const boost::system::error_code& e){
    std::cout << "I love Anthony very much, and I'm so grateful to him" << std::endl;
}

int main(){
    std::cout << "Program has started" << std::endl;

    boost::asio::io_context io_object;

    boost::asio::steady_timer timer(io_object, boost::asio::chrono::seconds(5));

    std::cout << "Its running!" << std::endl;
    timer.async_wait(&printfunc);
    io_object.run();

    return 0;  
}