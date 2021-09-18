//
// Created by chris on 1/5/21.
//

#ifndef XLANG_ENUMMEMBER_H
#define XLANG_ENUMMEMBER_H


#include <string>
#include "../common/SNode.h"

class sem::EnumMember : public sem::SNode {
public:
    EnumMember(const std::string& enum_name, const std::string& value);
    bool equals(const SNode& o) const override;

    std::string enum_name;
    std::string value;
};


#endif //XLANG_ENUMMEMBER_H
