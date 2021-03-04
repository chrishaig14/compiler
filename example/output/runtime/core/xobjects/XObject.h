//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XOBJECT_H
#define XLANG_XOBJECT_H

#include <string>
#include <vector>

class XObject {
private:
    bool reachable;
public:
    bool inserted;
    bool is_list;
    bool is_user;
    bool is_string;
    std::string class_name;

    XObject(std::string class_name);

    void set_reachable();

    void reset_reachable();

    bool is_reachable();

    virtual ~XObject();

    virtual void mark(std::vector<XObject*>& new_root) = 0;

};

#endif //XLANG_XOBJECT_H
