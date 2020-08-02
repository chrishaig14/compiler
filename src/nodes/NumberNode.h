//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_NUMBERNODE_H
#define UNTITLED1_NUMBERNODE_H


#include "Node.h"
#include "Visitor.h"

class NumberNode : public Node {
public:
    int number;

    NumberNode(int number);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

    bool operator==(NumberNode& other) const;

    json to_json() const override {
        json j;
        j["node"] = "number";
        j["number"] = this->number;
        return j;
    }
};


#endif //UNTITLED1_NUMBERNODE_H
