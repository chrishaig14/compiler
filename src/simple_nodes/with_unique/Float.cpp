//
// Created by chris on 11/4/21.
//

#include "Float.h"

using namespace sem;

Float::Float(const std::string& s) : sem::SNode(SNodeType::FLOAT) {
    this->str = s;
}
