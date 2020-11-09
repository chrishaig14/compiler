//
// Created by chris on 1/8/20.
//

#ifndef FUNCTIONNODE_H
#define FUNCTIONNODE_H


#include <string>
#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "TypeNode.h"
#include "BlockNode.h"
#include "NodeContainer.h"

class FunctionNode : public Node {
public:
    std::string identifier;
    std::vector<std::string> parameter_names;
    std::vector<TypeNode*> parameter_types;
    BlockNode* body;
    TypeNode* return_type;
    std::map<std::string, int> free_variables;


    void accept(Visitor& visitor) override;

    FunctionNode(std::string identifier, std::vector<std::string> parameter_names,
                 std::vector<TypeNode*> parameter_types, TypeNode* return_type, BlockNode* body) {
        assert(body != nullptr);
        this->identifier = identifier;
        this->parameter_names = parameter_names;
        this->parameter_types = parameter_types;
        this->body = body;
        this->return_type = return_type;
    }


    bool equal(NodeContainer other) const {
//        auto other_ptr = dynamic_cast<FunctionNode*>(other);
//        if (other_ptr == nullptr) return false;
//        return *this == *other_ptr;
return false;
    }

    bool operator==(FunctionNode& other) const {
        return false;
//        if (this->identifier != other.identifier) return false;
//        if (this->parameter_types.size() != other.parameter_types.size()) return false;
//        if (this->parameter_names.size() != other.parameter_names.size()) return false;
//        for (int i = 0; i < this->parameter_types.size(); i++) {
//            if (!this->parameter_types[i]->equal(other.parameter_types[i])) return false;
//        }
//        for (int i = 0; i < this->parameter_names.size(); i++) {
//            if (this->parameter_names[i] != other.parameter_names[i]) return false;
//        }
//        if (*this->body != *other.body) return false;
//        return true;
    }

    ~FunctionNode() {
//        for (auto pt: this->parameter_types) {
//            delete pt;
//        }
//        delete this->body;
//        delete this->return_type;
    }

    json to_json() const override {
        json j;
        j["node"] = "function";
//        j["identifier"] = this->identifier;
//        j["parameter_names"] = {};
//        j["parameter_types"] = {};
//        for (auto pn: this->parameter_names) {
//            j["parameter_names"].push_back(pn);
//        }
//        for (auto pt: this->parameter_types) {
//            j["parameter_types"].push_back(pt->to_json());
//        }
//        j["body"] = this->body->to_json();
//        j["return_type"] = this->return_type->to_json();
        return j;
    }

    std::map<std::string, std::string> constraints;
};


#endif //FUNCTIONNODE_H
