//
// Created by chris on 4/4/21.
//

#include <iostream>
#include "Integer.h"
using namespace sem;
Integer::Integer(const std::string& str) : SNode(SNodeType::INTEGER), str(str) {
}
