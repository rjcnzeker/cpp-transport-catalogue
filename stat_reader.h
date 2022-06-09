#pragma once

#include "transport_catalogue.h"

void ReadQuery(int num_queries, TransportCatalogue &transport_catalogue);

void PrintBus(const std::string& name, TransportCatalogue & transport_catalogue);