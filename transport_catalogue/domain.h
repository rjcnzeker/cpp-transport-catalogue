#pragma once

#include <string>
#include <vector>
#include "geo.h"

struct Stop {
    std::string name_;
    geo::Coordinates coordinates_;
};

struct StopComparator {
    bool operator()(Stop *lhs, const Stop *rhs) const {
        return lhs->name_ < rhs->name_;
    }
};

struct Bus {
    std::string name_;
    std::vector<Stop *> bus_stops_;
    bool there_and_back_;
    int unique_stops_;
    double coordinate_distance_;
    double distance_;
    float curvature_;

};
struct BusComparator {
    bool operator()(const Bus& lhs, const Bus& rhs) const {
        return lhs.name_ < rhs.name_;
    }
};


