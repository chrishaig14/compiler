//
// Created by chris on 1/8/20.
//

#ifndef IFNODE_H
#define IFNODE_H


#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "BlockNode.h"
#include "NodeContainer.h"
class IfNode : public Node {
public:
    NodeContainer condition;
    BlockNode& then;
    BlockNode selse;
    std::vector<std::pair<NodeContainer, std::reference_wrapper<BlockNode>>> elifs;

    IfNode(NodeContainer condition, BlockNode& then, const std::vector<std::pair<NodeContainer, std::reference_wrapper<BlockNode>>>& elifs, BlockNode selse);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

    bool operator==(IfNode& other) const;

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
