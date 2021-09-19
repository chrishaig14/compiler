//
// Created by chris on 4/4/21.
//

#ifndef XLANG_SNODE_H
#define XLANG_SNODE_H

#include <memory>
#include "../../sem.h"



class sem::SNode {
public:
    SNodeType type;
    explicit SNode(SNodeType type);

    virtual bool equals(const SNode& o) const = 0;

    bool operator!=(const SNode& other) const;

    bool operator==(const SNode& other) const;

    virtual ~SNode() = default;
};

#endif //XLANG_SNODE_H
