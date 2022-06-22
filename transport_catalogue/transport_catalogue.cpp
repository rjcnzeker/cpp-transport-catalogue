#include <string_view>
#include <utility>
#include <set>
#include <map>

#include "geo.h"
#include "transport_catalogue.h"

using namespace std;

namespace transport_catalogue {

    void TransportCatalogue::AddStop(std::string &name, geo::Coordinates coordinates,
                                     const map<string_view, int> &distances) {
        Stop stop;
        stop.name_ = name;
        stop.coordinates_ = coordinates;
        stops_.emplace_back(stop);
        stopname_to_stops_[(stops_.end() - 1)->name_] = &*(stops_.end() - 1);

        for (auto pair_stops : distances) {
            distances_.insert({{(stops_.end() - 1)->name_, pair_stops.first}, pair_stops.second});
        }
    }

    void TransportCatalogue::AddBus(std::string &name, const std::deque<std::string_view> &stops, bool there_and_back) {
        Bus bus;
        bus.name_ = name;

        set<string_view> unique_stops(stops.begin(), stops.end());

        bus.unique_stops_ = unique_stops.size();

        bool first_stop = true;
        double coordinates_distances = 0;
        int map_distance = 0;

        geo::Coordinates left_coord(0.0, 0.0);
        geo::Coordinates right_coord(0.0, 0.0);
        string_view left_stop, right_stop;
        for (std::string_view stop_name : stops) {
            bus.bus_stops_.push_back(stopname_to_stops_.at(stop_name));

            if (first_stop) {
                left_coord = stopname_to_stops_.at(stop_name)->coordinates_;
                left_stop = stop_name;
            }

            right_coord = stopname_to_stops_.at(stop_name)->coordinates_;
            right_stop = stop_name;

            if (!first_stop) {
                coordinates_distances += ComputeDistance(left_coord, right_coord);

                if (there_and_back) {
                    if (distances_.count({left_stop, right_stop}) == 0) {
                        map_distance += distances_.at({right_stop, left_stop}) * 2;
                    } else if (distances_.count({right_stop, left_stop}) == 0) {
                        map_distance += distances_.at({left_stop, right_stop}) * 2;
                    } else {
                        map_distance += distances_.at({left_stop, right_stop});
                        map_distance += distances_.at({right_stop, left_stop});
                    }

                } else {

                    if (distances_.count({left_stop, right_stop}) == 0) {
                        map_distance += distances_.at({right_stop, left_stop});
                    } else {
                        map_distance += distances_.at({left_stop, right_stop});
                    }
                }

            }

            first_stop = false;
            left_coord = right_coord;
            left_stop = right_stop;
        }

        string_view last_stop = right_stop;

        if (there_and_back && distances_.count({last_stop, last_stop}) != 0) {
            map_distance += distances_.at({last_stop, last_stop});
        }

        if (there_and_back) {
            coordinates_distances *= 2;
        }

        bus.there_and_back_ = there_and_back;
        bus.coordinate_distance_ = coordinates_distances;
        bus.distance_ = map_distance;
        bus.curvature_ = bus.distance_ / bus.coordinate_distance_;

        buses_.emplace_back(bus);
        busname_to_buses_[(buses_.end() - 1)->name_] = &*(buses_.end() - 1);
    }

    Bus TransportCatalogue::GetBus(string_view name) {
        name.remove_prefix(4);
        if (busname_to_buses_.count(name) != 0) {
            return *busname_to_buses_.at(name);
        }
        return Bus{};
    }

    std::set<std::string_view> TransportCatalogue::GetBusesOnStop(string& name) {
        string_view name_view = name;
        name_view.remove_prefix(5);

        set<string_view> buses_on_stop;

        for (auto [name_bus, bus] : busname_to_buses_) {
            for (auto stop : bus->bus_stops_) {
                if (stop->name_ == name_view) {
                    buses_on_stop.insert(name_bus);
                }
            }
        }


        return buses_on_stop;
    }

    Stop TransportCatalogue::GetStop(string_view name) {
        name.remove_prefix(5);

        if (stopname_to_stops_.count(name) != 0) {
            return *stopname_to_stops_.at(name);
        }
        return Stop{};
    }

    size_t
    TransportCatalogue::PairStopsHasher::operator()(const std::pair <std::string_view, std::string_view> &stops_pair) const {
        std::string left_plus_right(stops_pair.first.substr(0));
        left_plus_right += stops_pair.second.substr(0);

        return d_hasher_(left_plus_right);
    }

}
