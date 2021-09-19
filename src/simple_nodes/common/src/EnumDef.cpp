//
// Created by chris on 30/4/21.
//

#include "../include/EnumDef.h"

using namespace sem;

EnumDef::EnumDef(const std::string& id, VectorOfStrings values) : SNode(SNodeType::ENUM) {
    this->id = id;
    this->values = values;
}
