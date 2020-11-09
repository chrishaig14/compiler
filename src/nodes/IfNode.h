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
    NodeContainer then;
    NodeContainer selse;
    std::vector<std::pair<NodeContainer, NodeContainer>> elifs;

    IfNode(NodeContainer condition, NodeContainer then, const std::vector<std::pair<NodeContainer, NodeContainer>>& elifs, NodeContainer selse);

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
