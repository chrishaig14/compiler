//
// Created by chris on 1/8/20.
//

#ifndef LISTNODE_H
#define LISTNODE_H


#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "NodeContainer.h"
class ListNode : public Node {
public:
    std::vector<NodeContainer> elements;

    void accept(Visitor& visitor) override;

    ListNode(std::vector<NodeContainer> elements);

    bool equal(NodeContainer other) const override;

    bool operator==(const ListNode& other) const;
    bool operator!=(const ListNode& other) const;

    ~ListNode() {
//        for (auto e: this->elements) {
//            delete e;
//        }
    }

    json to_json() const override {
        json j;
        j["node"] = "list";
        j["elements"] = {};
//        for (auto e: this->elements) {
//            j["elements"].push_back(e->to_json());
//        }
        return j;
    }
};


#endif //LISTNODE_H
