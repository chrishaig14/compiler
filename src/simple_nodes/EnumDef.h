//
// Created by chris on 30/4/21.
//

#ifndef XLANG_ENUMDEF_H
#define XLANG_ENUMDEF_H


#include <string>
#include "SNode.h"
#include "../types.h"

class sem::EnumDef : public sem::SNode {
public:
    EnumDef();

    std::string id;
    VectorOfStrings values;
};


#endif //XLANG_ENUMDEF_H
