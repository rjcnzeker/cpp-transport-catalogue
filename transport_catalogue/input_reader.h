#pragma once

#include <istream>
#include <sstream>
#include <map>
#include <iostream>
#include <set>
#include <utility>
#include <vector>
#include <iomanip>

#include "transport_catalogue.h"

namespace transport_catalogue {

    class InputReader {
    public:
        explicit InputReader(int number_requests, std::istream &input);

        TransportCatalogue ProcessRequests();

    private:

        enum class RequestType {
            BUS,
            STOP,
        };
        std::map<RequestType, std::vector<std::string>> request_queue_;

        static void ProcessStops(TransportCatalogue &transport_catalogue, std::string_view &request);

        static void ProcessBuses(TransportCatalogue &transport_catalogue, std::string_view &request);
    };
}

