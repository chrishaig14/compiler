//
// Created by chris on 4/10/20.
//

#ifndef CLASSNODE_H
#define CLASSNODE_H


#include "Node.h"
#include "Visitor.h"
#include "TypeNode.h"

class ClassNode : public Node {
public:
    ClassNode(const std::string& className, std::vector<std::string> type_parameters,
              std::map<std::string, NodeContainer> members, std::map<std::string, NodeContainer> functions);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

    std::map<std::string, NodeContainer> members;
    std::vector<std::string> members_ordered;
    std::map<std::string, NodeContainer> methods;
    std::string class_name;
    std::vector<std::string> type_parameters;
};


#endif //CLASSNODE_H
