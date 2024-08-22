#if 0

/*========================================================*/
/* == ./libnbcompat/fnmatch.c == */
/*	$NetBSD: fnmatch.c,v 1.3 2004/08/23 03:32:12 jlam Exp $	*/

/*
 * Copyright (c) 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Guido van Rossum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <nbcompat.h>
#include <nbcompat/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char sccsid[] = "@(#)fnmatch.c  8.2 (Berkeley) 4/16/94";
#else
__RCSID("$NetBSD: fnmatch.c,v 1.3 2004/08/23 03:32:12 jlam Exp $");
#endif
#endif /* LIBC_SCCS and not lint */

/*
 * Function fnmatch() as specified in POSIX 1003.2-1992, section B.6.
 * Compares a filename or pathname to a pattern.
 */

#if 0
#include "namespace.h"
#endif

#include <nbcompat/assert.h>
#include <nbcompat/ctype.h>
#include <nbcompat/fnmatch.h>
#include <nbcompat/string.h>

#define	EOS	'\0'

static const char *rangematch(const char *, int, int);

static int
foldcase(int ch, int flags)
{

	if ((flags & FNM_CASEFOLD) != 0 && isupper(ch))
		return (tolower(ch));
	return (ch);
}

#define	FOLDCASE(ch, flags)	foldcase((unsigned char)(ch), (flags))

int
fnmatch(pattern, string, flags)
	const char *pattern, *string;
	int flags;
{
	const char *stringstart;
	char c, test;

	_DIAGASSERT(pattern != NULL);
	_DIAGASSERT(string != NULL);

	for (stringstart = string;;)
		switch (c = FOLDCASE(*pattern++, flags)) {
		case EOS:
			if ((flags & FNM_LEADING_DIR) && *string == '/')
				return (0);
			return (*string == EOS ? 0 : FNM_NOMATCH);
		case '?':
			if (*string == EOS)
				return (FNM_NOMATCH);
			if (*string == '/' && (flags & FNM_PATHNAME))
				return (FNM_NOMATCH);
			if (*string == '.' && (flags & FNM_PERIOD) &&
			    (string == stringstart ||
			    ((flags & FNM_PATHNAME) && *(string - 1) == '/')))
				return (FNM_NOMATCH);
			++string;
			break;
		case '*':
			c = FOLDCASE(*pattern, flags);
			/* Collapse multiple stars. */
			while (c == '*')
				c = FOLDCASE(*++pattern, flags);

			if (*string == '.' && (flags & FNM_PERIOD) &&
			    (string == stringstart ||
			    ((flags & FNM_PATHNAME) && *(string - 1) == '/')))
				return (FNM_NOMATCH);

			/* Optimize for pattern with * at end or before /. */
			if (c == EOS) {
				if (flags & FNM_PATHNAME)
					return ((flags & FNM_LEADING_DIR) ||
					    strchr(string, '/') == NULL ?
					    0 : FNM_NOMATCH);
				else
					return (0);
			} else if (c == '/' && flags & FNM_PATHNAME) {
				if ((string = strchr(string, '/')) == NULL)
					return (FNM_NOMATCH);
				break;
			}

			/* General case, use recursion. */
			while ((test = FOLDCASE(*string, flags)) != EOS) {
				if (!fnmatch(pattern, string,
					     flags & ~FNM_PERIOD))
					return (0);
				if (test == '/' && flags & FNM_PATHNAME)
					break;
				++string;
			}
			return (FNM_NOMATCH);
		case '[':
			if (*string == EOS)
				return (FNM_NOMATCH);
			if (*string == '/' && flags & FNM_PATHNAME)
				return (FNM_NOMATCH);
			if ((pattern =
			    rangematch(pattern, FOLDCASE(*string, flags),
				       flags)) == NULL)
				return (FNM_NOMATCH);
			++string;
			break;
		case '\\':
			if (!(flags & FNM_NOESCAPE)) {
				if ((c = FOLDCASE(*pattern++, flags)) == EOS) {
					c = '\\';
					--pattern;
				}
			}
			/* FALLTHROUGH */
		default:
			if (c != FOLDCASE(*string++, flags))
				return (FNM_NOMATCH);
			break;
		}
	/* NOTREACHED */
}

