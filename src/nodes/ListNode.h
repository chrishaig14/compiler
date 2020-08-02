//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_LISTNODE_H
#define UNTITLED1_LISTNODE_H


#include <vector>
#include "Node.h"
#include "Visitor.h"

class ListNode : public Node {
public:
    std::vector<Node*> elements;

    void accept(Visitor& visitor) override;

    ListNode(std::vector<Node*> elements);

    bool equal(Node* other) const override;

    bool operator==(ListNode& other) const;

    ~ListNode() {
        for (auto e: this->elements) {
            delete e;
        }
    }

    json to_json() const override {
        json j;
        j["node"] = "list";
        j["elements"] = {};
        for (auto e: this->elements) {
            j["elements"].push_back(e->to_json());
        }
        return j;
    }
};


#endif //UNTITLED1_LISTNODE_H
