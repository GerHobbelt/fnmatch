#if 0

/*========================================================*/
/* == ./gcc/include/fnmatch.h == */
/* Copyright (C) 1991-2024 Free Software Foundation, Inc.

NOTE: The canonical source of this file is maintained with the GNU C Library.
Bugs can be reported to bug-glibc@prep.ai.mit.edu.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 51 Franklin Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */

#ifndef	_FNMATCH_H

#define	_FNMATCH_H	1

#ifdef	__cplusplus
extern "C" {
#endif

#if defined (__cplusplus) || (defined (__STDC__) && __STDC__)
#undef	__P
#define	__P(args)	args
#else /* Not C++ or ANSI C.  */
#undef	__P
#define	__P(args)	()
/* We can get away without defining `const' here only because in this file
   it is used only inside the prototype for `fnmatch', which is elided in
   non-ANSI C where `const' is problematical.  */
#endif /* C++ or ANSI C.  */


/* We #undef these before defining them because some losing systems
   (HP-UX A.08.07 for example) define these in <unistd.h>.  */
#undef	FNM_PATHNAME
#undef	FNM_NOESCAPE
#undef	FNM_PERIOD

/* Bits set in the FLAGS argument to `fnmatch'.  */
#define	FNM_PATHNAME	(1 << 0) /* No wildcard can ever match `/'.  */
#define	FNM_NOESCAPE	(1 << 1) /* Backslashes don't quote special chars.  */
#define	FNM_PERIOD	(1 << 2) /* Leading `.' is matched only explicitly.  */

#if !defined (_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 2 || defined (_GNU_SOURCE)
#define	FNM_FILE_NAME	FNM_PATHNAME /* Preferred GNU name.  */
#define	FNM_LEADING_DIR	(1 << 3) /* Ignore `/...' after a match.  */
#define	FNM_CASEFOLD	(1 << 4) /* Compare without regard to case.  */
#endif

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#define	FNM_NOMATCH	1

/* Match STRING against the filename pattern PATTERN,
   returning zero if it matches, FNM_NOMATCH if not.  */
extern int fnmatch __P ((const char *__pattern, const char *__string,
			 int __flags));

#ifdef	__cplusplus
}
#endif

#endif /* fnmatch.h */

/*========================================================*/
/* == ./gcc/libiberty/fnmatch.c == */
/* Copyright (C) 1991-2024 Free Software Foundation, Inc.

NOTE: This source is derived from an old version taken from the GNU C
Library (glibc).

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 51 Franklin Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */

#ifdef HAVE_CONFIG_H
#if defined (CONFIG_BROKETS)
/* We use <config.h> instead of "config.h" so that a compilation
   using -I. -I$srcdir will use ./config.h rather than $srcdir/config.h
   (which it would do because it found this file in $srcdir).  */
#include <config.h>
#else
#include "config.h"
#endif
#endif


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* This code to undef const added in libiberty.  */
#ifndef __STDC__
/* This is a separate conditional since some stdc systems
   reject `defined (const)'.  */
#ifndef const
#define const
#endif
#endif

#include <errno.h>
#include <fnmatch.h>
#include <safe-ctype.h>

/* Comment out all this code if we are using the GNU C Library, and are not
   actually compiling the library itself.  This code is part of the GNU C
   Library, but also included in many other GNU distributions.  Compiling
   and linking in this code is a waste when using the GNU C library
   (especially if it is a shared library).  Rather than having every GNU
   program understand `configure --with-gnu-libc' and omit the object files,
   it is simpler to just do this in the source for each such file.  */

#if defined (_LIBC) || !defined (__GNU_LIBRARY__)


#if !defined(__GNU_LIBRARY__) && !defined(STDC_HEADERS)
extern int errno;
#endif

/* Match STRING against the filename pattern PATTERN, returning zero if
   it matches, nonzero if not.  */
int
fnmatch (const char *pattern, const char *string, int flags)
{
  register const char *p = pattern, *n = string;
  register unsigned char c;

#define FOLD(c)	((flags & FNM_CASEFOLD) ? TOLOWER (c) : (c))

  while ((c = *p++) != '\0')
    {
      c = FOLD (c);

      switch (c)
	{
	case '?':
	  if (*n == '\0')
	    return FNM_NOMATCH;
	  else if ((flags & FNM_FILE_NAME) && *n == '/')
	    return FNM_NOMATCH;
	  else if ((flags & FNM_PERIOD) && *n == '.' &&
		   (n == string || ((flags & FNM_FILE_NAME) && n[-1] == '/')))
	    return FNM_NOMATCH;
	  break;

	case '\\':
	  if (!(flags & FNM_NOESCAPE))
	    {
	      c = *p++;
	      c = FOLD (c);
	    }
	  if (FOLD ((unsigned char)*n) != c)
	    return FNM_NOMATCH;
	  break;

	case '*':
	  if ((flags & FNM_PERIOD) && *n == '.' &&
	      (n == string || ((flags & FNM_FILE_NAME) && n[-1] == '/')))
	    return FNM_NOMATCH;

	  for (c = *p++; c == '?' || c == '*'; c = *p++, ++n)
	    if (((flags & FNM_FILE_NAME) && *n == '/') ||
		(c == '?' && *n == '\0'))
	      return FNM_NOMATCH;

	  if (c == '\0')
	    return 0;

	  {
	    unsigned char c1 = (!(flags & FNM_NOESCAPE) && c == '\\') ? *p : c;
	    c1 = FOLD (c1);
	    for (--p; *n != '\0'; ++n)
	      if ((c == '[' || FOLD ((unsigned char)*n) == c1) &&
		  fnmatch (p, n, flags & ~FNM_PERIOD) == 0)
		return 0;
	    return FNM_NOMATCH;
	  }

	case '[':
	  {
	    /* Nonzero if the sense of the character class is inverted.  */
	    register int negate;

	    if (*n == '\0')
	      return FNM_NOMATCH;

	    if ((flags & FNM_PERIOD) && *n == '.' &&
		(n == string || ((flags & FNM_FILE_NAME) && n[-1] == '/')))
	      return FNM_NOMATCH;

	    negate = (*p == '!' || *p == '^');
	    if (negate)
	      ++p;

	    c = *p++;
	    for (;;)
	      {
		register unsigned char cstart = c, cend = c;

		if (!(flags & FNM_NOESCAPE) && c == '\\')
		  cstart = cend = *p++;

		cstart = cend = FOLD (cstart);

		if (c == '\0')
		  /* [ (unterminated) loses.  */
		  return FNM_NOMATCH;

		c = *p++;
		c = FOLD (c);

		if ((flags & FNM_FILE_NAME) && c == '/')
		  /* [/] can never match.  */
		  return FNM_NOMATCH;

		if (c == '-' && *p != ']')
		  {
		    cend = *p++;
		    if (!(flags & FNM_NOESCAPE) && cend == '\\')
		      cend = *p++;
		    if (cend == '\0')
		      return FNM_NOMATCH;
		    cend = FOLD (cend);

		    c = *p++;
		  }

		if (FOLD ((unsigned char)*n) >= cstart
		    && FOLD ((unsigned char)*n) <= cend)
		  goto matched;

		if (c == ']')
		  break;
	      }
	    if (!negate)
	      return FNM_NOMATCH;
	    break;

	  matched:;
	    /* Skip the rest of the [...] that already matched.  */
	    while (c != ']')
	      {
		if (c == '\0')
		  /* [... (unterminated) loses.  */
		  return FNM_NOMATCH;

		c = *p++;
		if (!(flags & FNM_NOESCAPE) && c == '\\')
		  /* XXX 1003.2d11 is unclear if this is right.  */
		  ++p;
	      }
	    if (negate)
	      return FNM_NOMATCH;
	  }
	  break;

	default:
	  if (c != FOLD ((unsigned char)*n))
	    return FNM_NOMATCH;
	}

      ++n;
    }

  if (*n == '\0')
    return 0;

  if ((flags & FNM_LEADING_DIR) && *n == '/')
    /* The FNM_LEADING_DIR flag says that "foo*" matches "foobar/frobozz".  */
    return 0;

  return FNM_NOMATCH;
}

#endif	/* _LIBC or not __GNU_LIBRARY__.  */

/*========================================================*/
/* == ./gcc/libiberty/fnmatch.txh == */
/*
@deftypefn Replacement int fnmatch (const char *@var{pattern}, @
  const char *@var{string}, int @var{flags})

Matches @var{string} against @var{pattern}, returning zero if it
matches, @code{FNM_NOMATCH} if not.  @var{pattern} may contain the
wildcards @code{?} to match any one character, @code{*} to match any
zero or more characters, or a set of alternate characters in square
brackets, like @samp{[a-gt8]}, which match one character (@code{a}
through @code{g}, or @code{t}, or @code{8}, in this example) if that one
character is in the set.  A set may be inverted (i.e., match anything
except what's in the set) by giving @code{^} or @code{!} as the first
character in the set.  To include those characters in the set, list them
as anything other than the first character of the set.  To include a
dash in the set, list it last in the set.  A backslash character makes
the following character not special, so for example you could match
against a literal asterisk with @samp{\*}.  To match a literal
backslash, use @samp{\\}.

@code{flags} controls various aspects of the matching process, and is a
boolean OR of zero or more of the following values (defined in
@code{<fnmatch.h>}):

@table @code

@item FNM_PATHNAME
@itemx FNM_FILE_NAME
@var{string} is assumed to be a path name.  No wildcard will ever match
@code{/}.

@item FNM_NOESCAPE
Do not interpret backslashes as quoting the following special character.

@item FNM_PERIOD
A leading period (at the beginning of @var{string}, or if
@code{FNM_PATHNAME} after a slash) is not matched by @code{*} or
@code{?} but must be matched explicitly.

@item FNM_LEADING_DIR
Means that @var{string} also matches @var{pattern} if some initial part
of @var{string} matches, and is followed by @code{/} and zero or more
characters.  For example, @samp{foo*} would match either @samp{foobar}
or @samp{foobar/grill}.

@item FNM_CASEFOLD
Ignores case when performing the comparison.

@end table

@end deftypefn
*/
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
/* == ./src/gnu/gcc/include/fnmatch.h == */
/* Copyright 1991, 1992, 1993, 1996 Free Software Foundation, Inc.

NOTE: The canonical source of this file is maintained with the GNU C Library.
Bugs can be reported to bug-glibc@prep.ai.mit.edu.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 51 Franklin Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */

#ifndef	_FNMATCH_H

#define	_FNMATCH_H	1

