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
    int count;
    std::string class_name;

    XObject(std::string class_name);

    virtual ~XObject();

    virtual TaggedObject* __eq__(TaggedObject* pObject);
    inline void inc_count(){this->count++;};
    inline void dec_count(){this->count--;};
};

#endif //XLANG_XOBJECT_H
