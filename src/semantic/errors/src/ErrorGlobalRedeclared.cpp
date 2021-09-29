//
// Created by chris on 12/8/21.
//

#include "../include/ErrorGlobalRedeclared.h"

Error* ErrorGlobalRedeclared::clone() const {
    return new ErrorGlobalRedeclared(this->name);
    return nullptr;
}

bool ErrorGlobalRedeclared::equal(const Error& other) const {
    const ErrorGlobalRedeclared& o = (const ErrorGlobalRedeclared&) other;
    bool name_eq = this->name == o.name;
    // bool node_eq = this->node == o.node;
    return name_eq;
}

std::string ErrorGlobalRedeclared::to_str(const CodeLines& code) const {
    return E_HLT("Error: redeclared ") + E_INFO(this->name);
}

ErrorGlobalRedeclared::ErrorGlobalRedeclared(std::string name) : name(name) {

}
