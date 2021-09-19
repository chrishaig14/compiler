//
// Created by chris on 30/4/21.
//

#include "../include/EnumDef.h"

using namespace sem;

EnumDef::EnumDef(const std::string& id, VectorOfStrings values) : Common(CommonType::ENUM) {
    this->id = id;
    this->values = values;
}

bool EnumDef::equals(const Common& o) const {
    auto& other = (const EnumDef&) o;
    bool id_ok = this->id == other.id;
    bool values_ok = this->values == other.values;
    return id_ok && values_ok;
}
