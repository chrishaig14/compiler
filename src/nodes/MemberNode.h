//
// Created by chris on 1/8/20.
//

#ifndef MEMBERNODE_H
#define MEMBERNODE_H


#include <string>
#include "Node.h"
#include "Visitor.h"
#include "NodeContainer.h"
class MemberNode : public Node {
public:
    NodeContainer parent;
    std::string child;

    MemberNode(NodeContainer parent, const std::string& child);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const {
//        auto other_ptr = dynamic_cast<MemberNode*>(other);
//        if (other_ptr == nullptr) return false;
//        return *this == *other_ptr;
return false;
    }

    bool operator==(MemberNode& other) const {
//        return this->parent->equal(other.parent) && this->child == other.child;
return false;
    }

    ~MemberNode() {
//        delete this->parent;
    }

    json to_json() const override {
        json j;
        j["node"] = "member";
//        j["parent"] = this->parent->to_json();
        j["child"] = this->child;
        return j;
    }
};


#endif //MEMBERNODE_H
