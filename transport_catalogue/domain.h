#pragma once

#include <string>
#include <vector>
#include "geo.h"

struct Stop {
    std::string name_;
    geo::Coordinates coordinates_;
};

struct Bus {
    std::string name_;
    std::vector<Stop *> bus_stops_;
    bool there_and_back_;
    int unique_stops_;
    double coordinate_distance_;
    double distance_;
    float curvature_;
/*
    bool operator< (const Bus &rhs) {
       return this->name_ < rhs.name_;
    }*/


};
struct BusComparator {
    // Помечаем компаратор как «прозрачный», чтобы с его помощью можно было сравнивать
    // не только кошек с кошками, но и со строками, задающими породу кошек
    //using is_transparent = std::true_type;

    bool operator()(const Bus& lhs, const Bus& rhs) const {
        return lhs.name_ < rhs.name_;
    }
};


