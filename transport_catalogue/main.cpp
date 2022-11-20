#include <iostream>
#include <string_view>
#include <fstream>

#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_router.h"
#include "serialization.h"
//#include "tests.h"

using namespace std;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }
    const std::string_view mode(argv[1]);
    if (mode == "make_base"sv) {
        TransportCatalogue::TransportCatalogue db;
        TransportRouter::TransportRouter trnsprt_routr;
        renderer::MapRenderer rend;
        RequestHandler req_hand(db, trnsprt_routr, rend);
        JsonReader::JsonReader j_r(db, trnsprt_routr, req_hand, rend);
        j_r.ParseJsonMakeBase(cin);
    } else if (mode == "process_requests"sv) {
        TransportCatalogue::TransportCatalogue db;
        TransportRouter::TransportRouter trnsprt_routr;
        renderer::MapRenderer rend;
        RequestHandler req_hand(db, trnsprt_routr, rend);
        JsonReader::JsonReader j_r(db, trnsprt_routr, req_hand, rend);
        j_r.ParseJsonProcessRequests(cin);
    } else {
        PrintUsage();
        return 1;
    }
}


