#include <iostream>
#include <memory>

#include "json_reader.h"
#include "transport_catalogue.h"
#include "json.h"
#include "transport_router.h"
#include "graph.h"
#include "router.h"

using namespace std;

using namespace transport_catalogue;

int main() {
    auto catalogue = make_unique<TransportCatalogue>();
    auto map_renderer = make_unique<renderer::MapRenderer>();

    auto transport_router = make_unique<TransportRouter>();

    auto request_handler = make_unique<RequestHandler>(*catalogue, *map_renderer, *transport_router);

    auto json_reader = json::JsonReader(*request_handler, *map_renderer, *transport_router);

    istream& input = cin;
    json::Document out = json_reader.ProcessRequests(input);

    Print(out, cout);

    return 0;
}