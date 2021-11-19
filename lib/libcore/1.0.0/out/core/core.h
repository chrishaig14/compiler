//
// Created by chris on 5/12/20.
//

#ifndef UNTITLED1_CORE_H
#define UNTITLED1_CORE_H

#include <iostream>
#include <map>
#include <assert.h>
#include "xobjects/XString.h"
#include "xobjects/Integer.h"
#include "xobjects/Boolean.h"
#include "xobjects/XList.h"
#include "xobjects/XDict.h"
#include "xobjects/XFile.h"
#include "xobjects/XObject.h"
#include "xobjects/XTuple.h"
#include "Function.h"
#include "GC.h"
#include "macros.h"
#include "basics.h"
#include "functions.h"

class core_D_core_D_Union : public XObject {
public:
    TaggedObject* o;
    TaggedObject* type;

    core_D_core_D_Union(TaggedObject* o, TaggedObject* type) : XObject("Union") {
        this->o = o;
        this->type = type;
    }
};

#endif //UNTITLED1_CORE_H