static const char *
rangematch(pattern, test, flags)
	const char *pattern;
	int test, flags;
{
	int negate, ok;
	char c, c2;

	_DIAGASSERT(pattern != NULL);

	/*
	 * A bracket expression starting with an unquoted circumflex
	 * character produces unspecified results (IEEE 1003.2-1992,
	 * 3.13.2).  This implementation treats it like '!', for
	 * consistency with the regular expression syntax.
	 * J.T. Conklin (conklin@ngai.kaleida.com)
	 */
	if ((negate = (*pattern == '!' || *pattern == '^')) != 0)
		++pattern;
	
	for (ok = 0; (c = FOLDCASE(*pattern++, flags)) != ']';) {
		if (c == '\\' && !(flags & FNM_NOESCAPE))
			c = FOLDCASE(*pattern++, flags);
		if (c == EOS)
			return (NULL);
		if (*pattern == '-' 
		    && (c2 = FOLDCASE(*(pattern+1), flags)) != EOS &&
		        c2 != ']') {
			pattern += 2;
			if (c2 == '\\' && !(flags & FNM_NOESCAPE))
				c2 = FOLDCASE(*pattern++, flags);
			if (c2 == EOS)
				return (NULL);
			if (c <= test && test <= c2)
				ok = 1;
		} else if (c == test)
			ok = 1;
	}
	return (ok == negate ? NULL : pattern);
}

/*========================================================*/
/* == ./libnbcompat/nbcompat/fnmatch.h == */
/*	$NetBSD: fnmatch.h,v 1.2 2004/08/10 18:47:55 jlam Exp $	*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)fnmatch.h	8.1 (Berkeley) 6/2/93
 */

#ifndef	_NBCOMPAT_FNMATCH_H_
#define	_NBCOMPAT_FNMATCH_H_

#if HAVE_FNMATCH_H
# include <fnmatch.h>
#endif

#if !HAVE_FNMATCH
#define	FNM_NOMATCH	1	/* Match failed. */
#define	FNM_NOSYS	2	/* Function not implemented. */

#define	FNM_NOESCAPE	0x01	/* Disable backslash escaping. */
#define	FNM_PATHNAME	0x02	/* Slash must be matched by slash. */
#define	FNM_PERIOD	0x04	/* Period must be matched by period. */
#define	FNM_CASEFOLD	0x08	/* Pattern is matched case-insensitive */
#define	FNM_LEADING_DIR	0x10	/* Ignore /<tail> after Imatch. */

int	 fnmatch(const char *, const char *, int);
#endif

#endif /* !_NBCOMPAT_FNMATCH_H_ */

/*========================================================*/
/* == ./musl/include/fnmatch.h == */
#ifndef	_FNMATCH_H
#define	_FNMATCH_H

#ifdef __cplusplus
extern "C" {
#endif

#define	FNM_PATHNAME 0x1
#define	FNM_NOESCAPE 0x2
#define	FNM_PERIOD   0x4
#define	FNM_LEADING_DIR	0x8           
#define	FNM_CASEFOLD	0x10
#define	FNM_FILE_NAME	FNM_PATHNAME

#define	FNM_NOMATCH 1
#define FNM_NOSYS   (-1)

int fnmatch(const char *, const char *, int);

#ifdef __cplusplus
}
#endif

#endif

/*========================================================*/
/* == ./musl/src/regex/fnmatch.c == */
/*
 * An implementation of what I call the "Sea of Stars" algorithm for
 * POSIX fnmatch(). The basic idea is that we factor the pattern into
 * a head component (which we match first and can reject without ever
 * measuring the length of the string), an optional tail component
 * (which only exists if the pattern contains at least one star), and
 * an optional "sea of stars", a set of star-separated components
 * between the head and tail. After the head and tail matches have
 * been removed from the input string, the components in the "sea of
 * stars" are matched sequentially by searching for their first
 * occurrence past the end of the previous match.
 *
 * - Rich Felker, April 2012
 */

#include <string.h>
#include <fnmatch.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>

#define END 0
#define UNMATCHABLE -2
#define BRACKET -3
#define QUESTION -4
#define STAR -5

static int str_next(const char *str, size_t n, size_t *step)
{
	if (!n) {
		*step = 0;
		return 0;
	}
	if (str[0] >= 128U) {
		wchar_t wc;
		int k = mbtowc(&wc, str, n);
		if (k<0) {
			*step = 1;
			return -1;
		}
		*step = k;
		return wc;
	}
	*step = 1;
	return str[0];
}

