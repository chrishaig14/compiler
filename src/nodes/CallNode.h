//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_CALLNODE_H
#define UNTITLED1_CALLNODE_H


#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "NodeContainer.h"
class CallNode : public Node {
public:
    NodeContainer function;
    std::vector<NodeContainer> arguments;

    CallNode(NodeContainer function, const std::vector<NodeContainer>& arguments);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

    bool operator==(CallNode& other) const;

    ~CallNode() {
//        delete this->function;
        for (auto a: this->arguments) {
//            delete a;
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

};


#endif //UNTITLED1_CALLNODE_H
