//
// Created by chris on 20/7/20.
//

#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <stack>
#include "CodeRunner.h"

class VirtualMachine {
    std::stack<CodeRunner> frame_stack;

};


#endif //VIRTUALMACHINE_H
