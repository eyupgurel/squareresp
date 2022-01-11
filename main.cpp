#include "me/matching_engine.h"










std::string get_pid() {
    std::stringstream s;
    s << std::this_thread::get_id();
    return s.str();
}

int main() {

    random_device rd;   // non-deterministic generator
    mt19937 gen(rd());
    uniform_real_distribution<double> sell_dist_price(3.33, 3.48);
    uniform_real_distribution<double> sell_dist_quantity(11.45, 1242.02);

    TOrders asks;

    std::vector<order> v_asks;


    for(int i =0; i<1000000; i++){
        auto rp = sell_dist_price(gen);
        auto rq = sell_dist_quantity(gen);
        long epoch_milli = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();
        //cout << i << endl;
        v_asks.emplace_back(rp, epoch_milli, rq, i, order::order_type::sell);

    }





    long epoch_milli_ask_insertion_start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

/*    for (auto it = begin (v_asks); it != end (v_asks); ++it) {
        asks.insert(*it );
    }*/

    asks.insert(begin(v_asks), end(v_asks));

    long epoch_milli_ask_insertion_end = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    cout << "epoch_milli_ask_insertion_start " <<  epoch_milli_ask_insertion_start << " epoch_milli_ask_insertion_end " << epoch_milli_ask_insertion_end << endl;

    cout << "time_elapsed_ask_insertion: " << epoch_milli_ask_insertion_end - epoch_milli_ask_insertion_start << endl;









    uniform_real_distribution<double> buy_dist_price(3.02, 3.29);
    uniform_real_distribution<double> buy_dist_quantity(12.01, 1242.02);

    TOrders bids;

    std::vector<order> v_bids;


    for(int i =0; i<1000000; i++){
        auto rp = buy_dist_price(gen);
        auto rq = buy_dist_quantity(gen);
        long epoch_milli = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();
        //cout << i << endl;
        v_bids.emplace_back(rp, epoch_milli, rq, i, order::order_type::buy);

    }





    long epoch_milli_bid_insertion_start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

/*    for (auto it = begin (v_bids); it != end (v_bids); ++it) {
        bids.insert(*it );
    }*/

    bids.insert(begin(v_bids), end(v_bids));

    long epoch_milli_bid_insertion_end = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    cout << "epoch_milli_bid_insertion_start " <<  epoch_milli_bid_insertion_start << " epoch_milli_bid_insertion_end " << epoch_milli_bid_insertion_end << endl;

    cout << "time_elapsed_bid_insertion: " << epoch_milli_bid_insertion_end - epoch_milli_bid_insertion_start << endl;




    //print_orders(asks);









/*    std::array<order, 6> a = {{{3.33, 8832111,423.32, 24827, order::order_type::sell},
                               {3.38, 8899498,323.32, 24909, order::order_type::sell},
                               {3.48, 8830498,11.45, 24339, order::order_type::sell},
                               {3.39, 8813298,12.01, 24539, order::order_type::sell},
                               {3.41, 8891002,1242.02, 24112, order::order_type::sell},
                               {3.39, 8899992,142.11, 24222, order::order_type::sell},

                               }};

    auto values = rxcpp::observable<>::iterate(a);
    values.subscribe(
            [](order v){printf("OnNext: %ld\n", v.id);},
            [](){printf("OnCompleted\n");});
    printf("//! [from sample]\n");*/



/*    asks.insert({3.33, 8832111,423.32, 24827, order::order_type::sell});
    asks.insert({3.38, 8899498,323.32, 24909, order::order_type::sell});
    asks.insert({
                        {3.48, 8830498,11.45, 24339, order::order_type::sell},
                        {3.39, 8813298,12.01, 24539, order::order_type::sell},
                        {3.41, 8891002,1242.02, 24112, order::order_type::sell},
                        {3.39, 8899992,142.11, 24222, order::order_type::sell},

    });
    asks.insert({3.33, 8872883, 122.25, 24823, order::order_type::sell});
    asks.insert({3.31, 8834888,22.78, 24829, order::order_type::sell});*/



   // TOrders bids;

/*
    bids.insert({3.27, 8832498,22.78, 24129, order::order_type::buy});
    bids.insert({3.27, 8832443,423.32, 24262, order::order_type::buy});
    bids.insert({3.23, 8832003, 122.25, 24833, order::order_type::buy});
    bids.insert({3.27, 8832498,323.32, 24509, order::order_type::buy});
    bids.insert({{3.28, 8831498,12.01, 24139, order::order_type::buy},
                    {3.29, 8831432,142.11, 24272, order::order_type::buy},
                    {3.22, 8831002,1242.02, 24211, order::order_type::buy},
                    {3.02, 8830498,88.24, 24590, order::order_type::buy}});
*/

    //order order1{3.42, 8854498,111.45, 449348, order::order_type::sell};



  //  auto m = processOrder(bids, asks, order1);

    //print_orders(asks);
    //print_orders(bids);



    uniform_real_distribution<double> market_sell_dist_quantity(12.01, 1242.02);

    TOrders various_orders;

    std::vector<order> v_various_orders;


    for(int i =1; i<=1000000; i++){
        auto rq = market_sell_dist_quantity(gen);
        long epoch_milli = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();
        v_various_orders.emplace_back(DBL_MIN, epoch_milli, rq, i, order::order_type::sell);

        auto rp_bid = buy_dist_price(gen);
        auto rq_bid = buy_dist_quantity(gen);
        epoch_milli = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();
        v_various_orders.emplace_back(rp_bid, epoch_milli, rq_bid, i + 2000000, order::order_type::buy);

    }

    various_orders.insert(begin(v_various_orders), end(v_various_orders));


    cout << "various_orders.size() " << various_orders.size() << endl;


    std::vector<match> matches;

    long epoch_milli_market_sell_start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    for(auto market_sell : various_orders){
        matchOrder(market_sell, bids, asks, matches);
    }
    long epoch_milli_market_sell_end = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    cout << "bids left " << bids.size() << endl;
    cout << "asks left " << asks.size() << endl;

    cout << "epoch_milli_market_sell_start " <<  epoch_milli_market_sell_start << " epoch_milli_market_sell_end " << epoch_milli_market_sell_end << endl;

    cout << "time_elapsed_market_sell: " << epoch_milli_market_sell_end - epoch_milli_market_sell_start << endl;

    cout << "match size " << matches.size() << endl;

    cout << "match per second " << matches.size() * 1000 / (epoch_milli_market_sell_end - epoch_milli_market_sell_start);


    return 0;







    order orderMarketSell{DBL_MIN, 8852298,530.25, 222228, order::order_type::sell};

    matchOrder(orderMarketSell, bids, asks, matches);

    order orderMarketBuy{DBL_MAX, 8852298,1500, 333228, order::order_type::buy};

    matchOrder(orderMarketBuy, bids, asks, matches);

    bids.get<PriceTimeIdx>().modify(bids.get<PriceTimeIdx>().begin(),change_quantity(0.12));

    print_orders(bids);

    zmq::socket_t sock(ctx, zmq::socket_type::rep);
    sock.bind("tcp://127.0.0.1:5555");

    while (true) {
        zmq::message_t z_in;
        sock.recv(z_in);

        auto jmsg_in = nlohmann::json::parse(z_in.to_string_view());
        int x = jmsg_in["randint"];
        std::string thread_id = jmsg_in["thread_id"];
        nlohmann::json jmsg_out;
        jmsg_out["result"] = x * x;
        jmsg_out["server"] = true;

        std::string msg_out = std::to_string(x);
        std::cout << "\nsending back:" << msg_out;
        zmq::message_t z_out(msg_out);
        sock.send(z_out, zmq::send_flags::none);

    }

    return 0;
}


