//
// Created by chris on 12/8/21.
//

#include "../include/ErrorClassNoMethodForOp.h"

Error* ErrorClassNoMethodForOp::clone() const {
    return new ErrorClassNoMethodForOp(this->class_name, this->op, this->node);
}

bool ErrorClassNoMethodForOp::equal(const Error& other) const {
    auto& o = (const ErrorClassNoMethodForOp&) other;
    bool class_ok = this->class_name == o.class_name;
    bool op_ok = this->op == o.op;
    bool node_ok = this->node == o.node;
    return class_ok and op_ok and node_ok;
}

std::string ErrorClassNoMethodForOp::to_str(const CodeLines& code) const {
    return "Error: no method for op " + this->op + " in class " + this->class_name;
}

ErrorClassNoMethodForOp::ErrorClassNoMethodForOp(std::string class_name, std::string op, const ast::ExpNode& node)
        : class_name(class_name), op(op), node(node) {

}
