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