#ifdef	__cplusplus
extern "C" {
#endif

#if defined (__cplusplus) || (defined (__STDC__) && __STDC__)
#undef	__P
#define	__P(args)	args
#else /* Not C++ or ANSI C.  */
#undef	__P
#define	__P(args)	()
/* We can get away without defining `const' here only because in this file
   it is used only inside the prototype for `fnmatch', which is elided in
   non-ANSI C where `const' is problematical.  */
#endif /* C++ or ANSI C.  */


/* We #undef these before defining them because some losing systems
   (HP-UX A.08.07 for example) define these in <unistd.h>.  */
#undef	FNM_PATHNAME
#undef	FNM_NOESCAPE
#undef	FNM_PERIOD

/* Bits set in the FLAGS argument to `fnmatch'.  */
#define	FNM_PATHNAME	(1 << 0) /* No wildcard can ever match `/'.  */
#define	FNM_NOESCAPE	(1 << 1) /* Backslashes don't quote special chars.  */
#define	FNM_PERIOD	(1 << 2) /* Leading `.' is matched only explicitly.  */

#if !defined (_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 2 || defined (_GNU_SOURCE)
#define	FNM_FILE_NAME	FNM_PATHNAME /* Preferred GNU name.  */
#define	FNM_LEADING_DIR	(1 << 3) /* Ignore `/...' after a match.  */
#define	FNM_CASEFOLD	(1 << 4) /* Compare without regard to case.  */
#endif

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#define	FNM_NOMATCH	1

/* Match STRING against the filename pattern PATTERN,
   returning zero if it matches, FNM_NOMATCH if not.  */
extern int fnmatch __P ((const char *__pattern, const char *__string,
			 int __flags));

#ifdef	__cplusplus
}
#endif

#endif /* fnmatch.h */

/*========================================================*/
/* == ./src/gnu/lib/libiberty/include/fnmatch.h == */
/* Copyright 1991, 1992, 1993, 1996 Free Software Foundation, Inc.

NOTE: The canonical source of this file is maintained with the GNU C Library.
Bugs can be reported to bug-glibc@prep.ai.mit.edu.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 51 Franklin Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */

#ifndef	_FNMATCH_H

#define	_FNMATCH_H	1

#ifdef	__cplusplus
extern "C" {
#endif

#if defined (__cplusplus) || (defined (__STDC__) && __STDC__)
#undef	__P
#define	__P(args)	args
#else /* Not C++ or ANSI C.  */
#undef	__P
#define	__P(args)	()
/* We can get away without defining `const' here only because in this file
   it is used only inside the prototype for `fnmatch', which is elided in
   non-ANSI C where `const' is problematical.  */
#endif /* C++ or ANSI C.  */


/* We #undef these before defining them because some losing systems
   (HP-UX A.08.07 for example) define these in <unistd.h>.  */
#undef	FNM_PATHNAME
#undef	FNM_NOESCAPE
#undef	FNM_PERIOD

/* Bits set in the FLAGS argument to `fnmatch'.  */
#define	FNM_PATHNAME	(1 << 0) /* No wildcard can ever match `/'.  */
#define	FNM_NOESCAPE	(1 << 1) /* Backslashes don't quote special chars.  */
#define	FNM_PERIOD	(1 << 2) /* Leading `.' is matched only explicitly.  */

#if !defined (_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 2 || defined (_GNU_SOURCE)
#define	FNM_FILE_NAME	FNM_PATHNAME /* Preferred GNU name.  */
#define	FNM_LEADING_DIR	(1 << 3) /* Ignore `/...' after a match.  */
#define	FNM_CASEFOLD	(1 << 4) /* Compare without regard to case.  */
#endif

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#define	FNM_NOMATCH	1

/* Match STRING against the filename pattern PATTERN,
   returning zero if it matches, FNM_NOMATCH if not.  */
extern int fnmatch __P ((const char *__pattern, const char *__string,
			 int __flags));

#ifdef	__cplusplus
}
#endif

#endif /* fnmatch.h */

/*========================================================*/
/* == ./src/gnu/lib/libiberty/src/fnmatch.c == */
/* Copyright (C) 1991, 1992, 1993 Free Software Foundation, Inc.

NOTE: This source is derived from an old version taken from the GNU C
Library (glibc).

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 51 Franklin Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */

#ifdef HAVE_CONFIG_H
#if defined (CONFIG_BROKETS)
/* We use <config.h> instead of "config.h" so that a compilation
   using -I. -I$srcdir will use ./config.h rather than $srcdir/config.h
   (which it would do because it found this file in $srcdir).  */
#include <config.h>
#else
#include "config.h"
#endif
#endif


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* This code to undef const added in libiberty.  */
#ifndef __STDC__
/* This is a separate conditional since some stdc systems
   reject `defined (const)'.  */
#ifndef const
#define const
#endif
#endif

#include <errno.h>
#include <fnmatch.h>
#include <safe-ctype.h>

/* Comment out all this code if we are using the GNU C Library, and are not
   actually compiling the library itself.  This code is part of the GNU C
   Library, but also included in many other GNU distributions.  Compiling
   and linking in this code is a waste when using the GNU C library
   (especially if it is a shared library).  Rather than having every GNU
   program understand `configure --with-gnu-libc' and omit the object files,
   it is simpler to just do this in the source for each such file.  */

#if defined (_LIBC) || !defined (__GNU_LIBRARY__)


#if !defined(__GNU_LIBRARY__) && !defined(STDC_HEADERS)
extern int errno;
#endif

/* Match STRING against the filename pattern PATTERN, returning zero if
   it matches, nonzero if not.  */
int
fnmatch (const char *pattern, const char *string, int flags)
{
  register const char *p = pattern, *n = string;
  register unsigned char c;

#define FOLD(c)	((flags & FNM_CASEFOLD) ? TOLOWER (c) : (c))

  while ((c = *p++) != '\0')
    {
      c = FOLD (c);

      switch (c)
	{
	case '?':
	  if (*n == '\0')
	    return FNM_NOMATCH;
	  else if ((flags & FNM_FILE_NAME) && *n == '/')
	    return FNM_NOMATCH;
	  else if ((flags & FNM_PERIOD) && *n == '.' &&
		   (n == string || ((flags & FNM_FILE_NAME) && n[-1] == '/')))
	    return FNM_NOMATCH;
	  break;

	case '\\':
	  if (!(flags & FNM_NOESCAPE))
	    {
	      c = *p++;
	      c = FOLD (c);
	    }
	  if (FOLD ((unsigned char)*n) != c)
	    return FNM_NOMATCH;
	  break;

	case '*':
	  if ((flags & FNM_PERIOD) && *n == '.' &&
	      (n == string || ((flags & FNM_FILE_NAME) && n[-1] == '/')))
	    return FNM_NOMATCH;

	  for (c = *p++; c == '?' || c == '*'; c = *p++, ++n)
	    if (((flags & FNM_FILE_NAME) && *n == '/') ||
		(c == '?' && *n == '\0'))
	      return FNM_NOMATCH;

	  if (c == '\0')
	    return 0;

	  {
	    unsigned char c1 = (!(flags & FNM_NOESCAPE) && c == '\\') ? *p : c;
	    c1 = FOLD (c1);
	    for (--p; *n != '\0'; ++n)
	      if ((c == '[' || FOLD ((unsigned char)*n) == c1) &&
		  fnmatch (p, n, flags & ~FNM_PERIOD) == 0)
		return 0;
	    return FNM_NOMATCH;
	  }

	case '[':
	  {
	    /* Nonzero if the sense of the character class is inverted.  */
	    register int negate;

	    if (*n == '\0')
	      return FNM_NOMATCH;

	    if ((flags & FNM_PERIOD) && *n == '.' &&
		(n == string || ((flags & FNM_FILE_NAME) && n[-1] == '/')))
	      return FNM_NOMATCH;

	    negate = (*p == '!' || *p == '^');
	    if (negate)
	      ++p;

	    c = *p++;
	    for (;;)
	      {
		register unsigned char cstart = c, cend = c;

		if (!(flags & FNM_NOESCAPE) && c == '\\')
		  cstart = cend = *p++;

		cstart = cend = FOLD (cstart);

		if (c == '\0')
		  /* [ (unterminated) loses.  */
		  return FNM_NOMATCH;

		c = *p++;
		c = FOLD (c);

		if ((flags & FNM_FILE_NAME) && c == '/')
		  /* [/] can never match.  */
		  return FNM_NOMATCH;

		if (c == '-' && *p != ']')
		  {
		    cend = *p++;
		    if (!(flags & FNM_NOESCAPE) && cend == '\\')
		      cend = *p++;
		    if (cend == '\0')
		      return FNM_NOMATCH;
		    cend = FOLD (cend);

		    c = *p++;
		  }

		if (FOLD ((unsigned char)*n) >= cstart
		    && FOLD ((unsigned char)*n) <= cend)
		  goto matched;

		if (c == ']')
		  break;
	      }
	    if (!negate)
	      return FNM_NOMATCH;
	    break;

	  matched:;
	    /* Skip the rest of the [...] that already matched.  */
	    while (c != ']')
	      {
		if (c == '\0')
		  /* [... (unterminated) loses.  */
		  return FNM_NOMATCH;

		c = *p++;
		if (!(flags & FNM_NOESCAPE) && c == '\\')
		  /* XXX 1003.2d11 is unclear if this is right.  */
		  ++p;
	      }
	    if (negate)
	      return FNM_NOMATCH;
	  }
	  break;

	default:
	  if (c != FOLD ((unsigned char)*n))
	    return FNM_NOMATCH;
	}

      ++n;
    }

  if (*n == '\0')
    return 0;

  if ((flags & FNM_LEADING_DIR) && *n == '/')
    /* The FNM_LEADING_DIR flag says that "foo*" matches "foobar/frobozz".  */
    return 0;

  return FNM_NOMATCH;
}

#endif	/* _LIBC or not __GNU_LIBRARY__.  */

/*========================================================*/
/* == ./src/gnu/lib/libiberty/src/fnmatch.txh == */
/*
@deftypefn Replacement int fnmatch (const char *@var{pattern}, const char *@var{string}, int @var{flags})

Matches @var{string} against @var{pattern}, returning zero if it
matches, @code{FNM_NOMATCH} if not.  @var{pattern} may contain the
wildcards @code{?} to match any one character, @code{*} to match any
zero or more characters, or a set of alternate characters in square
brackets, like @samp{[a-gt8]}, which match one character (@code{a}
through @code{g}, or @code{t}, or @code{8}, in this example) if that one
character is in the set.  A set may be inverted (i.e., match anything
except what's in the set) by giving @code{^} or @code{!} as the first
character in the set.  To include those characters in the set, list them
as anything other than the first character of the set.  To include a
dash in the set, list it last in the set.  A backslash character makes
the following character not special, so for example you could match
against a literal asterisk with @samp{\*}.  To match a literal
backslash, use @samp{\\}.

@code{flags} controls various aspects of the matching process, and is a
boolean OR of zero or more of the following values (defined in
@code{<fnmatch.h>}):

@table @code

@item FNM_PATHNAME
@itemx FNM_FILE_NAME
@var{string} is assumed to be a path name.  No wildcard will ever match
@code{/}.

@item FNM_NOESCAPE
Do not interpret backslashes as quoting the following special character.

@item FNM_PERIOD
A leading period (at the beginning of @var{string}, or if
@code{FNM_PATHNAME} after a slash) is not matched by @code{*} or
@code{?} but must be matched explicitly.

@item FNM_LEADING_DIR
Means that @var{string} also matches @var{pattern} if some initial part
of @var{string} matches, and is followed by @code{/} and zero or more
characters.  For example, @samp{foo*} would match either @samp{foobar}
or @samp{foobar/grill}.

@item FNM_CASEFOLD
Ignores case when performing the comparison.

@end table

@end deftypefn
*/
/*========================================================*/
/* == ./src/gnu/usr.bin/binutils/include/fnmatch.h == */
/* Copyright 1991, 1992, 1993, 1996 Free Software Foundation, Inc.

NOTE: The canonical source of this file is maintained with the GNU C Library.
Bugs can be reported to bug-glibc@prep.ai.mit.edu.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#ifndef	_FNMATCH_H

#define	_FNMATCH_H	1

#ifdef	__cplusplus
extern "C" {
#endif

#if defined (__cplusplus) || (defined (__STDC__) && __STDC__)
#undef	__P
#define	__P(args)	args
#else /* Not C++ or ANSI C.  */
#undef	__P
#define	__P(args)	()
/* We can get away without defining `const' here only because in this file
   it is used only inside the prototype for `fnmatch', which is elided in
   non-ANSI C where `const' is problematical.  */
