#include <deque>
#include "json.h"
#include "transport_catalogue.h"
#include "json_reader.h"

using namespace std::string_literals;

namespace json {
    void ProcessBaseBusRequests(const std::vector <json::Node> &buses_requests,
                                transport_catalogue::TransportCatalogue &catalogue) {
        std::string name;

        bool there_and_back;
        for (const auto &bus : buses_requests) {
            auto bus_date = bus.AsMap();
            name = bus_date.at("name"s).AsString();
            there_and_back = !bus_date.at("is_roundtrip"s).AsBool();

            std::deque<std::string> stops;
            for (const auto &stop : bus_date.at("stops"s).AsArray()) {
                stops.emplace_back(stop.AsString());
            }

            catalogue.AddBus(name, stops, there_and_back);
        }
    }

    void ProcessBaseStopRequests(const std::vector <json::Node> &stops_requests,
                                 transport_catalogue::TransportCatalogue &catalogue) {
        std::string name;
        geo::Coordinates coordinates(0.0, 0.0);

        for (const auto &stop : stops_requests) {
            auto stop_date = stop.AsMap();
            name = stop_date.at("name"s).AsString();
            coordinates.lng = stop_date.at("longitude"s).AsDouble();
            coordinates.lat = stop_date.at("latitude"s).AsDouble();
            std::map<std::string, int> distances{};
            for (auto &stop_distance : stop_date.at("road_distances"s).AsMap()) {
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
            if (request_data.at("type"s) == "Stop"s) {
                Stop stop = catalogue.GetStop(request_data.at("name"s).AsString());

                if (stop.name_.empty()) {
                    arr.push_back(Dict{
                            {"error_message"s, "not found"s},
                            {"request_id"s,    request_data.at("id"s).AsInt()}
                    });
                    continue;
                }
                std::set<std::string> buses_on_stop = catalogue.GetBusesOnStop(request_data.at("name"s).AsString());
                Array buses_on_stop_arr = {};
                for (const auto &bus : buses_on_stop) {
                    buses_on_stop_arr.push_back(bus);
                }
                arr.push_back(Dict{
                        {"buses"s,      buses_on_stop_arr},
                        {"request_id"s, request_data.at("id"s).AsInt()}
                });
                continue;
            }

            if (request_data.at("type"s) == "Bus"s) {
                Bus bus = catalogue.GetBus(request_data.at("name"s).AsString());

                if (bus.name_.empty()) {
                    arr.push_back(Dict{
                            {"request_id"s,    request_data.at("id"s).AsInt()},
                            {"error_message"s, "not found"s}});
                    continue;
                }

                int stops = bus.there_and_back_ ? (bus.bus_stops_.size() * 2) - 1 : bus.bus_stops_.size();
                arr.push_back(Dict{
                        {"request_id"s,       request_data.at("id"s).AsInt()},
                        {"curvature"s,        bus.curvature_},
                        {"route_length"s,     bus.distance_},
                        {"stop_count"s,        stops},
                        {"unique_stop_count"s, bus.unique_stops_}});
            }
        }

        return Document{arr};
    }

    Document ProcessRequests(std::istream &input, transport_catalogue::TransportCatalogue &catalogue) {
        json::Document document(json::Load(input));

        std::vector<json::Node> base_requests_buses;
        std::vector<json::Node> base_requests_stops;

        std::vector<json::Node> stat_requests;

        for (const auto &node : document.GetRoot().AsMap()) {
            if (node.first == "base_requests"s) {
                for (const auto &request : node.second.AsArray()) {
                    if (request.AsMap().at("type"s) == "Bus"s) {
                        base_requests_buses.push_back(request);
                    } else if (request.AsMap().at("type"s) == "Stop"s) {
                        base_requests_stops.push_back(request);
                    }
                }
                continue;
            } else if (node.first == "stat_requests"s) {
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