static int pat_next(const char *pat, size_t m, size_t *step, int flags)
{
	int esc = 0;
	if (!m || !*pat) {
		*step = 0;
		return END;
	}
	*step = 1;
	if (pat[0]=='\\' && pat[1] && !(flags & FNM_NOESCAPE)) {
		*step = 2;
		pat++;
		esc = 1;
		goto escaped;
	}
	if (pat[0]=='[') {
		size_t k = 1;
		if (k<m) if (pat[k] == '^' || pat[k] == '!') k++;
		if (k<m) if (pat[k] == ']') k++;
		for (; k<m && pat[k] && pat[k]!=']'; k++) {
			if (k+1<m && pat[k+1] && pat[k]=='[' && (pat[k+1]==':' || pat[k+1]=='.' || pat[k+1]=='=')) {
				int z = pat[k+1];
				k+=2;
				if (k<m && pat[k]) k++;
				while (k<m && pat[k] && (pat[k-1]!=z || pat[k]!=']')) k++;
				if (k==m || !pat[k]) break;
			}
		}
		if (k==m || !pat[k]) {
			*step = 1;
			return '[';
		}
		*step = k+1;
		return BRACKET;
	}
	if (pat[0] == '*')
		return STAR;
	if (pat[0] == '?')
		return QUESTION;
escaped:
	if (pat[0] >= 128U) {
		wchar_t wc;
		int k = mbtowc(&wc, pat, m);
		if (k<0) {
			*step = 0;
			return UNMATCHABLE;
		}
		*step = k + esc;
		return wc;
	}
	return pat[0];
}

static int match_bracket(const char *p, int k)
{
	wchar_t wc;
	int inv = 0;
	p++;
	if (*p=='^' || *p=='!') {
		inv = 1;
		p++;
	}
	if (*p==']') {
		if (k==']') return !inv;
		p++;
	} else if (*p=='-') {
		if (k=='-') return !inv;
		p++;
	}
	wc = p[-1];
	for (; *p != ']'; p++) {
		if (p[0]=='-' && p[1]!=']') {
			wchar_t wc2;
			int l = mbtowc(&wc2, p+1, 4);
			if (l < 0) return 0;
			if (wc<=wc2 && (unsigned)k-wc <= wc2-wc) return !inv;
			p += l-1;
			continue;
		}
		if (p[0]=='[' && (p[1]==':' || p[1]=='.' || p[1]=='=')) {
			const char *p0 = p+2;
			int z = p[1];
			p+=3;
			while (p[-1]!=z || p[0]!=']') p++;
			if (z == ':' && p-1-p0 < 16) {
				char buf[16];
				memcpy(buf, p0, p-1-p0);
				buf[p-1-p0] = 0;
				if (iswctype(k, wctype(buf))) return !inv;
			}
			continue;
		}
		if (*p < 128U) {
			wc = (unsigned char)*p;
		} else {
			int l = mbtowc(&wc, p, 4);
			if (l < 0) return 0;
			p += l-1;
		}
		if (wc==k) return !inv;
	}
	return inv;
}

static int fnmatch_internal(const char *pat, size_t m, const char *str, size_t n, int flags)
{
	const char *p, *ptail, *endpat;
	const char *s, *stail, *endstr;
	size_t pinc, sinc, tailcnt=0;
	int c, k;

	if (flags & FNM_PERIOD) {
		if (*str == '.' && *pat != '.')
			return FNM_NOMATCH;
	}
	for (;;) {
		switch ((c = pat_next(pat, m, &pinc, flags))) {
		case UNMATCHABLE:
			return FNM_NOMATCH;
		case STAR:
			pat++;
			m--;
			break;
		default:
			k = str_next(str, n, &sinc);
			if (k <= 0)
				return (c==END) ? 0 : FNM_NOMATCH;
			str += sinc;
			n -= sinc;
			if (c == BRACKET) {
				if (!match_bracket(pat, k))
					return FNM_NOMATCH;
			} else if (c != QUESTION && k != c) {
				return FNM_NOMATCH;
			}
			pat+=pinc;
			m-=pinc;
			continue;
		}
		break;
	}

	/* Compute real pat length if it was initially unknown/-1 */
	m = strnlen(pat, m);
	endpat = pat + m;

	/* Find the last * in pat and count chars needed after it */
	for (p=ptail=pat; p<endpat; p+=pinc) {
		switch (pat_next(p, endpat-p, &pinc, flags)) {
		case UNMATCHABLE:
			return FNM_NOMATCH;
		case STAR:
			tailcnt=0;
			ptail = p+1;
			break;
		default:
			tailcnt++;
			break;
		}
	}

	/* Past this point we need not check for UNMATCHABLE in pat,
	 * because all of pat has already been parsed once. */

	/* Compute real str length if it was initially unknown/-1 */
	n = strnlen(str, n);
	endstr = str + n;
	if (n < tailcnt) return FNM_NOMATCH;

	/* Find the final tailcnt chars of str, accounting for UTF-8.
	 * On illegal sequences we may get it wrong, but in that case
	 * we necessarily have a matching failure anyway. */
	for (s=endstr; s>str && tailcnt; tailcnt--) {
		if (s[-1] < 128U) s--;
		else while ((unsigned char)*--s-0x80U<0x40 && s>str);
	}
	if (tailcnt) return FNM_NOMATCH;
	stail = s;

	/* Check that the pat and str tails match */
	p = ptail;
	for (;;) {
		c = pat_next(p, endpat-p, &pinc, flags);
		p += pinc;
		if ((k = str_next(s, endstr-s, &sinc)) <= 0) {
			if (c != END) return FNM_NOMATCH;
			break;
		}
		s += sinc;
		if (c == BRACKET) {
			if (!match_bracket(p-pinc, k))
				return FNM_NOMATCH;
		} else if (c != QUESTION && k != c) {
			return FNM_NOMATCH;
		}
	}

	/* We're all done with the tails now, so throw them out */
	endstr = stail;
	endpat = ptail;

	/* Match pattern components until there are none left */
	while (pat<endpat) {
		p = pat;
		s = str;
		for (;;) {
			c = pat_next(p, endpat-p, &pinc, flags);
			p += pinc;
			/* Encountering * completes/commits a component */
			if (c == STAR) {
				pat = p;
				str = s;
				break;
			}
			k = str_next(s, endstr-s, &sinc);
			if (!k)
				return FNM_NOMATCH;
			if (c == BRACKET) {
				if (!match_bracket(p-pinc, k))
					break;
			} else if (c != QUESTION && k != c) {
				break;
			}
			s += sinc;
		}
		if (c == STAR) continue;
		/* If we failed, advance str, by 1 char if it's a valid
		 * char, or past all invalid bytes otherwise. */
		k = str_next(str, endstr-str, &sinc);
		if (k > 0) str += sinc;
		else for (str++; str_next(str, endstr-str, &sinc)<0; str++);
	}

	return 0;
}

