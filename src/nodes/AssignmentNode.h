//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_ASSIGNMENTNODE_H
#define UNTITLED1_ASSIGNMENTNODE_H


#include "Node.h"
#include "Visitor.h"

class AssignmentNode : public Node {
public:
    Node* lvalue;
    Node* rvalue;

    AssignmentNode(Node* lvalue, Node* rvalue);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

    bool operator==(AssignmentNode& other) const;

};


#endif //UNTITLED1_ASSIGNMENTNODE_H
