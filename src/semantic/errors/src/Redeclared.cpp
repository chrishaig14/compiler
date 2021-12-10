//
// Created by chris on 12/8/21.
//

#include "../include/Redeclared.h"

using namespace error;

Error* Redeclared::clone() const {
    return new Redeclared(this->node);
}

bool Redeclared::equal(const Error& other) const {
    bool node_eq = this->node == static_cast<const Redeclared&>(other).node;
    return node_eq;
}

std::string Redeclared::to_str(const CodeLines& code) const {
    return "error: redeclared " + this->node.identifier.str;
}

Redeclared::Redeclared(const ast::Declaration& node) : Error(ErrorType::redeclared), node(node) {

}