int fnmatch(const char *pat, const char *str, int flags)
{
	const char *s, *p;
	size_t inc;
	int c;
	if (flags & FNM_PATHNAME) for (;;) {
		for (s=str; *s && *s!='/'; s++);
		for (p=pat; (c=pat_next(p, -1, &inc, flags))!=END && c!='/'; p+=inc);
		if (c!=*s && (!*s || !(flags & FNM_LEADING_DIR)))
			return FNM_NOMATCH;
		if (fnmatch_internal(pat, p-pat, str, s-str, flags))
			return FNM_NOMATCH;
		if (!c) return 0;
		str = s+1;
		pat = p+inc;
	} else if (flags & FNM_LEADING_DIR) {
		for (s=str; *s; s++) {
			if (*s != '/') continue;
			if (!fnmatch_internal(pat, -1, str, s-str, flags))
				return 0;
		}
	}
	return fnmatch_internal(pat, -1, str, -1, flags);
}

/*========================================================*/
/* == ./src/include/fnmatch.h == */
/*	$OpenBSD: fnmatch.h,v 1.8 2005/12/13 00:35:22 millert Exp $	*/
/*	$NetBSD: fnmatch.h,v 1.5 1994/10/26 00:55:53 cgd Exp $	*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)fnmatch.h	8.1 (Berkeley) 6/2/93
 */

#ifndef	_FNMATCH_H_
#define	_FNMATCH_H_

#include <sys/cdefs.h>

#define	FNM_NOMATCH	1	/* Match failed. */
#define	FNM_NOSYS	2	/* Function not supported (unused). */

#define	FNM_NOESCAPE	0x01	/* Disable backslash escaping. */
#define	FNM_PATHNAME	0x02	/* Slash must be matched by slash. */
#define	FNM_PERIOD	0x04	/* Period must be matched by period. */
#if __BSD_VISIBLE
#define	FNM_LEADING_DIR	0x08	/* Ignore /<tail> after Imatch. */
#define	FNM_CASEFOLD	0x10	/* Case insensitive search. */
#define	FNM_IGNORECASE	FNM_CASEFOLD
#define	FNM_FILE_NAME	FNM_PATHNAME
#endif

__BEGIN_DECLS
int	 fnmatch(const char *, const char *, int);
__END_DECLS

#endif /* !_FNMATCH_H_ */

