//
// Created by chris on 12/8/21.
//

#include "../include/GlobalRedeclared.h"

using namespace error;

Error* GlobalRedeclared::clone() const {
    return new GlobalRedeclared(this->id);
}

bool GlobalRedeclared::equal(const Error& other) const {
    const GlobalRedeclared& o = (const GlobalRedeclared&) other;
    bool name_eq = this->id.str == o.id.str;
    // bool node_eq = this->node == o.node;
    return name_eq;
}

std::string GlobalRedeclared::to_str(const CodeLines& code) const {
    return E_HLT("Error: redeclared ") + E_INFO(this->id.str);
}

GlobalRedeclared::GlobalRedeclared(Token id) : Error(ErrorType::global_redeclared), id(id) {

}
