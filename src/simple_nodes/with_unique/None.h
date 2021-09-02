//
// Created by chris on 8/5/21.
//

#ifndef XLANG_NONE_H
#define XLANG_NONE_H


#include "../SNode.h"

class sem::None : public sem::SNode {
public:
    None();

    bool equals(const SNode& o) const override {
        return true;
    }

};


#endif //XLANG_NONE_H
