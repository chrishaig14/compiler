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

    void mark(std::vector<XObject*>& x) override;
};

extern Function2* function_String_add;
extern Function1* function_String_len;
extern Function2 function_String_add_p;
XObject* f_String_add(XObject* _a, XObject* _b);


#endif //XLANG_XSTRING_H
