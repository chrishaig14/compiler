//
// Created by chris on 6/10/20.
//

#include "TypeClassInfo.h"

bool TypeClassInfo::has_function(std::string function_name) {
    return this->functions.find(function_name) != this->functions.end();
}
