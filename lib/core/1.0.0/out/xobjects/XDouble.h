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

extern TaggedObject* m_core_c_Double_f_str;
extern Function1 m_core_c_Double_f_str_o;


#endif //XLANG_XDOUBLE_H
