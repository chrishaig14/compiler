//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XOBJECT_H
#define XLANG_XOBJECT_H

#include <string>
#include <vector>

class TaggedObject;

#define GC_TAGS ((unsigned long)0b111<<61)
#define GC_RETURN ((unsigned long)1 << 63)

inline unsigned long get_tag_value(unsigned long gc_info, unsigned long tag) {
    return gc_info & tag;
}

inline unsigned long set_tag_value(unsigned long gc_info, unsigned long tag) {
    return gc_info | tag;
}

inline unsigned long clear_tag_value(unsigned long gc_info, unsigned long tag) {
    return gc_info & ~tag;
}

inline unsigned long get_count_value(unsigned long gc_info) {
    return gc_info & ~GC_TAGS;
}

inline unsigned long inc_count_value(unsigned long gc_info) {
    unsigned long tag = gc_info & GC_TAGS;
    return tag + (gc_info & ~GC_TAGS) + 1;
}

inline unsigned long dec_count_value(unsigned long gc_info) {
    unsigned long tag = gc_info & GC_TAGS;
    return tag + (gc_info & ~GC_TAGS) - 1;
}

class XObject {
public:
    bool is_list;
    bool is_string;
    unsigned long gc_info;
    std::string class_name;

    XObject(std::string class_name);

    virtual ~XObject();

    virtual TaggedObject* __eq__(TaggedObject* pObject);

    inline void inc_count() {
        this->gc_info = inc_count_value(this->gc_info);
    };

    inline void dec_count() {
        this->gc_info = dec_count_value(this->gc_info);
    };
};

#endif //XLANG_XOBJECT_H
