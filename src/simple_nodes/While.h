//
// Created by chris on 10/4/21.
//

#ifndef XLANG_WHILE_H
#define XLANG_WHILE_H


#include "SNode.h"
#include "Block.h"

class sem::While : public sem::SNode {
public:
    USNode condition;
    std::unique_ptr<Block> body;

    While(USNode condition, std::unique_ptr<Block> body);

    bool equals(const SNode& o) const override {
        auto& other = (const While&) o;
        bool cond_ok = *this->condition == *other.condition;
        bool body_ok = *this->body == *other.body;
        return cond_ok && body_ok;
    }

};


#endif //XLANG_WHILE_H
