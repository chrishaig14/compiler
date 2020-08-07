//
// Created by chris on 6/8/20.
//

#ifndef UNTITLED1_CLASSLITERALFIELDNODE_H
#define UNTITLED1_CLASSLITERALFIELDNODE_H


#include "Node.h"
#include "Visitor.h"
class ClassLiteralFieldNode : public Node {
public:
    std::string identifier;
    std::map<std::string, Node*> init;

    ClassLiteralFieldNode(const std::string& identifier, const std::map<std::string, Node*>& init);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

};


#endif //UNTITLED1_CLASSLITERALFIELDNODE_H
