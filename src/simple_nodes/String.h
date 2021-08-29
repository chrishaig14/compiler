//
// Created by chris on 5/4/21.
//

#ifndef XLANG_STRING_H
#define XLANG_STRING_H


#include <string>
#include "SNode.h"

class sem::String : public sem::SNode {
public:
    std::string s;
    String(const std::string& s);

    bool equals(const SNode& o) const override {
        auto& other = (const String&) o;
        return this->s == other.s;
    }

};


#endif //XLANG_STRING_H
