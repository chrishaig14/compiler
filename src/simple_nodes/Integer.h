//
// Created by chris on 4/4/21.
//

#ifndef XLANG_INTEGER_H
#define XLANG_INTEGER_H


#include <string>
#include "SNode.h"

class sem::Integer : public sem::SNode {
public:
    explicit Integer(const std::string& str);
    std::string str;
};


#endif //XLANG_INTEGER_H
