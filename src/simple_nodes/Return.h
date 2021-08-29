//
// Created by chris on 4/4/21.
//

#ifndef XLANG_RETURN_H
#define XLANG_RETURN_H


#include "SNode.h"
#include <vector>
#include <string>

class sem::Return : public sem::SNode {
public:
    USNode expression;
    std::vector<std::string> reachables;
    Return(USNode& expression);
};


#endif //XLANG_RETURN_H
