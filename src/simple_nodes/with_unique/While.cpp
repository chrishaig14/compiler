//
// Created by chris on 10/4/21.
//

#include "While.h"

using namespace sem;

While::While(UExp condition, std::unique_ptr<Block> body) : SNode(SNodeType::WHILE), condition(std::move(condition)), body(std::move(body)) {
}

bool While::equals(const SNode& o) const {
    auto& other = (const While&) o;
    bool cond_ok = *this->condition == *other.condition;
    bool body_ok = *this->body == *other.body;
    return cond_ok && body_ok;
}
