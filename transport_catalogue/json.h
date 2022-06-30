#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {
    class Node;

    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
    public:

        using variant::variant;
        using Value = variant;

        const Value &GetValue() const;

        bool IsNull() const;

        bool IsArray() const;

        bool IsMap() const;

        bool IsBool() const;

        bool IsInt() const;

        bool IsDouble() const;

        bool IsPureDouble() const;

        bool IsString() const;

        const Array &AsArray() const;

        const Dict &AsMap() const;

        bool AsBool() const;

        int AsInt() const;

        double AsDouble() const;

        const std::string &AsString() const;

        bool operator==(const Node &rhs) const;

        bool operator!=(const Node &rhs) const;

    };



    class Document {
    public:
        explicit Document(Node root)
                : root_(std::move(root)) {}

        const Node &GetRoot() const {
            return root_;
        }

    private:

        Node root_;
    };

    inline bool operator==(const Document &lhs, const Document &rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }

    inline bool operator!=(const Document &lhs, const Document &rhs) {
        return !(lhs == rhs);
    }

    Document Load(std::istream &input);

    void Print(const Document &doc, std::ostream &output);

} // namespace json