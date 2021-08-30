//
// Created by chris on 4/4/21.
//

#ifndef XLANG_DECLARATION_H
#define XLANG_DECLARATION_H

#include <string>
#include "SNode.h"
#include <cassert>

class sem::Declaration : public sem::SNode {
public:
    std::string identifier;
    USNode expression;

    Declaration(std::string identifier, USNode expression);

    bool equals(const SNode& o) const override {
        assert(this->type == o.type);
        auto& other = (const Declaration&) o;
        bool id_ok = this->identifier == other.identifier;
        bool exp_ok = *this->expression == *other.expression;
        return id_ok && exp_ok;
    }
};


#endif //XLANG_DECLARATION_H
