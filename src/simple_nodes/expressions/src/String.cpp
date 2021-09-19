//
// Created by chris on 5/4/21.
//

#include "../include/String.h"
using namespace sem;
String::String(const std::string& s) : Exp(ExpType::STRING), s(s) {
}
