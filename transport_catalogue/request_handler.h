#pragma once

#include <optional>
#include <unordered_set>

#include "transport_catalogue.h"
#include "domain.h"
#include "json.h"
#include "map_renderer.h"

namespace cin_input {

    class InputReader {
    public:
        explicit InputReader(int number_requests, std::istream &input);

        transport_catalogue::TransportCatalogue ProcessRequests();

    private:

        enum class RequestType {
            BUS,
            STOP,
        };
        std::map<RequestType, std::vector<std::string>> request_queue_;

        static void
        ProcessStops(transport_catalogue::TransportCatalogue &transport_catalogue, std::string_view &request);

        static void
        ProcessBuses(transport_catalogue::TransportCatalogue &transport_catalogue, std::string_view &request);
    };
}

namespace cin_output {

    void ReadQuery(int num_queries, transport_catalogue::TransportCatalogue &transport_catalogue, std::istream &input);

    namespace print {

        void Buss(const std::string &name, transport_catalogue::TransportCatalogue &transport_catalogue,
                  std::ostream &output);

        void
        Stop(std::string &name, transport_catalogue::TransportCatalogue &transport_catalogue, std::ostream &output);

    }
}

class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(transport_catalogue::TransportCatalogue &db, const renderer::MapRenderer &renderer)
            : db_(db), renderer_(renderer) {}

    void AddBus(std::string name, std::deque<std::string> stops, bool there_and_back);

    void AddStop(std::string name, geo::Coordinates coordinates, const std::map<std::string, int> &distances);

    Bus GetBus(const std::string &name);

    Stop GetStop(const std::string &name);

    std::set<std::string> GetBusesOnStop(const std::string &name);

    std::set<Bus, BusComparator> GetBuses() const;

    // Этот метод будет нужен в следующей части итогового проекта
    void RenderMap(svg::Document &doc) const;

private:

    transport_catalogue::TransportCatalogue &db_;

    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const renderer::MapRenderer &renderer_;
};



