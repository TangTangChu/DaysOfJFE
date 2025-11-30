#include "utils/Utf8Helper.h"

std::string Utf8Prefix(const std::string &s, size_t nChars) {
    size_t i = 0;
    size_t count = 0;
    while (i < s.size() && count < nChars) {
        unsigned char c = (unsigned char)s[i];
        size_t step = 1;
        if ((c & 0x80) == 0x00)
            step = 1; // 1-byte
        else if ((c & 0xE0) == 0xC0)
            step = 2; // 2-byte
        else if ((c & 0xF0) == 0xE0)
            step = 3; // 3-byte
        else if ((c & 0xF8) == 0xF0)
            step = 4; // 4-byte
        else
            step = 1; // fallback

        i += step;
        count++;
    }
    return s.substr(0, i);
}

size_t Utf8Length(const std::string &s) {
    size_t i = 0, count = 0;
    while (i < s.size()) {
        unsigned char c = (unsigned char)s[i];
        size_t step = 1;
        if ((c & 0x80) == 0x00)
            step = 1;
        else if ((c & 0xE0) == 0xC0)
            step = 2;
        else if ((c & 0xF0) == 0xE0)
            step = 3;
        else if ((c & 0xF8) == 0xF0)
            step = 4;
        i += step;
        count++;
    }
    return count;
}
