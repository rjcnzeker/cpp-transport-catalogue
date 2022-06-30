#include <deque>
#include "json.h"
#include "transport_catalogue.h"
#include "json_reader.h"

using namespace std;

namespace json {

    Document JsonReader::ProcessRequests(istream &input) {

        Document document(Load(input));

        vector<Node> base_requests_buses;
        vector<Node> base_requests_stops;

        vector<Node> stat_requests;

        vector<Node> render_requests;


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
            } else if (node.first == "render_settings") {
                for (const auto &request : node.second.AsArray()) {
                    render_requests.push_back(request);
                }
            }
        }

        ProcessRenderRequests(render_requests);

        ProcessBaseStopRequests(base_requests_stops);

        ProcessBaseBusRequests(base_requests_buses);

        return ProcessStateRequests(stat_requests);
    }

    void JsonReader::ProcessBaseBusRequests(const vector<Node> &buses_requests) {
        string name;

        bool there_and_back;
        for (const auto &bus : buses_requests) {
            auto bus_date = bus.AsMap();
            name = bus_date.at("name"s).AsString();
            there_and_back = !bus_date.at("is_roundtrip"s).AsBool();

            deque<string> stops;
            for (const auto &stop : bus_date.at("stops"s).AsArray()) {
                stops.emplace_back(stop.AsString());
            }

            rh_.AddBus(name, stops, there_and_back);
        }
    }

    void JsonReader::ProcessBaseStopRequests(const vector<Node> &stops_requests) {
        string name;
        geo::Coordinates coordinates(0.0, 0.0);

        for (const auto &stop : stops_requests) {
            auto stop_date = stop.AsMap();
            name = stop_date.at("name"s).AsString();
            coordinates.lng = stop_date.at("longitude"s).AsDouble();
            coordinates.lat = stop_date.at("latitude"s).AsDouble();
            map<string, int> distances{};
            for (auto &stop_distance : stop_date.at("road_distances"s).AsMap()) {
                distances[stop_distance.first] = stop_distance.second.AsInt();
            }

            rh_.AddStop(name, coordinates, distances);
        }
    }

    Document JsonReader::ProcessStateRequests(const vector<Node> &state_stops_requests) {
        Array arr;
        for (const auto &request : state_stops_requests) {
            auto request_data = request.AsMap();
            if (request_data.at("type"s) == "Stop"s) {
                Stop stop = rh_.GetStop(request_data.at("name"s).AsString());

                if (stop.name_.empty()) {
                    arr.push_back(Dict{
                            {"error_message"s, "not found"s},
                            {"request_id"s,    request_data.at("id"s).AsInt()}
                    });
                    continue;
                }
                set<string> buses_on_stop = rh_.GetBusesOnStop(
                        request_data.at("name"s).AsString());
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
                Bus bus = rh_.GetBus(request_data.at("name"s).AsString());

                if (bus.name_.empty()) {
                    arr.push_back(Dict{
                            {"request_id"s,    request_data.at("id"s).AsInt()},
                            {"error_message"s, "not found"s}});
                    continue;
                }

                int stops = bus.there_and_back_ ? (bus.bus_stops_.size() * 2) - 1 : bus.bus_stops_.size();
                arr.push_back(Dict{
                        {"request_id"s,        request_data.at("id"s).AsInt()},
                        {"curvature"s,         bus.curvature_},
                        {"route_length"s,      bus.distance_},
                        {"stop_count"s,        stops},
                        {"unique_stop_count"s, bus.unique_stops_}});
            }
        }

        return Document{arr};
    }

    void JsonReader::ProcessRenderRequests(std::vector<Node> render_requests) {
        /*{
      "width": 1200.0,
      "height": 1200.0,

      "padding": 50.0,

      "line_width": 14.0,
      "stop_radius": 5.0,

      "bus_label_font_size": 20,
      "bus_label_offset": [7.0, 15.0],

      "stop_label_font_size": 20,
      "stop_label_offset": [7.0, -3.0],

      "underlayer_color": [255, 255, 255, 0.85],
      "underlayer_width": 3.0,

      "color_palette": [
        "green",
        [255, 160, 0],
        "red"
      ]
    } */


    }

}
