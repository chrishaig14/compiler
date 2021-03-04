//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XFILE_H
#define XLANG_XFILE_H

#include <fstream>
#include "XObject.h"

class XFile : public XObject {
public:
    std::ifstream f;

    XFile(const std::string& filename) : XObject("File") {
        f.open(filename);
    }

    void mark(std::vector<XObject*>& new_root) override {

    }
};

#endif //XLANG_XFILE_H
