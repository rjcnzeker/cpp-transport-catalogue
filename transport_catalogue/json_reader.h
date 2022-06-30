#pragma once

#include <istream>

#include "json.h"
#include "geo.h"
#include "transport_catalogue.h"


namespace json {
    Document ProcessRequests(std::istream &input, transport_catalogue::TransportCatalogue &catalogue);

    void ProcessBaseBusRequests(const std::vector<json::Node> &buses_requests);

    void ProcessBaseStopRequests(const std::vector<Node> &stops_requests);

//const renderer::MapRenderer& renderer_;
    Document ProcessStateRequests(const std::vector<Node> &state_stops_requests);
}