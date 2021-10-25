//
// Created by chris on 12/8/21.
//

#include "../include/GlobalRedeclared.h"

using namespace error;

Error* GlobalRedeclared::clone() const {
    return new GlobalRedeclared(this->name);
    return nullptr;
}

bool GlobalRedeclared::equal(const Error& other) const {
    const GlobalRedeclared& o = (const GlobalRedeclared&) other;
    bool name_eq = this->name == o.name;
    // bool node_eq = this->node == o.node;
    return name_eq;
}

std::string GlobalRedeclared::to_str(const CodeLines& code) const {
    return E_HLT("Error: redeclared ") + E_INFO(this->name);
}

GlobalRedeclared::GlobalRedeclared(std::string name) : name(name) {

}
