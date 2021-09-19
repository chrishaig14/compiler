//
// Created by chris on 13/4/21.
//

#include <cassert>
#include "../include/If.h"

using namespace sem;

If::If(UExp _condition, std::unique_ptr<Block> _then,
       std::vector<std::pair<UExp, std::unique_ptr<Block>>> elifs, std::unique_ptr<Block> _else) : Common(
        CommonType::IF), _condition(std::move(_condition)), _then(std::move(_then)),

                                                                                                             condition(*this->_condition),
                                                                                                             then(*this->_then),
                                                                                                             _else(std::move(
                                                                                                                     _else)),
                                                                                                             elifs(std::move(
                                                                                                                     elifs)) {
    assert(this->_condition != nullptr);
    assert(this->_then != nullptr);
}

bool If::equals(const Common& o) const {
    auto& other = (const If&) o;
    bool cond_ok = this->condition == other.condition;
    bool then_ok = this->then == other.then;
    bool else_ok = (this->_else == nullptr and other._else == nullptr) or
                   ((this->_else != nullptr and other._else != nullptr) and *this->_else == *other._else);
    return cond_ok && then_ok && else_ok;
}
