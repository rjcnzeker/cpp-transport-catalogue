#include <string>
#include <iostream>
#include "stat_reader.h"

using namespace std;

void ReadQuery(int num_queries, TransportCatalogue &transport_catalogue) {
    for (int i = 0; i < num_queries; ++i) {
        string line;
        getline(cin, line);

        if (line[0] == 'B') {
            PrintBus(line, transport_catalogue);
        }
    }
}

void PrintBus(const string& name, TransportCatalogue & transport_catalogue) {
    cout << name << ": "s;
    TransportCatalogue::Bus bus = transport_catalogue.GetBus(name);

    cout << '\n';
}