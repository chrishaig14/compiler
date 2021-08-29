//
// Created by chris on 11/4/21.
//

#ifndef XLANG_FLOAT_H
#define XLANG_FLOAT_H


#include <string>
#include "SNode.h"

class sem::Float : public sem::SNode {
public:
    Float();
    std::string str;
};


#endif //XLANG_FLOAT_H
