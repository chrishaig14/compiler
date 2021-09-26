//
// Created by chris on 26/9/21.
//

#include "../include/For.h"
#include "../../expressions/include/Exp.h"
#include "../include/Block.h"

using namespace sem;

For::For(const std::string& varname, UExp expression, std::unique_ptr<Block> body)
        : Common(CommonType::FOR), expression(std::move(expression)), body(std::move(body)) {
}

bool For::equals(const Common& o) const {
    auto& other = (const For&) o;
    bool cond_ok = *this->expression == *other.expression;
    bool body_ok = *this->body == *other.body;
    return cond_ok && body_ok;
}
