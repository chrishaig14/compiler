//
// Created by chris on 1/8/20.
//

#include "../include/Subscript.h"

using namespace ast;

Subscript::Subscript(ast::UExpNode& parent, ast::VectorOfExpNodesU& child, TextPosition start, TextPosition end)
        : ast::ExpNode(ExpNodeType::SUB, start, end), parent(std::move(parent)), child(std::move(child)) {
}


bool Subscript::equal(const ast::ExpNode& x) const {
    auto& other = (Subscript&) x;
    if (this->child.size() != other.child.size()) {
        return false;
    }
    for (size_t i = 0; i < this->child.size(); i++) {
        if (*this->child[i] != *other.child[i]) {
            return false;
        }
    }
    return *this->parent == *other.parent;
}

Subscript::~Subscript() {
    // delete this->parent;
    // for (auto c: this->child) {
    //     delete c;
    // }
}

nlohmann::json Subscript::to_json() const {
    nlohmann::json j;
    j["type"] = "subscript";
    j["subscript"]["parent"] = this->parent->to_json();
    j["subscript"]["child"] = this->child[0]->to_json();
    return j;
}

