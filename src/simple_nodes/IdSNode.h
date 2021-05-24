//
// Created by chris on 4/4/21.
//

#ifndef XLANG_IDSNODE_H
#define XLANG_IDSNODE_H

#include <string>
#include "SNode.h"

class IdSNode : public SNode {
public:
    std::string identifier;

    IdSNode();

    explicit IdSNode(std::string identifier);
};


#endif //XLANG_IDSNODE_H
