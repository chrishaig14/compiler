//
// Created by chris on 13/4/21.
//

#ifndef XLANG_IFSNODE_H
#define XLANG_IFSNODE_H


#include "SNode.h"
#include "Block.h"

class sem::IfSNode : public sem::SNode {
public:
    SNode* condition;
    Block* then;
    Block* _else;
    std::vector<std::pair<SNode*, Block*>> elifs;

    IfSNode(SNode* condition, Block* then, std::vector<std::pair<SNode*, Block*>> elifs, Block* _else);

    bool equals(const SNode& o) const override {
        auto& other = (const IfSNode&) o;
        return *this->condition == *other.condition && *this->then == *other.then && *this->_else == *other._else;
    }
};


#endif //XLANG_IFSNODE_H
