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
