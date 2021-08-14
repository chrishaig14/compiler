//
// Created by chris on 1/8/20.
//

#ifndef ASSIGNMENTNODE_H
#define ASSIGNMENTNODE_H


#include "Node.h"

#include "Node.h"
#include "TypeNode.h"

class AssignmentNode : public Node {
public:
    Node* lvalue;
    Node* rvalue;


    AssignmentNode(Node* lvalue, Node* rvalue, TextPosition start, TextPosition end);

    bool equal(const Node& x) const override;


    ~AssignmentNode();
    nlohmann::json to_json() const override;

};


#endif //ASSIGNMENTNODE_H
