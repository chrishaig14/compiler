//
// Created by chris on 12/8/21.
//

#include "../include/CantAssign.h"

using namespace error;

Error* CantAssign::clone() const {
    return nullptr;
}

bool CantAssign::equal(const Error& other) const {
    return false;
}

std::string CantAssign::to_str(const CodeLines& code) const {
    return std::string();
}

CantAssign::CantAssign(const ast::Statement& n) : n(n) {
}
