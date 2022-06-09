#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <iomanip>

#include "stat_reader.h"

using namespace std;

void ReadQuery(int num_queries, TransportCatalogue &transport_catalogue) {
    for (int i = 0; i < num_queries; ++i) {
        string line;
        getline(cin, line);

        if (line[0] == 'B') {
            PrintBus(line, transport_catalogue);
        }
    }
}

void PrintBus(const string& name, TransportCatalogue & transport_catalogue) {

    cout << name << ": "s;
    TransportCatalogue::Bus bus = transport_catalogue.GetBus(name);

    if (bus.bus_stops_.empty()) {
        cout << "not found" << '\n';
        return;
    }

    int stops = bus.there_and_back_ ? (bus.bus_stops_.size() * 2) - 1 : bus.bus_stops_.size();

    cout << stops << " stops on route, "s;

    cout << bus.unique_stops_ << " unique stops, "s;

    double lenght = bus.there_and_back_ ? bus.distance_ * 2 : bus.distance_;

    cout << setprecision(6) <<  lenght << " route length"s;

    cout << '\n';
}