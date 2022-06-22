#include "svg.h"

namespace svg {
    using namespace std::literals;
// ---------- ColorPrinter -------------

    void ColorPrinter::operator()(std::monostate) const {
        out << "none"sv;
    }

    void ColorPrinter::operator()(const std::string& color) const {
        out << color;
    }

    void ColorPrinter::operator()(svg::Rgb color) const {
        out << "rgb("sv << (int)color.red << ","sv <<
            (int)color.green << ","sv <<
            (int)color.blue << ")"sv;
    }

    void ColorPrinter::operator()(svg::Rgba color) const {
        out << "rgba("s << (int)color.red << ","sv <<
            (int)color.green << ","sv <<
            (int)color.blue << ","sv <<
            color.opacity << ")"sv;
    }

// --------------- Object ------------------

    void Object::Render(const RenderContext &context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

// ---------- Circle ------------------

    Circle &Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle &Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

// ---------- Polyline ---------------------

    Polyline &Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    // <polyline points="0,100 50,25 50,75 100,0" />
    void Polyline::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        out << "<polyline points=\""sv;
        if (!points_.empty()) {
            out << points_[0].x << ","sv << points_[0].y;
            for (size_t i = 1; i < points_.size(); ++i) {
                out << " "sv << points_[i].x << ","sv << points_[i].y;
            }
        }
        out << "\"";
        RenderAttrs(context.out);
        out << " />"sv;
    }

// ----------- Text ---------------

    Text &Text::SetPosition(Point pos) {
        position_ = pos;
        return *this;
    }

    Text &Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text &Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    Text &Text::SetFontFamily(std::string font_family) {
        font_family_ = std::move(font_family);
        return *this;
    }

    Text &Text::SetFontWeight(std::string font_weight) {
        font_weight_ = std::move(font_weight);
        return *this;
    }

    Text &Text::SetData(const std::string &data) {
        for (char i : data) {
            if (i == '\"') {
                data_ += "&quot;"s;
                continue;
            }
            if (i == '\'') {
                data_ += "&apos;"s;
                continue;
            }
            if (i == '<') {
                data_ += "&lt;"s;
                continue;
            }
            if (i == '>') {
                data_ += "&gt;"s;
                continue;
            }
            if (i == '&') {
                data_ += "&amp;"s;
                continue;
            }
            data_ += i;
        }

        return *this;
    }

    void Text::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        out << "<text"sv;
        out << " x=\""sv << position_.x << "\" y=\""sv << position_.y << "\""sv;
        out << " dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\""sv;
        out << " font-size=\""sv << font_size_ << "\""sv;
        if (!font_family_.empty()) {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }
        if (!font_weight_.empty()) {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        RenderAttrs(context.out);
        out << ">"sv << data_ << "</text>"sv;
    }

// ----------- Document --------------

    void Document::AddPtr(std::unique_ptr<Object> &&obj) {
        objects_.push_back(move(obj));
    }

    void Document::Render(std::ostream &out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;
        for (auto &obj : objects_) {
            obj->Render(out);
        }
        out << "</svg>"sv;
    }
}  // namespace svg