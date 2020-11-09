//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_STRINGNODE_H
#define UNTITLED1_STRINGNODE_H


#include <string>
#include "Node.h"
#include "Visitor.h"
#include "NodeContainer.h"
class StringNode : public Node {
public:
    std::string str;

    void accept(Visitor& visitor) override;

    StringNode(std::string str);

    bool equal(NodeContainer other) const override;

    bool operator==(StringNode& other) const;

    json to_json() const override {
        json j;
        j["node"] = "string";
        j["str"] = this->str;
        return j;
    }
};


#endif //UNTITLED1_STRINGNODE_H
