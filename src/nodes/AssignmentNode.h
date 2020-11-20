//
// Created by chris on 1/8/20.
//

#ifndef ASSIGNMENTNODE_H
#define ASSIGNMENTNODE_H


#include "Node.h"
#include "Visitor.h"
#include "Node.h"

class AssignmentNode : public Node {
public:
    Node* lvalue;
    Node* rvalue;

    AssignmentNode& assign() override { return *this; }

    const AssignmentNode& assign() const override { return *this; }

    AssignmentNode(Node* lvalue, Node* rvalue);

    bool equal(const Node& x) const override;


    ~AssignmentNode();

    json to_json() const override {
        json j;
        j["node"] = "assignment";
//        j["lvalue"] = this->lvalue->to_json();
//        j["rvalue"] = this->rvalue->to_json();
        return j;
    }

};


#endif //ASSIGNMENTNODE_H
