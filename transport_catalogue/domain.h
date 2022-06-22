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
};

