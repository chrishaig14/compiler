//
// Created by chris on 1/8/20.
//

#ifndef STRUCTNODE_H
#define STRUCTNODE_H


#include <string>
#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "DeclarationNode.h"
#include "FunctionNode.h"
#include "../utils.h"

class StructNode : public Node {
public:
    std::string identifier;
    std::vector<std::string> template_parameters;
    std::vector<std::pair<std::string, TypeNode*>> fields;

    StructNode(std::string identifier, std::vector<std::string> templateParameters,
               std::vector<std::pair<std::string, TypeNode*>> fields);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const {
//        auto other_ptr = dynamic_cast<StructNode*>(other);
//        if (other_ptr == nullptr) return false;
//        return *this == *other_ptr;
return false;
    }

    bool operator==(StructNode& other) const {
        return false;
//        if (this->identifier != other.identifier) return false;
//        if (this->template_parameters.size() != other.template_parameters.size()) return false;
//        if (this->fields.size() != other.fields.size()) return false;
//        for (int i = 0; i < this->template_parameters.size(); i++) {
//            if (this->template_parameters[i] != other.template_parameters[i]) return false;
//        }
//        for (int i = 0; i < this->fields.size(); i++) {
//            if (this->fields[i].first != other.fields[i].first) return false;
//            if (!this->fields[i].second->equal(other.fields[i].second)) return false;
//        }
//        return true;
    }

    ~StructNode() {
        for (auto f: this->fields) {
            delete f.second;
        }
    }

    json to_json() const override {
        json j;
        j["type"] = "class";
        j["fields"] = {};
        for (auto f: this->fields) {
//            j["fields"][f.first] = f.second->to_json();
        }
        return j;
    }

};


#endif //STRUCTNODE_H
