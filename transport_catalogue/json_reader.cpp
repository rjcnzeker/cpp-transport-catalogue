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

        Node render_requests;

        vector<Node> map_requests;

        for (const auto &node : document.GetRoot().AsDict()) {
            if (node.first == "base_requests"s) {
                for (const auto &request : node.second.AsArray()) {
                    if (request.AsDict().at("type"s) == "Bus"s) {
                        base_requests_buses.push_back(request);
                    } else if (request.AsDict().at("type"s) == "Stop"s) {
                        base_requests_stops.push_back(request);
                    }
                }
                continue;
            } else if (node.first == "stat_requests"s) {
                for (const auto &request : node.second.AsArray()) {
                    stat_requests.push_back(request);
                }
            } else if (node.first == "render_settings") {
                render_requests = node.second;
            } else if (node.first == "render_settings") {
                map_requests.push_back(node.second);
            }
        }

        ProcessRenderRequests(render_requests);

        ProcessBaseStopRequests(base_requests_stops);

        ProcessBaseBusRequests(base_requests_buses);

        return ProcessStateRequests(stat_requests);
    }

    void JsonReader::ProcessBaseBusRequests(const vector <Node> &buses_requests) {
        string name;

        bool there_and_back;
        for (const auto &bus : buses_requests) {
            auto bus_date = bus.AsDict();
            name = bus_date.at("name"s).AsString();
            there_and_back = !bus_date.at("is_roundtrip"s).AsBool();

            deque<string> stops;
            for (const auto &stop : bus_date.at("stops"s).AsArray()) {
                stops.emplace_back(stop.AsString());
            }

            rh_.AddBus(name, stops, there_and_back);
        }
    }

    void JsonReader::ProcessBaseStopRequests(const vector <Node> &stops_requests) {
        string name;
        geo::Coordinates coordinates(0.0, 0.0);

        for (const auto &stop : stops_requests) {
            auto stop_date = stop.AsDict();
            name = stop_date.at("name"s).AsString();
            coordinates.lng = stop_date.at("longitude"s).AsDouble();
            coordinates.lat = stop_date.at("latitude"s).AsDouble();
            map<string, int> distances{};
            for (auto &stop_distance : stop_date.at("road_distances"s).AsDict()) {
                distances[stop_distance.first] = stop_distance.second.AsInt();
            }

            rh_.AddStop(name, coordinates, distances);
        }
    }

    Document JsonReader::ProcessStateRequests(const vector <Node> &state_stops_requests) {
        Array arr;
        //TODO

        for (const auto &request : state_stops_requests) {
            auto request_data = request.AsDict();
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
                continue;
            }

            if (request_data.at("type"s) == "Map"s) {
                svg::Document doc;
                rh_.RenderMap(doc);

                ostringstream string_stream;
                doc.Render(string_stream);

                arr.push_back(Dict{
                        {"request_id"s,        request_data.at("id"s).AsInt()},
                        {"map"s,         string_stream.str()}});

            }
        }

        return Document{arr};
    }

    void JsonReader::ProcessRenderRequests(Node &render_requests) {
        Dict render_data = render_requests.AsDict();

        mr_.width_ = render_data.at("width").AsDouble();
        mr_.height_ = render_data.at("height").AsDouble();
        mr_.padding_ = render_data.at("padding").AsDouble();
        mr_.line_width_ = render_data.at("line_width").AsDouble();
        mr_.stop_radius_ = render_data.at("stop_radius").AsDouble();
        mr_.bus_label_font_size_ = render_data.at("bus_label_font_size").AsInt();

        svg::Point bus_label_offset;
        mr_.bus_label_offset_.x = render_data.at("bus_label_offset").AsArray()[0].AsDouble();
        mr_.bus_label_offset_.y = render_data.at("bus_label_offset").AsArray()[1].AsDouble();

        mr_.stop_label_font_size_ = render_data.at("stop_label_font_size").AsInt();

        svg::Point stop_label_offset;
        mr_.stop_label_offset_.x = render_data.at("stop_label_offset").AsArray()[0].AsDouble();
        mr_.stop_label_offset_.y = render_data.at("stop_label_offset").AsArray()[1].AsDouble();

        mr_.underlayer_color_ = InputColor(render_data.at("underlayer_color"));

        mr_.underlayer_width_ = render_data.at("underlayer_width").AsDouble();

        std::vector<svg::Color> color_palette;
        for (auto color_node : render_data.at("color_palette").AsArray()) {
            mr_.color_palette_.push_back(InputColor(color_node));
        }

    }

    svg::Color JsonReader::InputColor(Node &color_node) {
        if (color_node.IsArray()) {
            auto color_array = color_node.AsArray();
            if (color_array.size() == 3) {
                svg::Rgb rgb;
                rgb.red = color_array[0].AsInt();
                rgb.green = color_array[1].AsInt();
                rgb.blue = color_array[2].AsInt();
                return rgb;
            } else {
                svg::Rgba rgba;
                rgba.red = color_array[0].AsInt();
                rgba.green = color_array[1].AsInt();
                rgba.blue = color_array[2].AsInt();
                rgba.opacity = color_array[3].AsDouble();
                return rgba;
            }
        } else if (color_node.IsString()) {
            return color_node.AsString();
        }
        return "none";
    }

}
