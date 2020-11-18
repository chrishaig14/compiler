//
// Created by chris on 1/8/20.
//

#ifndef IFNODE_H
#define IFNODE_H


#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "BlockNode.h"
#include "Node.h"

class IfNode : public Node {
public:
    Node* condition;
    BlockNode* then;
    BlockNode* selse;
    std::vector<std::pair<Node*, BlockNode*>> elifs;

    IfNode(Node* condition, BlockNode* then, std::vector<std::pair<Node*, BlockNode*>> elifs, BlockNode* selse);

    void accept(Visitor& visitor) override;

    bool equal(const Node& other) const override ;

    ~IfNode() {
    }

    json to_json() const override {
        json j;
        j["node"] = "if";
//        j["condition"] = this->condition->to_json();
//        j["then"] = this->then->to_json();
        return j;
    }
};


#endif //IFNODE_H
