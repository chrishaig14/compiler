//
// Created by chris on 11/8/21.
//

#include "../include/ErrorNoMemberSuggestions.h"

using namespace error;

std::string ErrorNoMemberSuggestions::to_str(const CodeLines& code) const {
    std::string s = "Error no member: " + this->m.s_child + " candidates are: \n";

    for (auto& mm: this->clazz.members) {
        s += mm.first + mm.second->to_string() + "\n";
    }

    for (auto& mm: this->clazz.methods) {
        s += mm.first + ": " + mm.second->const_function_ft.to_string() + "\n";
    }

    return s;
}

ErrorNoMemberSuggestions::ErrorNoMemberSuggestions(const sem::Type& t, const ast::Member& m, const ConcreteClass& clazz)
        : m(m), t(t.clone()), clazz(clazz) {

}

bool ErrorNoMemberSuggestions::equal(const Error& other) const {
    const auto& o = (const ErrorNoMemberSuggestions&) other;
    bool act = o.m == this->m;
    bool exp = *o.t == *this->t;
    bool clz = &o.clazz == &this->clazz;
    return act && exp && clz;
}

Error* ErrorNoMemberSuggestions::clone() const {
    return new ErrorNoMemberSuggestions(*this->t, this->m, this->clazz);
}
