//
// Created by chris on 1/8/20.
//

#ifndef MEMBERNODE_H
#define MEMBERNODE_H


#include <string>
#include "ExpNode.h"
#include "../Type.h"
#include "../../scanner/Token.h"

#include "../ast.h"

enum class MemberType {
    NUM, STR
};

class ast::Member : public ast::ExpNode {
    ast::UExpNode _parent;
public:
    ExpNode& parent;
    MemberType type;
    std::string s_child;
    size_t n_child;
    TextPosition dot_pos;
    Token child_token;
    Member(ast::UExpNode parent, Token child_token);

    bool equal(const ast::ExpNode& x) const override;

    ~Member() override;
    nlohmann::json to_json() const override;

};


#endif //MEMBERNODE_H
