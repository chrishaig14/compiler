//
// Created by chris on 8/8/21.
//

#include "ErrorTypeMismatch.h"
#include "../../json/json.hpp"

ErrorTypeMismatch::ErrorTypeMismatch(const ast::TypeNode& expected, const ast::Node& value_node, const Entity& actual)
        : expected(expected), value_node(value_node), actual(actual) {
}

std::string ErrorTypeMismatch::to_str() const {
    std::string as = entity_to_string(actual);
    std::string pre_msg = "Expected " + E_HLT(expected.to_string()) + ", got " + E_HLT(as);
    pre_msg += " -- node: " + to_string(this->value_node.to_json());
    // std::string msg = highlight_one(value_node);
    // this->fail_ok(pre_msg, msg, value_node.start);
    return pre_msg;
}

bool ErrorTypeMismatch::equal(const Error& other) const {
    const auto& o = (const ErrorTypeMismatch&) other;
    bool act = o.actual == this->actual &&
               (*((EntityValue&) o.actual).value->type == *((EntityValue&) this->actual).value->type);
    bool exp = o.expected == this->expected;
    bool val = o.value_node == this->value_node;
    return act && exp && val;
}

Error* ErrorTypeMismatch::clone() const {
    return new ErrorTypeMismatch(this->expected, this->value_node, this->actual);
}
