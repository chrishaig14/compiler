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

    virtual ~AssignmentNode();

    json to_json() const override {
        json j;
        j["node"] = "assignment";
        j["lvalue"] = this->lvalue->to_json();
        j["rvalue"] = this->rvalue->to_json();
        return j;
    }

};


#endif //UNTITLED1_ASSIGNMENTNODE_H
