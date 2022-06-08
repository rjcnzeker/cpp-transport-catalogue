#include "input_reader.h"

#include <iostream>

using namespace std;

int main() {
    int number_requests;
    cin >> number_requests;
    string hh;
    getline(cin, hh);
   InputReader input_reader(number_requests);
   TransportCatalogue transport_catalogue = input_reader.RequestProcessing();


    cin >> number_requests;
}