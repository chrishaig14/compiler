//
// Created by chris on 13/4/21.
//

#ifndef XLANG_IFSNODE_H
#define XLANG_IFSNODE_H


#include "../common/include/SNode.h"
#include "../common/include/Block.h"

class sem::IfSNode : public sem::SNode {
public:
    USNode condition;
    std::unique_ptr<Block> then;
    std::unique_ptr<Block> _else;
    std::vector<std::pair<USNode, std::unique_ptr<Block>>> elifs;

    IfSNode(USNode condition, std::unique_ptr<Block> then, std::vector<std::pair<USNode, std::unique_ptr<Block>>> elifs,
            std::unique_ptr<Block> _else);

    bool equals(const SNode& o) const override {
        auto& other = (const IfSNode&) o;
        return *this->condition == *other.condition && *this->then == *other.then && *this->_else == *other._else;
    }
};


#endif //XLANG_IFSNODE_H
