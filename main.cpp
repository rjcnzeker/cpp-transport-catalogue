#include <iostream>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    int number_requests;
    cin >> number_requests;
    string hh;
    getline(cin, hh);
   InputReader input_reader(number_requests);
   TransportCatalogue transport_catalogue = input_reader.RequestProcessing();

    int number_queries;
    cin >> number_queries;
    getline(cin, hh);
    ReadQuery(number_queries, transport_catalogue);

}