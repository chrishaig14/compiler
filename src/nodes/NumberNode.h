//
// Created by chris on 1/8/20.
//

#ifndef NUMBERNODE_H
#define NUMBERNODE_H


#include "Node.h"
#include "Visitor.h"
#include "NodeContainer.h"

class NumberNode : public Node {
public:
    int number;

    NumberNode(int number);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

    bool operator==(const NumberNode& other) const;
    bool operator!=(const NumberNode& other) const;

    json to_json() const override {
        json j;
        j["node"] = "number";
        j["number"] = this->number;
        return j;
    }
};


#endif //NUMBERNODE_H