#endif /* C++ or ANSI C.  */


/* We #undef these before defining them because some losing systems
   (HP-UX A.08.07 for example) define these in <unistd.h>.  */
#undef	FNM_PATHNAME
#undef	FNM_NOESCAPE
#undef	FNM_PERIOD

/* Bits set in the FLAGS argument to `fnmatch'.  */
#define	FNM_PATHNAME	(1 << 0) /* No wildcard can ever match `/'.  */
#define	FNM_NOESCAPE	(1 << 1) /* Backslashes don't quote special chars.  */
#define	FNM_PERIOD	(1 << 2) /* Leading `.' is matched only explicitly.  */

#if !defined (_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 2 || defined (_GNU_SOURCE)
#define	FNM_FILE_NAME	FNM_PATHNAME /* Preferred GNU name.  */
#define	FNM_LEADING_DIR	(1 << 3) /* Ignore `/...' after a match.  */
#define	FNM_CASEFOLD	(1 << 4) /* Compare without regard to case.  */
#endif

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#define	FNM_NOMATCH	1

/* Match STRING against the filename pattern PATTERN,
   returning zero if it matches, FNM_NOMATCH if not.  */
extern int fnmatch __P ((const char *__pattern, const char *__string,
			 int __flags));

#ifdef	__cplusplus
}
#endif

#endif /* fnmatch.h */

/*========================================================*/
/* == ./src/gnu/usr.bin/binutils-2.17/include/fnmatch.h == */
/* Copyright 1991, 1992, 1993, 1996 Free Software Foundation, Inc.

NOTE: The canonical source of this file is maintained with the GNU C Library.
Bugs can be reported to bug-glibc@prep.ai.mit.edu.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 51 Franklin Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */

#ifndef	_FNMATCH_H

#define	_FNMATCH_H	1

#ifdef	__cplusplus
extern "C" {
#endif

#if defined (__cplusplus) || (defined (__STDC__) && __STDC__)
#undef	__P
#define	__P(args)	args
#else /* Not C++ or ANSI C.  */
#undef	__P
#define	__P(args)	()
/* We can get away without defining `const' here only because in this file
   it is used only inside the prototype for `fnmatch', which is elided in
   non-ANSI C where `const' is problematical.  */
#endif /* C++ or ANSI C.  */


/* We #undef these before defining them because some losing systems
   (HP-UX A.08.07 for example) define these in <unistd.h>.  */
#undef	FNM_PATHNAME
#undef	FNM_NOESCAPE
#undef	FNM_PERIOD

/* Bits set in the FLAGS argument to `fnmatch'.  */
#define	FNM_PATHNAME	(1 << 0) /* No wildcard can ever match `/'.  */
#define	FNM_NOESCAPE	(1 << 1) /* Backslashes don't quote special chars.  */
#define	FNM_PERIOD	(1 << 2) /* Leading `.' is matched only explicitly.  */

#if !defined (_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 2 || defined (_GNU_SOURCE)
#define	FNM_FILE_NAME	FNM_PATHNAME /* Preferred GNU name.  */
#define	FNM_LEADING_DIR	(1 << 3) /* Ignore `/...' after a match.  */
#define	FNM_CASEFOLD	(1 << 4) /* Compare without regard to case.  */
#endif

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#define	FNM_NOMATCH	1

/* Match STRING against the filename pattern PATTERN,
   returning zero if it matches, FNM_NOMATCH if not.  */
extern int fnmatch __P ((const char *__pattern, const char *__string,
			 int __flags));

#ifdef	__cplusplus
}
#endif

#endif /* fnmatch.h */

/*========================================================*/
/* == ./src/gnu/usr.bin/cvs/lib/fnmatch.c == */
/* Copyright (C) 1992 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.  */

/* Modified slightly by Brian Berliner <berliner@sun.com> and
   Jim Blandy <jimb@cyclic.com> for CVS use */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

/* IGNORE(@ */
/* #include <ansidecl.h> */
/* @) */
#include <errno.h>
#include <fnmatch.h>

#if !defined(__GNU_LIBRARY__) && !defined(STDC_HEADERS)
extern int errno;
#endif

/* Match STRING against the filename pattern PATTERN, returning zero if
   it matches, nonzero if not.  */
int
#if __STDC__
fnmatch (const char *pattern, const char *string, int flags)
#else
fnmatch (pattern, string, flags)
    char *pattern;
    char *string;
    int flags;
