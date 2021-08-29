//
// Created by chris on 4/4/21.
//

#ifndef XLANG_ASSIGNMENT_H
#define XLANG_ASSIGNMENT_H

#include "SNode.h"

class Assignment : public SNode {
public:
    USNode lvalue;
    USNode rvalue;
    Assignment(USNode& lvalue, USNode& rvalue);
};


#endif //XLANG_ASSIGNMENT_H
