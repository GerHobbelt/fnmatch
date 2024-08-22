/* == ./src/regress/lib/libc/fnmatch/fnm_test.c == */
/*	$OpenBSD: fnm_test.c,v 1.3 2019/01/25 00:19:26 millert Exp $	*/

/*
 * Public domain, 2008, Todd C. Miller <millert@openbsd.org>
 */

#include <fnmatch.h>

#include <stdio.h>
#include <stdlib.h>

static char* freadline(FILE* fp)
{
	size_t sz = 8192;
	char* line = malloc(sz + 1);
	if (!line) {
		fprintf(stderr, "out of memory\n");
		return NULL;
	}
	char *l = fgets(line, sz, fp);
	if (!l) {
		if (feof(fp)) {
			line[0] = 0;
			return NULL;
		}
		fprintf(stderr, "failed to read from file.\n");
		return NULL;
	}
	line[sz] = 0;
	return line;
}

static char* fparseln(FILE* fp, const char *delims)
{
	for (;;) {
		char* line = freadline(fp);
		if (!line)
			return NULL;
		char* s = line + strspn(line, " \t\r\n");
		s += strspn(s, delims);
		s += strspn(s, " \t\r\n");
		// was this line empty or merely a comment line? if so, ignore/skip:
		if (!*s) {
			free(line);
			continue;
		}
		memmove(line, s, strlen(s) + 1);
		return line;
	}
}

int
main(int argc, const char **argv)
{
	FILE *fp = stdin;
	char pattern[1024], string[1024];
	char *line;
	const char *delims = "#";
	int errors = 0;

	if (argc > 1) {
		if ((fp = fopen(argv[1], "r")) == NULL) {
			fprintf(stderr, "cannot open file: %s\n", argv[1]);
			return 1;
		}
	}

	/*
	 * Read in test file, which is formatted thusly:
	 *
	 * pattern string flags expected_result
	 *
	 * lines starting with '#' are comments
	 */
	for (;;) {
		int flags, got, want;

		line = fparseln(fp, delims);
		if (!line)
			break;
		got = sscanf(line, "%s %s 0x%x %d", pattern, string, &flags,
		    &want);
		if (got == EOF) {
			free(line);
			break;
		}
		if (got == 4) {
			got = fnmatch(pattern, string, flags);
			if (got != want) {
				fprintf(stderr, "Warning: %s %s %d: want %d, got %d", pattern,
				    string, flags, want, got);
				errors++;
			}
		} else {
			fprintf(stderr, "Warning: unrecognized line '%s'\n", line);
			errors++;
		}
		free(line);
	}

	if (errors > 0) {
		fprintf(stderr, "Total test error count: %d\n", errors);
		return 2;
	}

	return 0;
}