#endif
{
  register const char *p = pattern, *n = string;
  register char c;

  if ((flags & ~__FNM_FLAGS) != 0)
    {
      errno = EINVAL;
      return -1;
    }

  while ((c = *p++) != '\0')
    {
      switch (c)
	{
	case '?':
	  if (*n == '\0')
	    return FNM_NOMATCH;
	  else if ((flags & FNM_PATHNAME) && *n == '/')
	    return FNM_NOMATCH;
	  else if ((flags & FNM_PERIOD) && *n == '.' &&
		   (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
	    return FNM_NOMATCH;
	  break;
	  
	case '\\':
	  if (!(flags & FNM_NOESCAPE))
	    c = *p++;
	  if (FOLD_FN_CHAR (*n) != FOLD_FN_CHAR (c))
	    return FNM_NOMATCH;
	  break;
	  
	case '*':
	  if ((flags & FNM_PERIOD) && *n == '.' &&
	      (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
	    return FNM_NOMATCH;
	  
	  for (c = *p++; c == '?' || c == '*'; c = *p++, ++n)
	    if (((flags & FNM_PATHNAME) && *n == '/') ||
		(c == '?' && *n == '\0'))
	      return FNM_NOMATCH;
	  
	  if (c == '\0')
	    return 0;
	  
	  {
	    char c1 = (!(flags & FNM_NOESCAPE) && c == '\\') ? *p : c;
	    for (--p; *n != '\0'; ++n)
	      if ((c == '[' || FOLD_FN_CHAR (*n) == FOLD_FN_CHAR (c1)) &&
		  fnmatch(p, n, flags & ~FNM_PERIOD) == 0)
		return 0;
	    return FNM_NOMATCH;
	  }
	  
	case '[':
	  {
	    /* Nonzero if the sense of the character class is inverted.  */
	    register int not;
	    
	    if (*n == '\0')
	      return FNM_NOMATCH;
	    
	    if ((flags & FNM_PERIOD) && *n == '.' &&
		(n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
	      return FNM_NOMATCH;
	    
	    not = (*p == '!' || *p == '^');
	    if (not)
	      ++p;
	    
	    c = *p++;
	    for (;;)
	      {
		register char cstart = c, cend = c;
		
		if (!(flags & FNM_NOESCAPE) && c == '\\')
		  cstart = cend = *p++;
		
		if (c == '\0')
		  /* [ (unterminated) loses.  */
		  return FNM_NOMATCH;
		
		c = *p++;
		
		if ((flags & FNM_PATHNAME) && c == '/')
		  /* [/] can never match.  */
		  return FNM_NOMATCH;
		
		if (c == '-' && *p != ']')
		  {
		    cend = *p++;
		    if (!(flags & FNM_NOESCAPE) && cend == '\\')
		      cend = *p++;
		    if (cend == '\0')
		      return FNM_NOMATCH;
		    c = *p++;
		  }
		
		if (*n >= cstart && *n <= cend)
		  goto matched;
		
		if (c == ']')
		  break;
	      }
	    if (!not)
	      return FNM_NOMATCH;
	    break;
	    
	  matched:;
	    /* Skip the rest of the [...] that already matched.  */
	    while (c != ']')
	      {
		if (c == '\0')
		  /* [... (unterminated) loses.  */
		  return FNM_NOMATCH;
		
		c = *p++;
		if (!(flags & FNM_NOESCAPE) && c == '\\')
		  /* 1003.2d11 is unclear if this is right.  %%% */
		  ++p;
	      }
	    if (not)
	      return FNM_NOMATCH;
	  }
	  break;
	  
	default:
	  if (FOLD_FN_CHAR (c) != FOLD_FN_CHAR (*n))
	    return FNM_NOMATCH;
	}
      
      ++n;
    }

  if (*n == '\0')
    return 0;

  return FNM_NOMATCH;
}

/*========================================================*/
/* == ./src/gnu/usr.bin/cvs/lib/fnmatch.h == */
/* Copyright (C) 1992 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.  */

#ifndef	_FNMATCH_H

#define	_FNMATCH_H	1

/* Bits set in the FLAGS argument to `fnmatch'.  */
#undef FNM_PATHNAME
#define	FNM_PATHNAME	(1 << 0)/* No wildcard can ever match `/'.  */
#undef FNM_NOESCAPE
#define	FNM_NOESCAPE	(1 << 1)/* Backslashes don't quote special chars.  */
#undef FNM_PERIOD
#define	FNM_PERIOD	(1 << 2)/* Leading `.' is matched only explicitly.  */
#undef __FNM_FLAGS
#define	__FNM_FLAGS	(FNM_PATHNAME|FNM_NOESCAPE|FNM_PERIOD)

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#undef FNM_NOMATCH
#define	FNM_NOMATCH	1

/* Match STRING against the filename pattern PATTERN,
   returning zero if it matches, FNM_NOMATCH if not.  */
#if __STDC__
extern int fnmatch (const char *pattern, const char *string, int flags);
#else
extern int fnmatch ();
#endif

#endif	/* fnmatch.h */

/*========================================================*/
/* == ./src/gnu/usr.sbin/mkhybrid/src/match.c == */
/*
 * 27-Mar-96: Jan-Piet Mens <jpm@mens.de>
 * added 'match' option (-m) to specify regular expressions NOT to be included
 * in the CD image.
 */

#ifdef APPLE_HYB
/*
 * Added a number of routines to create lists of files to hidden from
 * the ISO9660 and/or Joliet trees. James Pearson (j.pearson@ge.ucl.ac.uk)
 * January 1999 (these will probably appear in mkisofs in the future)
 */
#endif /* APPLE_HYB */

#include "config.h"
#include <prototyp.h>
#include <stdio.h>
#include <fnmatch.h>
#ifndef VMS
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#else
#include <stdlib.h>
#endif
#endif
#include <inttypes.h>
#include <string.h>
#include "match.h"

#define MAXMATCH 1000
static char *mat[MAXMATCH];

int  add_match(fn)
char * fn;
{
  register int i;

  for (i=0; mat[i] && i<MAXMATCH; i++);
  if (i == MAXMATCH) {
    fprintf(stderr,"Can't exclude RE '%s' - too many entries in table\n",fn);
    return 1;
  }

 
  mat[i] = (char *) malloc(strlen(fn)+1);
  if (! mat[i]) {
    fprintf(stderr,"Can't allocate memory for excluded filename\n");
    return 1;
  }

  strcpy(mat[i],fn);

  return 0;
}

int matches(fn)
char * fn;
{
  /* very dumb search method ... */
  register int i;

  for (i=0; mat[i] && i<MAXMATCH; i++) {
    if (fnmatch(mat[i], fn, FNM_PATHNAME) != FNM_NOMATCH) {
      return 1; /* found -> excluded filenmae */
    }
  }
  return 0; /* not found -> not excluded */
}

/* ISO9660/RR hide */

static char *i_mat[MAXMATCH];

int  i_add_match(fn)
char * fn;
{
  register int i;

  for (i=0; i_mat[i] && i<MAXMATCH; i++);
  if (i == MAXMATCH) {
    fprintf(stderr,"Can't exclude RE '%s' - too many entries in table\n",fn);
    return 1;
  }

 
  i_mat[i] = (char *) malloc(strlen(fn)+1);
  if (! i_mat[i]) {
    fprintf(stderr,"Can't allocate memory for excluded filename\n");
    return 1;
  }

  strcpy(i_mat[i],fn);

  return 0;
}

int i_matches(fn)
char * fn;
{
  /* very dumb search method ... */
  register int i;

  for (i=0; i_mat[i] && i<MAXMATCH; i++) {
    if (fnmatch(i_mat[i], fn, FNM_PATHNAME) != FNM_NOMATCH) {
      return 1; /* found -> excluded filenmae */
    }
  }
  return 0; /* not found -> not excluded */
}

intptr_t i_ishidden()
{
  return((intptr_t)i_mat[0]);
}

/* Joliet hide */

static char *j_mat[MAXMATCH];

int  j_add_match(fn)
char * fn;
{
  register int i;

  for (i=0; j_mat[i] && i<MAXMATCH; i++);
  if (i == MAXMATCH) {
    fprintf(stderr,"Can't exclude RE '%s' - too many entries in table\n",fn);
    return 1;
  }

 
  j_mat[i] = (char *) malloc(strlen(fn)+1);
  if (! j_mat[i]) {
    fprintf(stderr,"Can't allocate memory for excluded filename\n");
    return 1;
  }

  strcpy(j_mat[i],fn);

  return 0;
}

int j_matches(fn)
char * fn;
{
  /* very dumb search method ... */
  register int i;

  for (i=0; j_mat[i] && i<MAXMATCH; i++) {
    if (fnmatch(j_mat[i], fn, FNM_PATHNAME) != FNM_NOMATCH) {
      return 1; /* found -> excluded filenmae */
    }
  }
  return 0; /* not found -> not excluded */
}

intptr_t j_ishidden()
{
  return((intptr_t)j_mat[0]);
}

#ifdef APPLE_HYB

/* HFS hide */

static char *hfs_mat[MAXMATCH];

int hfs_add_match(fn)
char * fn;
{
  register int i;

  for (i=0; hfs_mat[i] && i<MAXMATCH; i++);
  if (i == MAXMATCH) {
    fprintf(stderr,"Can't exclude RE '%s' - too many entries in table\n",fn);
    return 1;
  }

 
  hfs_mat[i] = (char *) malloc(strlen(fn)+1);
  if (! hfs_mat[i]) {
    fprintf(stderr,"Can't allocate memory for excluded filename\n");
    return 1;
  }

  strcpy(hfs_mat[i],fn);

  return 0;
}

void hfs_add_list(file)
char *file;
{
  FILE *fp;
  char name[1024];

  if ((fp = fopen(file, "r")) == NULL) {
    fprintf(stderr,"Can't open hidden file list %s\n", file);
    exit (1);
  }

  while (fscanf(fp, "%s", name) != EOF) {
    if (hfs_add_match(name)) {
      fclose(fp);
      return;
    }
  }

  fclose(fp);
}


int hfs_matches(fn)
char * fn;
{
  /* very dumb search method ... */
  register int i;

  for (i=0; hfs_mat[i] && i<MAXMATCH; i++) {
    if (fnmatch(hfs_mat[i], fn, FNM_PATHNAME) != FNM_NOMATCH) {
      return 1; /* found -> excluded filenmae */
    }
  }
  return 0; /* not found -> not excluded */
}

intptr_t hfs_ishidden()
{
  return((intptr_t)hfs_mat[0]);
}

/* These will probably appear in mkisofs in the future */

void add_list(file)
char *file;
{
  FILE *fp;
  char name[1024];

  if ((fp = fopen(file, "r")) == NULL) {
    fprintf(stderr,"Can't open exclude file list %s\n", file);
    exit (1);
  }

  while (fscanf(fp, "%s", name) != EOF) {
    if (add_match(name)) {
      fclose(fp);
      return;
    }
  }

  fclose(fp);
}

void i_add_list(file)
char *file;
{
  FILE *fp;
  char name[1024];

  if ((fp = fopen(file, "r")) == NULL) {
    fprintf(stderr,"Can't open hidden file list %s\n", file);
    exit (1);
  }

  while (fscanf(fp, "%s", name) != EOF) {
    if (i_add_match(name)) {
      fclose(fp);
      return;
    }
  }

  fclose(fp);
}

void j_add_list(file)
char *file;
{
  FILE *fp;
  char name[1024];

  if ((fp = fopen(file, "r")) == NULL) {
    fprintf(stderr,"Can't open hidden file list %s\n", file);
    exit (1);
  }

  while (fscanf(fp, "%s", name) != EOF) {
    if (j_add_match(name)) {
      fclose(fp);
      return;
    }
  }

  fclose(fp);
}

#endif /* APPLE_HYB */

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

/*========================================================*/
/* == ./src/regress/lib/libc/fnmatch/fnm_test.c == */
/*	$OpenBSD: fnm_test.c,v 1.3 2019/01/25 00:19:26 millert Exp $	*/

/*
 * Public domain, 2008, Todd C. Miller <millert@openbsd.org>
 */

#include <err.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

int
main(int argc, char **argv)
{
	FILE *fp = stdin;
	char pattern[1024], string[1024];
	char *line;
	const char delim[3] = {'\0', '\0', '#'};
	int errors = 0, flags, got, want;

	if (argc > 1) {
		if ((fp = fopen(argv[1], "r")) == NULL)
			err(1, "%s", argv[1]);
	}

	/*
	 * Read in test file, which is formatted thusly:
	 *
	 * pattern string flags expected_result
	 *
	 * lines starting with '#' are comments
	 */
	for (;;) {
		line = fparseln(fp, NULL, NULL, delim, 0);
		if (!line)
			break;
		got = sscanf(line, "%s %s 0x%x %d", pattern, string, &flags,
		    &want);
		if (got == EOF) {
			free(line);
			break;
		}
		if (pattern[0] == '#') {
			free(line);
			continue;
		}
		if (got == 4) {
			got = fnmatch(pattern, string, flags);
			if (got != want) {
				warnx("%s %s %d: want %d, got %d", pattern,
				    string, flags, want, got);
				errors++;
			}
		} else {
			warnx("unrecognized line '%s'\n", line);
			errors++;
		}
		free(line);
	}
	exit(errors);
}

/*========================================================*/
/* == ./src/regress/lib/libc/fnmatch/fnm_test.in == */

/bin/[[:alpha:][:alnum:]]* /bin/ls 0x2 0
/bin/[[:upper:]][[:alnum:]] /bin/ls 0x10 0
/bin/[[:opper:][:alnum:]]* /bin/ls 0x0 1
[[:alpha:][:alnum:]]*.c foo1.c 0x4 0
[[:upper:]]* FOO 0x0 0
[![:space:]]* bar 0x0 0
# 'te\st' 'test'; no match if FNM_NOESCAPE
te\st test 0x0 0  
te\st test 0x1 1
te\st test 0x1e 0
# 'te\\st' 'te\st'; no match if FNM_NOESCAPE
te\\st te\st 0x0 0
te\\st te\st 0x1 1
te\\st te\st 0x1e 0
# 'te\*t' 'te*t'; no match if FNM_NOESCAPE
te\*t te*t 0x0 0
te\*t te*t 0x1 1
te\*t te*t 0x1e 0
# 'te\*t' 'test'; no match
te\*t test 0x0 1
te\*t test 0x1f 1
# 'te\?t' 'te?t'; no match if FNM_NOESCAPE
te\?t te?t 0x0 0
te\?t te?t 0x1 1
te\?t te?t 0x1e 0
# 'te\?t' 'test'; no match
te\?t test 0x0 1
te\?t test 0x1f 1
# 'tesT' 'test'; match if FNM_CASEFOLD
tesT test 0x0 1
tesT test 0xf 1
tesT test 0x10 0
# 'test' 'Test'; match if FNM_CASEFOLD
test Test 0x0 1
test Test 0xf 1
test Test 0x10 0
# 'tEst' 'teSt'; match if FNM_CASEFOLD
tEst teSt 0x0 1
tEst teSt 0xf 1
tEst teSt 0x10 0
# '?est' 'test'; match always
?est test 0x0 0
?est test 0x1f 0
# 'te?t' 'test'; match always
te?t test 0x0 0
te?t test 0x1f 0
# 'tes?' 'test'; match always
tes? test 0x0 0
tes? test 0x1f 0
# 'test?' 'test'; no match
test? test 0x0 1
test? test 0x1f 1
# '*' always matches anything
*  test 0x0 0
*  test 0x1f 0
# '*test' 'test'; match always
*test test 0x0 0
*test test 0x1f 0
# '*est' 'test'; match always
*est test 0x0 0
*est test 0x1f 0
# '*st' 'test'; match always
*st test 0x0 0
*st test 0x1f 0
# 't*t' 'test'; match always
t*t test 0x0 0
t*t test 0x1f 0
# 'te*t' 'test'; match always
te*t test 0x0 0
te*t test 0x1f 0
# 'te*st' 'test'; match always
te*st test 0x0 0
te*st test 0x1f 0
# 'te*' 'test'; match always
te* test 0x0 0
te* test 0x1f 0
# 'tes*' 'test'; match always
tes* test 0x0 0
tes* test 0x1f 0
# 'test*' 'test'; match always
test* test 0x0 0
test* test 0x1f 0
# '.[\-\t]' '.t'; match always
.[\-\t] .t 0x0 0
.[\-\t] .t 0x1f 0
# 'test*?*[a-z]*' 'testgoop'; match always
test*?*[a-z]* testgoop 0x0 0
test*?*[a-z]* testgoop 0x1f 0
# 'te[^abc]t' 'test'; match always
te[^abc]t test 0x0 0
te[^abc]t test 0x1f 0
# 'te[^x]t' 'test'; match always
te[^x]t test 0x0 0
te[^x]t test 0x1f 0
# 'te[!x]t' 'test'; match always
te[!x]t test 0x0 0
te[^x]t test 0x1f 0
# 'te[^x]t' 'text'; no match
te[^x]t text 0x0 1
te[^x]t text 0x1f 1
# 'te[^\x]t' 'text'; no match
te[^\x]t text 0x0 1
te[^\x]t text 0x1f 1
# 'te[^\x' 'text'; no match
te[^\x text 0x0 1
te[^\x text 0x1f 1
# 'te[/]t' 'text'; no match
te[/]t text 0x0 1
te[/]t text 0x1f 1
# 'te[S]t' 'test'; match if FNM_CASEFOLD
te[S]t test 0x0 1
te[S]t test 0xf 1
te[S]t test 0x10 0
# 'te[r-t]t' 'test'; match always
te[r-t]t test 0x0 0
te[r-t]t test 0x1f 0
# 'te[r-t]t' 'teSt'; match if FNM_CASEFOLD
te[r-t]t teSt 0x0 1
te[r-t]t teSt 0xf 1
te[r-t]t teSt 0x10 0
# 'te[r-T]t' 'test'; match if FNM_CASEFOLD
te[r-T]t test 0x0 1
te[r-T]t test 0xf 1
te[r-T]t test 0x10 0
# 'te[R-T]t' 'test'; match if FNM_CASEFOLD
te[R-T]t test 0x0 1
te[R-T]t test 0xf 1
te[R-T]t test 0x10 0
# 'te[r-Tz]t' 'tezt'; match always
te[r-Tz]t tezt 0x0 0
te[r-Tz]t tezt 0x1f 0
# 'te[R-T]t' 'tent'; no match
te[R-T]t tent 0x0 1
te[R-T]t tent 0x1f 1
# 'tes[]t]' 'test'; match always
tes[]t] test 0x0 0
tes[]t] test 0x1f 0
# 'tes[t-]' 'test'; match always
tes[t-] test 0x0 0
tes[t-] test 0x1f 0
# 'tes[t-]]' 'test]'; match always
tes[t-]] test] 0x0 0
tes[t-]] test] 0x1f 0
# 'tes[t-]]' 'test'; no match
tes[t-]] test 0x0 1
tes[t-]] test 0x1f 1
# 'tes[u-]' 'test'; no match
tes[u-] test 0x0 1
tes[u-] test 0x1f 1
# 'tes[t-]' 'tes[t-]'; no match
tes[t-] test[t-] 0x0 1
tes[t-] test[t-] 0x1f 1
# 'test[/-/]' 'test[/-/]'; no match
test[/-/] test/-/ 0x0 1
test[/-/] test/-/ 0x1f 1
# 'test[\/-/]' 'test[/-/]'; no match
test[\/-/] test/-/ 0x0 1
test[\/-/] test/-/ 0x1f 1
# 'test[/-\/]' 'test[/-/]'; no match
test[/-\/] test/-/ 0x0 1
test[/-\/] test/-/ 0x1f 1
# 'test[/-/]' 'test/'; no match if APR_FNM_PATHNAME
test[/-/] test/ 0x0 0
test[/-/] test/ 0x2 1
test[/-/] test/ 0x1d 0
# 'test[\/-/]' 'test/'; no match if APR_FNM_PATHNAME
test[\/-/] test/ 0x0 0
test[\/-/] test/ 0x2 1
test[\/-/] test/ 0x1d 0
# 'test[/-\/]' 'test/'; no match if APR_FNM_PATHNAME
test[/-\/] test/ 0x0 0
test[/-\/] test/ 0x2 1
test[/-\/] test/ 0x1d 0
# '/test' 'test'; no match
/test test 0x0 1
/test test 0x1f 1
# 'test' '/test'; no match
test /test 0x0 1
test /test 0x1f 1
# 'test/' 'test'; no match
test/ test 0x0 1
test/ test 0x1f 1
# 'test' 'test/'; match if FNM_LEADING_DIR
test test/ 0x0 1
test test/ 0x17 1
test test/ 0x8 0
# '\/test' '/test'; match unless FNM_NOESCAPE
\/test /test 0x0 0
\/test /test 0x1 1
\/test /test 0x1e 0
# '*test' '/test'; match unless FNM_PATHNAME
*test /test 0x0 0
*test /test 0x2 1
*test /test 0x1d 0
# '/*/test' '/test'; no match
/*/test /test 0x0 1
/*/test /test 0x1f 1
# '/*/test' '/test/test'; match always
/*/test /test/test 0x0 0
/*/test /test/test 0x1f 0
# 'test/this' 'test/'; match never
test/this test/ 0x0 1
test/this test/ 0x1f 1
# 'test/' 'test/this'; match never
test/ test/this 0x0 1
test/ test/this 0x1f 1
# 'test*/this' 'test/this'; match always
test*/this test/this 0x0 0
test*/this test/this 0x1f 0
# 'test*/this' 'test/that'; match never
test*/this test/that 0x0 1
test*/this test/that 0x1f 1
# 'test/*this' 'test/this'; match always
test/*this test/this 0x0 0
test/*this test/this 0x1f 0
# '.*' '.this'; match always
.* .this 0x0 0
.* .this 0x1f 0
# '*' '.this'; fails if FNM_PERIOD
* .this 0x0 0
* .this 0x4 1
* .this 0x1b 0
# '?this' '.this'; fails if FNM_PERIOD
?this .this 0x0 0
?this .this 0x4 1
?this .this 0x1b 0
# '[.]this' '.this'; fails if FNM_PERIOD
[.]this .this 0x0 0
[.]this .this 0x4 1
[.]this .this 0x1b 0
# 'test/this' 'test/this'; match always
test/this test/this 0x0 0
test/this test/this 0x1f 0
# 'test?this' 'test/this'; fails if FNM_PATHNAME
test?this test/this 0x0 0
test?this test/this 0x2 1
test?this test/this 0x1d 0
# 'test*this' 'test/this'; fails if FNM_PATHNAME
test*this test/this 0x0 0
test*this test/this 0x2 1
test*this test/this 0x1d 0
# 'test[/]this' 'test/this'; fails if FNM_PATHNAME
test[/]this test/this 0x0 0
test[/]this test/this 0x2 1
test[/]this test/this 0x1d 0
# 'test/.*' 'test/.this'; match always
test/.* test/.this 0x0 0
test/.* test/.this 0x1f 0
# 'test/*' 'test/.this'; fails if FNM_PERIOD and FNM_PATHNAME
test/* test/.this 0x0 0
test/* test/.this 0x6 1
test/* test/.this 0x19 0
# 'test/?' 'test/.this'; fails if FNM_PERIOD and FNM_PATHNAME
test/?this test/.this 0x0 0
test/?this test/.this 0x6 1
test/?this test/.this 0x19 0
# 'test/[.]this' 'test/.this'; fails if FNM_PERIOD and FNM_PATHNAME
test/[.]this test/.this 0x0 0
test/[.]this test/.this 0x6 1
test/[.]this test/.this 0x19 0

/*========================================================*/
/* == ./src/usr.bin/cvs/file.c == */
/*	$OpenBSD: file.c,v 1.276 2023/08/11 04:48:14 guenther Exp $	*/
/*
 * Copyright (c) 2006 Joris Vink <joris@openbsd.org>
 * Copyright (c) 2004 Jean-Francois Brousseau <jfb@openbsd.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <libgen.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "atomicio.h"
#include "cvs.h"
#include "remote.h"

#define CVS_IGN_STATIC	0x01	/* pattern is static, no need to glob */

