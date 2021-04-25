//
// Created by chris on 5/12/20.
//

#include <ctime>
#include <bitset>
#include <cstring>
#include <sstream>
#include "core.h"
#include "xobjects/XString.h"
#include "xobjects/XFile.h"
#include "xobjects/XList.h"
#include "xobjects/XDict.h"
#include "xobjects/XTuple.h"


void foo(float f) {
    std::cout << f << std::endl;
}


TaggedObject* core_D_dict_subscript_f(TaggedObject* _l, TaggedObject* i) {
    XDict* dict = (XDict*) UNTAG(_l);
    int h = hash(i);
    if (dict->l.find(h) == dict->l.end()) {
        throw std::runtime_error("DictKeyError: key not found");
    }
    return dict->l.at(h);
}

TaggedObject* core_D_list_subscript_f(TaggedObject* _l, TaggedObject* i) {
    XList* list = (XList*) UNTAG(_l);
    unsigned long index = GET_INT(i);
    if (index >= list->l->size()) {
        throw std::runtime_error("List index out of range");
    }
    return (*list->l)[index];
}

TaggedObject* core_D_string_subscript_f(TaggedObject* _l, TaggedObject* i) {
    XString* str = (XString*) UNTAG(_l);
    unsigned long index = GET_INT(i);
    std::cout << "String is: " << str->s << std::endl;
    if (index >= str->s.size()) {
        throw std::runtime_error("String index out of range: " + std::to_string(index) + " but length is " +
                                 std::to_string(str->s.size()));
    }
    return NEW(XString, std::string(1, str->s[index]));
}


DEFINE_FUNCTION(2, core_D_String_D___sub__)


TaggedObject* core_D_List_D_len_f(TaggedObject* a) {
    return MAKE_INT(CAST(a, XList)->lv.size());
}

TaggedObject* core_D_List_D___sub___f(TaggedObject* a, TaggedObject* b) {
    long idx = GET_INT(b);
    unsigned long list_len = CAST(a, XList)->lv.size();
    if (idx >= list_len) {
        throw std::runtime_error("List index " + std::to_string(idx) +
                                 (" out of range of list with length " + std::to_string(list_len)));
    }
    return CAST(a, XList)->lv[idx];
}


DEFINE_FUNCTION(2, core_D_dict_subscript)

