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

    XObject(std::string class_name) {
        this->class_name = class_name;
        this->is_list = false;
        this->inserted = false;
        this->reachable = false;
        this->is_string = false;
        this->is_user = false;
    }

    void set_reachable() {
        // std::cout << "Set object " << this << " (" << this->class_name << ") reachable" << std::endl;
        this->reachable = true;
    }

    void reset_reachable() {
        // std::cout << "Reset object " << this << " (" << this->class_name << ") reachable" << std::endl;
        this->reachable = false;
    }

    bool is_reachable() {
        return this->reachable;
    }

    virtual ~XObject() {
        // std::cout << "Deleted object " << this << " (" << this->class_name << ")" << std::endl;
    }

    virtual void mark(std::vector<XObject*>& new_root) = 0;

};

#endif //XLANG_XOBJECT_H
