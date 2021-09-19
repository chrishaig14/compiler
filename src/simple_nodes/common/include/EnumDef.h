//
// Created by chris on 30/4/21.
//

#ifndef XLANG_ENUMDEF_H
#define XLANG_ENUMDEF_H


#include <string>
#include "SNode.h"
#include "../../../types.h"

class sem::EnumDef : public sem::SNode {
public:
    EnumDef(const std::string& id, VectorOfStrings values);

    std::string id;
    VectorOfStrings values;

    bool equals(const SNode& o) const override {
        auto& other = (const EnumDef&) o;
        bool id_ok = this->id == other.id;
        bool values_ok = this->values == other.values;
        return id_ok && values_ok;
    }
};


#endif //XLANG_ENUMDEF_H
