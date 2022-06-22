#include <iostream>

#include "request_handler.h"

using namespace std;

using namespace transport_catalogue;

int main() {
    std::istream &input = std::cin;
    int number_requests;
    input >> number_requests;
    string hh;
    getline(input, hh);

    cin_input::InputReader input_reader(number_requests, input);
    TransportCatalogue transport_catalogue = input_reader.ProcessRequests();

    int number_queries;
    cin >> number_queries;
    getline(cin, hh);
    cin_output::ReadQuery(number_queries, transport_catalogue, input);
}