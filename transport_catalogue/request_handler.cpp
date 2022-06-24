#include "request_handler.h"

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <iomanip>

using namespace std;

namespace cin_input {

    InputReader::InputReader(int number_requests, istream &input) : request_queue_() {
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

    void InputReader::ProcessBuses(transport_catalogue::TransportCatalogue &transport_catalogue, string_view &request) {
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

    void InputReader::ProcessStops(transport_catalogue::TransportCatalogue &transport_catalogue, string_view &request) {
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

    void ReadQuery(int num_queries, transport_catalogue::TransportCatalogue &transport_catalogue, istream &input) {

        ostream &output = cout;

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

    void print::Stop(string &name, transport_catalogue::TransportCatalogue &transport_catalogue, ostream &output) {
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

    void
    print::Buss(const string &name, transport_catalogue::TransportCatalogue &transport_catalogue, ostream &output) {

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

namespace json {

    Document RequestHandler::JSONProcessRequests(istream &input) {
        json::Document document(json::Load(input));

        std::vector<json::Node> base_requests_buses;
        std::vector<json::Node> base_requests_stops;

        std::vector<json::Node> stat_requests;
        std::vector<json::Node> stat_requests_buses;

        for (const auto &node : document.GetRoot().AsMap()) {
            if (node.first == "base_requests") {
                for (const auto &request : node.second.AsArray()) {
                    if (request.AsMap().at("type") == "Bus") {
                        base_requests_buses.push_back(request);
                    } else if (request.AsMap().at("type") == "Stop") {
                        base_requests_stops.push_back(request);
                    }
                }
                continue;
            } else if (node.first == "stat_requests") {
                for (const auto &request : node.second.AsArray()) {

                    stat_requests.push_back(request);

                }
            }
        }

        ProcessBaseStopRequests(base_requests_stops);

        ProcessBaseBusRequests(base_requests_buses);

        return ProcessStateRequests(stat_requests);
    }

    void RequestHandler::ProcessBaseBusRequests(const vector <json::Node> &buses_requests) const {
        std::string name;
        std::deque<std::string> stops;
        bool there_and_back;
        for (const auto &bus : buses_requests) {
            auto bus_date = bus.AsMap();
            name = bus_date.at("name").AsString();
            there_and_back = !bus_date.at("is_roundtrip").AsBool();
            for (const auto &stop : bus_date.at("stops").AsArray()) {
                stops.emplace_back(stop.AsString());
            }
            db_.AddBus(name, stops, there_and_back);
        }
    }

    void RequestHandler::ProcessBaseStopRequests(const vector <json::Node> &stops_requests) const {
        std::string name;
        geo::Coordinates coordinates(0.0, 0.0);

        for (const auto &stop : stops_requests) {
            auto stop_date = stop.AsMap();
            name = stop_date.at("name").AsString();
            coordinates.lng = stop_date.at("longitude").AsDouble();
            coordinates.lat = stop_date.at("latitude").AsDouble();
            std::map<std::string, int> distances{};
            for (auto &stop_distance : stop_date.at("road_distances").AsMap()) {
                distances[stop_distance.first] = stop_distance.second.AsInt();
            }

            db_.AddStop(name, coordinates, distances);
        }
    }

    Document RequestHandler::ProcessStateRequests(const std::vector<Node> &state_stops_requests) {
        Array arr;
        for (const auto &request : state_stops_requests) {
            auto request_data = request.AsMap();
            if (request_data.at("type") == "Stop") {
                Stop stop = db_.GetStop(request_data.at("name").AsString());
                if (stop.name_.empty()) {
                    arr.emplace_back(Dict{
                            {"request_id"s, request_data.at("id").AsInt()},
                            {"error_message"s,  "not found"s} });
                    continue;
                }
                std::set<string_view> buses_on_stop = db_.GetBusesOnStop(request_data.at("name").AsString());
                vector<string> buses_on_stop_vect(buses_on_stop.begin(), buses_on_stop.end());

                arr.emplace_back(Dict{
                        {"request_id"s, request_data.at("id").AsInt()},
                        {"buses"s,     Array{buses_on_stop_vect.begin(), buses_on_stop_vect.end()}}});
                continue;
            }
            if (request_data.at("type") == "Bus") {


                Bus bus = db_.GetBus(request_data.at("name").AsString());
                if (bus.name_.empty()) {
                    arr.emplace_back(Dict{
                            {"request_id"s,       request_data.at("id").AsInt()},
                            { "error_message"s,  "not found"s }});
                    continue;
                }

                int stops = bus.there_and_back_ ? (bus.bus_stops_.size() * 2) - 1 : bus.bus_stops_.size();

                arr.emplace_back(Dict{
                        {"request_id"s,       request_data.at("id").AsInt()},
                        {"curvature"s,        bus.curvature_},
                        {"route_length"s,    bus.distance_},
                        {"stop_count",        stops},
                        {"unique_stop_count", bus.unique_stops_}}); //TODO
            }
        }

        return Document{arr};
    }
}



