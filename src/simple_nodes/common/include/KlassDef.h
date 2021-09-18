//
// Created by chris on 11/4/21.
//

#ifndef XLANG_KLASSDEF_H
#define XLANG_KLASSDEF_H


#include <string>
#include "SNode.h"
#include "../../with_unique/FunctionDef.h"

class sem::KlassDef : public sem::SNode {
public:
    std::string identifier;
    VectorOfStrings members;
    std::vector<std::unique_ptr<sem::FunctionDef>> methods;

    KlassDef(std::string identifier, VectorOfStrings members)
            : SNode(SNodeType::CLASS), identifier(identifier), members(members) {
    }

    bool equals(const SNode& o) const override {
        auto& other = (const KlassDef&) o;
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
};


#endif //XLANG_KLASSDEF_H