#define CVS_CHAR_ISMETA(c)	((c == '*') || (c == '?') || (c == '['))

extern int print_stdout;
extern int build_dirs;

/*
 * Standard patterns to ignore.
 */
static const char *cvs_ign_std[] = {
	".",
	"..",
	"*.o",
	"*.a",
	"*.bak",
	"*.orig",
	"*.rej",
	"*.old",
	"*.exe",
	"*.depend",
	"*.obj",
	"*.elc",
	"*.ln",
	"*.olb",
	"CVS",
	"core",
	"cvslog*",
	"*.core",
	".#*",
	"*~",
	"_$*",
	"*$",
};

char *cvs_directory_tag = NULL;
struct ignore_head cvs_ign_pats;
struct ignore_head dir_ign_pats;
struct ignore_head checkout_ign_pats;

RB_GENERATE(cvs_flisthead, cvs_filelist, flist, cvs_filelist_cmp);

void
cvs_file_init(void)
{
	int i;
	FILE *ifp;
	char path[PATH_MAX], buf[MAXNAMLEN];

	TAILQ_INIT(&cvs_ign_pats);
	TAILQ_INIT(&dir_ign_pats);
	TAILQ_INIT(&checkout_ign_pats);

	/* standard patterns to ignore */
	for (i = 0; i < (int)(sizeof(cvs_ign_std)/sizeof(char *)); i++)
		cvs_file_ignore(cvs_ign_std[i], &cvs_ign_pats);

	if (cvs_homedir == NULL)
		return;

	/* read the cvsignore file in the user's home directory, if any */
	(void)xsnprintf(path, PATH_MAX, "%s/.cvsignore", cvs_homedir);

	ifp = fopen(path, "r");
	if (ifp == NULL) {
		if (errno != ENOENT)
			cvs_log(LP_ERRNO,
			    "failed to open user's cvsignore file `%s'", path);
	} else {
		while (fgets(buf, MAXNAMLEN, ifp) != NULL) {
			buf[strcspn(buf, "\n")] = '\0';
			if (buf[0] == '\0')
				continue;

			cvs_file_ignore(buf, &cvs_ign_pats);
		}

		(void)fclose(ifp);
	}
}

void
cvs_file_ignore(const char *pat, struct ignore_head *list)
{
	char *cp;
	size_t len;
	struct cvs_ignpat *ip;

	ip = xmalloc(sizeof(*ip));
	len = strlcpy(ip->ip_pat, pat, sizeof(ip->ip_pat));
	if (len >= sizeof(ip->ip_pat))
		fatal("cvs_file_ignore: truncation of pattern '%s'", pat);

	/* check if we will need globbing for that pattern */
	ip->ip_flags = CVS_IGN_STATIC;
	for (cp = ip->ip_pat; *cp != '\0'; cp++) {
		if (CVS_CHAR_ISMETA(*cp)) {
			ip->ip_flags &= ~CVS_IGN_STATIC;
			break;
		}
	}

	TAILQ_INSERT_TAIL(list, ip, ip_list);
}

