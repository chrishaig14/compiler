//
// Created by chris on 8/5/21.
//

#ifndef XLANG_TERNARY_H
#define XLANG_TERNARY_H


#include "SNode.h"

class sem::Ternary : public sem::SNode {

public:
    Ternary(SNode* ext, SNode* true_case, SNode* false_case);
    SNode* ext;
    SNode* true_case;
    SNode* false_case;

    bool equals(const SNode& o) const override {
        // auto& other = (const Ternary&) o;
        return false;
    }
};


#endif //XLANG_TERNARY_H
