//
// Created by chris on 12/8/21.
//

#include "../include/ErrorEnumNoValue.h"

Error* ErrorEnumNoValue::clone() const {
    return new ErrorEnumNoValue(this->enum_name, this->value_name, this->node, this->p_enum);
}

bool ErrorEnumNoValue::equal(const Error& other) const {
    auto& o = (const ErrorEnumNoValue&) other;
    bool enum_ok = this->enum_name == o.enum_name;
    bool value_ok = this->value_name == o.value_name;
    bool node_ok = this->node == o.node;
    return enum_ok and value_ok and node_ok;
}

std::string ErrorEnumNoValue::to_str(const CodeLines& code) const {
    return "Error: enum " + this->enum_name + " has no value " + this->value_name;
}

ErrorEnumNoValue::ErrorEnumNoValue(std::string a, const std::string b, const ast::Member& node,
                                   const Enum& p_enum)
        : enum_name(a), value_name(b), node(node), p_enum(p_enum) {

}
