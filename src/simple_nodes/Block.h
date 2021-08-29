//
// Created by chris on 4/4/21.
//

#ifndef XLANG_BLOCK_H
#define XLANG_BLOCK_H

#include "SNode.h"
#include <vector>
#include <string>
class sem::Block : public sem::SNode {
public:
    bool unwrap;
    std::vector<std::string> locals;
    std::vector<SNode*> nodes;
    Block(bool unwrap);
    Block();
};


#endif //XLANG_BLOCK_H
