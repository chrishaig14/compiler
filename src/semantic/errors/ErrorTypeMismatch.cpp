//
// Created by chris on 8/8/21.
//

#include "ErrorTypeMismatch.h"

ErrorTypeMismatch::ErrorTypeMismatch(const TypeNode& expected, const Node& value_node, const Entity& actual) : expected(
        expected), value_node(value_node), actual(actual) {
}

std::string ErrorTypeMismatch::to_str() {
    std::string as;
    if (actual.type == E_TYPE::VALUE) {
        as = actual.value->type->to_string();
    } else {
        as = entity_to_string(actual);
    }
    std::string pre_msg = "Expected " + E_HLT(expected.to_string()) + ", got " + E_HLT(as);
    // std::string msg = highlight_one(value_node);
    // this->fail_ok(pre_msg, msg, value_node.start);
    return pre_msg;
}
