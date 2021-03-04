//
// Created by chris on 4/3/21.
//

#ifndef XLANG_TUPLE_H
#define XLANG_TUPLE_H

#include "XObject.h"

class Tuple : public XObject {
public:

    std::vector<XObject*> members;

    Tuple(const std::string& n, int num);

    virtual XObject* get_member(int i) = 0;

    virtual void mark(std::vector<XObject*>& new_root) override;
};

class Tuple2 : public Tuple {
public:

    Tuple2(XObject* mem_1, XObject* mem_2);
    XObject* get_member(int i) override;
};

class Tuple3 : public Tuple {
public:

    Tuple3(XObject* mem_1, XObject* mem_2, XObject* mem_3);

    XObject* get_member(int i) override;
};

#endif //XLANG_TUPLE_H
