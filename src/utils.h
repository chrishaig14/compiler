//
// Created by chris on 2/8/20.
//

#ifndef UTILS_H
#define UTILS_H

//#include "instructions/Instruction.h"
//#include "nodes/nodes.h"
#include <vector>
#include <map>
#include <algorithm>
#include "types.h"
#include "instructions/Instruction.h"

std::ostream& operator<<(std::ostream& out, const CodeLabel v);

template<typename T>
bool item_in_vec(T item, std::vector<T> vec) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
}


#endif //UTILS_H
