//
// Created by chris on 4/4/21.
//

#include "Id.h"
using namespace sem;
Id::Id(const std::string& identifier) : SNode(SNodeType::ID) {
    this->identifier = identifier;
}
