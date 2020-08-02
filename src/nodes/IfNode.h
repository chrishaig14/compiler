//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_IFNODE_H
#define UNTITLED1_IFNODE_H


#include <vector>
#include "Node.h"
#include "Visitor.h"

class IfNode : public Node {
public:
    Node* condition;
    std::vector<Node*> then;

    IfNode(Node* condition, std::vector<Node*> then);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

    bool operator==(IfNode& other) const;

    ~IfNode() {
        delete this->condition;
        for (auto st: this->then) {
            delete st;
        }
    }

    json to_json() const override {
        json j;
        j["node"] = "if";
        j["condition"] = this->condition->to_json();
        j["then"] = {};
        for (auto st: this->then) {
            j["then"].push_back(st->to_json());
        }
        return j;
    }
};


#endif //UNTITLED1_IFNODE_H
