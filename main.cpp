#include "test/churn.h"
#include <matching_engine.h>
#include <zmq_addon.hpp>

#include "rxcpp/rx.hpp"
namespace Rx {
    using namespace rxcpp;
    using namespace rxcpp::sources;
    using namespace rxcpp::operators;
    using namespace rxcpp::util;
}
using namespace Rx;

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
            {"ot", o.ot},
            {"cud", o.cud},
    };
}

void from_json(const nlohmann::json& j, order& o) {
    o.price = j.at("price").get<double>();
    o.epochMilli = j.at("epochMilli").get<long>();
    o.quantity = j.at("quantity").get<double>();
    o.id = j.at("id").get<long>();
    o.ot = j.at("ot").get<int>();
    o.cud = j.at("cud").get<int>();
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

void to_json(nlohmann::json& j, const engine_state& es) {
    j = nlohmann::json{
            {"asks", es.asks},
            {"bids", es.bids},
            {"matches", es.matches}
    };
}

void from_json(const nlohmann::json& j, engine_state& es) {
    auto asks = j.at("asks").get<nlohmann::json::array_t>();
    for(auto ask : asks){
        order ord(ask["price"],ask["epochMilli"],ask["quantity"],ask["id"],ask["ot"],ask["cud"]);
        es.asks.emplace_back(ord);
    }
    auto bids = j.at("bids").get<nlohmann::json::array_t>();
    for(auto bid : bids){
        order ord(bid["price"],bid["epochMilli"],bid["quantity"],bid["id"],bid["ot"],bid["cud"]);
        es.bids.emplace_back(ord);
    }
    auto matches = j.at("matches").get<nlohmann::json::array_t>();
    for(auto j_match : matches){
        match match(j_match["requestingOrderId"],j_match["respondingOrderId"],j_match["matchAmount"]);
        es.matches.emplace_back(match);
    }
}
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

static zmq::context_t ctx;

inline void write_orders(TOrders& orders, std::vector<order>& vorders){
    auto ordersBeg = orders.get<PriceTimeIdx>().cbegin();
    auto ordersEnd = orders.get<PriceTimeIdx>().cend();
    for(auto it=ordersBeg; it!=ordersEnd;++it )
        vorders.emplace_back(*it);
}

int main(int argc, char * argv[]) {

    TOrders asks;
    TOrders bids;
    std::vector<match> matches;

    zmq::socket_t sock(ctx, zmq::socket_type::rep);
    sock.bind(argv[1]);

/*    while (true) {
        zmq::message_t z_in;
        sock.recv(z_in);

        auto jmsg_in = nlohmann::json::parse(z_in.to_string_view());

        for(auto o: jmsg_in){
            order ord(o["price"],o["epochMilli"],o["quantity"],o["id"],o["ot"],o["cud"]);
            match_order(ord, bids, asks, matches);
        }


        std::vector<order> v_asks;
        write_orders(asks, v_asks);
        std::vector<order> v_bids;
        write_orders(bids, v_bids);

        engine_state es(v_asks, v_bids, matches);

        nlohmann::json jmsg_out_engine_state(es);
        zmq::message_t z_out_engine_state(jmsg_out_engine_state.dump());
        sock.send(z_out_engine_state, zmq::send_flags::none);

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


