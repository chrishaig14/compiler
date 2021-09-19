//
// Created by chris on 19/9/21.
//

#ifndef XLANG_TOP_MODULE_H
#define XLANG_TOP_MODULE_H

#include "../../sem.h"

class sem::Module {
public:
    std::vector<std::unique_ptr<Top>> nodes;
};


#endif //XLANG_TOP_MODULE_H
