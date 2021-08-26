//
// Created by chris on 11/8/21.
//

#include "ErrorNoMember.h"

std::string ErrorNoMember::to_str() const {
    return "Error no member: " + this->m.s_child;
}

ErrorNoMember::ErrorNoMember(const TypeNode& t, const ast::Member& m) : m(m), t(t) {

}

bool ErrorNoMember::equal(const Error& other) const {
    const auto& o = (const ErrorNoMember&) other;
    bool act = o.m == this->m;
    bool exp = o.t == this->t;
    return act && exp;
}

Error* ErrorNoMember::clone() const {
    return new ErrorNoMember(this->t, this->m);
}
