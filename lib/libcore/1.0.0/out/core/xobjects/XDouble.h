//
// Created by chris on 23/3/21.
//

#ifndef XLANG_XDOUBLE_H
#define XLANG_XDOUBLE_H


#include "XObject.h"
#include "../Function.h"

class XDouble : public XObject {
public:
    XDouble(double d);
    ~XDouble() override;
    TaggedObject* __eq__(TaggedObject* pObject) override;
    double d;
};
extern Function1 core_D_core_D_Double_D_str_o;
extern TaggedObject* core_D_core_D_Double_D_str;

TaggedObject* core_D_core_D_Double_D_str_f(TaggedObject* _i);

extern Function2 core_D_core_D_Double_D___sub___o;
extern TaggedObject* core_D_core_D_Double_D___sub__;

TaggedObject* core_D_core_D_Double_D___sub___f(TaggedObject* a, TaggedObject* b);

#endif //XLANG_XDOUBLE_H
