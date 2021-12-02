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
    Block body;

    While(UExp condition, const Block& body);

    While(const While& other);

    bool equals(const Common& o) const override;
    std::unique_ptr<Common> clone() const override;

};


#endif //XLANG_WHILE_H
