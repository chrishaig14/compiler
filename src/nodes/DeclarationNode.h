//
// Created by chris on 1/8/20.
//

#ifndef DECLARATIONNODE_H
#define DECLARATIONNODE_H


#include <string>
#include "Node.h"
#include "TypeNode.h"
#include "Visitor.h"
#include "NodeContainer.h"
class DeclarationNode : public Node {
public:
    std::string identifier;
    TypeNode* type;
    NodeContainer expression;

    DeclarationNode(const std::string& identifier, TypeNode* type, NodeContainer expression);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const {
//        auto other_ptr = dynamic_cast<DeclarationNode*>(other);
//        if (other_ptr == nullptr) return false;
//        return *this == *other_ptr;
return false;
    }

    bool operator==(DeclarationNode& other) const {
        return false;
//        if (this->type == nullptr && other.type != nullptr) return false;
//        if (this->type != nullptr && other.type == nullptr) return false;
//        if (this->expression == nullptr && other.expression != nullptr) return false;
//        if (this->expression != nullptr && other.expression == nullptr) return false;
//        bool same_type = this->type == nullptr or this->type->equal(other.type);
//        bool same_expression = this->expression == nullptr or this->expression->equal(other.expression);
//        bool same_identifier = this->identifier == other.identifier;
//        return same_identifier && same_type && same_expression;
    }

    ~DeclarationNode() {
//        if (this->type != nullptr) delete this->type;
//        if (this->expression != nullptr) delete this->expression;
    }

    json to_json() const override {
        json j;
        j["node"] = "declaration";
        j["identifier"] = this->identifier;
        j["expression"] = {};
//        if (this->expression != nullptr) {
//            j["expression"] = this->expression->to_json();
//        }
        if (this->type != nullptr) {
//            j["type"] = this->type->to_json();
        }
        return j;
    }

};


#endif //DECLARATIONNODE_H
