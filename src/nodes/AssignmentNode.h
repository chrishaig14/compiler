//
// Created by chris on 1/8/20.
//

#ifndef ASSIGNMENTNODE_H
#define ASSIGNMENTNODE_H


#include "Node.h"
#include "Visitor.h"
#include "NodeContainer.h"

class AssignmentNode : public Node {
public:
    NodeContainer lvalue;
    NodeContainer rvalue;

    AssignmentNode(NodeContainer lvalue, NodeContainer rvalue);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

    bool operator==(AssignmentNode& other) const;

    virtual ~AssignmentNode();

    json to_json() const override {
        json j;
        j["node"] = "assignment";
//        j["lvalue"] = this->lvalue->to_json();
//        j["rvalue"] = this->rvalue->to_json();
        return j;
    }

};


#endif //ASSIGNMENTNODE_H
