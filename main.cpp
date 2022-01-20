#include "test/churn.h"
#include <matching_engine.h>


std::string get_pid() {
    std::stringstream s;
    s << std::this_thread::get_id();
    return s.str();
}

void to_json(nlohmann::json& j, const order& o) {
    j = nlohmann::json{
            {"price", o.price},
            {"epochMilli", o.epochMilli},
            {"quantity", o.quantity},
            {"id", o.id},
            {"ot", o.ot}
    };
}

void from_json(const nlohmann::json& j, order& o) {
    o.price = j.at("price").get<double>();
    o.epochMilli = j.at("epochMilli").get<long>();
    o.quantity = j.at("quantity").get<double>();
    o.id = j.at("id").get<long>();
    o.ot = j.at("ot").get<int>();
}

void to_json(nlohmann::json& j, const match& m) {
    j = nlohmann::json{
            {"requestingOrderId", m.requestingOrderId},
            {"respondingOrderId", m.respondingOrderId},
            {"matchAmount", m.matchAmount}
    };
}

void from_json(const nlohmann::json& j, match& m) {
    m.requestingOrderId = j.at("requestingOrderId").get<long>();
    m.respondingOrderId = j.at("respondingOrderId").get<long>();
    m.matchAmount = j.at("matchAmount").get<double>();
}


#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

static zmq::context_t ctx;

int main() {

    TOrders asks;
    TOrders bids;
    std::vector<match> matches;

    zmq::socket_t sock(ctx, zmq::socket_type::rep);
    sock.bind("tcp://127.0.0.1:4444");

/*    while (true) {
        zmq::message_t z_in;
        sock.recv(z_in);

        auto jmsg_in = nlohmann::json::parse(z_in.to_string_view());

        for(auto o: jmsg_in){
            order ord(o["price"],o["epochMilli"],o["quantity"],o["id"],o["ot"]);
            matchOrder(ord, bids, asks, matches);
        }
        nlohmann::json jmsg_out(matches);
        zmq::message_t z_out(jmsg_out.dump());
        sock.send(z_out, zmq::send_flags::none);
        asks.clear();
        bids.clear();
        matches.clear();
    }*/


    rxcpp::observable<>::range(1, 2000).
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
}
#pragma clang diagnostic pop


