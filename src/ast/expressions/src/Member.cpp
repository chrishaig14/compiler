//
// Created by chris on 1/8/20.
//

#include "../include/Member.h"

using namespace ast;

Member::Member(ast::UExpNode parent, Token child_token) : ast::ExpNode(ExpNodeType::MEMBER, parent->start, child_token.end_pos),
                                                  _parent(std::move(parent)), parent(*_parent) {
    this->child_token = child_token;
    this->type = MemberType::STR;
    this->s_child = child_token.str;
}

Member::~Member() {
    // delete this->parent;
}

bool Member::equal(const ast::ExpNode& x) const {
    auto& other = (Member&) x;
    if (this->type != other.type) {
        return false;
    }
    if (this->type == MemberType::NUM) {
        if (this->n_child != other.n_child) {
            return false;
        }
    } else {
        if (this->s_child != other.s_child) {
            return false;
        }
    };
    return this->parent == other.parent;
}

nlohmann::json Member::to_json() const {
    return {{"type",   "member"},
            {"member", {{"parent", this->parent.to_json()}, {"child", this->s_child}}}};
}

