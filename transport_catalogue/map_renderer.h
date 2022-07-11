#pragma once

#include "geo.h"
#include "svg.h"
#include "domain.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <utility>
#include <vector>
#include <set>

namespace renderer {

    inline const double EPSILON = 1e-6;

    inline bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    class SphereProjector {
    public:
        // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
        template<typename PointInputIt>
        SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                        double max_width, double max_height, double padding);

        // Проецирует широту и долготу в координаты внутри SVG-изображения
        svg::Point operator()(geo::Coordinates coords) const;

    private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };

    class MapRenderer {
    public:

        double width_;
        double height_;
        double padding_;
        double line_width_;
        double stop_radius_;
        int bus_label_font_size_;
        svg::Point bus_label_offset_;
        int stop_label_font_size_;
        svg::Point stop_label_offset_;
        svg::Color underlayer_color_;
        double underlayer_width_;
        std::vector<svg::Color> color_palette_;

        void Render(svg::Document& doc, std::set<const Bus*, BusComparator>& buses) const;

        void PrintLines(svg::Document& doc, const std::set<const Bus*, BusComparator>& buses,
                        const std::vector<std::vector<svg::Point>>& buses_screen_coords) const;

        void PrintBusesNames(svg::Document& doc, std::set<const Bus*, BusComparator>& buses,
                             const std::vector<std::vector<svg::Point>>& buses_screen_coords) const;

        void PrintCircles(svg::Document& doc, const SphereProjector& proj,
                          std::set<Stop*, StopComparator>& all_stops) const;

        void PrintStopsNames(svg::Document& doc, const SphereProjector& proj,
                             std::set<Stop*, StopComparator>& all_stops) const;
    };
}