int
cvs_file_chkign(const char *file)
{
	int flags;
	struct cvs_ignpat *ip;

	flags = FNM_PERIOD;
	if (cvs_nocase)
		flags |= FNM_CASEFOLD;

	TAILQ_FOREACH(ip, &cvs_ign_pats, ip_list) {
		if (ip->ip_flags & CVS_IGN_STATIC) {
			if (cvs_file_cmpname(file, ip->ip_pat) == 0)
				return (1);
		} else if (fnmatch(ip->ip_pat, file, flags) == 0)
			return (1);
	}

	TAILQ_FOREACH(ip, &dir_ign_pats, ip_list) {
		if (ip->ip_flags & CVS_IGN_STATIC) {
			if (cvs_file_cmpname(file, ip->ip_pat) == 0)
				return (1);
		} else if (fnmatch(ip->ip_pat, file, flags) == 0)
			return (1);
	}

	TAILQ_FOREACH(ip, &checkout_ign_pats, ip_list) {
		if (ip->ip_flags & CVS_IGN_STATIC) {
			if (cvs_file_cmpname(file, ip->ip_pat) == 0)
				return (1);
		} else if (fnmatch(ip->ip_pat, file, flags) == 0)
			return (1);
	}

	return (0);
}

void
cvs_file_run(int argc, char **argv, struct cvs_recursion *cr)
{
	int i;
	struct cvs_flisthead fl;

	RB_INIT(&fl);

	for (i = 0; i < argc; i++) {
		STRIP_SLASH(argv[i]);
		cvs_file_get(argv[i], FILE_USER_SUPPLIED, &fl, 0);
	}

	cvs_file_walklist(&fl, cr);
	cvs_file_freelist(&fl);
}

struct cvs_filelist *
cvs_file_get(char *name, int flags, struct cvs_flisthead *fl, int type)
{
	char *p;
	struct cvs_filelist *l, find;

	for (p = name; p[0] == '.' && p[1] == '/';)
		p += 2;

	find.file_path = p;
	l = RB_FIND(cvs_flisthead, fl, &find);
	if (l != NULL)
		return (l);

	l = xmalloc(sizeof(*l));
	l->file_path = xstrdup(p);
	l->flags = flags;
	l->type = type;

	RB_INSERT(cvs_flisthead, fl, l);
	return (l);
}

struct cvs_file *
cvs_file_get_cf(const char *d, const char *f, const char *fpath, int fd,
	int type, int flags)
{
	const char *p;
	struct cvs_file *cf;

	for (p = fpath; p[0] == '.' && p[1] == '/';)
		p += 2;

	cf = xcalloc(1, sizeof(*cf));

	cf->file_name = xstrdup(f);
	cf->file_wd = xstrdup(d);
	cf->file_path = xstrdup(p);
	cf->fd = fd;
	cf->repo_fd = -1;
	cf->file_type = type;
	cf->file_status = 0;
	cf->file_flags = flags;
	cf->in_attic = 0;
	cf->file_ent = NULL;

	if (cf->fd != -1)
		cf->file_flags |= FILE_ON_DISK;

	if (cvsroot_is_remote() || cvs_server_active == 1)
		cvs_validate_directory(cf->file_path);

	return (cf);
}

void
cvs_file_walklist(struct cvs_flisthead *fl, struct cvs_recursion *cr)
{
	int fd, type;
	struct stat st;
	struct cvs_file *cf;
	struct cvs_filelist *l, *nxt;
	char *d, dbuf[PATH_MAX], *f, fbuf[PATH_MAX];
	char repo[PATH_MAX], fpath[PATH_MAX];

	for (l = RB_MIN(cvs_flisthead, fl); l != NULL; l = nxt) {
		if (cvs_quit)
			fatal("received signal %d", sig_received);

		cvs_log(LP_TRACE, "cvs_file_walklist: element '%s'",
		    l->file_path);

		if (strlcpy(fbuf, l->file_path, sizeof(fbuf)) >= sizeof(fbuf))
			fatal("cvs_file_walklist: truncation");
		if ((f = basename(fbuf)) == NULL)
			fatal("cvs_file_walklist: basename failed");

		if (strlcpy(dbuf, l->file_path, sizeof(dbuf)) >= sizeof(dbuf))
			fatal("cvs_file_walklist: truncation");
		if ((d = dirname(dbuf)) == NULL)
			fatal("cvs_file_walklist: dirname failed");

		type = l->type;
		if ((fd = open(l->file_path, O_RDONLY)) != -1) {
			if (type == 0) {
				if (fstat(fd, &st) == -1) {
					cvs_log(LP_ERRNO, "%s", l->file_path);
					(void)close(fd);
					goto next;
				}

				if (S_ISDIR(st.st_mode))
					type = CVS_DIR;
				else if (S_ISREG(st.st_mode))
					type = CVS_FILE;
				else {
					cvs_log(LP_ERR,
					    "ignoring bad file type for %s",
					    l->file_path);
					(void)close(fd);
					goto next;
				}
			}
		} else if (cvsroot_is_local()) {
			/*
			 * During checkout -p, do not use any locally
			 * available directories.
			 */
			if ((cmdp->cmd_flags & CVS_USE_WDIR) &&
			    (cvs_cmdop != CVS_OP_CHECKOUT || !print_stdout))
				if (stat(d, &st) == -1) {
					cvs_log(LP_ERRNO, "%s", d);
					goto next;
				}

			cvs_get_repository_path(d, repo, PATH_MAX);
			(void)xsnprintf(fpath, PATH_MAX, "%s/%s",
			    repo, f);

			if ((fd = open(fpath, O_RDONLY)) == -1) {
				strlcat(fpath, RCS_FILE_EXT, PATH_MAX);
				fd = open(fpath, O_RDONLY);
			}

			if (fd != -1 && type == 0) {
				if (fstat(fd, &st) == -1)
					fatal("cvs_file_walklist: %s: %s",
					     fpath, strerror(errno));

				if (S_ISDIR(st.st_mode))
					type = CVS_DIR;
				else if (S_ISREG(st.st_mode))
					type = CVS_FILE;
				else {
					cvs_log(LP_ERR,
					    "ignoring bad file type for %s",
					    l->file_path);
					(void)close(fd);
					goto next;
				}

				/* this file is not in our working copy yet */
				(void)close(fd);
				fd = -1;
			} else if (fd != -1) {
				close(fd);
				fd = -1;
			}
		}

		cf = cvs_file_get_cf(d, f, l->file_path, fd, type, l->flags);
		if (cf->file_type == CVS_DIR) {
			cvs_file_walkdir(cf, cr);
		} else {
			if (l->flags & FILE_USER_SUPPLIED) {
				cvs_parse_tagfile(cf->file_wd,
				    &cvs_directory_tag, NULL, NULL);

				if (cvs_directory_tag == NULL &&
				    cvs_specified_tag != NULL)
					cvs_directory_tag =
					    xstrdup(cvs_specified_tag);

				if (cvsroot_is_local()) {
					cvs_get_repository_path(cf->file_wd,
					    repo, PATH_MAX);
					cvs_repository_lock(repo,
					    (cmdp->cmd_flags & CVS_LOCK_REPO));
				}
			}

			if (cr->fileproc != NULL)
				cr->fileproc(cf);

			if (l->flags & FILE_USER_SUPPLIED) {
				if (cvsroot_is_local() &&
				    (cmdp->cmd_flags & CVS_LOCK_REPO)) {
					cvs_repository_unlock(repo);
				}
				free(cvs_directory_tag);
				cvs_directory_tag = NULL;
			}
		}

		cvs_file_free(cf);

next:
		nxt = RB_NEXT(cvs_flisthead, fl, l);
	}
}

