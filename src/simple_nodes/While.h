//
// Created by chris on 10/4/21.
//

#ifndef XLANG_WHILE_H
#define XLANG_WHILE_H


#include "SNode.h"
#include "Block.h"

class sem::While : public sem::SNode {
public:
    USNode condition;
    Block* body;

    While(USNode condition, Block* body);
};


#endif //XLANG_WHILE_H
