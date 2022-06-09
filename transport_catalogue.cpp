#include <string_view>
#include <utility>
#include <set>

#include "geo.h"
#include "transport_catalogue.h"

using namespace std;

void TransportCatalogue::AddStop(std::string &name, Coordinates coordinates) {
    Stop stop;
    stop.name_ = std::move(name);
    stop.coordinates_ = coordinates;
    stops_.emplace_back(stop);
    stopname_to_stops_[(stops_.end() - 1)->name_] = &*(stops_.end() - 1);
}

void TransportCatalogue::AddBus(std::string &name, const std::deque<std::string_view> &stops, bool there_and_back) {
    Bus bus;
    bus.name_ = name;

    set<string_view> unique_stops(stops.begin(), stops.end());

    bus.unique_stops_ = unique_stops.size();

    bool first_stop = true;
    double distances = 0;

    Coordinates left(0.0, 0.0);
    Coordinates right(0.0, 0.0);
    for (std::string_view stop_name : stops) {
        bus.bus_stops_.push_back(stopname_to_stops_.at(stop_name));

        if (first_stop) {
             left = stopname_to_stops_.at(stop_name)->coordinates_;
        }
        right = stopname_to_stops_.at(stop_name)->coordinates_;

        if (!first_stop) {
            distances += ComputeDistance(left, right);
        }

        first_stop = false;
        left = right;
    }
    bus.there_and_back_ = there_and_back;
    bus.distance_ = distances;

    buses_.emplace_back(bus);
    busname_to_buses_[(buses_.end() - 1)->name_] = &*(buses_.end() - 1);
}

TransportCatalogue::Bus TransportCatalogue::GetBus(std::string_view name) {
    name.remove_prefix(4);
    if (busname_to_buses_.count(name) != 0) {
        return *busname_to_buses_.at(name);
    }
    return Bus{};
}
