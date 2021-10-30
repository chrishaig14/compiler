//
// Created by chris on 11/8/21.
//

#include "../include/NoMember.h"

using namespace error;

std::string NoMember::to_str(const CodeLines& code) const {
    return "Error no member: " + this->m.s_child;
}

NoMember::NoMember(const sem::Type& t, const ast::Member& m) : Error(ErrorType::no_member), m(m), t(t) {

}

bool NoMember::equal(const Error& other) const {
    const auto& o = (const NoMember&) other;
    bool act = o.m == this->m;
    bool exp = o.t == this->t;
    return act && exp;
}

Error* NoMember::clone() const {
    return new NoMember(this->t, this->m);
}
