//
// Created by chris on 26/4/21.
//

#ifndef XLANG_BREAK_H
#define XLANG_BREAK_H


#include <vector>
#include <string>
#include "SNode.h"

class Break : public SNode {
public:
    std::vector<std::string> reachables;
    Break();
};


#endif //XLANG_BREAK_H
