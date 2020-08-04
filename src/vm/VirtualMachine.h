//
// Created by chris on 20/7/20.
//

#ifndef UNTITLED1_VIRTUALMACHINE_H
#define UNTITLED1_VIRTUALMACHINE_H

#include <stack>
#include "CodeRunner.h"

class VirtualMachine {
    std::stack<CodeRunner> frame_stack;

};


#endif //UNTITLED1_VIRTUALMACHINE_H
