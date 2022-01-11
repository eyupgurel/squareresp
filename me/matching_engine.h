//
// Created by egl on 11.01.2022.
//

#ifndef SQUARERESP_MATCHING_ENGINE_H
#define SQUARERESP_MATCHING_ENGINE_H

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
#include <chrono>




using namespace boost::asio;
using ip::tcp;

using namespace boost::multi_index;

static zmq::context_t ctx;

using namespace boost::asio;
using ip::tcp;

using namespace boost::multi_index;

#include <regex>
#include <random>
using namespace std;
using namespace std::chrono;

#include "rxcpp/rx.hpp"
namespace Rx {
    using namespace rxcpp;
    using namespace rxcpp::sources;
    using namespace rxcpp::operators;
    using namespace rxcpp::util;
}
using namespace Rx;


struct match{
    match(long requestingOrderId, long respondingOrderId, double matchAmount)
            :requestingOrderId(requestingOrderId),
             respondingOrderId(respondingOrderId),
             matchAmount(matchAmount){}
    long requestingOrderId;
    long respondingOrderId;
    double matchAmount;
};


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

std::ostream &operator<<(std::ostream &rOut, match const &rMatch);

std::ostream &operator<<(std::ostream &rOut, order const &rOrder);

inline void print_orders(TOrders& orders);

inline void erase_last(TOrders& orders);

inline void erase_first(TOrders& orders);

void matchOrder(order& ord, TOrders& bids, TOrders& asks, std::vector<match>& matches);

inline match matchSellOrder(TOrders& bids, order& ord);

inline match matchBuyOrder(TOrders& asks, order& ord);
inline match processSellOrder(TOrders& bids, TOrders& asks, order& ord);
inline match processBuyOrder(TOrders& bids, TOrders& asks, order& ord);

inline match processOrder(TOrders& bids, TOrders& asks, order& ord);




#endif //SQUARERESP_MATCHING_ENGINE_H
