//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_RETURNNODE_H
#define UNTITLED1_RETURNNODE_H


#include "Node.h"
#include "Visitor.h"

class ReturnNode : public Node {
public:
    explicit ReturnNode(Node* expression);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

    bool operator==(ReturnNode& other) const;

    ~ReturnNode() {
        delete this->expression;
    }

    json to_json() const override {
        json j;
        j["node"] = "return";
        j["expression"] = this->expression->to_json();
        return j;
    }

    Node* expression;
};

#endif //UNTITLED1_RETURNNODE_H
