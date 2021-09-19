//
// Created by chris on 10/4/21.
//

#ifndef XLANG_WHILE_H
#define XLANG_WHILE_H


#include "Common.h"
#include "Block.h"
#include "../../expressions/include/Exp.h"

class sem::While : public sem::Common {
public:
    UExp condition;
    std::unique_ptr<Block> body;

    While(UExp condition, std::unique_ptr<Block> body);

    bool equals(const Common& o) const override;

};


#endif //XLANG_WHILE_H
