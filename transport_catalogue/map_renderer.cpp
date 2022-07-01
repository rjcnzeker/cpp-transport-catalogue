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

            if (color_count == color_palette_.size()) {
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