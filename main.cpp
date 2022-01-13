#include "me/matching_engine.h"
#include "test/data_stream.h"


std::string get_pid() {
    std::stringstream s;
    s << std::this_thread::get_id();
    return s.str();
}

int main() {
    random_device rd;   // non-deterministic generator
    mt19937 gen(rd());

    std::vector<order> v_asks;
    prepareOrderVector(1000000,order_type::sell,3.33, 3.48,11.45, 1242.02,v_asks);

    TOrders asks;
    prepareOrderSet(v_asks,asks);



    std::vector<order> v_bids;
    prepareOrderVector(1000000,order_type::buy,3.02, 3.29,12.01, 1242.02,v_bids);

    TOrders bids;
    prepareOrderSet(v_bids,bids);

    std::vector<order> v_various_orders;
    prepareOrderVector(500000,order_type::sell,DBL_MIN,11.45, 1242.02,v_various_orders);
    prepareOrderVector(500000, order_type::buy,3.02, 3.29,12.01, 1242.02,v_various_orders);


    TOrders various_orders;

    various_orders.insert(begin(v_various_orders), end(v_various_orders));

    cout << "various_orders.size() " << various_orders.size() << endl;

    std::vector<match> matches;

    long epoch_milli_various_order_start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    for(auto order : various_orders){
        matchOrder(order, bids, asks, matches);
    }
    long epoch_milli_various_order_end = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    cout << "bids left " << bids.size() << endl;
    cout << "asks left " << asks.size() << endl;

    auto duration = epoch_milli_various_order_end - epoch_milli_various_order_start;

    cout << "time_elapsed_various_order_processed: " << duration << endl;

    cout << "match size " << matches.size() << endl;

    if(duration == 0)
        duration ++;

    cout << "match per second " << matches.size() * 1000 / duration;


    return 0;







    order orderMarketSell{DBL_MIN, 8852298,530.25, 222228, order_type::sell};

    matchOrder(orderMarketSell, bids, asks, matches);

    order orderMarketBuy{DBL_MAX, 8852298,1500, 333228, order_type::buy};

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


