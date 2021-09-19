//
// Created by chris on 13/4/21.
//

#ifndef XLANG_IF_H
#define XLANG_IF_H


#include "Common.h"
#include "Block.h"
#include "../../expressions/include/Exp.h"

class sem::If : public sem::Common {
    UExp _condition;
    std::unique_ptr<Block> _then;
public:
    Exp& condition;
    Block& then;
    std::unique_ptr<Block> _else;
    std::vector<std::pair<UExp, std::unique_ptr<Block>>> elifs;

    If(UExp condition, std::unique_ptr<Block> then, std::vector<std::pair<UExp, std::unique_ptr<Block>>> elifs,
       std::unique_ptr<Block> _else);

    bool equals(const Common& o) const override;
};


#endif //XLANG_IF_H
