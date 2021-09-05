//
// Created by chris on 1/8/20.
//

#include <cassert>
#include "Function.h"

using namespace ast;

bool Function::equal(const ast::Node& x) const {
    const auto& other = (Function&) x;
//    return false;
    if (this->identifier != other.identifier) {
        return false;
    }
    if (this->parameter_types.size() != other.parameter_types.size()) {
        return false;
    }
    if (this->parameter_names.size() != other.parameter_names.size()) {
        return false;
    }
    for (size_t i = 0; i < this->parameter_types.size(); i++) {
        if (this->parameter_types[i].get() != other.parameter_types[i].get()) {
            return false;
        }
    }
    for (size_t i = 0; i < this->parameter_names.size(); i++) {
        if (this->parameter_names[i] != other.parameter_names[i]) {
            return false;
        }
    }
    return *this->body == *other.body;
}

Function::Function(std::string identifier, const VectorOfStrings& parameter_names, ast::VectorOfUTypes& parameter_types,
                   ast::UTypeNode& return_type, std::unique_ptr<ast::Block>& body, TextPosition start, TextPosition end)
        : ast::Node(NodeType::FUNC, start, end), _parameter_types(std::move(parameter_types)), body(std::move(body)),
          return_type(std::move(return_type)) {
    for (auto& t: this->_parameter_types) {
        this->parameter_types.emplace_back(*t);
    }
    // for (auto p: parameter_types) {
    //     assert(p != nullptr);
    // }
    this->implicit = nullptr;
    // assert(return_type != nullptr);
    // assert(body != nullptr);
    this->identifier = identifier;
    this->parameter_names = parameter_names;
    // this->parameter_types = parameter_types;
}

Function::~Function() {
    // for (auto* pt: this->parameter_types) {
    //     delete pt;
    // }
    // delete this->body;
    // delete this->return_type;
}

nlohmann::json Function::to_json() const {
    std::vector<nlohmann::json> params;
    for (size_t i = 0; i < this->parameter_names.size(); i++) {
        params.push_back({{"id",   this->parameter_names[i]},
                          {"type", this->parameter_types[i].get().to_json()}});
    }
    return {{"type",     "function"},
            {"function", {{"id", this->identifier}, {"parameters", params}, {"body", this->body->to_json()}, {"return_type", this->return_type->to_json()}}}};
}

