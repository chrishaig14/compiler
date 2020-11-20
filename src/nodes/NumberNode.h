//
// Created by chris on 1/8/20.
//

#ifndef NUMBERNODE_H
#define NUMBERNODE_H


#include "Node.h"
#include "Visitor.h"
#include "Node.h"

class NumberNode : public Node {
public:
    int num;

    NumberNode(int number);

    bool equal(const Node& x) const override;


    json to_json() const override {
        json j;
        j["node"] = "number";
        j["number"] = this->num;
        return j;
    }

    NumberNode& number() override;
    const NumberNode& number() const override;

};


#endif //NUMBERNODE_H
