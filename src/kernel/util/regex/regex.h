/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

/* DON'T EVEN THINK ABOUT EDITING THIS, go see regex/Makefile,
 * search for mkh */
#ifndef _REGEX_H_
#define _REGEX_H_ /* never again */
#include <util/coTypes.h>
/* ========= begin header generated by ./mkh ========= */
#ifdef __cplusplus
extern "C" {
#endif

/* === regex2.h === */

typedef off_t regoff_t;
typedef struct
{
    int re_magic;
    size_t re_nsub; /* number of parenthesized subexpressions */
    const char *re_endp; /* end pointer for REG_PEND */
    struct re_guts *re_g; /* none of your business :-) */
} regex_t;
typedef struct
{
    regoff_t rm_so; /* start of match */
    regoff_t rm_eo; /* end of match */
} regmatch_t;

/* === regcomp.c === */
int UTILEXPORT regcomp(regex_t *, const char *, int);
#define REG_BASIC 0000
#define REG_EXTENDED 0001
#define REG_ICASE 0002
#define REG_NOSUB 0004
#define REG_NEWLINE 0010
#define REG_NOSPEC 0020
#define REG_PEND 0040
#define REG_DUMP 0200

/* === regerror.c === */
#define REG_NOMATCH 1
#define REG_BADPAT 2
#define REG_ECOLLATE 3
#define REG_ECTYPE 4
#define REG_EESCAPE 5
#define REG_ESUBREG 6
#define REG_EBRACK 7
#define REG_EPAREN 8
#define REG_EBRACE 9
#define REG_BADBR 10
#define REG_ERANGE 11
#define REG_ESPACE 12
#define REG_BADRPT 13
#define REG_EMPTY 14
#define REG_ASSERT 15
#define REG_INVARG 16
#define REG_ATOI 255 /* convert name to number (!) */
#define REG_ITOA 0400 /* convert number to name (!) */
size_t UTILEXPORT regerror(int, const regex_t *, char *, size_t);

/* === regexec.c === */
int UTILEXPORT regexec(const regex_t *, const char *, size_t, regmatch_t[], int);
#define REG_NOTBOL 00001
#define REG_NOTEOL 00002
#define REG_STARTEND 00004
#define REG_TRACE 00400 /* tracing of execution */
#define REG_LARGE 01000 /* force large representation */
#define REG_BACKR 02000 /* force use of backref code */

/* === regfree.c === */
void UTILEXPORT regfree(regex_t *);

#ifdef __cplusplus
}
#endif
/* ========= end header generated by ./mkh ========= */
#endif
