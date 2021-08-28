//
// Created by chris on 2/4/21.
//

#ifndef XLANG_DEFAULTCONSTRUCTOR_H
#define XLANG_DEFAULTCONSTRUCTOR_H


#include "Node.h"
#include "ast.h"

class ast::DefaultConstructor : public ast::Node {
public:
    DefaultConstructor(Node* class_node, TextPosition start, TextPosition end) : ast::Node(NodeType::DEF_CONST,
                                                                                           start,
                                                                                           end) {
        this->class_node = class_node;
    }

    bool equal(const ast::Node& other) const override {
        return false;
    }

    nlohmann::json to_json() const override {
        return nlohmann::json();
    }

    Node* class_node;
};


#endif //XLANG_DEFAULTCONSTRUCTOR_H
