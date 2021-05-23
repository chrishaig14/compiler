//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XOBJECT_H
#define XLANG_XOBJECT_H

#include <string>
#include <vector>

class TaggedObject;

class XObject {
public:
    bool is_list;
    bool is_string;
    unsigned long count;
    std::string class_name;

    XObject(std::string class_name);

    virtual ~XObject();

    virtual TaggedObject* __eq__(TaggedObject* pObject);

    inline void inc_count() {
        unsigned long tag = this->count & ((unsigned long) 1 << 63);
        this->count = tag + (this->count & ~((unsigned long) 1 << 63)) + 1;
    };

    inline void dec_count() {
        unsigned long tag = this->count & ((unsigned long) 1 << 63);
        this->count = tag + (this->count & ~((unsigned long) 1 << 63)) - 1;
    };
};

#endif //XLANG_XOBJECT_H
