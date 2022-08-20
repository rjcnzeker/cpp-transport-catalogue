#pragma once

#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"

using namespace std;

namespace transport_catalogue {

    using namespace graph;

    class TransportRouter {
    public:
        TransportRouter() = default;

        void SetWaitTime(double wait_time);

        void SetVelocity(double velocity);

        void SetGraph(const unordered_map<std::string_view, Stop*>& stops);

        void
        AddBus(const unordered_map<std::pair<std::string, std::string>, int, TransportCatalogue::PairStopsHasher>& distances,
               const deque <string>& stops,
               bool there_and_back, const string& name);

        void SetRouter();

        std::optional<graph::Router<double>::RouteInfo> GetRoute(const string& from, const string& to);

        graph::Edge<double> GetEdge(size_t id);

        string GetStopByVertexId(size_t id);

        string GetBusById(size_t id);

    private:
        unique_ptr <DirectedWeightedGraph<double>> graph_;

        unique_ptr <Router<double>> router_;

        map <string, size_t> out_stops_names_to_id_;
        map <string, size_t> in_stops_names_to_id_;

        map <size_t, string> id_out_stops_by_name;

        vector <string> buses_by_id;

        double bus_wait_time_;
        double bus_velocity_;

    };

}