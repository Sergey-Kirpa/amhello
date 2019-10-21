#include <config.h>
#include <stdio.h>
#include <string.h>

/**
 * Returns the length of UTF-8 code points including the first byte,
 * or 0 if it is an invalid code point.
 *
 * \param b0 the first byte of UTF-8 code points.
 */
static inline int
utf8CodePointsLength(const char b0)
{
    if ((b0 & 0x80) == 0)
        return 1;
    if ((b0 & 0xC0) != 0xC0)
      return 0; // invalid code point
    if ((b0 & 0xE0) == 0xC0)
        return 2;
    if ((b0 & 0xF0) == 0xE0)
        return 3;
    if ((b0 & 0xF8) == 0xF0)
        return 4;
    return 0; // invalid code point
}

/**
 * Utility routine to tell whether a sequence of bytes is valid UTF-8.
 * This must be called with the length pre-determined by the first byte.
 * If presented with a length > 4, this returns false.  The Unicode
 * definition of UTF-8 goes up to 4-byte code points.
 */
static bool
isValidUtf8(const unsigned char* source, int length)
{
    unsigned char a;
    const unsigned char* srcptr = source + length;
    switch (length) {
        default: return false;
        // Everything else falls through when "true"...
        case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
        case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
        case 2: if ((a = (*--srcptr)) > 0xBF) return false;


        switch (*source) {
            // no fall-through in this inner switch
            case 0xE0: if (a < 0xA0) return false; break;
            case 0xED: if (a > 0x9F) return false; break;
            case 0xF0: if (a < 0x90) return false; break;
            case 0xF4: if (a > 0x8F) return false; break;
            default:   if (a < 0x80) return false; break;
        }


        case 1: if (*source >= 0x80 && *source < 0xC2) return false;
    }
    if (*source > 0xF4)
        return false;
    return true;
}

/**
 * Returns whether the given input is a valid UTF-8 encoding.
 */
static bool
isValidUtf8String(const char *source, const char *sourceEnd) {
    while (source < sourceEnd) {
        const auto length = utf8CodePointsLength(*source);
        if (length > sourceEnd - source || !isValidUtf8(reinterpret_cast<const unsigned char*>(source), length))
            return false;
        source += length;
    }
    return true; // including zero-length input
}

int
main (void)
{
  puts ("Hello World!");
  puts ("This is " PACKAGE_STRING ".");

  const char *text = "test";
  
  isValidUtf8String(text, text + strlen(text));

  return 0;
}
