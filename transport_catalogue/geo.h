#pragma once

#include <cmath>

namespace geo {

    const int EARTH_RADIUS = 6371000;

    struct Coordinates {

        double lat;
        double lng;

        Coordinates(double lat, double lng) : lat(lat), lng(lng) {
        }

        Coordinates() = default;

        bool operator==(const Coordinates &other) const;

        bool operator!=(const Coordinates &other) const;
    };

    double ComputeDistance(Coordinates from, Coordinates to);
}
