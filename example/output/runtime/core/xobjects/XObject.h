//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XOBJECT_H
#define XLANG_XOBJECT_H

#include <string>
#include <vector>

class TaggedObject;

class XObject {
private:
    bool reachable;
public:
    bool inserted;
    bool is_list;
    bool is_user;
    bool is_string;
    int count;
    std::string class_name;

    XObject(std::string class_name);

    void set_reachable();

    void reset_reachable();

    bool is_reachable();

    virtual ~XObject();

    virtual void mark(std::vector<XObject*>& new_root) = 0;

    virtual TaggedObject* __eq__(TaggedObject* pObject);
    virtual void inc_count();
    virtual void dec_count();
};

TaggedObject* setr(TaggedObject* x);
#endif //XLANG_XOBJECT_H
