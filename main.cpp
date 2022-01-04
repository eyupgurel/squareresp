#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>


#include <string>
#include <iostream>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <map>
#include <cfloat>

using namespace boost::asio;
using ip::tcp;

using namespace boost::multi_index;

static zmq::context_t ctx;


struct match{
    match(long requestingOrderId, long respondingOrderId, double matchAmount)
            :requestingOrderId(requestingOrderId),
             respondingOrderId(respondingOrderId),
             matchAmount(matchAmount){}
    long requestingOrderId;
    long respondingOrderId;
    double matchAmount;
};

std::ostream &operator<<(std::ostream &rOut, match const &rMatch) {
    return rOut << "requestingOrderId:" << rMatch.requestingOrderId << " respondingOrderId:" << rMatch.respondingOrderId << " matchAmount:" << rMatch.matchAmount << "\n";
}



struct order {
    enum class order_type{buy = 0, sell = 1};
    order(double price, long epochMilli, double quantity, long id, order_type order_type)
            : price(price),
              epochMilli(epochMilli),
              quantity(quantity),
              id(id),
              order_type(order_type){
            }
    double price;
    long epochMilli;
    double quantity;
    long id;
    order_type order_type;
};


std::ostream &operator<<(std::ostream &rOut, order const &rOrder) {
    auto type = rOrder.order_type == order::order_type::buy ? "buy" : "sell";
    return rOut << "price:" << rOrder.price << " timestamp:" << rOrder.epochMilli << " quantity:" << rOrder.quantity << " id:" << rOrder.id << " order type:" << type << "\n";
}

struct PriceIdx {
};
struct TimeIdx {
};
struct PriceTimeIdx {
};


typedef boost::multi_index_container<
        order,
        boost::multi_index::indexed_by<
                boost::multi_index::ordered_non_unique<boost::multi_index::tag<PriceIdx>, boost::multi_index::member<order, double, &order::price> >,
                boost::multi_index::ordered_non_unique<boost::multi_index::tag<TimeIdx>, boost::multi_index::member<order, long, &order::epochMilli>>,
                boost::multi_index::ordered_non_unique<boost::multi_index::tag<PriceTimeIdx>, boost::multi_index::composite_key<order, boost::multi_index::member<order, double, &order::price>,
                        boost::multi_index::member<order, long, &order::epochMilli>>>>
> TOrders;

typedef TOrders::index<PriceIdx>::type TPriceIdx;
typedef TOrders::index<TimeIdx>::type TTimeIdx;
typedef TOrders::index<PriceTimeIdx>::type TPriceTimeIdx;

inline void print_orders(TOrders& orders){
    auto ordersBeg = orders.get<PriceTimeIdx>().cbegin();
    auto ordersEnd = orders.get<PriceTimeIdx>().cend();
    std::copy(ordersBeg, ordersEnd, std::ostream_iterator<order>(std::cout << "\n"));
}

inline void erase_last(TOrders& orders){
    auto ordersEnd = orders.get<PriceTimeIdx>().cend();
    auto last = std::next(ordersEnd, -1);
    orders.get<PriceTimeIdx>().erase(last, ordersEnd);
    print_orders(orders);
}

inline void erase_first(TOrders& orders){
    auto ordersBeg = orders.get<PriceTimeIdx>().begin();
    auto next = std::next(ordersBeg, 1);
    orders.get<PriceTimeIdx>().erase(ordersBeg, next);
    print_orders(orders);
}

struct change_quantity
{
    explicit change_quantity(const double new_quantity):new_quantity(new_quantity){}

    void operator()(order& order)
    {
        order.quantity = new_quantity;
    }

private:
    double new_quantity;
};


inline match matchSellOrder(TOrders& bids, order& ord){
    if(ord.order_type != order::order_type::sell) throw std::invalid_argument("It must be a sell order.");
    auto bestBid = std::next(bids.get<PriceTimeIdx>().cend(), -1);
    double matchAmount = 0.0;
    if(bestBid->quantity <= ord.quantity){
        ord.quantity -= bestBid->quantity;
        erase_last(bids);
        matchAmount = bestBid->quantity;
    } else{
        bids.get<PriceTimeIdx>().modify(bestBid,change_quantity(bestBid->quantity - ord.quantity));
        matchAmount = ord.quantity;
        ord.quantity = 0.0;
    }
    return {ord.id, bestBid->id, matchAmount};
}

