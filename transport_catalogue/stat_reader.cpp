#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <iomanip>

#include "stat_reader.h"

using namespace std;

using namespace transport_catalogue;

void transport_catalogue::ReadQuery(int num_queries, TransportCatalogue &transport_catalogue, istream &input) {

    ostream &output = cout;

    for (int i = 0; i < num_queries; ++i) {
        string line;
        getline(input, line);

        if (line[0] == 'B') {
            transport_catalogue::print::Bus(line, transport_catalogue, output);
        }

        if (line[0] == 'S') {
            transport_catalogue::print::Stop(line, transport_catalogue, output);
        }
    }
}

void transport_catalogue::print::Stop(const string &name, TransportCatalogue &transport_catalogue, ostream &output) {
    output << name << ": ";

    if (transport_catalogue.GetStop(name).name_.empty()) {
        output << "not found" << '\n';
        return;
    }

    set<string_view> buses_on_stop = transport_catalogue.GetBusesOnStop(name);

    if (buses_on_stop.empty()) {
        output << "no buses" << '\n';
        return;
    }

    output << "buses";

    for (string_view bus : buses_on_stop) {
        output << " " << bus;
    }

    output << '\n';
}

void transport_catalogue::print::Bus(const string &name, TransportCatalogue &transport_catalogue, ostream &output) {

    output << name << ": "s;
    TransportCatalogue::Bus bus = transport_catalogue.GetBus(name);

    if (bus.bus_stops_.empty()) {
        output << "not found" << '\n';
        return;
    }

    int stops = bus.there_and_back_ ? (bus.bus_stops_.size() * 2) - 1 : bus.bus_stops_.size();

    output << stops << " stops on route, "s;

    output << bus.unique_stops_ << " unique stops, "s;

    // double distance = bus.there_and_back_ ? bus.distance_ * 2 : bus.distance_;

    output << setprecision(6) << bus.distance_ << " route length, "s;

    output << setprecision(6) << bus.curvature_ << " curvature"s;

    output << '\n';
}