/*========================================================*/
/* == ./src/lib/libc/gen/fnmatch.3 == */
/*
.\"	$OpenBSD: fnmatch.3,v 1.17 2022/07/30 07:19:30 jsg Exp $
.\"
.\" Copyright (c) 1989, 1991, 1993
.\"	The Regents of the University of California.  All rights reserved.
.\"
.\" This code is derived from software contributed to Berkeley by
.\" Guido van Rossum.
.\" Redistribution and use in source and binary forms, with or without
.\" modification, are permitted provided that the following conditions
.\" are met:
.\" 1. Redistributions of source code must retain the above copyright
.\"    notice, this list of conditions and the following disclaimer.
.\" 2. Redistributions in binary form must reproduce the above copyright
.\"    notice, this list of conditions and the following disclaimer in the
.\"    documentation and/or other materials provided with the distribution.
.\" 3. Neither the name of the University nor the names of its contributors
.\"    may be used to endorse or promote products derived from this software
.\"    without specific prior written permission.
.\"
.\" THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
.\" ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
.\" IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
.\" ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
.\" FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
.\" DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
.\" OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
.\" HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
.\" LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
.\" OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
.\" SUCH DAMAGE.
.\"
.\"     @(#)fnmatch.3	8.3 (Berkeley) 4/28/95
.\"
.Dd $Mdocdate: July 30 2022 $
.Dt FNMATCH 3
.Os
.Sh NAME
.Nm fnmatch
.Nd match filename or pathname using shell globbing rules
.Sh SYNOPSIS
.In fnmatch.h
.Ft int
.Fn fnmatch "const char *pattern" "const char *string" "int flags"
.Sh DESCRIPTION
The
.Fn fnmatch
function matches patterns according to the globbing rules used by the shell.
It checks the string specified by the
.Fa string
argument to see if it matches the pattern specified by the
.Fa pattern
argument.
.Pp
The
.Fa flags
argument modifies the interpretation of
.Fa pattern
and
.Fa string .
The value of
.Fa flags
is the bitwise inclusive
.Tn OR
of any of the following
constants, which are defined in the include file
.In fnmatch.h .
.Bl -tag -width FNM_PATHNAME
.It Dv FNM_NOESCAPE
Normally, every occurrence of a backslash
.Pq Sq \e
followed by a character in
.Fa pattern
is replaced by that character.
This is done to negate any special meaning for the character.
If the
.Dv FNM_NOESCAPE
flag is set, a backslash character is treated as an ordinary character.
.It Dv FNM_PATHNAME
Slash characters in
.Fa string
must be explicitly matched by slashes in
.Fa pattern .
If this flag is not set, then slashes are treated as regular characters.
.It Dv FNM_PERIOD
Leading periods in
.Fa string
must be explicitly matched by periods in
.Fa pattern .
If this flag is not set, then leading periods are treated as regular
characters.
The definition of
.Dq leading
is related to the specification of
.Dv FNM_PATHNAME .
A period is always leading
if it is the first character in
.Fa string .
Additionally, if
.Dv FNM_PATHNAME
is set,
a period is leading
if it immediately follows a slash.
.It Dv FNM_LEADING_DIR
Ignore
.Sq /*
rest after successful
.Fa pattern
matching.
.It Dv FNM_CASEFOLD
Ignore case distinctions in both the
.Fa pattern
and the
.Fa string .
.El
.Sh RETURN VALUES
The
.Fn fnmatch
function returns zero if
.Fa string
matches the pattern specified by
.Fa pattern ,
otherwise, it returns the value
.Dv FNM_NOMATCH .
.Sh SEE ALSO
.Xr sh 1 ,
.Xr glob 3 ,
.Xr regex 3 ,
.Xr glob 7
.Sh STANDARDS
The
.Fn fnmatch
function conforms to
.St -p1003.2-92
and
.St -xpg4.2 .
.Pp
Note, however, that the flags
.Dv FNM_LEADING_DIR
and
.Dv FNM_CASEFOLD
are extensions and should not be used by applications striving for
strict standards conformance.
.Sh HISTORY
A predecessor to
.Fn fnmatch ,
.Fn gmatch ,
first appeared in the Programmer's Workbench (PWB/UNIX).
The
.Fn fnmatch
function first appeared in
.Bx 4.3 Reno .
.Sh BUGS
The pattern
.Ql *
matches the empty string, even if
.Dv FNM_PATHNAME
is specified.
*/
/*========================================================*/
/* == ./src/lib/libc/gen/fnmatch.c == */
/*	$OpenBSD: fnmatch.c,v 1.23 2020/10/13 04:42:28 guenther Exp $	*/

