//
// Created by chris on 11/4/21.
//

#include "../include/Float.h"

using namespace sem;

Float::Float(const std::string& s) : sem::Exp(ExpType::FLOAT) {
    this->str = s;
}
