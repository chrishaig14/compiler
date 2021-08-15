//
// Created by chris on 11/8/21.
//

#include "ErrorNoMemberSuggestions.h"

std::string ErrorNoMemberSuggestions::to_str() const {
    std::string s = "Error no member: " + this->m.s_child + " candidates are: ";
    for (auto mm: this->clazz.members) {
        s += mm.first + ", ";
    }
    return s;
}

ErrorNoMemberSuggestions::ErrorNoMemberSuggestions(const TypeNode& t, const MemberNode& m, const Class& clazz)
        : m(m), t(t), clazz(clazz) {

}

bool ErrorNoMemberSuggestions::equal(const Error& other) const {
    const auto& o = (const ErrorNoMemberSuggestions&) other;
    bool act = o.m == this->m;
    bool exp = o.t == this->t;
    bool clz = &o.clazz == &this->clazz;
    return act && exp && clz;
}

Error* ErrorNoMemberSuggestions::clone() const {
    return new ErrorNoMemberSuggestions(this->t, this->m, this->clazz);
}
