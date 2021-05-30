//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XFILE_H
#define XLANG_XFILE_H

#include <fstream>
#include "XObject.h"
#include "../Function.h"
#include "XString.h"
#include "../macros.h"

class XFile : public XObject {
public:
    std::fstream f;
    std::ios_base::openmode omode;
    XFile(const std::string& filename, const std::string& mode);

};

extern Function1 core_D_file_D_File_D_eof_o;
extern Function2 core_D_file_D_File_D_open_o;
extern Function1 core_D_file_D_File_D_read_line_o;
extern Function2 core_D_file_D_File_D_write_o;

extern TaggedObject* core_D_file_D_File_D_eof;
extern TaggedObject* core_D_file_D_File_D_open;
extern TaggedObject* core_D_file_D_File_D_read_line;
extern TaggedObject* core_D_file_D_File_D_write;

TaggedObject* core_D_file_D_File_D_eof_f(TaggedObject* o);
TaggedObject* core_D_file_D_File_D_open_f(TaggedObject* o, TaggedObject* m);
TaggedObject* core_D_file_D_File_D_read_line_f(TaggedObject* o);
TaggedObject* core_D_file_D_File_D_write_f(TaggedObject* o, TaggedObject* w);


#endif //XLANG_XFILE_H
