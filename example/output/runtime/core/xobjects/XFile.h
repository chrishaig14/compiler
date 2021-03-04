//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XFILE_H
#define XLANG_XFILE_H

#include <fstream>
#include "XObject.h"
#include "../functions.h"

class XFile : public XObject {
public:
    std::ifstream f;

    XFile(const std::string& filename);

    void mark(std::vector<XObject*>& new_root) override;
};

extern Function1* function_File_read_line;
extern Function1 function_File_read_line_p;


#endif //XLANG_XFILE_H
