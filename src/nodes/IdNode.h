//
// Created by chris on 1/8/20.
//

#ifndef IDNODE_H
#define IDNODE_H


#include <string>
#include "Node.h"
#include "Visitor.h"
#include "NodeContainer.h"

class IdNode : public Node {
public:
    void accept(Visitor& visitor) override;

    std::string identifier;

    IdNode(std::string identifier);

    bool operator==(const IdNode& other) const;
    bool operator!=(const IdNode& other) const;;

    json to_json() const override {
        json j;
        j["node"] = "id";
        j["id"] = this->identifier;
        j["start"] = this->start;
        j["end"] = this->end;
        return j;
    }
};


#endif //IDNODE_H