void
cvs_file_walkdir(struct cvs_file *cf, struct cvs_recursion *cr)
{
	int l, type;
	FILE *fp;
	int nbytes;
	size_t bufsize;
	struct stat st;
	struct dirent *dp;
	struct cvs_ent *ent;
	struct cvs_ignpat *ip;
	struct cvs_ent_line *line;
	struct cvs_flisthead fl, dl;
	CVSENTRIES *entlist;
	char *buf, *ebuf, *cp, repo[PATH_MAX], fpath[PATH_MAX];

	cvs_log(LP_TRACE, "cvs_file_walkdir(%s)", cf->file_path);

	if (cr->enterdir != NULL)
		cr->enterdir(cf);

	if (cr->fileproc != NULL)
		cr->fileproc(cf);

	if (cf->file_status == FILE_SKIP)
		return;

	/*
	 * If this is a repository-only command, do not touch any
	 * locally available directories or try to create them.
	 */
	if (!(cmdp->cmd_flags & CVS_USE_WDIR)) {
		RB_INIT(&fl);
		RB_INIT(&dl);
		goto walkrepo;
	}

	/*
	 * If we do not have an admin directory inside here, dont bother,
	 * unless we are running export or import.
	 */
	(void)xsnprintf(fpath, PATH_MAX, "%s/%s", cf->file_path,
	    CVS_PATH_CVSDIR);

	l = stat(fpath, &st);
	if (cvs_cmdop != CVS_OP_EXPORT && cvs_cmdop != CVS_OP_IMPORT &&
	    (l == -1 || (l == 0 && !S_ISDIR(st.st_mode)))) {
		return;
	}

	cvs_parse_tagfile(cf->file_path, &cvs_directory_tag, NULL, NULL);

	/*
	 * check for a local .cvsignore file
	 */
	(void)xsnprintf(fpath, PATH_MAX, "%s/.cvsignore", cf->file_path);

	if ((fp = fopen(fpath, "r")) != NULL) {
		while (fgets(fpath, PATH_MAX, fp) != NULL) {
			fpath[strcspn(fpath, "\n")] = '\0';
			if (fpath[0] == '\0')
				continue;

			cvs_file_ignore(fpath, &dir_ign_pats);
		}

		(void)fclose(fp);
	}

	if (fstat(cf->fd, &st) == -1)
		fatal("cvs_file_walkdir: %s %s", cf->file_path,
		    strerror(errno));

	if ((uintmax_t)st.st_size > SIZE_MAX)
		fatal("cvs_file_walkdir: %s: file size too big", cf->file_name);

	bufsize = (st.st_size > st.st_blksize) ? st.st_size : st.st_blksize;

	buf = xmalloc(bufsize);
	RB_INIT(&fl);
	RB_INIT(&dl);

	while ((nbytes = getdents(cf->fd, buf, bufsize)) > 0) {
		ebuf = buf + nbytes;
		cp = buf;

		while (cp < ebuf) {
			dp = (struct dirent *)cp;
			if (!strcmp(dp->d_name, ".") ||
			    !strcmp(dp->d_name, "..") ||
			    !strcmp(dp->d_name, CVS_PATH_CVSDIR) ||
			    dp->d_fileno == 0) {
				cp += dp->d_reclen;
				continue;
			}

			(void)xsnprintf(fpath, PATH_MAX, "%s/%s",
			    cf->file_path, dp->d_name);

			if (cvs_file_chkign(dp->d_name) &&
			    cvs_cmdop != CVS_OP_RLOG &&
			    cvs_cmdop != CVS_OP_RTAG) {
				if (cvs_cmdop == CVS_OP_IMPORT)
					cvs_import_ignored(fpath);
				cp += dp->d_reclen;
				continue;
			}

			/*
			 * nfs and afs will show d_type as DT_UNKNOWN
			 * for files and/or directories so when we encounter
			 * this we call lstat() on the path to be sure.
			 */
			if (dp->d_type == DT_UNKNOWN) {
				if (lstat(fpath, &st) == -1)
					fatal("'%s': %s", fpath,
					    strerror(errno));

				switch (st.st_mode & S_IFMT) {
				case S_IFDIR:
					type = CVS_DIR;
					break;
				case S_IFREG:
					type = CVS_FILE;
					break;
				default:
					type = FILE_SKIP;
					break;
				}
			} else {
				switch (dp->d_type) {
				case DT_DIR:
					type = CVS_DIR;
					break;
				case DT_REG:
					type = CVS_FILE;
					break;
				default:
					type = FILE_SKIP;
					break;
				}
			}

			if (type == FILE_SKIP) {
				if (verbosity > 1) {
					cvs_log(LP_NOTICE, "ignoring `%s'",
					    dp->d_name);
				}
				cp += dp->d_reclen;
				continue;
			}

			switch (type) {
			case CVS_DIR:
				if (cr->flags & CR_RECURSE_DIRS)
					cvs_file_get(fpath, 0, &dl, CVS_DIR);
				break;
			case CVS_FILE:
				cvs_file_get(fpath, 0, &fl, CVS_FILE);
				break;
			default:
				fatal("type %d unknown, shouldn't happen",
				    type);
			}

			cp += dp->d_reclen;
		}
	}

	if (nbytes == -1)
		fatal("cvs_file_walkdir: %s %s", cf->file_path,
		    strerror(errno));

	free(buf);

	while ((ip = TAILQ_FIRST(&dir_ign_pats)) != NULL) {
		TAILQ_REMOVE(&dir_ign_pats, ip, ip_list);
		free(ip);
	}

	entlist = cvs_ent_open(cf->file_path);
	TAILQ_FOREACH(line, &(entlist->cef_ent), entries_list) {
		ent = cvs_ent_parse(line->buf);

		(void)xsnprintf(fpath, PATH_MAX, "%s/%s", cf->file_path,
		    ent->ce_name);

		if (!(cr->flags & CR_RECURSE_DIRS) &&
		    ent->ce_type == CVS_ENT_DIR)
			continue;
		if (ent->ce_type == CVS_ENT_DIR)
			cvs_file_get(fpath, 0, &dl, CVS_DIR);
		else if (ent->ce_type == CVS_ENT_FILE)
			cvs_file_get(fpath, 0, &fl, CVS_FILE);

		cvs_ent_free(ent);
	}

walkrepo:
	if (cvsroot_is_local()) {
		cvs_get_repository_path(cf->file_path, repo, PATH_MAX);
		cvs_repository_lock(repo, (cmdp->cmd_flags & CVS_LOCK_REPO));
	}

	if (cr->flags & CR_REPO) {
		xsnprintf(fpath, sizeof(fpath), "%s/%s", cf->file_path,
		    CVS_PATH_STATICENTRIES);

		if (!(cmdp->cmd_flags & CVS_USE_WDIR) ||
		    stat(fpath, &st) == -1 || build_dirs == 1)
			cvs_repository_getdir(repo, cf->file_path, &fl, &dl,
			    (cr->flags & CR_RECURSE_DIRS) ?
			    REPOSITORY_DODIRS : 0);
	}

	cvs_file_walklist(&fl, cr);
	cvs_file_freelist(&fl);

	if (cvsroot_is_local() && (cmdp->cmd_flags & CVS_LOCK_REPO))
		cvs_repository_unlock(repo);

	if (cvs_directory_tag != NULL && cmdp->cmd_flags & CVS_USE_WDIR) {
		cvs_write_tagfile(cf->file_path, cvs_directory_tag, NULL);
		free(cvs_directory_tag);
		cvs_directory_tag = NULL;
	}

	cvs_file_walklist(&dl, cr);
	cvs_file_freelist(&dl);

	if (cr->leavedir != NULL)
		cr->leavedir(cf);
}

void
cvs_file_freelist(struct cvs_flisthead *fl)
{
	struct cvs_filelist *f, *nxt;

	for (f = RB_MIN(cvs_flisthead, fl); f != NULL; f = nxt) {
		nxt = RB_NEXT(cvs_flisthead, fl, f);
		RB_REMOVE(cvs_flisthead, fl, f);
		free(f->file_path);
		free(f);
	}
}

void
cvs_file_classify(struct cvs_file *cf, const char *tag)
{
	size_t len;
	struct stat st;
	BUF *b1, *b2;
	int server_has_file, notag;
	int rflags, ismodified, rcsdead;
	CVSENTRIES *entlist = NULL;
	const char *state;
	char repo[PATH_MAX], rcsfile[PATH_MAX];

	cvs_log(LP_TRACE, "cvs_file_classify(%s, %s)", cf->file_path,
	    (tag != NULL) ? tag : "none");

	if (!strcmp(cf->file_path, ".")) {
		cf->file_status = FILE_UPTODATE;
		return;
	}

	cvs_get_repository_path(cf->file_wd, repo, PATH_MAX);
	(void)xsnprintf(rcsfile, PATH_MAX, "%s/%s",
	    repo, cf->file_name);

	if (cf->file_type == CVS_FILE) {
		len = strlcat(rcsfile, RCS_FILE_EXT, PATH_MAX);
		if (len >= PATH_MAX)
			fatal("cvs_file_classify: truncation");
	}

	cf->file_rpath = xstrdup(rcsfile);

	if (cmdp->cmd_flags & CVS_USE_WDIR) {
		entlist = cvs_ent_open(cf->file_wd);
		cf->file_ent = cvs_ent_get(entlist, cf->file_name);
	} else
		cf->file_ent = NULL;

	if (cf->file_ent != NULL) {
		if (cvs_specified_tag == NULL)
			tag = cf->file_ent->ce_tag;

		if (cf->file_flags & FILE_ON_DISK &&
		    cf->file_ent->ce_type == CVS_ENT_FILE &&
		    cf->file_type == CVS_DIR && tag != NULL) {
			cf->file_status = FILE_SKIP;
			return;
		}

		if (cf->file_flags & FILE_ON_DISK &&
		    cf->file_ent->ce_type == CVS_ENT_DIR &&
		    cf->file_type == CVS_FILE && tag != NULL) {
			cf->file_status = FILE_SKIP;
			return;
		}

		if (cf->file_flags & FILE_INSIDE_ATTIC &&
		    cf->file_ent->ce_type == CVS_ENT_DIR &&
		    cf->file_type != CVS_DIR) {
			cf->file_status = FILE_SKIP;
			return;
		}

		if (cf->file_flags & FILE_ON_DISK &&
		    cf->file_ent->ce_type == CVS_ENT_DIR &&
		    cf->file_type != CVS_DIR)
			fatal("%s is supposed to be a directory, but it is not",
			    cf->file_path);
		if (cf->file_flags & FILE_ON_DISK &&
		    cf->file_ent->ce_type == CVS_ENT_FILE &&
		    cf->file_type != CVS_FILE)
			fatal("%s is supposed to be a file, but it is not",
			    cf->file_path);
	}

	if (cf->file_type == CVS_DIR) {
		if (!(cmdp->cmd_flags & CVS_USE_WDIR))
			cf->file_status = FILE_UPTODATE;
		else if (cf->fd == -1 && stat(rcsfile, &st) != -1)
			cf->file_status = DIR_CREATE;
		else if (cf->file_ent != NULL || cvs_cmdop == CVS_OP_RLOG ||
		    cvs_cmdop == CVS_OP_RTAG)
			cf->file_status = FILE_UPTODATE;
		else
			cf->file_status = FILE_UNKNOWN;

		return;
	}

	rflags = RCS_READ;
	switch (cvs_cmdop) {
	case CVS_OP_COMMIT:
	case CVS_OP_TAG:
	case CVS_OP_RTAG:
		rflags = RCS_WRITE;
		break;
	case CVS_OP_ADMIN:
	case CVS_OP_IMPORT:
	case CVS_OP_LOG:
	case CVS_OP_RLOG:
		rflags |= RCS_PARSE_FULLY;
		break;
	default:
		break;
	}

	cf->repo_fd = open(cf->file_rpath, O_RDONLY);
	if (cf->repo_fd != -1) {
		cf->file_rcs = rcs_open(cf->file_rpath, cf->repo_fd, rflags);
		if (cf->file_rcs == NULL)
			fatal("cvs_file_classify: failed to parse RCS");
	} else {
		(void)xsnprintf(rcsfile, PATH_MAX, "%s/%s/%s%s",
		     repo, CVS_PATH_ATTIC, cf->file_name, RCS_FILE_EXT);

		cf->repo_fd = open(rcsfile, O_RDONLY);
		if (cf->repo_fd != -1) {
			free(cf->file_rpath);
			cf->file_rpath = xstrdup(rcsfile);
			cf->file_rcs = rcs_open(cf->file_rpath,
			    cf->repo_fd, rflags);
			if (cf->file_rcs == NULL)
				fatal("cvs_file_classify: failed to parse RCS");
			cf->in_attic = 1;
		} else {
			cf->file_rcs = NULL;
		}
	}

	notag = 0;
	cf->file_flags |= FILE_HAS_TAG;
	if (tag != NULL && cf->file_rcs != NULL) {
		if ((cf->file_rcsrev = rcs_translate_tag(tag, cf->file_rcs))
		    == NULL) {
			cf->file_rcsrev = rcs_translate_tag(NULL, cf->file_rcs);
			if (cf->file_rcsrev != NULL) {
				notag = 1;
				cf->file_flags &= ~FILE_HAS_TAG;
			}
		}
	} else if (cf->file_ent != NULL && cf->file_ent->ce_tag != NULL) {
		cf->file_rcsrev = rcsnum_alloc();
		rcsnum_cpy(cf->file_ent->ce_rev, cf->file_rcsrev, 0);
	} else if (cf->file_rcs != NULL) {
		cf->file_rcsrev = rcs_translate_tag(NULL, cf->file_rcs);
	} else {
		cf->file_rcsrev = NULL;
	}

	ismodified = rcsdead = 0;
	if ((cf->file_flags & FILE_ON_DISK) && cf->file_ent != NULL) {
		if (fstat(cf->fd, &st) == -1)
			fatal("cvs_file_classify: %s", strerror(errno));

		if (st.st_mtime != cf->file_ent->ce_mtime)
			ismodified = 1;
	}

	server_has_file = 0;
	if (cvs_server_active == 1 && cf->file_ent != NULL &&
	    cf->file_ent->ce_mtime == CVS_SERVER_UPTODATE) {
		server_has_file = 1;
		ismodified = 0;
	}

	if ((server_has_file == 1) || (cf->fd != -1))
		cf->file_flags |= FILE_ON_DISK;

	if (ismodified == 1 &&
	    (cf->file_flags & FILE_ON_DISK) && cf->file_rcs != NULL &&
	    cf->file_ent != NULL && !RCSNUM_ISBRANCH(cf->file_ent->ce_rev) &&
	    cf->file_ent->ce_status != CVS_ENT_ADDED) {
		b1 = rcs_rev_getbuf(cf->file_rcs, cf->file_ent->ce_rev, 0);
		b2 = buf_load_fd(cf->fd);

		if (buf_differ(b1, b2))
			ismodified = 1;
		else
			ismodified = 0;
		buf_free(b1);
		buf_free(b2);
	}

	if (cf->file_rcs != NULL && cf->file_rcsrev != NULL &&
	    !RCSNUM_ISBRANCH(cf->file_rcsrev)) {
		state = rcs_state_get(cf->file_rcs, cf->file_rcsrev);
		if (state == NULL)
			fatal("failed to get state for HEAD for %s",
			    cf->file_path);
		if (!strcmp(state, RCS_STATE_DEAD))
			rcsdead = 1;

		if (cvs_specified_date == -1 && cvs_directory_date == -1 &&
		    tag == NULL && cf->in_attic &&
		    !RCSNUM_ISBRANCHREV(cf->file_rcsrev))
			rcsdead = 1;

		cf->file_rcs->rf_dead = rcsdead;
	}

	/*
	 * 10 Sin
	 * 20 Goto hell
	 * (I welcome you if-else hell)
	 */
	if (cf->file_ent == NULL) {
		if (cf->file_rcs == NULL) {
			if (!(cf->file_flags & FILE_ON_DISK)) {
				cvs_log(LP_NOTICE,
				    "nothing known about '%s'",
				    cf->file_path);
			}

			cf->file_status = FILE_UNKNOWN;
		} else if (rcsdead == 1 || !(cf->file_flags & FILE_HAS_TAG)) {
			if (!(cf->file_flags & FILE_ON_DISK)) {
				cf->file_status = FILE_UPTODATE;
			} else if (cvs_cmdop != CVS_OP_ADD) {
				cf->file_status = FILE_UNKNOWN;
			}
		} else if (notag == 0 && cf->file_rcsrev != NULL) {
			cf->file_status = FILE_CHECKOUT;
		} else {
			cf->file_status = FILE_UPTODATE;
		}

		return;
	}

	switch (cf->file_ent->ce_status) {
	case CVS_ENT_ADDED:
		if (!(cf->file_flags & FILE_ON_DISK)) {
			if (cvs_cmdop != CVS_OP_REMOVE) {
				cvs_log(LP_NOTICE,
				    "warning: new-born %s has disappeared",
				    cf->file_path);
			}
			cf->file_status = FILE_REMOVE_ENTRY;
		} else if (cf->file_rcs == NULL || rcsdead == 1 ||
		    !(cf->file_flags & FILE_HAS_TAG)) {
			cf->file_status = FILE_ADDED;
		} else {
			cvs_log(LP_NOTICE,
			    "conflict: %s already created by others",
			    cf->file_path);
			cf->file_status = FILE_CONFLICT;
		}
		break;
	case CVS_ENT_REMOVED:
		if (cf->file_flags & FILE_ON_DISK) {
			cvs_log(LP_NOTICE,
			    "%s should be removed but is still there",
			    cf->file_path);
			cf->file_status = FILE_REMOVED;
		} else if (cf->file_rcs == NULL || rcsdead == 1) {
			cf->file_status = FILE_REMOVE_ENTRY;
		} else {
			if (rcsnum_differ(cf->file_ent->ce_rev,
			    cf->file_rcsrev) && cvs_cmdop != CVS_OP_ADD) {
				cvs_log(LP_NOTICE,
				    "conflict: removed %s was modified"
				    " by a second party",
				    cf->file_path);
				cf->file_status = FILE_CONFLICT;
			} else {
				cf->file_status = FILE_REMOVED;
			}
		}
		break;
	case CVS_ENT_REG:
		if (cf->file_rcs == NULL || cf->file_rcsrev == NULL ||
		    rcsdead == 1 || (reset_tag == 1 && cf->in_attic == 1) ||
		    (notag == 1 && tag != NULL)) {
			if (!(cf->file_flags & FILE_ON_DISK)) {
				cvs_log(LP_NOTICE,
				    "warning: %s's entry exists but"
				    " is no longer in the repository,"
				    " removing entry",
				     cf->file_path);
				cf->file_status = FILE_REMOVE_ENTRY;
			} else {
				if (ismodified) {
					cvs_log(LP_NOTICE,
					    "conflict: %s is no longer "
					    "in the repository but is "
					    "locally modified",
					    cf->file_path);
					if (cvs_cmdop == CVS_OP_COMMIT)
						cf->file_status = FILE_UNLINK;
					else
						cf->file_status = FILE_CONFLICT;
				} else if (cvs_cmdop != CVS_OP_IMPORT) {
					cvs_log(LP_NOTICE,
					    "%s is no longer in the "
					    "repository",
					    cf->file_path);

					cf->file_status = FILE_UNLINK;
				}
			}
		} else if (cf->file_rcsrev == NULL) {
			cf->file_status = FILE_UNLINK;
		} else {
			if (!(cf->file_flags & FILE_ON_DISK)) {
				if (cvs_cmdop != CVS_OP_REMOVE) {
					cvs_log(LP_NOTICE,
					    "warning: %s was lost",
					    cf->file_path);
				}
				cf->file_status = FILE_LOST;
			} else {
				if (ismodified == 1)
					cf->file_status = FILE_MODIFIED;
				else
					cf->file_status = FILE_UPTODATE;
				if (rcsnum_differ(cf->file_ent->ce_rev,
				    cf->file_rcsrev)) {
					if (cf->file_status == FILE_MODIFIED)
						cf->file_status = FILE_MERGE;
					else
						cf->file_status = FILE_PATCH;
				}
			}
		}
		break;
	case CVS_ENT_UNKNOWN:
		if (cvs_server_active != 1)
			fatal("server-side questionable in local mode?");
		cf->file_status = FILE_UNKNOWN;
		break;
	default:
		break;
	}
}

