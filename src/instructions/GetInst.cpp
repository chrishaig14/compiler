//
// Created by chris on 2/8/20.
//

#include "GetInst.h"

GetInst::GetInst(const std::string& identifier) : identifier(identifier) {}

std::string GetInst::to_string() const {
    return "GET " + this->identifier;
}
