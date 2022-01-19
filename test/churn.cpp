//
// Created by egl on 13.01.2022.
//

#include "churn.h"
#include <data_stream.h>
void churn(){
    TOrders asks;
    TOrders bids;

    std::vector<order> v_various_orders;
    prepareOrderVector(250000,1,0.0,11.45, 1242.02,v_various_orders);
    prepareOrderVector(250000, 0,3.02, 3.29,12.01, 1242.02,v_various_orders);
    prepareOrderVector(250000, 0,DBL_MAX, 12.01, 1242.02,v_various_orders);
    prepareOrderVector(250000,1,3.33, 3.48,11.45, 1242.02,v_various_orders);

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

    cout << "match per second " << matches.size() * 1000 / duration << endl;
}