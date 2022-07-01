#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
using namespace std;

namespace renderer {

    svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
        return {
                (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

    template<typename PointInputIt>
    SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width,
                                     double max_height, double padding)
            : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    void MapRenderer::Render(svg::Document &doc, std::set<Bus, BusComparator> &buses) const {

        vector<vector<geo::Coordinates>> geo_coords_buses(buses.size());
        vector<geo::Coordinates> geo_coords;

        //Распределяем координаты по автобусам, и делаем один контейнер со всеми
        int count = 0;
        for (const Bus &bus : buses) {
            for (auto &stop : bus.bus_stops_) {
                geo_coords_buses[count].push_back(stop->coordinates_);
                geo_coords.push_back(stop->coordinates_);
            }
            ++count;
        }

        //По массиву всех координат вычисляем параметры проецирования
        const renderer::SphereProjector proj{
                geo_coords.begin(), geo_coords.end(), width_, height_,
                padding_
        };

        vector<vector<svg::Point>> buses_screen_coords(geo_coords_buses.size());

        //Координаты по автобусам преобразуем в точки на экране
        count = 0;
        for (const auto &bus_stops_coords : geo_coords_buses) {
            for (const auto geo_coord : bus_stops_coords) {
                const svg::Point screen_coord = proj(geo_coord);
                buses_screen_coords[count].push_back(screen_coord);
            }
            ++count;
        }

        //Выводим линии
        PrintLines(doc, buses, buses_screen_coords);
/*
        //Вывод кружочков остановок
        for (const auto &buss_points : buses_screen_coords) {
            for (const auto &point : buss_points) {
                svg::Circle stop_circle(point, stop_radius_);
                stop_circle.SetFillColor("white");
                doc.Add(stop_circle);
            }
        }
*/

    }

    //Вывод линий
    void MapRenderer::PrintLines(svg::Document &doc, const std::set<Bus, BusComparator> &buses,
                                 const std::vector<std::vector<svg::Point>> &buses_screen_coords) const {
        int count = 0;
        int color_count = 0;
        for (const auto &bus : buses) {
            if (bus.bus_stops_.empty()) {
                continue;
            }

            if (color_count == static_cast<int>(color_palette_.size())) {
                color_count = 0;
            }
            svg::Polyline line{};
            line.SetStrokeColor(color_palette_[color_count]).
                    SetFillColor("none").
                    SetStrokeWidth(line_width_).
                    SetStrokeLineCap(svg::StrokeLineCap::ROUND).
                    SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            if (!bus.there_and_back_) {
                for (const auto &point : buses_screen_coords[count]) {
                    line.AddPoint(point);
                }
            } else {
                for (const auto &point : buses_screen_coords[count]) {
                    line.AddPoint(point);
                }
                for (auto i = buses_screen_coords[count].rbegin() + 1;
                     i < buses_screen_coords[count].rend(); ++i) {
                    line.AddPoint(*i);
                }
            }
            doc.Add(line);
            ++count;
            ++color_count;
        }
    }

} //namespace renderer