//
// Created by egl on 13.01.2022.
//

#include "churn.h"
#include <data_stream.h>
void churn(){

    const int order_count = 250000;
    long epoch_milli = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    ).count();

    TOrders asks;
    TOrders bids;
    std::vector<order> v_various_orders;
    prepareOrderVector(order_count,1,0.0,11.45, 1242.02,v_various_orders);
    prepareOrderVector(order_count, 0,3.02, 3.29,12.01, 1242.02,v_various_orders);
    prepareOrderVector(order_count, 0,DBL_MAX, 12.01, 1242.02,v_various_orders);
    prepareOrderVector(order_count,1,3.33, 3.48,11.45, 1242.02,v_various_orders);

    TOrders various_orders;

    various_orders.insert(begin(v_various_orders), end(v_various_orders));
    cout << "various_orders.size() " << various_orders.size() << endl;
    std::vector<match> matches;
    long epoch_milli_various_order_start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    for(auto ord : various_orders){
        if(epoch_milli - ord.epochMilli > 274877906944) {
            matchOrder(ord, bids, asks, matches);
        } else if (ord.ot == 0) {
            bids.insert(ord);
        } else {
            asks.insert(ord);
        }
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

    cout << "match per second " << matches.size() * 1000 / duration << endl;
    cout << "order per second " << order_count * 4 * 1000 / duration << endl;
}