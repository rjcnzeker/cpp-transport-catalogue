#include "json_builder.h"

namespace json {
    //------------------Builder----------------------------

    Builder::Builder() {
        node_stacks_.push_back(&root_);
    }

    DictItemContext Builder::StartDict() {
        if (node_stacks_.back()->IsArray()) {
            const_cast<Array&>(node_stacks_.back()->AsArray()).push_back(Dict());
            Node* node = &const_cast<Array&>(node_stacks_.back()->AsArray()).back();
            node_stacks_.push_back(node);
        } else {
            *node_stacks_.back() = Dict();
        }
        return std::move(*this);
    }

    ArrayItemContext Builder::StartArray() {
        if (node_stacks_.back()->IsArray()) {
            const_cast<Array&>(node_stacks_.back()->AsArray()).push_back(Array());
            Node* node = &const_cast<Array&>(node_stacks_.back()->AsArray()).back();
            node_stacks_.push_back(node);
        } else {
            *node_stacks_.back() = Array();
        }
        return std::move(*this);
    }

    Builder& Builder::EndDict() {
        node_stacks_.erase(node_stacks_.end() - 1);
        return *this;
    }

    Builder& Builder::EndArray() {
        node_stacks_.erase(node_stacks_.end() - 1);
        return *this;
    }

    KeyContext Builder::Key(std::string key) {
        node_stacks_.emplace_back(&const_cast<Dict&>(node_stacks_.back()->AsDict())[key]);
        return std::move(*this);
    }

    Builder& Builder::Value(Node value) {
        if (node_stacks_.back()->IsArray()) {
            const_cast<Array&>(node_stacks_.back()->AsArray()).push_back(value);
        } else {
            *node_stacks_.back() = value;
            node_stacks_.erase(node_stacks_.end() - 1);
        }
        return *this;
    }

    Node Builder::Build() {
        return std::move(root_);
    }

    //----------------KeyContext----------------

    KeyContext::KeyContext(Builder&& builder)
            : builder_(builder) {}

    DictItemContext KeyContext::Value(Node value) {
        return std::move(builder_.Value(std::move(value)));
    }

    ArrayItemContext KeyContext::StartArray() {
        return std::move(builder_.StartArray());
    }

    DictItemContext KeyContext::StartDict() {
        return std::move(builder_.StartDict());
    }

    //------------------DictItemContext------------------

    DictItemContext::DictItemContext(Builder&& builder)
            : builder_(builder) {}

    KeyContext DictItemContext::Key(std::string key) {
        return std::move(builder_.Key(std::move(key)));
    }

    Builder& DictItemContext::EndDict() {
        return builder_.EndDict();
    }

    //-----------------ArrayItemContext------------------------

    ArrayItemContext::ArrayItemContext(Builder&& builder)
            : builder_(builder) {}

    ArrayItemContext& ArrayItemContext::Value(Node value) {
        builder_.Value(std::move(value));
        return *this;
    }

    DictItemContext ArrayItemContext::StartDict() {
        return std::move(builder_.StartDict());
    }

    ArrayItemContext ArrayItemContext::StartArray() {
        return std::move(builder_.StartArray());
    }

    Builder& ArrayItemContext::EndArray() {
        return builder_.EndArray();
    }

} // namespace json