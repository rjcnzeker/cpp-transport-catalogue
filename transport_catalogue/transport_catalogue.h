#pragma once

#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <map>
#include <unordered_map>

#include "geo.h"

namespace transport_catalogue {

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
            double coordinate_distance_;
            double distance_;
            float curvature_;
        };

        TransportCatalogue() = default;

        void AddStop(std::string &name, Coordinates coordinates, const std::map<std::string_view, int> &distances);

        void AddBus(std::string &name, const std::deque<std::string_view> &stops, bool there_and_back);

        Bus GetBus(std::string_view name);

        std::set<std::string_view> GetBusesOnStop(std::string_view name);

        Stop GetStop(std::string_view name);

    private:

        std::deque<Stop> stops_;
        std::unordered_map<std::string_view, Stop *> stopname_to_stops_;

        std::deque<Bus> buses_;
        std::unordered_map<std::string_view, const Bus *> busname_to_buses_;

        struct PairStopsHasger {
            size_t operator()(const std::pair<std::string_view, std::string_view> &stops_pair) const;

        private:
            std::hash<std::string> d_hasher_;
        };

        std::unordered_map<std::pair<std::string_view, std::string_view>, int, PairStopsHasger> distances_;
    };
}
