//
// Created by chris on 2/4/21.
//

#ifndef XLANG_DEFAULTCONSTRUCTOR_H
#define XLANG_DEFAULTCONSTRUCTOR_H


#include "ExpNode.h"

class ast::DefaultConstructor : public ast::ExpNode {
public:
    DefaultConstructor(ExpNode* class_node, TextPosition start, TextPosition end) : ast::ExpNode(ExpNodeType::DEF_CONST,
                                                                                           start,
                                                                                           end) {
        this->class_node = class_node;
    }

    bool equal(const ast::ExpNode& other) const override {
        return false;
    }

    nlohmann::json to_json() const override {
        return nlohmann::json();
    }

    ExpNode* class_node;
};


#endif //XLANG_DEFAULTCONSTRUCTOR_H
