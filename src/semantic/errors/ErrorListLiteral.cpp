//
// Created by chris on 12/8/21.
//

#include "ErrorListLiteral.h"

Error* ErrorListLiteral::clone() const {
    return nullptr;
}

bool ErrorListLiteral::equal(const Error& other) const {
    return false;
}

std::string ErrorListLiteral::to_str() const {
    return std::string();
}

ErrorListLiteral::ErrorListLiteral(const TypeNode& node, const ObjectType& type, TextPosition position,
                                   const Node& node_1) {

}
