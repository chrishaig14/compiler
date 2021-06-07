//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XOBJECT_H
#define XLANG_XOBJECT_H

#include <string>
#include <vector>

class TaggedObject;

struct gc_info_t {
    bool RETURN: 1;
    bool UNUSED_TAG1: 1;
    bool UNUSED_TAG2: 1;
    bool UNUSED_TAG3: 1;
    unsigned long count: 60;
};

class XObject {
public:
    bool is_list;
    bool is_string;
    gc_info_t gc_info;
    std::string class_name;

    XObject(std::string class_name);

    virtual ~XObject();

    virtual TaggedObject* __eq__(TaggedObject* pObject);

    inline void inc_count() {
        this->gc_info.count++;
    };

    inline void dec_count() {
        this->gc_info.count--;
    };
};

#endif //XLANG_XOBJECT_H
