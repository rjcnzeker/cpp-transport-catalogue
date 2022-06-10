#pragma once

#include "transport_catalogue.h"

namespace transport_catalogue {

    void ReadQuery(int num_queries, TransportCatalogue &transport_catalogue);

    namespace Print {

        void Bus(const std::string &name, TransportCatalogue &transport_catalogue);

        void Stop(const std::string &name, TransportCatalogue &transport_catalogue);

    }
}