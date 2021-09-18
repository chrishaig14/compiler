//
// Created by chris on 26/4/21.
//

#ifndef XLANG_CONTINUE_H
#define XLANG_CONTINUE_H


#include "../common/include/SNode.h"
#include <vector>
#include <string>

class sem::Continue : public sem::SNode {
public:
    std::vector<std::string> reachables;
    Continue();

    bool equals(const SNode& o) const override {
        return true;
    }
};


#endif //XLANG_CONTINUE_H
