//
// Created by chris on 1/8/20.
//

#ifndef LISTNODE_H
#define LISTNODE_H


#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "Node.h"
#include "../types.h"

class ListNode : public Node {
public:
    VectorOfNodes elements;

    void accept(Visitor& visitor) override;

    ListNode(VectorOfNodes elements);

    bool equal(const Node& other) const;

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

    ListNode& lst() override;
    const ListNode& lst() const override;
};


#endif //LISTNODE_H
