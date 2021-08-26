//
// Created by chris on 12/8/21.
//

#include "ErrorRedeclared.h"

Error* ErrorRedeclared::clone() const {
    return new ErrorRedeclared(this->name, this->node);
    return nullptr;
}

bool ErrorRedeclared::equal(const Error& other) const {
    const ErrorRedeclared& o = (const ErrorRedeclared&) other;
    bool name_eq = this->name == o.name;
    bool node_eq = this->node == o.node;
    return name_eq && node_eq;
}

std::string ErrorRedeclared::to_str() const {
    return "error: redeclared " + this->name;
}

ErrorRedeclared::ErrorRedeclared(std::string name, const ast::Declaration& node) : name(name), node(node) {

}
