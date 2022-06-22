#include "domain.h"

size_t
PairStopsHasher::operator()(const std::pair <std::string_view, std::string_view> &stops_pair) const {
    std::string left_plus_right(stops_pair.first.substr(0));
    left_plus_right += stops_pair.second.substr(0);

    return d_hasher_(left_plus_right);
}