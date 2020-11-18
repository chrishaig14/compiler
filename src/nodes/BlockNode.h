//
// Created by chris on 2/8/20.
//

#ifndef BLOCKNODE_H
#define BLOCKNODE_H


#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "Node.h"
#include "../types.h"

class BlockNode : public Node {
public:

    BlockNode(VectorOfNodes nodes) : nodes(nodes) { this->ntype = BLOCK; }

    VectorOfNodes nodes;

    bool equal(const Node& p) const override{
        auto& other = p.block();
        if (this->nodes.size() != other.nodes.size()) {
            return false;
        }
        for (int i = 0; i < this->nodes.size(); i++) {
            if (*this->nodes[i] != *other.nodes[i]) {
                return false;
            }
        };
        return true;
    }

    void accept(Visitor& visitor) override;

    json to_json() {
        json j;
        j["node"] = "block";
        j["block"] = {};
        for (auto n: this->nodes) {
//            j["block"].push_back(n->to_json());
        }
        return j;
    }

    BlockNode& block() override;
    const BlockNode& block() const override;
};


#endif //BLOCKNODE_H
