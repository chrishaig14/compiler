//
// Created by chris on 12/4/21.
//

#ifndef XLANG_LISTSNODE_H
#define XLANG_LISTSNODE_H


#include <vector>
#include "SNode.h"

class ListSNode : public SNode {

public:
    std::vector<SNode*> elements;
    explicit ListSNode(std::vector<SNode*> elements);
};


#endif //XLANG_LISTSNODE_H
