//
// Created by chris on 11/4/21.
//

#include "../include/TemplateKlassDef.h"

using namespace sem;

bool TemplateKlassDef::equals(const Top& o) const {
    auto& other = (const TemplateKlassDef&) o;
    if (this->identifier != other.identifier) {
        return false;
    }
    if (this->members.size() != other.members.size()) {
        return false;
    }
    for (size_t i = 0; i < this->members.size(); i++) {
        if (this->members[i] != other.members[i]) {
            return false;
        }
    }
    return true;
}

TemplateKlassDef::TemplateKlassDef(std::string identifier, VectorOfStrings members)
        : Top(TopType::CLASS), identifier(identifier), members(members) {
}
