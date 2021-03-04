//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XSTRING_H
#define XLANG_XSTRING_H

#include "XObject.h"

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

#endif //XLANG_XSTRING_H
