//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XLIST_H
#define XLANG_XLIST_H

#include "XObject.h"
#include "../xobject.h"
#include "../functions.h"
#include "../../macros.h"
#include "../GC.h"

class XList : public XObject {
public:
    std::vector<XObject*> l;

    XList(const std::vector<XObject*> v);

    void mark(std::vector<XObject*>& new_root) override;
};

XObject* f_List_add(XObject* _a, XObject* _b);
XObject* f_List_len(XObject* _l);
extern Function2* function_List_add;
extern Function1* function_List_len;
extern Function2 function_List_add_p;
extern Function1 function_List_len_p;


#endif //XLANG_XLIST_H
