// напишите решение с нуля
// код сохраните в свой git-репозиторий

#include "input_reader.h"

using namespace std;

InputReader::InputReader(int number_requests) : request_queue_() {
    for (int i = 0; i < number_requests; ++i) {
        string line;
        getline(cin, line);
        if (line[0] == 'B') {
            line = line.substr(line.find_first_not_of(' ', 3));
            request_queue_[RequestType::BUS].push_back(move(line));
            continue;
        }
        if (line[0] == 'S') {
            line = line.substr(line.find_first_not_of(' ', 4));
            request_queue_[RequestType::STOP].push_back(move(line));
            continue;
        }
    }
}

TransportCatalogue InputReader::RequestProcessing() {
    TransportCatalogue transport_catalogue;

    for (string_view request : request_queue_[RequestType::STOP]) {
        StopsProcessing(transport_catalogue, request);
    }

    for (string_view request : request_queue_[RequestType::BUS]) {
        BusesProcessing(transport_catalogue, request);
    }

    return transport_catalogue;
}

void InputReader::BusesProcessing(TransportCatalogue &transport_catalogue, string_view &request) {
    string bus_name = string(request.substr(0, request.find_first_of(':')));
    request.remove_prefix(request.find_first_not_of(' ', bus_name.size() + 1));

    bool there_and_back = false;
    char separator = request[min(request.find('-'), request.find('>'))];
    if (separator == '-') {
        there_and_back = true;
    }

    deque<string_view> stops;
    while (!request.empty()) {
        string_view stop = request.substr(0, request.find(separator));
        stop = stop.substr(0, stop.find_last_not_of(' ') + 1);
        stops.emplace_back(stop);


        if (request.find_first_of(' ', stop.size()) > request.size()) {
            break;
        }

        request.remove_prefix(request.find(separator) + 1);
        request.remove_prefix(request.find_first_not_of(' '));
    }

    transport_catalogue.AddBus(bus_name, stops, there_and_back);
}

void InputReader::StopsProcessing(TransportCatalogue &transport_catalogue, string_view &request) {
    string stop_name = string(request.substr(0, request.find_first_of(':')));
    request.remove_prefix(request.find_first_not_of(' ', stop_name.size() + 1));

    Coordinates coordinates(0.0, 0.0);
    string_view latitude = request.substr(0, request.find_first_of(','));
    coordinates.lat = stod(string(latitude));
    request.remove_prefix(latitude.size() + 1);
    string_view longitude = request.substr(1, request.find_first_of(',') - 1);
    coordinates.lng = stod(string(longitude));
    request.remove_prefix(longitude.size() + 2);

    auto request_size = request.size();
    bool have_distances = true;
    if (request.size() == request.npos) {
        have_distances = false;
    }

    map<string_view, int> distances;
    while (have_distances) {
        request.remove_prefix(request.find_first_not_of(' '));
        string distance_to_next_str = string(request.substr(0, request.find_first_of('m')));
        int distance_to_next = stoi(distance_to_next_str);
        request.remove_prefix(distance_to_next_str.size() + 2);

        request.remove_prefix(request.find(' ') + 1);
        string_view name_next = request.substr(0, request.find(','));

        distances.insert({name_next, distance_to_next});

        auto request_next_words_size = request.find_first_of(' ', name_next.size());
        if (request_next_words_size > request.size()) {
            break;
        }

        request.remove_prefix(max(name_next.size(), request.find_first_of(' ', name_next.size())));


        //distances.insert({{stop_name, name_next}, distance_to_next});
    }


    transport_catalogue.AddStop(stop_name, coordinates, distances);
}



