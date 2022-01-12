//
// Created by egl on 11.01.2022.
//

#include "matching_engine.h"

std::ostream &operator<<(std::ostream &rOut, match const &rMatch) {
    return rOut << "requestingOrderId:" << rMatch.requestingOrderId << " respondingOrderId:" << rMatch.respondingOrderId << " matchAmount:" << rMatch.matchAmount << "\n";
}

std::ostream &operator<<(std::ostream &rOut, order const &rOrder) {
    auto type = rOrder.ot == order_type::buy ? "buy" : "sell";
    return rOut << "price:" << rOrder.price << " timestamp:" << rOrder.epochMilli << " quantity:" << rOrder.quantity << " id:" << rOrder.id << " order type:" << type << "\n";
}

inline void print_orders(TOrders& orders){
    auto ordersBeg = orders.get<PriceTimeIdx>().cbegin();
    auto ordersEnd = orders.get<PriceTimeIdx>().cend();
    std::copy(ordersBeg, ordersEnd, std::ostream_iterator<order>(std::cout << "\n"));
}

inline void erase_last(TOrders& orders){
    auto ordersEnd = orders.get<PriceTimeIdx>().cend();
    auto last = std::next(ordersEnd, -1);
    orders.get<PriceTimeIdx>().erase(last, ordersEnd);
    //print_orders(orders);
}

inline void erase_first(TOrders& orders){
    auto ordersBeg = orders.get<PriceTimeIdx>().begin();
    auto next = std::next(ordersBeg, 1);
    orders.get<PriceTimeIdx>().erase(ordersBeg, next);
    //print_orders(orders);
}

void matchOrder(order& ord, TOrders& bids, TOrders& asks, std::vector<match>& matches){
    auto m =  processOrder(bids, asks, ord);
    if(m.requestingOrderId!=0)
        matches.push_back(m);
    while(ord.quantity>0 && m.requestingOrderId!=0){
        m = processOrder(bids, asks, ord);
        //print_orders(asks);
        //print_orders(bids);
        matches.push_back(m);
    }
}

inline match matchSellOrder(TOrders& bids, order& ord){
    if(ord.ot != order_type::sell) throw std::invalid_argument("It must be a sell order.");
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
    if(ord.ot != order_type::buy) throw std::invalid_argument("It must be a buy order.");
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
    if(ord.ot != order_type::sell) throw std::invalid_argument("It must be a sell order.");
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
    if(ord.ot != order_type::buy) throw std::invalid_argument("It must be a buy order.");
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
    switch(ord.ot){
        case order_type::buy:
            return processBuyOrder(bids, asks, ord);

        case order_type::sell:
            return processSellOrder(bids, asks, ord);

        default:
            throw std::invalid_argument("Invalid order type.");
    }
}

