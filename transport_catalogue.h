#pragma once

#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "geo.h"

class TransportCatalogue {

public:

    struct Stop {
        std::string name_;
        Coordinates coordinates_;
    };

    struct Bus {
        std::string name_;
        std::vector<Stop *> bus_stops_;
        bool there_and_back_;
        int unique_stops_;
        double distance_;
    };

    TransportCatalogue() = default;

    void AddStop(std::string &name, Coordinates coordinates);

    void AddBus(std::string &name, const std::deque<std::string_view>& stops, bool there_and_back);

    Bus GetBus(std::string_view name);

private:

    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, Stop *> stopname_to_stops_;

    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Bus *> busname_to_buses_;


};