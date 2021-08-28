//
// Created by chris on 1/8/20.
//

#include "Declaration.h"

using namespace ast;

Declaration::Declaration(const std::string& identifier, TypeNode* type, UNode& expression, TextPosition start,
                         TextPosition eq_pos, TextPosition end) : ast::Node(NodeType::DECL, start, end),
                                                                  identifier(identifier), type(type),
                                                                  expression(std::move(expression)) {
    this->eq_pos = eq_pos;
}

bool Declaration::equal(const ast::Node& x) const {
    const auto& other = (Declaration&) x;
    return this->identifier == other.identifier && *this->expression == *other.expression &&
           ((this->type != nullptr && other.type != nullptr && *this->type == *other.type) ||
            (this->type == nullptr && other.type == nullptr));
}

Declaration::~Declaration() {

    // delete this->type;


    // delete this->expression;

}

nlohmann::json Declaration::to_json() const {
    nlohmann::json j;
    j["type"] = "declaration";
    j["declaration"]["identifier"] = this->identifier;
    j["declaration"]["expression"] = this->expression->to_json();
    j["declaration"]["type"] = this->type != nullptr ? this->type->to_json() : nlohmann::json();
    return j;
}


