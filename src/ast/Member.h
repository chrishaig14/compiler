//
// Created by chris on 1/8/20.
//

#ifndef MEMBERNODE_H
#define MEMBERNODE_H


#include <string>
#include "Node.h"
#include "Type.h"
#include "../scanner/Token.h"

#include "ast.h"

enum class MemberType {
    NUM, STR
};

class ast::Member : public ast::Node {
    ast::UNode _parent;
public:
    Node& parent;
    MemberType type;
    std::string s_child;
    size_t n_child;
    TextPosition dot_pos;
    Token child_token;
    Member(ast::UNode parent, Token child_token);

    bool equal(const ast::Node& x) const override;

    ~Member() override;
    nlohmann::json to_json() const override;

};


#endif //MEMBERNODE_H
