//
// Created by chris on 23/3/21.
//

#ifndef XLANG_XDOUBLE_H
#define XLANG_XDOUBLE_H


#include "../xobject.h"

class XDouble : public XObject {
public:
    XDouble(double d);
    ~XDouble() override;
    TaggedObject* __eq__(TaggedObject* pObject) override;
    double d;
};


#endif //XLANG_XDOUBLE_H
