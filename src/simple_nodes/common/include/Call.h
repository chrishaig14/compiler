//
// Created by chris on 4/4/21.
//

#ifndef XLANG_CALL_H
#define XLANG_CALL_H


#include <vector>
#include "SNode.h"

class sem::Call : public sem::SNode {
public:
    UExp function;
    std::vector<UExp> arguments;
    Call(UExp function, std::vector<UExp> arguments);

    bool equals(const SNode& o) const override;
};


#endif //XLANG_CALL_H
