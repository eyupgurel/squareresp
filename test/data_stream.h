//
// Created by egl on 11.01.2022.
//

#ifndef SQUARERESP_DATA_STREAM_H
#define SQUARERESP_DATA_STREAM_H

#include <random>
#include <chrono>
#include "../me/matching_engine.h"

using namespace std;
using namespace std::chrono;

void prepareOrderVector(int count, order_type order_type, double price, double quantity_dist_min, double quantity_dist_max,
                        std::vector<order>& v_orders);

void prepareOrderVector(int count, order_type order_type, double price_dist_min, double price_dist_max,
                        double quantity_dist_min, double quantity_dist_max,
                        std::vector<order>& v_orders);
void prepareOrderSet (std::vector<order>& v_orders,
                      TOrders& orders);



#endif //SQUARERESP_DATA_STREAM_H
