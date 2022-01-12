//
// Created by egl on 11.01.2022.
//

#include "data_stream.h"

static int counter = 0;

void prepareOrderVector(int count, order_type order_type, double price, double quantity_dist_min, double quantity_dist_max,
                        std::vector<order>& v_orders) {
    random_device rd;   // non-deterministic generator
    mt19937 gen(rd());
    uniform_real_distribution<double> quantity_dist(quantity_dist_min, quantity_dist_max);

    for(int i =1; i<=count; i++){
        auto qd = quantity_dist(gen);
        long epoch_milli = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();
        v_orders.emplace_back(price, epoch_milli, qd, ++counter, order_type);
    }
}


void prepareOrderVector(int count, order_type order_type, double price_dist_min, double price_dist_max,
                        double quantity_dist_min, double quantity_dist_max,
                        std::vector<order>& v_orders) {
    random_device rd;   // non-deterministic generator
    mt19937 gen(rd());
    uniform_real_distribution<double> price_dist(price_dist_min, price_dist_max);
    uniform_real_distribution<double> quantity_dist(quantity_dist_min, quantity_dist_max);


    for(int i =1; i<=count; i++){
        auto pd = price_dist(gen);
        auto qd = quantity_dist(gen);
        long epoch_milli = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();
        v_orders.emplace_back(pd, epoch_milli, qd, ++counter, order_type);
    }
}

void prepareOrderSet (std::vector<order>& v_orders,
                      TOrders& orders) {
    long epoch_milli_start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    orders.insert(begin(v_orders), end(v_orders));
    long epoch_milli_end = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    cout << "epoch_milli_start " <<  epoch_milli_start << " epoch_milli_end " << epoch_milli_end << endl;
    cout << "time_elapsed_insertion: " << epoch_milli_end - epoch_milli_start << endl;
}
