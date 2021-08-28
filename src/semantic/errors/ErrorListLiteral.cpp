//
// Created by chris on 12/8/21.
//

#include "ErrorListLiteral.h"

Error* ErrorListLiteral::clone() const {
    return new ErrorListLiteral(this->node, this->type, this->position, this->node_1);
}

bool ErrorListLiteral::equal(const Error& other) const {
    const ErrorListLiteral& o = (const ErrorListLiteral&) other;
    bool node_ok = this->node == o.node;
    bool type_ok = this->type == o.type;
    bool node_1_ok = this->node_1 == this->node_1;
    return node_ok and type_ok and node_1_ok;
}

std::string ErrorListLiteral::to_str() const {
    return std::string();
}

ErrorListLiteral::ErrorListLiteral(const TypeNode& node, const ObjectType& type, TextPosition position,
                                   const ast::Node& node_1)
        : node(node), type(type), position(position), node_1(node_1) {

}
