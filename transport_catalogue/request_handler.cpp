#include "request_handler.h"

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <iomanip>

using namespace std;

namespace cin_input {

    InputReader::InputReader(int number_requests, istream& input) : request_queue_() {
        for (int i = 0; i < number_requests; ++i) {
            string line;
            getline(input, line);
            if (line[0] == 'B') {
                line = line.substr(line.find_first_not_of(' ', 3));
                request_queue_[RequestType::BUS].push_back(move(line));
                continue;
            }
            if (line[0] == 'S') {
                line = line.substr(line.find_first_not_of(' ', 4));
                request_queue_[RequestType::STOP].push_back(move(line));
                continue;
            }
        }
    }

    transport_catalogue::TransportCatalogue InputReader::ProcessRequests() {
        transport_catalogue::TransportCatalogue transport_catalogue;

        for (string_view request : request_queue_[RequestType::STOP]) {
            ProcessStops(transport_catalogue, request);
        }
        for (string_view request : request_queue_[RequestType::BUS]) {
            ProcessBuses(transport_catalogue, request);
        }
        return transport_catalogue;
    }

    void InputReader::ProcessBuses(transport_catalogue::TransportCatalogue& transport_catalogue, string_view& request) {
        string bus_name = string(request.substr(0, request.find_first_of(':')));
        request.remove_prefix(request.find_first_not_of(' ', bus_name.size() + 1));

        bool there_and_back = false;
        char separator = request[min(request.find('-'), request.find('>'))];
        if (separator == '-') {
            there_and_back = true;
        }

        deque<string> stops;
        while (!request.empty()) {
            string_view stop = request.substr(0, request.find(separator));
            stop = stop.substr(0, stop.find_last_not_of(' ') + 1);
            stops.emplace_back(stop);

            if (request.find_first_of(' ', stop.size()) > request.size()) {
                break;
            }

            request.remove_prefix(request.find(separator) + 1);
            request.remove_prefix(request.find_first_not_of(' '));
        }
        transport_catalogue.AddBus(bus_name, stops, there_and_back);
    }

    void InputReader::ProcessStops(transport_catalogue::TransportCatalogue& transport_catalogue, string_view& request) {
        string stop_name = string(request.substr(0, request.find_first_of(':')));
        request.remove_prefix(request.find_first_not_of(' ', stop_name.size() + 1));

        geo::Coordinates coordinates(0.0, 0.0);
        string_view latitude = request.substr(0, request.find_first_of(','));
        coordinates.lat = stod(string(latitude));
        request.remove_prefix(latitude.size() + 1);
        string_view longitude = request.substr(1, request.find_first_of(',') - 1);
        coordinates.lng = stod(string(longitude));
        request.remove_prefix(longitude.size() + 2);

        bool have_distances = true;
        if (request.size() == request.npos) {
            have_distances = false;
        }

        map<string, int> distances;
        while (have_distances) {
            request.remove_prefix(request.find_first_not_of(' '));
            string distance_to_next_str = string(request.substr(0, request.find_first_of('m')));
            int distance_to_next = stoi(distance_to_next_str);
            request.remove_prefix(distance_to_next_str.size() + 2);

            request.remove_prefix(request.find(' ') + 1);
            string name_next = string(request.substr(0, request.find(',')));

            distances.insert({name_next, distance_to_next});

            auto request_next_words_size = request.find_first_of(' ', name_next.size());
            if (request_next_words_size > request.size()) {
                break;
            }
            request.remove_prefix(max(name_next.size(), request.find_first_of(' ', name_next.size())));
        }
        transport_catalogue.AddStop(stop_name, coordinates, distances);
    }
} // namespace cin_input_reader

namespace cin_output {

    void ReadQuery(int num_queries, transport_catalogue::TransportCatalogue& transport_catalogue, istream& input) {

        ostream& output = cout;

        for (int i = 0; i < num_queries; ++i) {
            string line;
            getline(input, line);

            if (line[0] == 'B') {
                print::Buss(line, transport_catalogue, output);
            }

            if (line[0] == 'S') {
                print::Stop(line, transport_catalogue, output);
            }
        }
    }

    void print::Stop(string& name, transport_catalogue::TransportCatalogue& transport_catalogue, ostream& output) {
        output << name << ": ";

        if (transport_catalogue.GetStop(name).name_.empty()) {
            output << "not found" << '\n';
            return;
        }

        set<string> buses_on_stop = transport_catalogue.GetBusesOnStop(name);

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

    void
    print::Buss(const string& name, transport_catalogue::TransportCatalogue& transport_catalogue, ostream& output) {

        output << name << ": "s;
        Bus bus = transport_catalogue.GetBus(name);

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
}// namespace cin_output_reader

void RequestHandler::AddBus(string name, deque<string> stops, bool there_and_back) {
    db_.AddBus(name, stops, there_and_back);
}

void RequestHandler::AddStop(string name, geo::Coordinates coordinates, const map<string, int>& distances) {
    db_.AddStop(name, coordinates, distances);
}

Bus RequestHandler::GetBus(const string& name) {
    return db_.GetBus(name);
}

Stop RequestHandler::GetStop(const string& name) {
    return db_.GetStop(name);
}

set<string> RequestHandler::GetBusesOnStop(const string& name) {
    return db_.GetBusesOnStop(name);
}

set<const Bus*, BusComparator> RequestHandler::GetBuses() const {
    return db_.GetBuses();
}

void RequestHandler::RenderMap(svg::Document& doc) const {

    set<const Bus*, BusComparator> buses = GetBuses();
    renderer_.Render(doc, buses);

}