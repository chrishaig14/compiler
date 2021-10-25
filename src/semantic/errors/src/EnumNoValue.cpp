//
// Created by chris on 12/8/21.
//

#include "../include/EnumNoValue.h"

using namespace error;

Error* EnumNoValue::clone() const {
    return new EnumNoValue(this->enum_name, this->value_name, this->node, this->p_enum);
}

bool EnumNoValue::equal(const Error& other) const {
    auto& o = (const EnumNoValue&) other;
    bool enum_ok = this->enum_name == o.enum_name;
    bool value_ok = this->value_name == o.value_name;
    bool node_ok = this->node == o.node;
    return enum_ok and value_ok and node_ok;
}

std::string EnumNoValue::to_str(const CodeLines& code) const {
    return "Error: enum " + this->enum_name + " has no value " + this->value_name;
}

EnumNoValue::EnumNoValue(std::string a, const std::string b, const ast::Member& node, const Enum& p_enum)
        : enum_name(a), value_name(b), node(node), p_enum(p_enum) {

}
