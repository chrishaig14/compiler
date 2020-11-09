//
// Created by chris on 4/10/20.
//

#ifndef INSTANCENODE_H
#define INSTANCENODE_H


#include "Node.h"
#include "FunctionNode.h"

class InstanceNode : public Node {
public:
    InstanceNode(const std::string& className, const std::string& typeName,
                 const std::vector<FunctionNode*>& functions);

private:
    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

    std::string class_name;
    std::string type_name;
    std::vector<FunctionNode*> functions;

};


#endif //INSTANCENODE_H
