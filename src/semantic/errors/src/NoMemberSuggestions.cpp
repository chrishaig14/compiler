//
// Created by chris on 11/8/21.
//

#include "../include/NoMemberSuggestions.h"

using namespace error;

std::string NoMemberSuggestions::to_str(const CodeLines& code) const {
    std::string s = "Error no member: " + this->m.s_child + " candidates are: \n";

    for (auto& mm: this->clazz.members) {
        s += mm.first + mm.second->to_string() + "\n";
    }

    for (auto& mm: this->clazz.methods) {
        s += mm.first + ": " + mm.second->const_function_ft.to_string() + "\n";
    }

    return s;
}

NoMemberSuggestions::NoMemberSuggestions(const sem::Type& t, const ast::Member& m, const ConcreteClass& clazz)
        : m(m), t(t.clone()), clazz(clazz) {

}

bool NoMemberSuggestions::equal(const Error& other) const {
    const auto& o = (const NoMemberSuggestions&) other;
    bool act = o.m == this->m;
    bool exp = *o.t == *this->t;
    bool clz = &o.clazz == &this->clazz;
    return act && exp && clz;
}

Error* NoMemberSuggestions::clone() const {
    return new NoMemberSuggestions(*this->t, this->m, this->clazz);
}