void
cvs_file_free(struct cvs_file *cf)
{
	free(cf->file_name);
	free(cf->file_wd);
	free(cf->file_path);
	free(cf->file_rcsrev);
	free(cf->file_rpath);
	if (cf->file_ent != NULL)
		cvs_ent_free(cf->file_ent);
	if (cf->file_rcs != NULL)
		rcs_close(cf->file_rcs);
	if (cf->fd != -1)
		(void)close(cf->fd);
	if (cf->repo_fd != -1)
		(void)close(cf->repo_fd);
	free(cf);
}

int
cvs_file_cmpname(const char *name1, const char *name2)
{
	return (cvs_nocase == 0) ? (strcmp(name1, name2)) :
	    (strcasecmp(name1, name2));
}

int
cvs_file_cmp(const char *file1, const char *file2)
{
	struct stat stb1, stb2;
	int fd1, fd2, ret;

	ret = 0;

	if ((fd1 = open(file1, O_RDONLY|O_NOFOLLOW)) == -1)
		fatal("cvs_file_cmp: open: `%s': %s", file1, strerror(errno));
	if ((fd2 = open(file2, O_RDONLY|O_NOFOLLOW)) == -1)
		fatal("cvs_file_cmp: open: `%s': %s", file2, strerror(errno));

	if (fstat(fd1, &stb1) == -1)
		fatal("cvs_file_cmp: `%s': %s", file1, strerror(errno));
	if (fstat(fd2, &stb2) == -1)
		fatal("cvs_file_cmp: `%s': %s", file2, strerror(errno));

	if (stb1.st_size != stb2.st_size ||
	    (stb1.st_mode & S_IFMT) != (stb2.st_mode & S_IFMT)) {
		ret = 1;
		goto out;
	}

	if (S_ISBLK(stb1.st_mode) || S_ISCHR(stb1.st_mode)) {
		if (stb1.st_rdev != stb2.st_rdev)
			ret = 1;
		goto out;
	}

	if (S_ISREG(stb1.st_mode)) {
		void *p1, *p2;

		if ((uintmax_t)stb1.st_size > SIZE_MAX) {
			ret = 1;
			goto out;
		}

		if ((p1 = mmap(NULL, stb1.st_size, PROT_READ,
		    MAP_FILE, fd1, (off_t)0)) == MAP_FAILED)
			fatal("cvs_file_cmp: mmap failed");

		if ((p2 = mmap(NULL, stb1.st_size, PROT_READ,
		    MAP_FILE, fd2, (off_t)0)) == MAP_FAILED)
			fatal("cvs_file_cmp: mmap failed");

		madvise(p1, stb1.st_size, MADV_SEQUENTIAL);
		madvise(p2, stb1.st_size, MADV_SEQUENTIAL);

		ret = memcmp(p1, p2, stb1.st_size);

		(void)munmap(p1, stb1.st_size);
		(void)munmap(p2, stb1.st_size);
	}

out:
	(void)close(fd1);
	(void)close(fd2);

	return (ret);
}

int
cvs_file_copy(const char *from, const char *to)
{
	struct stat st;
	struct timespec ts[2];
	int src, dst, ret;

	ret = 0;

	cvs_log(LP_TRACE, "cvs_file_copy(%s,%s)", from, to);

	if (cvs_noexec == 1)
		return (0);

	if ((src = open(from, O_RDONLY)) == -1)
		fatal("cvs_file_copy: open: `%s': %s", from, strerror(errno));

	if (fstat(src, &st) == -1)
		fatal("cvs_file_copy: `%s': %s", from, strerror(errno));

	if (S_ISREG(st.st_mode)) {
		char *p;
		int saved_errno;

		if ((uintmax_t)st.st_size > SIZE_MAX) {
			ret = -1;
			goto out;
		}

		if ((dst = open(to, O_CREAT|O_TRUNC|O_WRONLY,
		    st.st_mode & (S_IRWXU|S_IRWXG|S_IRWXO))) == -1)
			fatal("cvs_file_copy: open `%s': %s",
			    to, strerror(errno));

		if ((p = mmap(NULL, st.st_size, PROT_READ,
		    MAP_FILE, src, (off_t)0)) == MAP_FAILED) {
			saved_errno = errno;
			(void)unlink(to);
			fatal("cvs_file_copy: mmap: %s", strerror(saved_errno));
		}

		madvise(p, st.st_size, MADV_SEQUENTIAL);

		if (atomicio(vwrite, dst, p, st.st_size) != (size_t)st.st_size) {
			saved_errno = errno;
			(void)unlink(to);
			fatal("cvs_file_copy: `%s': %s", from,
			    strerror(saved_errno));
		}

		(void)munmap(p, st.st_size);

		ts[0] = st.st_atim;
		ts[1] = st.st_mtim;

		if (futimens(dst, ts) == -1) {
			saved_errno = errno;
			(void)unlink(to);
			fatal("cvs_file_copy: futimes: %s",
			    strerror(saved_errno));
		}
		(void)close(dst);
	}
out:
	(void)close(src);

	return (ret);
}

int
cvs_filelist_cmp(struct cvs_filelist *f1, struct cvs_filelist *f2)
{
	return (strcmp(f1->file_path, f2->file_path));
}

#endif
