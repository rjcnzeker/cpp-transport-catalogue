//

#include <memory>
#include <iostream>
#include <exception>
#include "transport_router.h"

//
// Created by rjcnz on 28.07.2022.

void transport_catalogue::TransportRouter::SetWaitTime(double wait_time) {
    bus_wait_time_ = wait_time;
}

void transport_catalogue::TransportRouter::SetVelocity(double velocity) {
    bus_velocity_ = velocity;
}

void transport_catalogue::TransportRouter::SetGraph(const unordered_map<std::string_view, Stop*>& stops) {
    graph_ = make_unique<graph::DirectedWeightedGraph<double>>(stops.size() * 2);
    //Остановки на улице (до ожидания) имеют четный индекс, после - нечетный
    size_t count = 0;
    for (const auto& stop : stops) {
        size_t id_out_stop = count;
        out_stops_names_to_id_.insert({stop.second->name_, id_out_stop});
        ++count;

        size_t id_in_stop = count;
        in_stops_names_to_id_.insert({stop.second->name_, id_in_stop});
        ++count;

        graph::Edge<double> wait_edge{id_out_stop, id_in_stop, bus_wait_time_, graph::TypeEdge::WAIT};
        graph_->AddEdge(wait_edge);
    }
}

void transport_catalogue::TransportRouter::AddBus(
        const unordered_map<std::pair<std::string, std::string>, int, TransportCatalogue::PairStopsHasher>& distances,
        const deque<string>& stops,
        bool there_and_back) {
    if (!graph_) {
        throw logic_error("Graph was not initialized");
    }

    deque<string> stops_copy{stops.begin(), stops.end()};

    //От первого до предпоследнего
    for (int l = 0; l < stops_copy.size() - 1; ++l) {
        double distance_from_l_stop = 0;
        int span_count = 1;

        //От левой остановки до последней
        for (int r = l + 1; r < stops_copy.size(); ++r) {
            if (stops_copy[r] == stops_copy[l]) {
                continue;
            }

            //В метрах
            distance_from_l_stop += distances.at({stops_copy[r - 1], stops_copy[r]});

            //В минутах
            double time_travel = (distance_from_l_stop / bus_velocity_) / 60;

            graph::Edge<double> edge{in_stops_names_to_id_.at(stops_copy[l]),
                                     out_stops_names_to_id_.at(stops_copy[r]),
                                     time_travel,
                                     graph::TypeEdge::BUS, span_count};

            graph_->AddEdge(edge);

            ++span_count;
        }
    }

    if (there_and_back) {
        deque<string> stops_copy_reverse{stops_copy.rbegin(), stops_copy.rend()};

        //От первого до предпоследнего
        for (int l = 0; l < stops_copy_reverse.size() - 1; ++l) {
            double distance_from_l_stop = 0;
            int span_count = 1;

            //От левой остановки до последней
            for (int r = l + 1; r < stops_copy_reverse.size(); ++r) {
                //В метрах
                distance_from_l_stop += distances.at({stops_copy_reverse[r - 1], stops_copy_reverse[r]});

                //В минутах
                double time_travel = (distance_from_l_stop / bus_velocity_) / 60;

                graph::Edge<double> edge{in_stops_names_to_id_.at(stops_copy_reverse[l]),
                                         out_stops_names_to_id_.at(stops_copy_reverse[r]),
                                         time_travel,
                                         graph::TypeEdge::BUS, span_count};

                graph_->AddEdge(edge);

                ++span_count;
            }
        }

    }

}


