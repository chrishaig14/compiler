//
// Created by chris on 6/8/20.
//

#ifndef CLASSLITERALFIELDNODE_H
#define CLASSLITERALFIELDNODE_H


#include "Node.h"

#include "TypeNode.h"

class ClassLiteralFieldNode : public Node {
public:
    ObjectType* type;

    bool equal(const Node& other) const override;

    ~ClassLiteralFieldNode();

    VectorOfStrings init_names;
    VectorOfNodes init_values;

    ClassLiteralFieldNode(ObjectType* type, const std::unordered_map<std::string, Node*>& init);
    ClassLiteralFieldNode& clsfld() override;
    const ClassLiteralFieldNode& clsfld() const override;

};


#endif //CLASSLITERALFIELDNODE_H
