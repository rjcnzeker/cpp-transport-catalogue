#include <iostream>
#include <memory>

#include "json_reader.h"
#include "transport_catalogue.h"
#include "json.h"

using namespace std;

using namespace transport_catalogue;

int main() {
    auto catalogue = make_unique<TransportCatalogue>();
    auto map_renderer = make_unique<renderer::MapRenderer>();
    auto request_handler = make_unique<RequestHandler>(*catalogue, *map_renderer);

    auto json_reader = json::JsonReader(*request_handler, *map_renderer);

    istream &input = cin;
    json::Document out = json_reader.ProcessRequests(input);

    Print(out,cout);

    return 0;
}