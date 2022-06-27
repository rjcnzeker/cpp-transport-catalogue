#include <deque>
#include "json.h"
#include "transport_catalogue.h"
#include "json_reader.h"

using namespace std;

namespace json {
    void ProcessBaseBusRequests(const vector <json::Node> &buses_requests,
                                transport_catalogue::TransportCatalogue &catalogue) {
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
            catalogue.AddBus(name, stops, there_and_back);
        }
    }

    void ProcessBaseStopRequests(const vector <json::Node> &stops_requests,
                                 transport_catalogue::TransportCatalogue &catalogue) {
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

            catalogue.AddStop(name, coordinates, distances);
        }
    }

    Document ProcessStateRequests(const std::vector<Node> &state_stops_requests,
                                  transport_catalogue::TransportCatalogue &catalogue) {
        Array arr;
        for (const auto &request : state_stops_requests) {
            auto request_data = request.AsMap();
            if (request_data.at("type") == "Stop") {
                Stop stop = catalogue.GetStop(request_data.at("name").AsString());

                if (stop.name_.empty()) {
                    arr.push_back(Dict{
                            {"error_message", "not found"s},
                            {"request_id",    request_data.at("id").AsInt()}
                    });
                    continue;
                }
                std::set<string> buses_on_stop = catalogue.GetBusesOnStop(request_data.at("name").AsString());
                Array buses_on_stop_arr = {};
                for (const auto &bus : buses_on_stop) {
                    buses_on_stop_arr.push_back(bus);
                }
                arr.push_back(Dict{
                        {"buses",      buses_on_stop_arr},
                        {"request_id", request_data.at("id").AsInt()}
                });
                continue;
            }

            if (request_data.at("type") == "Bus") {
                Bus bus = catalogue.GetBus(request_data.at("name").AsString());

                if (bus.name_.empty()) {
                    arr.push_back(Dict{
                            {"request_id"s,    request_data.at("id").AsInt()},
                            {"error_message"s, "not found"s}});
                    continue;
                }

                int stops = bus.there_and_back_ ? (bus.bus_stops_.size() * 2) - 1 : bus.bus_stops_.size();
                arr.push_back(Dict{
                        {"request_id"s,       request_data.at("id").AsInt()},
                        {"curvature"s,        bus.curvature_},
                        {"route_length"s,     bus.distance_},
                        {"stop_count",        stops},
                        {"unique_stop_count", bus.unique_stops_}});
            }
        }

        return Document{arr};
    }

    Document ProcessRequests(istream &input, transport_catalogue::TransportCatalogue &catalogue) {
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

        ProcessBaseStopRequests(base_requests_stops, catalogue);

        ProcessBaseBusRequests(base_requests_buses, catalogue);

        return ProcessStateRequests(stat_requests, catalogue);
    }
}
