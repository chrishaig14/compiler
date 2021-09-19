//
// Created by chris on 9/5/21.
//

#include "../include/Dict.h"

using namespace sem;

Dict::Dict(std::vector<std::pair<UExp, UExp>> items) : Exp(ExpType::DICT), items(std::move(items)) {
}
