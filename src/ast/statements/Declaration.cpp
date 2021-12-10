//
// Created by chris on 1/8/20.
//

#include "Declaration.h"

using namespace ast;

Declaration::Declaration(Token identifier, ast::UTypeNode type, ast::UExpNode expression, TextPosition start,
                         TextPosition eq_pos, TextPosition end) : ast::Statement(StatementType::DECL, start, end),
                                                                  _expression(std::move(expression)), expression(*_expression),
                                                                  identifier(identifier),
                                                                  type(std::move(type)) {
    this->eq_pos = eq_pos;
}

bool Declaration::equal(const ast::Statement& x) const {
    const auto& other = (Declaration&) x;
    return this->identifier == other.identifier && this->expression == other.expression &&
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
    // j["declaration"]["identifier"] = this->identifier;
    j["declaration"]["expression"] = this->expression.to_json();
    j["declaration"]["type"] = this->type != nullptr ? this->type->to_json() : nlohmann::json();
    return j;
}


