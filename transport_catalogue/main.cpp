#include <iostream>
#include <memory>

#include "json_reader.h"
#include "transport_catalogue.h"
#include "json.h"

using namespace std;

using namespace transport_catalogue;

int main() {
    auto catalogue = make_unique<TransportCatalogue>();

    istream &input = cin;
    json::Document out = json::ProcessRequests(input, *catalogue);

    Print(out,cout);

}