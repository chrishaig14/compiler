//
// Created by chris on 3/3/21.
//

#include "XFile.h"

Function1 function_File_read_line_p = Function1(f_File_read_line);

XFile::XFile(const std::string& filename) : XObject("File") {
    f.open(filename);
}

void XFile::mark(std::vector<XObject*>& new_root) {

}

TaggedObject* f_File_read_line(TaggedObject* o) {
    XFile* f = (XFile*) UNTAG(o);
    std::string line;
    std::getline(f->f, line);
    return NEW(XString, line);
}
