//
// Created by chris on 4/10/20.
//

#ifndef UNTITLED1_CLASSNODE_H
#define UNTITLED1_CLASSNODE_H


#include "Node.h"
#include "Visitor.h"
#include "TypeNode.h"

class ClassNode : public Node {
    std::map<std::string, FunctionTypeNode*> functions;
    std::string class_name;
    std::string type_name;
public:
    ClassNode(const std::string& className, const std::string& typeName,
              const std::map<std::string, FunctionTypeNode*>& functions);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

};


#endif //UNTITLED1_CLASSNODE_H
