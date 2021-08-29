//
// Created by chris on 9/5/21.
//

#ifndef XLANG_DICTSNODE_H
#define XLANG_DICTSNODE_H


#include <vector>
#include "SNode.h"

class sem::DictSNode : public sem::SNode {

public:
    explicit DictSNode(const std::vector<std::pair<SNode*, SNode*>>& items);
    std::vector<std::pair<SNode*, SNode*>> items;
};


#endif //XLANG_DICTSNODE_H
