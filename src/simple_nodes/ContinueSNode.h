//
// Created by chris on 26/4/21.
//

#ifndef XLANG_CONTINUESNODE_H
#define XLANG_CONTINUESNODE_H


#include "SNode.h"
#include <vector>
#include <string>

class ContinueSNode : public SNode {
public:
    std::vector<std::string> reachables;
    ContinueSNode();
};


#endif //XLANG_CONTINUESNODE_H
