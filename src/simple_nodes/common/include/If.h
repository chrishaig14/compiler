//
// Created by chris on 13/4/21.
//

#ifndef XLANG_IF_H
#define XLANG_IF_H


#include "Common.h"
#include "Block.h"
#include "../../expressions/include/Exp.h"

class sem::If : public sem::Common {
    UExp _condition;
public:
    Exp& condition;
    Block then;
    std::unique_ptr<Block> _else;
    std::vector<std::pair<UExp, Block>> elifs;

    If(UExp condition, const Block& then, std::vector<std::pair<UExp, Block>> elifs, std::unique_ptr<Block> _else);

    If(const If& other)
            : sem::Common(CommonType::IF), _condition(other.condition.clone()), condition(*this->_condition),
              then(other.then),
              _else(static_cast<Block*>(other._else != nullptr ? other._else->clone().release() : nullptr)) {
        for (auto& e: other.elifs) {
            this->elifs.emplace_back(nullptr, e.second);
        }

    }

    bool equals(const Common& o) const override;
    std::unique_ptr<Common> clone() const override;
};


#endif //XLANG_IF_H
