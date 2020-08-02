//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_FUNCTIONNODE_H
#define UNTITLED1_FUNCTIONNODE_H


#include <string>
#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "TypeNode.h"

class FunctionNode : public Node {
public:
    std::string identifier;
    std::vector<std::string> parameter_names;
    std::vector<TypeNode*> parameter_types;
    std::vector<Node*> body;
    TypeNode* return_type;


    void accept(Visitor& visitor) override;

    FunctionNode(std::string identifier, std::vector<std::string> parameter_names,
                 std::vector<TypeNode*> parameter_types, TypeNode* return_type, std::vector<Node*> body) {
        this->identifier = identifier;
        this->parameter_names = parameter_names;
        this->parameter_types = parameter_types;
        this->body = body;
        this->return_type = return_type;
    }


    bool equal(Node* other) const {
        auto other_ptr = dynamic_cast<FunctionNode*>(other);
        if (other_ptr == nullptr) return false;
        return *this == *other_ptr;
    }

    bool operator==(FunctionNode& other) const {
        if (this->identifier != other.identifier) return false;
        if (this->parameter_types.size() != other.parameter_types.size()) return false;
        if (this->parameter_names.size() != other.parameter_names.size()) return false;
        if (this->body.size() != other.body.size()) return false;
        for (int i = 0; i < this->parameter_types.size(); i++) {
            if (!this->parameter_types[i]->equal(other.parameter_types[i])) return false;
        }
        for (int i = 0; i < this->parameter_names.size(); i++) {
            if (this->parameter_names[i] != other.parameter_names[i]) return false;
        }
        for (int i = 0; i < this->body.size(); i++) {
            if (!this->body[i]->equal(other.body[i])) return false;
        }
        return true;
    }

    ~FunctionNode() {
        for (auto pt: this->parameter_types) {
            delete pt;
        }
        for (auto st: this->body) {
            delete st;
        }
        delete this->return_type;
    }

    json to_json() const override {
        json j;
        j["node"] = "function";
        j["identifier"] = this->identifier;
        j["parameter_names"] = {};
        j["parameter_types"] = {};
        j["body"] = {};
        for (auto pn: this->parameter_names) {
            j["parameter_names"].push_back(pn);
        }
        for (auto pt: this->parameter_types) {
            j["parameter_types"].push_back(pt->to_json());
        }
        for (auto st: this->body) {
            j["body"].push_back(st->to_json());
        }
        j["return_type"] = this->return_type->to_json();
        return j;
    }

};


#endif //UNTITLED1_FUNCTIONNODE_H
