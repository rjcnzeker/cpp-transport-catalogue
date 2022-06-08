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

    cout << setprecision(8);

    for (string_view request : request_queue_[RequestType::STOP]) {
        string stop_name = string(request.substr(0, request.find_first_of(':')));
        request.remove_prefix(request.find_first_not_of(' ', stop_name.size() + 1));

        Coordinates coordinates;
        string_view latitude = request.substr(0, request.find_first_of(','));
        coordinates.lat = stod(string(latitude));

        request.remove_prefix(latitude.size() + 1);

        string_view longitude = request.substr(request.find_first_not_of(' '));
        coordinates.lng = stod(string(longitude));

        transport_catalogue.AddStop(stop_name, coordinates);

        cout << coordinates.lat << " " << coordinates.lng << endl;
    }

    for (string_view request : request_queue_[RequestType::BUS]) {
        string bus_name = string(request.substr(0, request.find_first_of(':')));
        request.remove_prefix(request.find_first_not_of(' ', bus_name.size() + 1));

    }

    return transport_catalogue;
}



