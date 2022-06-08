#include "geo.h"

#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class TransportCatalogue {
public:

    TransportCatalogue() = default;

    void AddStop(std::string &name, Coordinates coordinates);

private:

    struct Stop {
        std::string name_;
        Coordinates coordinates_;
    };
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, Stop *> stopname_to_stops_;

    struct Bus {
        std::string name_;
        std::vector<Stop *> bus_stops_;
    };

    std::deque<Stop> buses_;
    std::unordered_map<std::string_view, Bus *> stopname_to_buses_;

};