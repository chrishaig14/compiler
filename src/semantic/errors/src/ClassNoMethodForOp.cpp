//
// Created by chris on 12/8/21.
//

#include "../include/ClassNoMethodForOp.h"

using namespace error;

Error* ClassNoMethodForOp::clone() const {
    return new ClassNoMethodForOp(this->class_name, this->op, this->node);
}

bool ClassNoMethodForOp::equal(const Error& other) const {
    auto& o = (const ClassNoMethodForOp&) other;
    bool class_ok = this->class_name == o.class_name;
    bool op_ok = this->op == o.op;
    bool node_ok = this->node == o.node;
    return class_ok and op_ok and node_ok;
}

std::string ClassNoMethodForOp::to_str(const CodeLines& code) const {
    return "Error: no method for op " + this->op + " in class " + this->class_name;
}

ClassNoMethodForOp::ClassNoMethodForOp(std::string class_name, std::string op, const ast::ExpNode& node)
        :Error(ErrorType::class_no_method_for_op), class_name(class_name), op(op), node(node) {

}
