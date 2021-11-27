//
// Created by chris on 11/4/21.
//

#include "../include/KlassDef.h"

using namespace sem;

bool KlassDef::equals(const Top& o) const {
    auto& other = (const KlassDef&) o;
    if (this->identifier != other.identifier) {
        return false;
    }
    if (this->attributes.size() != other.attributes.size()) {
        return false;
    }
    for (size_t i = 0; i < this->attributes.size(); i++) {
        if (this->attributes[i] != other.attributes[i]) {
            return false;
        }
    }
    return true;
}

KlassDef::KlassDef(std::string identifier, VectorOfStrings attributes)
        : Top(TopType::CLASS), identifier(identifier), attributes(attributes) {
}
