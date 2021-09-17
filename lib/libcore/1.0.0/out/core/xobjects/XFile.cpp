//
// Created by chris on 3/3/21.
//

#include "XFile.h"
#include <map>

DEFINE_FUNCTION(1, core_D_file_D_File_D_eof)
DEFINE_FUNCTION(2, core_D_file_D_File_D_open)
DEFINE_FUNCTION(1, core_D_file_D_File_D_read_line)
DEFINE_FUNCTION(2, core_D_file_D_File_D_write)

XFile::XFile(const std::string& filename, const std::string& mode) : XObject("File") {
    std::map<std::string, std::ios_base::openmode> map_openmode;
    map_openmode["r"] = std::ios_base::in;
    map_openmode["w"] = std::ios_base::out | std::ios_base::trunc;
    map_openmode["a"] = std::ios_base::out | std::ios_base::app;
    map_openmode["r+"] = std::ios_base::in | std::ios_base::out;
    map_openmode["w+"] = std::ios_base::in | std::ios_base::out | std::ios_base::trunc;
    map_openmode["a+"] = std::ios_base::in | std::ios_base::out | std::ios_base::app;
    this->omode = map_openmode.at(mode);
    f.open(filename, omode);
    if (!f.is_open()) {
        throw std::runtime_error("Couldn't open file: " + filename);
    }
}


TaggedObject* core_D_file_D_File_D_eof_f(TaggedObject* o) {
    XFile* f = CAST(o, XFile);
    TaggedObject* b = MAKE_BOOL(f->f.peek() == std::ifstream::traits_type::eof());
    f->f.clear();
    return b;
}

TaggedObject* core_D_file_D_File_D_open_f(TaggedObject* o, TaggedObject* mode) {
    TaggedObject* new_file = NEW(XFile, CAST(o, XString)->s, CAST(mode, XString)->s);
    XFile* f = CAST(GC::declare(new_file), XFile);
    return new_file;
}

TaggedObject* core_D_file_D_File_D_write_f(TaggedObject* o, TaggedObject* w) {
    XFile* f = CAST(o, XFile);
    const std::string& s = CAST(w, XString)->s;
    std::cout << "S: " << s << std::endl;
    std::cout << "file: " << f->f.is_open() << std::endl;
    f->f << s;
    f->f.flush();
    return nullptr;
}

TaggedObject* core_D_file_D_File_D_read_line_f(TaggedObject* o) {
    XFile* f = CAST(o, XFile);
    std::string line;
    if (!(f->omode & std::ios_base::in)) {
        throw std::runtime_error("Cannot read on this file");
    }
    std::getline(f->f, line);
    return NEW(XString, line);
}
