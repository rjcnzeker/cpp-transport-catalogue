#pragma once

#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <map>
#include <unordered_map>

#include "geo.h"
#include "domain.h"

namespace transport_catalogue {

    class TransportCatalogue {
    public:

        TransportCatalogue() = default;

        void AddStop(std::string &name, geo::Coordinates coordinates, const std::map<std::string_view, int> &distances);

        void AddBus(std::string &name, const std::deque<std::string_view> &stops, bool there_and_back);

        Bus GetBus(std::string_view name);

        std::set<std::string_view> GetBusesOnStop(std::string& name);

        Stop GetStop(std::string_view name);

    private:

        std::deque<Stop> stops_;
        std::unordered_map<std::string_view, Stop *> stopname_to_stops_;

        std::deque<Bus> buses_;
        std::unordered_map<std::string_view, const Bus *> busname_to_buses_;

        std::unordered_map<std::pair<std::string_view, std::string_view>, int, PairStopsHasher> distances_;
    };
}
