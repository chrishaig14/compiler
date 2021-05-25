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

#define LIBP(a, b, c, d, e) a ## _v ## b ## _ ## c ## _ ##  d ## _ ##  e
#define LIB(a, b, c, d, e)  LIBP(a,b,c,d,e)
#define LIBENT(x) LIB(LIBNAME, LIBMAJV, LIBMINV, LIBPATV, x)

#define LIBNAME core
#define LIBMAJV 1
#define LIBMINV 0
#define LIBPATV 0

class LIBENT(Union) : public XObject {
public:
    TaggedObject* o;
    TaggedObject* type;

    LIBENT(Union)(TaggedObject* o, TaggedObject* type) : XObject("Union") {
        this->o = o;
        this->type = type;
    }
};

#endif //UNTITLED1_CORE_H
