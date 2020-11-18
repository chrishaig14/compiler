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
#include "Node.h"

class FunctionNode : public Node {
public:
    std::string identifier;
    std::vector<std::string> parameter_names;
    VectorOfTypes parameter_types;
    BlockNode* body;
    TypeNode* return_type;
    std::map<std::string, int> free_variables;


    void accept(Visitor& visitor) override;

    FunctionNode(std::string identifier, std::vector<std::string> parameter_names,
                 VectorOfTypes parameter_types, TypeNode* return_type, BlockNode* body)
            : body(body), return_type(return_type) {
        this->identifier = identifier;
        this->parameter_names = parameter_names;
        this->parameter_types = parameter_types;
    }


    bool equal(const Node& x) const override ;

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
