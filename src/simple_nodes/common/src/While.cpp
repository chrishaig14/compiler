//
// Created by chris on 10/4/21.
//

#include "../include/While.h"

using namespace sem;

While::While(UExp condition, const Block& body)
        : Common(CommonType::WHILE), condition(std::move(condition)), body(body) {
}

bool While::equals(const Common& o) const {
    auto& other = (const While&) o;
    bool cond_ok = *this->condition == *other.condition;
    bool body_ok = this->body == other.body;
    return cond_ok && body_ok;
}

std::unique_ptr<Common> While::clone() const {
    return std::make_unique<While>(*this);
}

While::While(const While& other) : sem::Common(CommonType::WHILE), body(other.body) {
    this->condition = other.condition->clone();
}
