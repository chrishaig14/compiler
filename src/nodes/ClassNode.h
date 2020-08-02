//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_CLASSNODE_H
#define UNTITLED1_CLASSNODE_H


#include <string>
#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "DeclarationNode.h"
#include "FunctionNode.h"

class ClassNode : public Node {
public:
    std::string identifier;
    std::vector<std::string> template_parameters;
    std::vector<DeclarationNode*> fields;
    std::vector<FunctionNode*> methods;

    ClassNode(std::string identifier, std::vector<std::string> templateParameters,
              std::vector<DeclarationNode*> fields, std::vector<FunctionNode*> methods);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const {
        auto other_ptr = dynamic_cast<ClassNode*>(other);
        if (other_ptr == nullptr) return false;
        return *this == *other_ptr;
    }

    bool operator==(ClassNode& other) const {
        if (this->identifier != other.identifier) return false;
        if (this->template_parameters.size() != other.template_parameters.size()) return false;
        if (this->fields.size() != other.fields.size()) return false;
        if (this->methods.size() != other.methods.size()) return false;
        for (int i = 0; i < this->template_parameters.size(); i++) {
            if (this->template_parameters[i] != other.template_parameters[i]) return false;
        }
        for (int i = 0; i < this->fields.size(); i++) {
            if (!this->fields[i]->equal(other.fields[i])) return false;
        }
        for (int i = 0; i < this->methods.size(); i++) {
            if (!this->methods[i]->equal(other.methods[i])) return false;
        }
        return true;
    }

    ~ClassNode() {
        for (auto f: this->fields) {
            delete f;
        }
        for (auto m: this->methods) {
            delete m;
        }
    }

};


#endif //UNTITLED1_CLASSNODE_H
