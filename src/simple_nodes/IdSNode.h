//
// Created by chris on 4/4/21.
//

#ifndef XLANG_IDSNODE_H
#define XLANG_IDSNODE_H

#include <string>
#include "SNode.h"

class sem::IdSNode : public sem::SNode {
public:
    std::string identifier;

    explicit IdSNode(const std::string& identifier);
};


#endif //XLANG_IDSNODE_H
