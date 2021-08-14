//
// Created by chris on 2/4/21.
//

#ifndef XLANG_DEFAULTCONSTRUCTORNODE_H
#define XLANG_DEFAULTCONSTRUCTORNODE_H


#include "Node.h"

class DefaultConstructorNode : public Node {
public:
    DefaultConstructorNode(Node* class_node, TextPosition start, TextPosition end) : Node(NodeType::DEF_CONST,
                                                                                          start,
                                                                                          end) {
        this->class_node = class_node;
    }

    bool equal(const Node& other) const override {
        return false;
    }

    nlohmann::json to_json() const override {
        return nlohmann::json();
    }

    Node* class_node;
};


#endif //XLANG_DEFAULTCONSTRUCTORNODE_H
