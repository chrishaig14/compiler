//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_DECLARATIONNODE_H
#define UNTITLED1_DECLARATIONNODE_H


#include <string>
#include "Node.h"
#include "Visitor.h"

class DeclarationNode : public Node {
public:
    std::string identifier;
    Node* type;
    Node* expression;

    DeclarationNode(const std::string& identifier, Node* type, Node* expression);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const {
        auto other_ptr = dynamic_cast<DeclarationNode*>(other);
        if (other_ptr == nullptr) return false;
        return *this == *other_ptr;
    }

    bool operator==(DeclarationNode& other) const {
        if (this->type == nullptr && other.type != nullptr) return false;
        if (this->type != nullptr && other.type == nullptr) return false;
        if (this->expression == nullptr && other.expression != nullptr) return false;
        if (this->expression != nullptr && other.expression == nullptr) return false;
        bool same_type = this->type == nullptr or this->type->equal(other.type);
        bool same_expression = this->expression == nullptr or this->expression->equal(other.expression);
        bool same_identifier = this->identifier == other.identifier;
        return same_identifier && same_type && same_expression;
    }

    ~DeclarationNode() {
        if (this->type != nullptr) delete this->type;
        if (this->expression != nullptr) delete this->expression;
    }

    json to_json() const override {
        json j;
        j["node"] = "declaration";
        j["identifier"] = this->identifier;
        j["expression"] = {};
        if (this->expression != nullptr) {
            j["expression"] = this->expression->to_json();
        }
        if (this->type != nullptr) {
            j["type"] = this->type->to_json();
        }
        return j;
    }

};


#endif //UNTITLED1_DECLARATIONNODE_H
