#pragma once

#include <unordered_set>
#include <filesystem>

#include "transport_catalogue.h"
#include "domain.h"
#include "map_renderer.h"
#include "transport_router.h"

class RequestHandler {

public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(TransportCatalogue::TransportCatalogue& trnsprt_ctlg,
                   TransportRouter::TransportRouter& trnsprt_routr_,
                   renderer::MapRenderer& map_rendr_);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<domain::BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает информацию о маршруте (запрос Route)
    std::optional<domain::RoutStat> GetRouteStat(std::string_view stop_from, std::string_view stop_to) const;

    // Возвращает маршруты, проходящие через
    std::optional<const std::unordered_set<const domain::Bus*>*>
    GetBusesByStop(const std::string_view& stop_name) const;

    std::vector<const domain::Bus*> GetBusesLex() const;

    // Возвращает перечень уникальных остановок в лекс порядке через которые проходят маршруты
    const std::vector<const domain::Stop*> GetUnicLexStopsIncludeBuses() const;

    svg::Document RenderMap() const;

    void CallDsrlz(const std::filesystem::path& path);

    void CallSrlz(const std::filesystem::path& path) const;

private:
    domain::BusStat CreateBusStat(const domain::Bus* bus) const;

    TransportCatalogue::TransportCatalogue& trnsprt_ctlg_;

    TransportRouter::TransportRouter& trnsprt_routr_;

    renderer::MapRenderer& map_rendr_;
};
