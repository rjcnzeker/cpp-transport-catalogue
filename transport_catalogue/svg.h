#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <optional>
#include <variant>
#include <iomanip>

namespace svg {

    using namespace std::literals;

    struct Point {
        Point() = default;

        Point(double x, double y)
                : x(x), y(y) {
        }

        double x = 0;
        double y = 0;
    };

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента */
    struct RenderContext {
        RenderContext(std::ostream &out)
                : out(out) {
        }

        RenderContext(std::ostream &out, int indent_step, int indent = 0)
                : out(out), indent_step(indent_step), indent(indent) {
        }

        RenderContext Indented() const {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream &out;
        int indent_step = 0;
        int indent = 0;
    };

    struct Rgb {
        Rgb() = default;

        Rgb(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue) {}

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };

    struct Rgba : public Rgb {
        Rgba() = default;

        Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity)
                : Rgb(red, green, blue), opacity(opacity) {}

        double opacity = 1.0;
    };

    using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

    inline const Color NoneColor{};

    struct ColorPrinter {
        std::ostream &out;

        void operator()(std::monostate) const;

        void operator()(const std::string& color) const;

        void operator()(svg::Rgb color) const;

        void operator()(svg::Rgba color) const;
    };

    inline std::ostream &operator<<(std::ostream &out, svg::Color color) {
        std::visit(ColorPrinter{out}, color);
        return out;
    }

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    inline std::ostream &operator<<(std::ostream &out, svg::StrokeLineCap value) {
        switch (value) {
            case StrokeLineCap::BUTT :
                return out << "butt"sv;
            case StrokeLineCap::ROUND :
                return out << "round"sv;
            case StrokeLineCap::SQUARE :
                return out << "square"sv;
            default:
                return out << ""sv;
        }
    }

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    inline std::ostream &operator<<(std::ostream &out, StrokeLineJoin value) {
        switch (value) {
            case StrokeLineJoin::ARCS:
                return out << "arcs"sv;
            case StrokeLineJoin::BEVEL:
                return out << "bevel"sv;
            case StrokeLineJoin::MITER:
                return out << "miter"sv;
            case StrokeLineJoin::MITER_CLIP:
                return out << "miter-clip"sv;
            case StrokeLineJoin::ROUND:
                return out << "round"sv;
            default:
                return out;
        }
    }

    template<typename Owner>
    class PathProps {
    public:
        Owner &SetFillColor(Color fill_color);

        Owner &SetStrokeColor(Color stroke_color);

        Owner &SetStrokeWidth(double width);

        Owner &SetStrokeLineCap(StrokeLineCap line_cap);

        Owner &SetStrokeLineJoin(StrokeLineJoin line_join);

        virtual ~PathProps() = default;

    protected:

        void RenderAttrs(std::ostream &out) const;

    private:

        Owner &AsOwner() {
            return static_cast<Owner &>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> stroke_width_;
        std::optional<StrokeLineCap> stroke_line_cap_;
        std::optional<StrokeLineJoin> stroke_line_join_;
    };

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
    class Object {
    public:
        void Render(const RenderContext &context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext &context) const = 0;
    };

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle() = default;

        Circle(const Point &center, double radius) : center_(center), radius_(radius) {
        }

        Circle &SetCenter(Point center);

        Circle &SetRadius(double radius);

    private:
        void RenderObject(const RenderContext &context) const override;

        Point center_;
        double radius_ = 1.0;
    };

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        Polyline() = default;

        // Добавляет очередную вершину к ломаной линии
        Polyline &AddPoint(Point point);

    private:
        void RenderObject(const RenderContext &context) const override;

        std::vector<Point> points_ = {};
    };

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
    class Text final : public Object, public PathProps<Text> {
    public:
        Text() = default;

        // Задаёт координаты опорной точки (атрибуты x и y)
        Text &SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text &SetOffset(Point offset);

        // Задаёт размеры шрифта (атрибут font-size)
        Text &SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text &SetFontFamily(std::string font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text &SetFontWeight(std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text &SetData(const std::string &data);

    private:

        void RenderObject(const RenderContext &context) const override;

        Point position_;
        Point offset_;
        int font_size_ = 1;
        std::string font_family_;
        std::string font_weight_;
        std::string data_;

    };

    class ObjectContainer;

    class Drawable {
    public:
        virtual void Draw(ObjectContainer &obj_container) const = 0;

        virtual ~Drawable() = default;
    };

    class ObjectContainer {
    public:
        //Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
        template<typename T>
        void Add(T obj);

        virtual void AddPtr(std::unique_ptr<Object> &&obj) = 0;

        ~ObjectContainer() = default;

    protected:
        std::deque<std::unique_ptr<Object>> objects_;
    };

    class Document final : public ObjectContainer {
    public:
        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object> &&obj) override;

        // Выводит в ostream svg-представление документа
        void Render(std::ostream &out) const;
    };

    template<typename T>
    void ObjectContainer::Add(T obj) {
        objects_.emplace_back(std::make_unique<T>(std::move(obj)));
    }

    //------------- PathProps -------------

    template<typename Owner>
    Owner &PathProps<Owner>::SetFillColor(Color fill_color) {
        fill_color_ = std::move(fill_color);
        return AsOwner();
    }

    template<typename Owner>
    Owner &PathProps<Owner>::SetStrokeColor(Color stroke_color) {
        stroke_color_ = std::move(stroke_color);
        return AsOwner();
    }

    template<typename Owner>
    Owner &PathProps<Owner>::SetStrokeWidth(double width) {
        stroke_width_ = width;
        return AsOwner();
    }

    template<typename Owner>
    Owner &PathProps<Owner>::SetStrokeLineCap(StrokeLineCap line_cap) {
        stroke_line_cap_ = line_cap;
        return AsOwner();
    }

    template<typename Owner>
    Owner &PathProps<Owner>::SetStrokeLineJoin(StrokeLineJoin line_join) {
        stroke_line_join_ = line_join;
        return AsOwner();
    }

    template<typename Owner>
    void PathProps<Owner>::RenderAttrs(std::ostream &out) const {
        if (fill_color_) {
            out << " fill=\""sv << *fill_color_ << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv << *stroke_color_ << "\""sv;
        }
        if (stroke_width_) {
            out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
        }
        if (stroke_line_cap_) {
            out << " stroke-linecap=\""sv << *stroke_line_cap_ << "\""sv;
        }
        if (stroke_line_join_) {
            out << " stroke-linejoin=\""sv << *stroke_line_join_ << "\""sv;
        }
    }

}  // namespace svg