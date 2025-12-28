#ifndef _MUS_SV_H
#define _MUS_SV_H

#include <string.h>
#include <ctype.h>

typedef struct {
    const char* string;
    size_t from;
    size_t until;
} MUS_StringView;

static inline MUS_StringView MUS_sv_until_delim(MUS_StringView* sv, char delim) {
    size_t i = 0;
    while (sv->from + i < sv->until && sv->string[sv->from + i] != delim) ++i;
    MUS_StringView result = {sv->string, sv->from, sv->from + i};
    sv->from = sv->from + i + 1;
    return result;
}
static inline void MUS_sv_trim_left(MUS_StringView* sv, size_t n){if (n > sv->until-sv->from) n = sv->until-sv->from; sv->from+=n;}
static inline void MUS_sv_trim_right(MUS_StringView* sv, size_t n){if (n > sv->until-sv->from) n = sv->until-sv->from; sv->until-=n;}
static inline void MUS_sv_strip_left(MUS_StringView* sv) {while (sv->from < sv->until && isspace(sv->string[sv->from])) ++sv->from;}
static inline void MUS_sv_strip_right(MUS_StringView* sv) {while (sv->from < sv->until && isspace(sv->string[sv->until - 1])) --sv->until;}
static inline void MUS_sv_strip(MUS_StringView* sv) {MUS_sv_strip_left(sv); MUS_sv_strip_right(sv);}
static inline bool MUS_sv_eq(MUS_StringView* a, MUS_StringView* b) {
    size_t len_a = a->until - a->from;
    size_t len_b = b->until - b->from;
    if (len_a != len_b) return false;
    return memcmp(a->string + a->from, b->string + b->from, len_a) == 0;
}
static inline bool MUS_sv_ends_with(MUS_StringView* sv, char* cstr) {
    size_t sv_len = sv->until - sv->from;
    size_t cstr_len = strlen(cstr);
    if (cstr_len > sv_len) return false;
    return memcmp(sv->string + sv->until - cstr_len, cstr, cstr_len) == 0;
}
static inline bool MUS_sv_starts_with(MUS_StringView* sv, char* cstr) {
    size_t sv_len = sv->until - sv->from;
    size_t cstr_len = strlen(cstr);
    if (cstr_len > sv_len) return false;
    return memcmp(sv->string + sv->from, cstr, cstr_len) == 0;
}
static inline void MUS_sv_to_cstr(MUS_StringView* sv, char* cstr){
    size_t sv_len = sv->until - sv->from;
    memcpy(cstr, sv->string+sv->from, sv_len);
    cstr[sv_len]='\0';
}
static inline MUS_StringView MUS_sv_from_cstr(const char* cstr) {return (MUS_StringView){cstr, 0, strlen(cstr)};}
static inline MUS_StringView MUS_sv_from_parts(const char* cstr, size_t from, size_t until) {return (MUS_StringView){cstr, from, until};}

#define MUS_sv_len(sv) ((sv)->until - (sv)->from)
#define MUS_SV_Fmt "%.*s"
#define MUS_SV_Arg(sv) (int)MUS_sv_len(sv), (sv)->string + (sv)->from

#endif //_MUS_SV_H

#ifndef MUS_SV_STRIP_PREFIX_GUARD_
#define MUS_SV_STRIP_PREFIX_GUARD_
    #ifndef MUS_DONT_STRIP_PREFIX
        #define StringView MUS_StringView
        #define sv_until_delim MUS_sv_until_delim
        #define sv_trim_left MUS_sv_trim_left
        #define sv_trim_right MUS_sv_trim_right
        #define sv_strip_left MUS_sv_strip_left
        #define sv_strip_right MUS_sv_strip_right
        #define sv_strip MUS_sv_strip
        #define sv_eq MUS_sv_eq
        #define sv_ends_with MUS_sv_ends_with
        #define sv_starts_with MUS_sv_starts_with
        #define sv_to_cstr MUS_sv_to_cstr
        #define sv_from_cstr MUS_sv_from_cstr
        #define sv_from_parts MUS_sv_from_parts
        #define sv_len MUS_sv_len
        #define SV_Fmt MUS_SV_Fmt
        #define SV_Arg MUS_SV_Arg
    #endif
#endif // MUS_SV_STRIP_PREFIX_GUARD_