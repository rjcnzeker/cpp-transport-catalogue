#include <string_view>
#include <utility>

#include "geo.h"
#include "transport_catalogue.h"

void TransportCatalogue::AddStop(std::string &name, Coordinates coordinates) {
    Stop stop;
    stop.name_ = std::move(name);
    stop.coordinates_ = coordinates;
    stops_.emplace_back(stop);
    stopname_to_stops_[(stops_.end() - 1)->name_] = &*(stops_.end() - 1);
}

void TransportCatalogue::AddBus(std::string &name, std::deque<std::string_view> stops, bool there_and_back) {
    Bus bus;
    bus.name_ = name;

    for (std::string_view stop_name : stops) {
        bus.bus_stops_.push_back(stopname_to_stops_.at(stop_name));
    }

    bus.there_and_back_ = there_and_back;

    buses_.emplace_back(bus);
    busname_to_buses_[(buses_.end() - 1)->name_] = &*(buses_.end() - 1);
}

TransportCatalogue::Bus TransportCatalogue::GetBus(std::string_view name) {
    if (busname_to_buses_.count(name) != 0) {
        return *busname_to_buses_.at(name);
    }
    return Bus{};
}
