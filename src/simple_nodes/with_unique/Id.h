//
// Created by chris on 4/4/21.
//

#ifndef XLANG_ID_H
#define XLANG_ID_H

#include <string>
#include "../SNode.h"

class sem::Id : public sem::SNode {
public:
    std::string identifier;

    explicit Id(const std::string& identifier);
    bool equals(const SNode& o) const override;
};


#endif //XLANG_ID_H
