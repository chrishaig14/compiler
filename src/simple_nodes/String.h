//
// Created by chris on 5/4/21.
//

#ifndef XLANG_STRING_H
#define XLANG_STRING_H


#include <string>
#include "SNode.h"

class String : public SNode {
public:
    std::string s;
    String(const std::string& s);
};


#endif //XLANG_STRING_H
