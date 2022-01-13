#include "test/churn.h"


std::string get_pid() {
    std::stringstream s;
    s << std::this_thread::get_id();
    return s.str();
}



int main() {

    rxcpp::observable<>::range(1, 23).
    subscribe_on(rxcpp::observe_on_new_thread()).
            map([](int v) {
                return std::make_tuple(get_pid(), v);}).
            as_blocking().
                subscribe(
            rxcpp::util::apply_to(
                    [](const std::string pid, int v) {
                        printf("[thread %s] OnNext: %d\n", pid.c_str(), v);

                        churn();

                    }),
            [](){printf("[thread %s] OnCompleted\n", get_pid().c_str());});





    return 0;







/*    order orderMarketSell{DBL_MIN, 8852298,530.25, 222228, order_type::sell};

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

    }*/

    return 0;
}


