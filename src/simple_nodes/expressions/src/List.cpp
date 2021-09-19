//
// Created by chris on 12/4/21.
//

#include "../include/List.h"
using namespace sem;
List::List(std::vector<UExp> elements) : Exp(ExpType::LIST), elements(std::move(elements)) {
}