/* Copyright (c) 2011, VMware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the VMware, Inc. nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL VMWARE, INC. OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 2008, 2016 Todd C. Miller <millert@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* Authored by William A. Rowe Jr. <wrowe; apache.org, vmware.com>, April 2011
 *
 * Derived from The Open Group Base Specifications Issue 7, IEEE Std 1003.1-2008
 * as described in;
 *   http://pubs.opengroup.org/onlinepubs/9699919799/functions/fnmatch.html
 *
 * Filename pattern matches defined in section 2.13, "Pattern Matching Notation"
 * from chapter 2. "Shell Command Language"
 *   http://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_13
 * where; 1. A bracket expression starting with an unquoted <circumflex> '^'
 * character CONTINUES to specify a non-matching list; 2. an explicit <period> '.'
 * in a bracket expression matching list, e.g. "[.abc]" does NOT match a leading
 * <period> in a filename; 3. a <left-square-bracket> '[' which does not introduce
 * a valid bracket expression is treated as an ordinary character; 4. a differing
 * number of consecutive slashes within pattern and string will NOT match;
 * 5. a trailing '\' in FNM_ESCAPE mode is treated as an ordinary '\' character.
 *
 * Bracket expansion defined in section 9.3.5, "RE Bracket Expression",
 * from chapter 9, "Regular Expressions"
 *   http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap09.html#tag_09_03_05
 * with no support for collating symbols, equivalence class expressions or
 * character class expressions.  A partial range expression with a leading
 * hyphen following a valid range expression will match only the ordinary
 * <hyphen> and the ending character (e.g. "[a-m-z]" will match characters
 * 'a' through 'm', a <hyphen> '-', or a 'z').
 *
 * Supports BSD extensions FNM_LEADING_DIR to match pattern to the end of one
 * path segment of string, and FNM_CASEFOLD to ignore alpha case.
 *
 * NOTE: Only POSIX/C single byte locales are correctly supported at this time.
 * Notably, non-POSIX locales with FNM_CASEFOLD produce undefined results,
 * particularly in ranges of mixed case (e.g. "[A-z]") or spanning alpha and
 * nonalpha characters within a range.
 *
 * XXX comments below indicate porting required for multi-byte character sets
 * and non-POSIX locale collation orders; requires mbr* APIs to track shift
 * state of pattern and string (rewinding pattern and string repeatedly).
 *
 * Certain parts of the code assume 0x00-0x3F are unique with any MBCS (e.g.
 * UTF-8, SHIFT-JIS, etc).  Any implementation allowing '\' as an alternate
 * path delimiter must be aware that 0x5C is NOT unique within SHIFT-JIS.
 */

#include <fnmatch.h>
#include <string.h>
#include <ctype.h>

#include "charclass.h"

#define	RANGE_MATCH	1
#define	RANGE_NOMATCH	0
#define	RANGE_ERROR	(-1)

static int
classmatch(const char *pattern, char test, int foldcase, const char **ep)
{
	const char * const mismatch = pattern;
	const char *colon;
	const struct cclass *cc;
	int rval = RANGE_NOMATCH;
	size_t len;

	if (pattern[0] != '[' || pattern[1] != ':') {
		*ep = mismatch;
		return RANGE_ERROR;
	}
	pattern += 2;

	if ((colon = strchr(pattern, ':')) == NULL || colon[1] != ']') {
		*ep = mismatch;
		return RANGE_ERROR;
	}
	*ep = colon + 2;
	len = (size_t)(colon - pattern);

	if (foldcase && strncmp(pattern, "upper:]", 7) == 0)
		pattern = "lower:]";
	for (cc = cclasses; cc->name != NULL; cc++) {
		if (!strncmp(pattern, cc->name, len) && cc->name[len] == '\0') {
			if (cc->isctype((unsigned char)test))
				rval = RANGE_MATCH;
			break;
		}
	}
	if (cc->name == NULL) {
		/* invalid character class, treat as normal text */
		*ep = mismatch;
		rval = RANGE_ERROR;
	}
	return rval;
}

/* Most MBCS/collation/case issues handled here.  Wildcard '*' is not handled.
 * EOS '\0' and the FNM_PATHNAME '/' delimiters are not advanced over,
 * however the "\/" sequence is advanced to '/'.
 *
 * Both pattern and string are **char to support pointer increment of arbitrary
 * multibyte characters for the given locale, in a later iteration of this code
 */