inline match matchBuyOrder(TOrders& asks, order& ord){
    if(ord.order_type != order::order_type::buy) throw std::invalid_argument("It must be a buy order.");
    auto bestAsk = asks.get<PriceTimeIdx>().cbegin();
    double matchAmount = 0.0;
    if(bestAsk->quantity <= ord.quantity){
        ord.quantity -= bestAsk->quantity;
        erase_first(asks);
        matchAmount = bestAsk->quantity;
    } else{
        asks.get<PriceTimeIdx>().modify(bestAsk,change_quantity(bestAsk->quantity - ord.quantity));
        matchAmount = ord.quantity;
        ord.quantity = 0.0;
    }
    return {ord.id, bestAsk->id, matchAmount};
}


inline match processSellOrder(TOrders& bids, TOrders& asks, order& ord){
    if(ord.order_type != order::order_type::sell) throw std::invalid_argument("It must be a sell order.");
    match m {0,0,0.0};
    //Find the best bid.
    auto bestBid = std::next(bids.get<PriceTimeIdx>().cend(), -1);
    if(bestBid->price >= ord.price){ // If best bid is better than (or equal) the sell order price then this order may be deemed as a market order asking for immediate execution
        m =  matchSellOrder(bids, ord);
    } else {
        asks.insert(ord); // This is a limit order and it must be booked
    }
    return m;
}

inline match processBuyOrder(TOrders& bids, TOrders& asks, order& ord){
    if(ord.order_type != order::order_type::buy) throw std::invalid_argument("It must be a buy order.");
    match m {0,0,0.0};
    //Find the best ask.
    auto bestAsk = asks.get<PriceTimeIdx>().cbegin();
    if(bestAsk->price <= ord.price){ // If best ask is better than (or equal) the buy order price then this order may be deemed as a market order asking for immediate execution
        m =  matchBuyOrder(asks, ord);
    } else {
        bids.insert(ord); // This is a limit order and it must be booked
    }
    return m;
}

inline match processOrder(TOrders& bids, TOrders& asks, order& ord){
    switch(ord.order_type){
        case order::order_type::buy:
            return processBuyOrder(bids, asks, ord);

        case order::order_type::sell:
            return processSellOrder(bids, asks, ord);

        default:
            throw std::invalid_argument("Invalid order type.");
    }
}

inline void matchOrder(order& ord, TOrders& bids, TOrders& asks, std::vector<match>& matches){
    auto m =  processOrder(bids, asks, ord);
    if(m.requestingOrderId!=0)
        matches.push_back(m);
    while(ord.quantity>0 && m.requestingOrderId!=0){
        m = processOrder(bids, asks, ord);
        print_orders(asks);
        print_orders(bids);
        matches.push_back(m);
    }
}

int main() {

    TOrders asks;

    asks.insert({3.33, 8832111,423.32, 24827, order::order_type::sell});
    asks.insert({3.38, 8899498,323.32, 24909, order::order_type::sell});
    asks.insert({
                        {3.48, 8830498,11.45, 24339, order::order_type::sell},
                        {3.39, 8813298,12.01, 24539, order::order_type::sell},
                        {3.41, 8891002,1242.02, 24112, order::order_type::sell},
                        {3.39, 8899992,142.11, 24222, order::order_type::sell},

    });
    asks.insert({3.33, 8872883, 122.25, 24823, order::order_type::sell});
    asks.insert({3.31, 8834888,22.78, 24829, order::order_type::sell});



    TOrders bids;

    bids.insert({3.27, 8832498,22.78, 24129, order::order_type::buy});
    bids.insert({3.27, 8832443,423.32, 24262, order::order_type::buy});
    bids.insert({3.23, 8832003, 122.25, 24833, order::order_type::buy});
    bids.insert({3.27, 8832498,323.32, 24509, order::order_type::buy});
    bids.insert({{3.28, 8831498,12.01, 24139, order::order_type::buy},
                    {3.29, 8831432,142.11, 24272, order::order_type::buy},
                    {3.22, 8831002,1242.02, 24211, order::order_type::buy},
                    {3.02, 8830498,88.24, 24590, order::order_type::buy}});

    order order1{3.42, 8854498,111.45, 449348, order::order_type::sell};



    auto m = processOrder(bids, asks, order1);

    print_orders(asks);
    print_orders(bids);

    std::vector<match> matches;

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


