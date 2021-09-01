//
// Created by chris on 4/4/21.
//

#ifndef XLANG_CALL_H
#define XLANG_CALL_H


#include <vector>
#include "SNode.h"

class sem::Call : public sem::SNode {
public:
    USNode function;
    std::vector<USNode> arguments;
    Call(USNode function, std::vector<USNode> arguments);

    bool equals(const SNode& o) const override {
        auto& other = (const Call&) o;
        return *this->function == *other.function && this->arguments == other.arguments;
    }
};


#endif //XLANG_CALL_H
