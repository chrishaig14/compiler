//
// Created by chris on 4/4/21.
//

#include "../include/Declaration.h"
#include "../../expressions/include/Exp.h"

using namespace sem;

Declaration::Declaration(const std::string& identifier, UExp _expression)
        : Common(CommonType::DECLARATION), _expression(std::move(_expression)), identifier(identifier),
          expression(*this->_expression) {
    assert(this->_expression != nullptr);
}

bool Declaration::equals(const Common& o) const {
    assert(this->type == o.type);
    auto& other = (const Declaration&) o;
    bool id_ok = this->identifier == other.identifier;
    bool exp_ok = this->expression == other.expression;
    return id_ok && exp_ok;
}

std::unique_ptr<Common> Declaration::clone() const {
    return std::make_unique<Declaration>(*this);
}
