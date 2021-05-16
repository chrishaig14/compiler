//
// Created by chris on 1/8/20.
//

#ifndef MEMBERNODE_H
#define MEMBERNODE_H


#include <string>
#include "Node.h"
#include "TypeNode.h"
#include "../scanner/Token.h"

enum class MemberType {
    NUM, STR
};

class MemberNode : public Node {
public:
    MemberType type;
    Node* parent;
    std::string s_child;
    size_t n_child;
    TypeNode* parent_t;
    TextPosition dot_pos;
    Token child_token;
    MemberNode(Node* parent, Token child_token);

    bool equal(const Node& x) const override;

    ~MemberNode();

    MemberNode& member() override;
    const MemberNode& member() const override;
    bool is_class_static_member;
    std::string replace_with_path;
};


#endif //MEMBERNODE_H
