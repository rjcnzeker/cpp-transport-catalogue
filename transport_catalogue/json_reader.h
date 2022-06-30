#pragma once

#include <istream>

#include "json.h"
#include "geo.h"
#include "transport_catalogue.h"
#include "request_handler.h"


namespace json {
    class JsonReader {

    public:

        explicit JsonReader(RequestHandler &request_handler) : rh_(request_handler) {
        }

        Document ProcessRequests(std::istream &input);

    private:
        void ProcessBaseBusRequests(const std::vector<json::Node> &buses_requests);

        void ProcessBaseStopRequests(const std::vector<json::Node> &stops_requests);

        Document ProcessStateRequests(const std::vector<Node> &state_stops_requests);

        RequestHandler &rh_;
    };


}