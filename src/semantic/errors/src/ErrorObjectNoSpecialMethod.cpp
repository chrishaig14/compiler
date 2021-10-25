//
// Created by chris on 12/8/21.
//

#include "../include/ErrorObjectNoSpecialMethod.h"
#include <simple_nodes/common/include/TypeObject.h>

using namespace error;

Error* ErrorObjectNoSpecialMethod::clone() const {
    return new ErrorObjectNoSpecialMethod(*this->type, this->method_name, this->node);
}

bool ErrorObjectNoSpecialMethod::equal(const Error& other) const {
    auto& o = (const ErrorObjectNoSpecialMethod&) other;
    bool type_ok = *this->type == *o.type;
    bool method_ok = this->method_name == o.method_name;
    bool node_ok = this->node == o.node;
    return type_ok and method_ok and node_ok;
}

std::string ErrorObjectNoSpecialMethod::to_str(const CodeLines& code) const {
    return "Error: object of type " + this->type->to_string() + " has no special method " + this->method_name;
}

ErrorObjectNoSpecialMethod::ErrorObjectNoSpecialMethod(const sem::Type& type, const std::string& method_name,
                                                       const ast::Subscript& node)
        : node(node), method_name(method_name), type(type.clone()) {

}
