//
// Created by chris on 4/4/21.
//

#include "../include/FunctionDef.h"

using namespace sem;

FunctionDef::FunctionDef(std::string identifier, VectorOfStrings params, const Block& body,
                         std::map<std::string, std::set<std::string>> instances)
        : Top(TopType::FUNCTION), identifier(identifier), params(params), body(body), instances(instances) {
}

bool FunctionDef::equals(const Top& o) const {
    auto& other = (const FunctionDef&) o;
    bool id_ok = this->identifier == other.identifier;
    bool params_ok = this->params == other.params;
    bool body_ok = this->body == other.body;
    return id_ok && params_ok && body_ok;
}
