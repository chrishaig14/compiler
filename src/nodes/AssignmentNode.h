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
    std::unique_ptr<Node> lvalue;
    std::unique_ptr<Node> rvalue;


    AssignmentNode(std::unique_ptr<Node>& lvalue, std::unique_ptr<Node>& rvalue, TextPosition start, TextPosition end);

    bool equal(const Node& x) const override;


    ~AssignmentNode();
    nlohmann::json to_json() const override;

};


#endif //ASSIGNMENTNODE_H
