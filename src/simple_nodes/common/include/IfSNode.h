//
// Created by chris on 13/4/21.
//

#ifndef XLANG_IFSNODE_H
#define XLANG_IFSNODE_H


#include "SNode.h"
#include "Block.h"
#include "../../expressions/include/Exp.h"

class sem::IfSNode : public sem::SNode {
    UExp _condition;
    std::unique_ptr<Block> _then;
public:
    Exp& condition;
    Block& then;
    std::unique_ptr<Block> _else;
    std::vector<std::pair<UExp, std::unique_ptr<Block>>> elifs;

    IfSNode(UExp condition, std::unique_ptr<Block> then, std::vector<std::pair<UExp, std::unique_ptr<Block>>> elifs,
            std::unique_ptr<Block> _else);

    bool equals(const SNode& o) const override;
};


#endif //XLANG_IFSNODE_H
