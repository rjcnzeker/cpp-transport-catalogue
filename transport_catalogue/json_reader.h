#pragma once

#include <istream>

#include "json.h"
#include "geo.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "transport_router.h"


namespace json {

    class JsonReader {

    public:

        explicit JsonReader(RequestHandler& request_handler, renderer::MapRenderer& mr,
                            transport_catalogue::TransportRouter& router)
                : rh_(request_handler), mr_(mr), tr_(router) {
        }

        Document ProcessRequests(std::istream& input);

    private:
        RequestHandler& rh_;

        renderer::MapRenderer& mr_;

        transport_catalogue::TransportRouter& tr_;

        void ProcessBaseBusRequests(const std::vector<json::Node>& buses_requests);

        void ProcessBaseStopRequests(const std::vector<json::Node>& stops_requests);

        Document ProcessStateRequests(const std::vector<Node>& state_stops_requests);

        void ProcessRenderRequests(Node& render_requests);

        static svg::Color InputColor(Node& color_node);

        void ProcessRouterSittings(Node& node);
    };

} // namespace json