static int fnmatch_ch(const char **pattern, const char **string, int flags)
{
	const char * const mismatch = *pattern;
	const int nocase = !!(flags & FNM_CASEFOLD);
	const int escape = !(flags & FNM_NOESCAPE);
	const int slash = !!(flags & FNM_PATHNAME);
	int result = FNM_NOMATCH;
	const char *startch;
	int negate;

	if (**pattern == '[') {
		++*pattern;

		/* Handle negation, either leading ! or ^ operators */
		negate = (**pattern == '!') || (**pattern == '^');
		if (negate)
			++*pattern;

		/* ']' is an ordinary char at the start of the range pattern */
		if (**pattern == ']')
			goto leadingclosebrace;

		while (**pattern) {
			if (**pattern == ']') {
				++*pattern;
				/* XXX: Fix for MBCS character width */
				++*string;
				return (result ^ negate);
			}

			if (escape && (**pattern == '\\')) {
				++*pattern;

				/* Patterns must terminate with ']', not EOS */
				if (!**pattern)
					break;
			}

			/* Patterns must terminate with ']' not '/' */
			if (slash && (**pattern == '/'))
				break;

			/* Match character classes. */
			switch (classmatch(*pattern, **string, nocase, pattern)) {
			case RANGE_MATCH:
				result = 0;
				continue;
			case RANGE_NOMATCH:
				/* Valid character class but no match. */
				continue;
			default:
				/* Not a valid character class. */
				break;
			}
			if (!**pattern)
				break;

leadingclosebrace:
			/* Look at only well-formed range patterns;
			 * "x-]" is not allowed unless escaped ("x-\]")
			 * XXX: Fix for locale/MBCS character width
			 */
			if (((*pattern)[1] == '-') && ((*pattern)[2] != ']')) {
				startch = *pattern;
				*pattern += (escape && ((*pattern)[2] == '\\')) ? 3 : 2;

				/*
				 * NOT a properly balanced [expr] pattern, EOS
				 * terminated or ranges containing a slash in
				 * FNM_PATHNAME mode pattern fall out to to the
				 * rewind and test '[' literal code path.
				 */
				if (!**pattern || (slash && (**pattern == '/')))
					break;

				/* XXX: handle locale/MBCS comparison, advance by MBCS char width */
				if ((**string >= *startch) && (**string <= **pattern))
					result = 0;
				else if (nocase &&
				    (isupper((unsigned char)**string) ||
				     isupper((unsigned char)*startch) ||
				     isupper((unsigned char)**pattern)) &&
				    (tolower((unsigned char)**string) >=
				     tolower((unsigned char)*startch)) &&
				    (tolower((unsigned char)**string) <=
				     tolower((unsigned char)**pattern)))
					result = 0;

				++*pattern;
				continue;
			}

			/* XXX: handle locale/MBCS comparison, advance by MBCS char width */
			if ((**string == **pattern))
				result = 0;
			else if (nocase && (isupper((unsigned char)**string) ||
			    isupper((unsigned char)**pattern)) &&
			    (tolower((unsigned char)**string) ==
			    tolower((unsigned char)**pattern)))
				result = 0;

			++*pattern;
		}
		/*
		 * NOT a properly balanced [expr] pattern;
		 * Rewind and reset result to test '[' literal
		 */
		*pattern = mismatch;
		result = FNM_NOMATCH;
	} else if (**pattern == '?') {
		/* Optimize '?' match before unescaping **pattern */
		if (!**string || (slash && (**string == '/')))
			return FNM_NOMATCH;
		result = 0;
		goto fnmatch_ch_success;
	} else if (escape && (**pattern == '\\') && (*pattern)[1]) {
		++*pattern;
	}

	/* XXX: handle locale/MBCS comparison, advance by the MBCS char width */
	if (**string == **pattern)
		result = 0;
	else if (nocase && (isupper((unsigned char)**string) ||
	    isupper((unsigned char)**pattern)) &&
	    (tolower((unsigned char)**string) ==
	    tolower((unsigned char)**pattern)))
		result = 0;

	/* Refuse to advance over trailing slash or NULs */
	if (**string == '\0' || **pattern == '\0' ||
	    (slash && ((**string == '/') || (**pattern == '/'))))
		return result;

fnmatch_ch_success:
	++*pattern;
	++*string;
	return result;
}


