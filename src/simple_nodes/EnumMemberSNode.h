//
// Created by chris on 1/5/21.
//

#ifndef XLANG_ENUMMEMBERSNODE_H
#define XLANG_ENUMMEMBERSNODE_H


#include <string>
#include "SNode.h"

class EnumMemberSNode : public SNode {
public:
    EnumMemberSNode(const std::string& enum_name, const std::string& value);

    std::string enum_name;
    std::string value;
};


#endif //XLANG_ENUMMEMBERSNODE_H
