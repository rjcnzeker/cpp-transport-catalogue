#include "request_handler.h"
#include "serialization.h"

//----------------------------------------------------------------------------
RequestHandler::RequestHandler(TransportCatalogue::TransportCatalogue& trnsprt_ctlg,
                               TransportRouter::TransportRouter& trnsprt_routr_,
                               renderer::MapRenderer& map_rendr_)
        : trnsprt_ctlg_(trnsprt_ctlg), trnsprt_routr_(trnsprt_routr_), map_rendr_(map_rendr_) {

}

//----------------------------------------------------------------------------
std::optional<domain::BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
    if (auto opt_bus = trnsprt_ctlg_.FindBus(bus_name); opt_bus) {
        return CreateBusStat(opt_bus.value());
    } else {
        return std::nullopt;
    }
}

//----------------------------------------------------------------------------
std::optional<domain::RoutStat>
RequestHandler::GetRouteStat(std::string_view stop_from, std::string_view stop_to) const {
    if (trnsprt_routr_.GetGraphIsNoInit()) {
        std::cerr << "CreateGraph" << std::endl;
        trnsprt_routr_.CreateGraph(trnsprt_ctlg_);
    }
    return trnsprt_routr_.GetRouteStat(trnsprt_ctlg_.FindStop(stop_from).value()->id,
                                       trnsprt_ctlg_.FindStop(stop_to).value()->id);
}

//----------------------------------------------------------------------------
std::optional<const std::unordered_set<const domain::Bus*>*>
RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
    if (auto opt_stop = trnsprt_ctlg_.FindStop(stop_name); opt_stop) {
        if (trnsprt_ctlg_.GetBusesFromStop().count(stop_name) != 0) {
            return &trnsprt_ctlg_.GetBusesFromStop().at(stop_name);
        } else {
            static std::unordered_set<const domain::Bus*> emp{};
            return &emp;
        }
    } else {
        // если остановок с таким именем нет
        return std::nullopt;
    }

}

//----------------------------------------------------------------------------
std::vector<const domain::Bus*> RequestHandler::GetBusesLex() const {
    return trnsprt_ctlg_.GetBusesLex();
}

//----------------------------------------------------------------------------
const std::vector<const domain::Stop*> RequestHandler::GetUnicLexStopsIncludeBuses() const {
    // использую сет как фильтр уникальных и сортировщик, кладу в вектор так как потом надо будет только итерироваться
    std::set<const domain::Stop*, domain::CmpStops> set;
    for (const auto& bus : trnsprt_ctlg_.GetBuses()) {
        for (const auto& stop : bus.stops) {
            set.insert(stop);
        }
    }
    return {set.begin(), set.end()};
}

//----------------------------------------------------------------------------
svg::Document RequestHandler::RenderMap() const {
    return map_rendr_.GetDocMapBus();
}

//----------------------------------------------------------------------------
void RequestHandler::CallDsrlz(const std::filesystem::path& path) {
    Serialization d_srlz;
    d_srlz.Deserialize(path, trnsprt_ctlg_, map_rendr_, trnsprt_routr_);
    map_rendr_.SetBuses(GetBusesLex());
    map_rendr_.SetUnicStops(GetUnicLexStopsIncludeBuses());
}

//----------------------------------------------------------------------------
void RequestHandler::CallSrlz(const std::filesystem::path& path) const {
    Serialization srlz;
    srlz.Serialize(trnsprt_ctlg_, map_rendr_, trnsprt_routr_, path);
}

//----------------------------------------------------------------------------
domain::BusStat RequestHandler::CreateBusStat(const domain::Bus* bus) const {
    return trnsprt_ctlg_.GetBusStat(bus);
}
//----------------------------------------------------------------------------