int fnmatch(const char *pattern, const char *string, int flags)
{
	static const char dummystring[2] = {' ', 0};
	const int escape = !(flags & FNM_NOESCAPE);
	const int slash = !!(flags & FNM_PATHNAME);
	const int leading_dir = !!(flags & FNM_LEADING_DIR);
	const char *dummyptr, *matchptr, *strendseg;
	int wild;
	/* For '*' wild processing only; suppress 'used before initialization'
	 * warnings with dummy initialization values;
	 */
	const char *strstartseg = NULL;
	const char *mismatch = NULL;
	int matchlen = 0;

	if (*pattern == '*')
		goto firstsegment;

	while (*pattern && *string) {
		/*
		 * Pre-decode "\/" which has no special significance, and
		 * match balanced slashes, starting a new segment pattern.
		 */
		if (slash && escape && (*pattern == '\\') && (pattern[1] == '/'))
			++pattern;
		if (slash && (*pattern == '/') && (*string == '/')) {
			++pattern;
			++string;
		}

firstsegment:
		/*
		 * At the beginning of each segment, validate leading period
		 * behavior.
		 */
		if ((flags & FNM_PERIOD) && (*string == '.')) {
		    if (*pattern == '.')
			    ++pattern;
		    else if (escape && (*pattern == '\\') && (pattern[1] == '.'))
			    pattern += 2;
		    else
			    return FNM_NOMATCH;
		    ++string;
		}

		/*
		 * Determine the end of string segment.  Presumes '/'
		 * character is unique, not composite in any MBCS encoding
		 */
		if (slash) {
			strendseg = strchr(string, '/');
			if (!strendseg)
				strendseg = strchr(string, '\0');
		} else {
			strendseg = strchr(string, '\0');
		}

		/*
		 * Allow pattern '*' to be consumed even with no remaining
		 * string to match.
		 */
		while (*pattern) {
			if ((string > strendseg) ||
			    ((string == strendseg) && (*pattern != '*')))
				break;

			if (slash && ((*pattern == '/') ||
			    (escape && (*pattern == '\\') && (pattern[1] == '/'))))
				break;

			/*
			 * Reduce groups of '*' and '?' to n '?' matches
			 * followed by one '*' test for simplicity.
			 */
			for (wild = 0; (*pattern == '*') || (*pattern == '?'); ++pattern) {
				if (*pattern == '*') {
					wild = 1;
				} else if (string < strendseg) {  /* && (*pattern == '?') */
					/* XXX: Advance 1 char for MBCS locale */
					++string;
				}
				else {  /* (string >= strendseg) && (*pattern == '?') */
					return FNM_NOMATCH;
				}
			}

			if (wild) {
				strstartseg = string;
				mismatch = pattern;

				/*
				 * Count fixed (non '*') char matches remaining
				 * in pattern * excluding '/' (or "\/") and '*'.
				 */
				for (matchptr = pattern, matchlen = 0; 1; ++matchlen) {
					if ((*matchptr == '\0') ||
					    (slash && ((*matchptr == '/') ||
					    (escape && (*matchptr == '\\') &&
					    (matchptr[1] == '/'))))) {
						/* Compare precisely this many
						 * trailing string chars, the
						 * resulting match needs no
						 * wildcard loop.
						 */
						/* XXX: Adjust for MBCS */
						if (string + matchlen > strendseg)
							return FNM_NOMATCH;

						string = strendseg - matchlen;
						wild = 0;
						break;
					}

					if (*matchptr == '*') {
						/*
						 * Ensure at least this many
						 * trailing string chars remain
						 * for the first comparison.
						 */
						/* XXX: Adjust for MBCS */
						if (string + matchlen > strendseg)
							return FNM_NOMATCH;

						/*
						 * Begin first wild comparison
						 * at the current position.
						 */
						break;
					}

					/*
					 * Skip forward in pattern by a single
					 * character match Use a dummy
					 * fnmatch_ch() test to count one
					 * "[range]" escape.
					 */
					/* XXX: Adjust for MBCS */
					if (escape && (*matchptr == '\\') &&
					    matchptr[1]) {
						matchptr += 2;
					} else if (*matchptr == '[') {
						dummyptr = dummystring;
						fnmatch_ch(&matchptr, &dummyptr,
						    flags);
					} else {
						++matchptr;
					}
				}
			}

			/* Incrementally match string against the pattern. */
			while (*pattern && (string < strendseg)) {
				/* Success; begin a new wild pattern search. */
				if (*pattern == '*')
					break;

				if (slash && ((*string == '/') ||
				    (*pattern == '/') || (escape &&
				    (*pattern == '\\') && (pattern[1] == '/'))))
					break;

				/*
				 * Compare ch's (the pattern is advanced over
				 * "\/" to the '/', but slashes will mismatch,
				 * and are not consumed).
				 */
				if (!fnmatch_ch(&pattern, &string, flags))
					continue;

				/*
				 * Failed to match, loop against next char
				 * offset of string segment until not enough
				 * string chars remain to match the fixed
				 * pattern.
				 */
				if (wild) {
					/* XXX: Advance 1 char for MBCS locale */
					string = ++strstartseg;
					if (string + matchlen > strendseg)
						return FNM_NOMATCH;

					pattern = mismatch;
					continue;
				} else
					return FNM_NOMATCH;
			}
		}

		if (*string && !((slash || leading_dir) && (*string == '/')))
			return FNM_NOMATCH;

		if (*pattern && !(slash && ((*pattern == '/') ||
		    (escape && (*pattern == '\\') && (pattern[1] == '/')))))
			return FNM_NOMATCH;

		if (leading_dir && !*pattern && *string == '/')
			return 0;
	}

	/* Where both pattern and string are at EOS, declare success.  */
	if (!*string && !*pattern)
		return 0;

	/* Pattern didn't match to the end of string. */
	return FNM_NOMATCH;
}

#endif
