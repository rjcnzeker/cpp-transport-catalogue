#pragma once

#include "transport_catalogue.h"

namespace transport_catalogue {

    void ReadQuery(int num_queries, TransportCatalogue &transport_catalogue, std::istream &input);

    namespace print {

        void Bus(const std::string &name, TransportCatalogue &transport_catalogue, std::ostream &output);

        void Stop(const std::string &name, TransportCatalogue &transport_catalogue, std::ostream &output);

    }
}