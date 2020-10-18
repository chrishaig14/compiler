//
// Created by chris on 4/10/20.
//

#ifndef UNTITLED1_CLASSNODE_H
#define UNTITLED1_CLASSNODE_H


#include "Node.h"
#include "Visitor.h"
#include "TypeNode.h"

class ClassNode : public Node {
public:
    ClassNode(const std::string& className, std::vector<std::string> type_parameters,
              std::map<std::string, TypeNode*> members, std::map<std::string, FunctionNode*> functions);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

    std::map<std::string, TypeNode*> members;
    std::vector<std::string> members_ordered;
    std::map<std::string, FunctionNode*> methods;
    std::string class_name;
    std::vector<std::string> type_parameters;
};


#endif //UNTITLED1_CLASSNODE_H
