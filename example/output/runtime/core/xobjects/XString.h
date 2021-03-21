//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XSTRING_H
#define XLANG_XSTRING_H

#include "XObject.h"
#include "../functions.h"
#include "../GC.h"

class XString : public XObject {
public:
    std::string s;

    XString(const std::string& x);

    ~XString();

    TaggedObject* __eq__(TaggedObject* other) override;
};

extern TaggedObject* function_String_add;
extern TaggedObject* function_String_len;
extern TaggedObject* function_String_eq;
extern Function2 function_String_add_p;
XObject* f_String_add(XObject* _a, XObject* _b);


#endif //XLANG_XSTRING_H
