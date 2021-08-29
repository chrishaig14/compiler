//
// Created by chris on 12/4/21.
//

#ifndef XLANG_LIST_H
#define XLANG_LIST_H


#include <vector>
#include "SNode.h"

class List : public SNode {

public:
    std::vector<USNode> elements;
    explicit List(std::vector<USNode>& elements);
};


#endif //XLANG_LIST_H
