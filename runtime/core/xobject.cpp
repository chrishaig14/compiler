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
        return GET_INT(n);
    }
    core_D_Tuple2* t = dynamic_cast<core_D_Tuple2*>(UNTAG(n));
    int h = 0;

    if (t != nullptr) {
        h = hash(t->mem_1);
        h += hash(t->mem_2);
    } else {
        XString* s = CAST(n, XString);
        const std::string& str = s->s;
        for (size_t i = 0; i < str.size(); i++) {
            h = (h + int(str[i])) << 5;
        }
    }
    return h;
}

TaggedObject* TRUE = (TaggedObject*) (unsigned long) TRUE_TAG;
TaggedObject* FALSE = (TaggedObject*) (unsigned long) FALSE_TAG;