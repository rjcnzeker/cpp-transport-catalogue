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


class InputReader {
public:
    explicit InputReader(int number_requests);

    TransportCatalogue RequestProcessing();

private:
    enum class RequestType {
        BUS,
        STOP,
    };
    std::map<RequestType, std::vector<std::string>> request_queue_;

    static void StopsProcessing(TransportCatalogue &transport_catalogue, std::string_view &request) ;

    static void BusesProcessing(TransportCatalogue &transport_catalogue, std::string_view &request) ;
};