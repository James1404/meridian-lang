/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf src/meridian_keywords.gperf  */
/* Computed positions: -k'1-2' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 1 "src/meridian_keywords.gperf"

#include "meridian_tokens.h"
#line 11 "src/meridian_keywords.gperf"
struct Keyword {
    const char* name;
    int tt;
};
/* maximum key range = 22, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
Meridian_KeywordsHash (register const char *str, register size_t len)
{
  static unsigned char asso_values[] =
    {
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 10, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23,  0, 23,
      23,  5, 10, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23,  0, 23, 23,
       5,  0, 10, 23, 10,  5, 23, 23,  0,  0,
       0, 23, 23, 23,  5, 23,  5, 23, 23, 23,
      23,  0, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

struct Keyword *
Meridian_KeywordsLookup (register const char *str, register size_t len)
{
  enum
    {
      TOTAL_KEYWORDS = 15,
      MIN_WORD_LENGTH = 1,
      MAX_WORD_LENGTH = 5,
      MIN_HASH_VALUE = 1,
      MAX_HASH_VALUE = 22
    };

  static struct Keyword wordlist[] =
    {
      {""},
#line 31 "src/meridian_keywords.gperf"
      {":", TOKEN_COLON},
#line 32 "src/meridian_keywords.gperf"
      {"::", TOKEN_ANNOTATE},
#line 18 "src/meridian_keywords.gperf"
      {"let", TOKEN_LET},
#line 26 "src/meridian_keywords.gperf"
      {"else", TOKEN_ELSE},
#line 20 "src/meridian_keywords.gperf"
      {"match", TOKEN_MATCH},
#line 30 "src/meridian_keywords.gperf"
      {"=", TOKEN_ASSIGN},
#line 19 "src/meridian_keywords.gperf"
      {"in", TOKEN_IN},
#line 28 "src/meridian_keywords.gperf"
      {"def", TOKEN_DEF},
#line 27 "src/meridian_keywords.gperf"
      {"type", TOKEN_TYPE},
      {""}, {""},
#line 23 "src/meridian_keywords.gperf"
      {"fn", TOKEN_FN},
      {""},
#line 21 "src/meridian_keywords.gperf"
      {"true", TOKEN_TRUE},
#line 22 "src/meridian_keywords.gperf"
      {"false", TOKEN_FALSE},
      {""},
#line 24 "src/meridian_keywords.gperf"
      {"if", TOKEN_IF},
      {""},
#line 25 "src/meridian_keywords.gperf"
      {"then", TOKEN_THEN},
      {""}, {""},
#line 29 "src/meridian_keywords.gperf"
      {"->", TOKEN_ARROW}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = Meridian_KeywordsHash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
#line 33 "src/meridian_keywords.gperf"

TokenType Meridian_GetKeyword(String src, Token t) {
    struct Keyword* k = Meridian_KeywordsLookup(src.raw + t.start, t.len);

    if(!k) {
        return TOKEN_IDENT;
    }

    return (TokenType)k->tt;
}