//
// Created by chris on 1/8/20.
//

#ifndef CALLNODE_H
#define CALLNODE_H


#include <vector>
#include "Node.h"

#include "Node.h"
#include "../types.h"
class CallNode : public Node {
public:
    Node* function;
    VectorOfNodes arguments;

    CallNode(Node* function, const VectorOfNodes& arguments);

    bool equal(const Node& other) const override ;

    ~CallNode() {
        delete this->function;
        for (auto a: this->arguments) {
            delete a;
        }
    }

    json to_json() const override {
        json j;
        j["node"] = "call";
//        j["function"] = this->function->to_json();
        j["arguments"] = {};
        for (auto a: this->arguments) {
//            j["arguments"].push_back(a->to_json());
        }
        return j;
    }

    CallNode& call() override;
    const CallNode& call() const override;

};


#endif //CALLNODE_H
