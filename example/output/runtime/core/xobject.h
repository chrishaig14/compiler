//
// Created by chris on 17/12/20.
//

#ifndef UNTITLED1_XOBJECT_H
#define UNTITLED1_XOBJECT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

#define TRUE_TAG 0b0010
#define FALSE_TAG 0b0100
#define INT_TAG 0b0110
#define OBJECT_TAG 0b1100
#define REACHABLE_FLAG 0b0001


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

inline XObject* UNTAG(XObject* l) {
    return (XObject*) ((unsigned long) l & 0xfffffffffffffff0);
}


inline XObject* PTR_TO_OBJ(XObject* x) {
    return (XObject*) ((unsigned long) x & 0xfffffffffffffff0);
}

inline XObject* TAG(XObject* s) { return (XObject*) ((unsigned long) s | OBJECT_TAG); }

inline bool has_tag(XObject* p, unsigned long tag) {
    return ((unsigned long) p & 0b1110) == tag;
}

inline bool is_reachable(XObject* p) {
    return ((unsigned long) p & 0b1);
}

inline void set_reachable(XObject** p) {
    *p = (XObject*) ((unsigned long) *p | 0b1);
}


inline XObject* INT_TO_PTR(long x) { return (XObject*) ((unsigned long) (x << 4) | INT_TAG); }

inline XObject* BOOL_TO_PTR(bool x) { return (XObject*) (unsigned long) (x ? TRUE_TAG : FALSE_TAG); }

inline bool PTR_TO_BOOL(XObject* x) { return (unsigned long) x == TRUE_TAG; }


inline long PTR_TO_INT(XObject* x) { return (long) x >> 4; }

class XList : public XObject {
public:
    std::vector<XObject*> l;

    XList(const std::vector<XObject*> v) : XObject("List") {
        this->l = v;
        this->is_list = true;
    }

    void mark(std::vector<XObject*>& new_root) override {
        int list_len = this->l.size();
        if (list_len != 0) {
            if (has_tag(this->l[0], INT_TAG)) {
                // don't gc ints as they are not heap-allocated
            } else {
                for (int j = 0; j < list_len; j++) {
                    XObject* element = PTR_TO_OBJ(this->l[j]);
                    if (!element->is_reachable() && !element->inserted) {
                        new_root.push_back(element);
                        element->inserted = true;
                    }
                }
            }
        }
    }
};

class XUserObject : public XObject {
public:
    std::unordered_map<std::string, XObject*> members;

    XUserObject(std::string class_name) : XObject(class_name) {
        this->is_user = true;
    }

    virtual ~XUserObject() {}

    virtual std::vector<XObject*> get_members() = 0;
    virtual XObject* get_member(const std::string& s) = 0;
    virtual void set_member(const std::string& s, XObject* v) = 0;

    virtual void mark(std::vector<XObject*>& new_root) override {
        for (auto& m: this->get_members()) {
            if (has_tag(m, OBJECT_TAG)) {
                XObject* element = UNTAG(m);
                if (!element->is_reachable() && !element->inserted) {
                    new_root.push_back(element);
                    element->inserted = true;
                }
            }
        }
    }
};

class XString : public XObject {
public:
    std::string s;

    XString(const std::string& x) : XObject("String") {
        this->s = x;
        this->is_string = true;
    }

    ~XString() {
        // std::cout << "Deleted String '" << this->s << "' (" << this << ")" << std::endl;
    }

    void mark(std::vector<XObject*>& x) override {}
};

inline XString* PTR_TO_STRING(XObject* l) {
    return (XString*) ((unsigned long) l & 0xfffffffffffffff0);
}

XObject* hash(XObject* n);


class XDict : public XObject {
public:
    std::unordered_map<XObject*, XObject*> l;

    XDict(const std::unordered_map<XObject*, XObject*>& v) : XObject("Dict") {
        for(auto& it: v){
            this->l[hash(it.first)] = it.second;
        }
        this->is_list = false;
    }

    void mark(std::vector<XObject*>& new_root) override {
        // int list_len = this->l.size();
        // if (list_len != 0) {
        //     if (has_tag(this->l[0], INT_TAG)) {
        //         // don't gc ints as they are not heap-allocated
        //     } else {
        //         for (int j = 0; j < list_len; j++) {
        //             XObject* element = PTR_TO_OBJ(this->l[j]);
        //             if (!element->is_reachable() && !element->inserted) {
        //                 new_root.push_back(element);
        //                 element->inserted = true;
        //             }
        //         }
        //     }
        // }
    }
};

#endif //UNTITLED1_XOBJECT_H