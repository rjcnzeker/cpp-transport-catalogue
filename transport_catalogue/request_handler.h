#pragma once

#pragma once

#include "transport_catalogue.h"
#include "domain.h"

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

        static void ProcessStops(transport_catalogue::TransportCatalogue &transport_catalogue, std::string_view &request);

        static void ProcessBuses(transport_catalogue::TransportCatalogue &transport_catalogue, std::string_view &request);
    };
}

namespace cin_output {

    void ReadQuery(int num_queries, transport_catalogue::TransportCatalogue &transport_catalogue, std::istream &input);

    namespace print {

        void Buss(const std::string &name, transport_catalogue::TransportCatalogue &transport_catalogue, std::ostream &output);

        void Stop( std::string &name, transport_catalogue::TransportCatalogue &transport_catalogue, std::ostream &output);

    }
}



// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
/*
class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

    // Возвращает информацию о маршруте (запрос Buss)
    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};
*/