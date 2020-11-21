//
// Created by chris on 4/10/20.
//

#ifndef CLASSNODE_H
#define CLASSNODE_H


#include "Node.h"

#include "TypeNode.h"
#include "FunctionNode.h"

class ClassNode : public Node {
public:

    bool equal(const Node& other) const;
    ClassNode(const std::string& className, std::vector<std::string> type_parameters,
              std::map<std::string, TypeNode*> members, std::map<std::string, FunctionNode*> functions);
    ClassNode& cls() override;
    const ClassNode& cls() const override;

    std::map<std::string, TypeNode*> members;
    std::vector<std::string> members_ordered;
    std::map<std::string, FunctionNode*> methods;
    std::string class_name;
    std::vector<std::string> type_parameters;
};


#endif //CLASSNODE_H
