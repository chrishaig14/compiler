//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_TYPENODE_H
#define UNTITLED1_TYPENODE_H


#include <string>
#include <vector>
#include "Node.h"
#include "Visitor.h"

class TypeNode : public Node {
public:
    std::string identifier;
    std::vector<TypeNode*> type_parameters;

    TypeNode(const std::string& identifier, const std::vector<TypeNode*>& typeParameters);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override {
        auto other_ptr = dynamic_cast<TypeNode*>(other);
        if (other_ptr == nullptr) return false;
        return *this == *other_ptr;
    }

    bool operator==(TypeNode& other) const {
        if (this->identifier != other.identifier) return false;
        if (this->type_parameters.size() != other.type_parameters.size()) return false;
        for (int i = 0; i < this->type_parameters.size(); ++i) {
            if (!this->type_parameters[i]->equal(other.type_parameters[i]))return false;
        }
        return true;
    }
};


#endif //UNTITLED1_TYPENODE_H
