//
// Created by chris on 26/4/21.
//

#ifndef XLANG_BREAK_H
#define XLANG_BREAK_H


#include <vector>
#include <string>
#include "../SNode.h"

class sem::Break : public sem::SNode {
public:
    std::vector<std::string> reachables;
    Break();

    bool equals(const SNode& o) const override {
        return true;
    }
};


#endif //XLANG_BREAK_H
