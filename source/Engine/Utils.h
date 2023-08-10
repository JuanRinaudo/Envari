#ifndef UTILS_H
#define UTILS_H

static i32   Stricmp(const char* str1, const char* str2)         { i32 d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
static i32   Strnicmp(const char* str1, const char* str2, i32 n) { i32 d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
static bool  Strstrn(const char* haystack, const char* needle, i32 n) {
    i32 hi = 0;
    i32 ni = 0;
    size_t nlen = strlen(needle);
    while (hi < n)
    {
        if(ni == nlen) { return true; }
        if(haystack[hi] == needle[ni]) { ni++; } else { ni = 0; }
        hi++;
    }
    return false;
}
static char* Strdup(const char *str)                             { size_t len = strlen(str) + 1; void* buf = malloc(len); Assert(buf); return (char*)memcpy(buf, (const void*)str, len); }
static char* Strdup(const char *str, size_t *len)                { *len = strlen(str) + 1; void* buf = malloc(*len); Assert(buf); return (char*)memcpy(buf, (const void*)str, *len); }
static void  Strtrim(char* str)                                  { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

#endif