//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_IFNODE_H
#define UNTITLED1_IFNODE_H


#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "BlockNode.h"

class IfNode : public Node {
public:
    Node* condition;
    BlockNode* then;
    BlockNode* selse;
    std::vector<std::pair<Node*, BlockNode*>> elifs;

    IfNode(Node* condition, BlockNode* then, const std::vector<std::pair<Node*, BlockNode*>>& elifs, BlockNode* selse);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

    bool operator==(IfNode& other) const;

    ~IfNode() {
        delete this->condition;
        delete this->then;
        delete this->selse;
    }

    json to_json() const override {
        json j;
        j["node"] = "if";
        j["condition"] = this->condition->to_json();
        j["then"] = this->then->to_json();
        return j;
    }
};


#endif //UNTITLED1_IFNODE_H
