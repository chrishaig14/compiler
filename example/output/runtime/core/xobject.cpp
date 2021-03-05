//
// Created by chris on 17/12/20.
//

#include "xobject.h"
#include "core.h"
#include "xobjects/XString.h"
#include "xobjects/XFile.h"
#include "xobjects/XDict.h"
#include "xobjects/XTuple.h"

int hash(TaggedObject* n) {
    if (has_tag(n, INT_TAG)) {
        return PTR_TO_INT(n);
    }
    XTuple2* t = dynamic_cast<XTuple2*>(UNTAG(n));
    int h = 0;

    if (t != nullptr) {
        h = hash(t->get_member(1));
        h += hash(t->get_member(2));
    } else {
        XString* s = CAST(n, XString);
        const std::string& str = s->s;
        for (int i = 0; i < str.size(); i++) {
            h += int(str[i]);
        }
    }
    return h;
}

TaggedObject* f_File_read_line(TaggedObject* o) {
    XFile* f = (XFile*) UNTAG(o);
    std::string line;
    std::getline(f->f, line);
    return NEW(XString, line);
}

