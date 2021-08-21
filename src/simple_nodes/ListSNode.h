//
// Created by chris on 12/4/21.
//

#ifndef XLANG_LISTSNODE_H
#define XLANG_LISTSNODE_H


#include <vector>
#include "SNode.h"

class ListSNode : public SNode {

public:
    std::vector<USNode> elements;
    explicit ListSNode(std::vector<USNode>& elements);
};


#endif //XLANG_LISTSNODE_H
