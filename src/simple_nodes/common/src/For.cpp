//
// Created by chris on 26/9/21.
//

#include "../include/For.h"
#include "../../expressions/include/Exp.h"

using namespace sem;

For::For(const std::string& varname, UExp expression, const Block& body)
        : Common(CommonType::FOR), varname(varname), expression(std::move(expression)), body(body) {
}

bool For::equals(const Common& o) const {
    auto& other = (const For&) o;
    bool var_ok = this->varname == other.varname;
    bool cond_ok = *this->expression == *other.expression;
    bool body_ok = this->body == other.body;
    return var_ok and cond_ok && body_ok;
}

std::unique_ptr<Common> For::clone() const {
    return std::make_unique<For>(*this);
}

For::For(const For& other) : sem::Common(CommonType::FOR),body(other.body){
    this->expression = other.expression->clone();
    this->varname = other.varname;
}
