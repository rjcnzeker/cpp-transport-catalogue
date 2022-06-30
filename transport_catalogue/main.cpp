#include <iostream>
#include <memory>

#include "json_reader.h"
#include "transport_catalogue.h"
#include "json.h"

using namespace std;

using namespace transport_catalogue;

int main() {
    auto catalogue = make_unique<TransportCatalogue>();
    auto request_handler = make_unique<RequestHandler>(*catalogue);
    auto json_reader = json::JsonReader(*request_handler);

    istream &input = cin;
    json::Document out = json_reader.ProcessRequests(input);

    Print(out,cout);
}