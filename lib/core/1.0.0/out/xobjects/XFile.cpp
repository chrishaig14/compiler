//
// Created by chris on 3/3/21.
//

#include "XFile.h"

Function1 m_core_c_File_f_read_line_o = Function1(m_core_c_File_f_read_line_f);

XFile::XFile(const std::string& filename) : XObject("File") {
    f.open(filename);
}

TaggedObject* m_core_c_File_f_read_line_f(TaggedObject* o) {
    XFile* f = (XFile*) UNTAG(o);
    std::string line;
    std::getline(f->f, line);
    return NEW(XString, line);
}
