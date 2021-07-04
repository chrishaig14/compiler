//
// Created by chris on 24/3/21.
//

#ifndef XLANG_METHODNODE_H
#define XLANG_METHODNODE_H

#include <string>
#include "Node.h"

#include "Node.h"
#include "TypeNode.h"

enum class MethodType {
    NUM, STR
};

class MethodNode : public Node {
public:
    MethodType type;
    Node* parent;
    std::string s_child;
    int n_child;
    TypeNode* parent_t;


    MethodNode(Node* parent, const std::string& child, TextPosition start, TextPosition end);
    MethodNode(Node* parent, int child, TextPosition start, TextPosition end);

    bool equal(const Node& x) const override;

    ~MethodNode();

    std::string actual_function_name;
    unsigned long n_partial;
};


#endif //XLANG_METHODNODE_H
