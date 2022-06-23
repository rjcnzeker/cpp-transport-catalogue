#include <iostream>
#include <memory>

#include "request_handler.h"
#include "json.h"

using namespace std;

using namespace transport_catalogue;

int main() {
    auto catalogue = make_unique<TransportCatalogue>();
    json::RequestHandler request_handler(*catalogue);

    istream &input = cin;
    json::Document out  = request_handler.JSONProcessRequests(input);

    Print(out,cout);
   // Print(hhh,cout);
}