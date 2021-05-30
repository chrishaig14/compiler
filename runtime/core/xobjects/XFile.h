//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XFILE_H
#define XLANG_XFILE_H

#include <fstream>
#include "XObject.h"
#include "../Function.h"
#include "../xobjects/XString.h"
#include "../../macros.h"

class XFile : public XObject {
public:
    std::ifstream f;

    XFile(const std::string& filename);

};

extern TaggedObject* core_D_file_D_File_f_read_line;

extern Function1 core_D_file_D_File_f_read_line_o;

TaggedObject* core_D_file_D_File_f_read_line_f(TaggedObject* o);


#endif //XLANG_XFILE_H
