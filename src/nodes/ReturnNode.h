//
// Created by chris on 1/8/20.
//

#ifndef RETURNNODE_H
#define RETURNNODE_H


#include "Node.h"
#include "Visitor.h"
#include "Node.h"
class ReturnNode : public Node {
public:
    explicit ReturnNode(Node* expression);

    void accept(Visitor& visitor) override;

    bool equal(const Node& other) const;

    ~ReturnNode() {
        delete this->expression;
    }

    json to_json() const override {
        json j;
        j["node"] = "return";
//        j["expression"] = this->expression->to_json();
        return j;
    }

    ReturnNode &retrn() override;

    const ReturnNode &retrn() const override;

    Node* expression;
};

#endif //RETURNNODE_H
