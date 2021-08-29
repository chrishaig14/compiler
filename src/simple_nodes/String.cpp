//
// Created by chris on 5/4/21.
//

#include "String.h"
using namespace sem;
String::String(const std::string& s) : SNode(SNodeType::STRING), s(s) {
}
