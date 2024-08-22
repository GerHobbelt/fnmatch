#include "test.h"

#include "fnmatch.h"

typedef struct test_fnmatch_s {
	int result;
	const char* pattern;
	const char* string;
	int flags;
} test_fnmatch_t;

static const test_fnmatch_t _data[] = {

	/* fixed string */
	{ 0, "test", "test", FNM_PATHNAME },
	{ FNM_NOMATCH, "test", "tes", FNM_PATHNAME },
	{ FNM_NOMATCH, "test", "testa", FNM_PATHNAME },

	/* question mark */
	{ 0, "tes?", "test", FNM_PATHNAME },
	{ 0, "?est", "test", FNM_PATHNAME },
	{ FNM_NOMATCH, "test?", "test", FNM_PATHNAME },
	{ FNM_NOMATCH, "test?", "test/", FNM_PATHNAME },

	/* asterisk */
	{ 0, "t*t", "tt", FNM_PATHNAME },
	{ 0, "t*t", "test", FNM_PATHNAME },
	{ 0, "t*t", "ttt", FNM_PATHNAME },
	{ FNM_NOMATCH, "t*t", "tte", FNM_PATHNAME },

	/* brackets */
	{ 0, "t[a-e]st", "test", FNM_PATHNAME },
	{ FNM_NOMATCH, "t[a-d]st", "test", FNM_PATHNAME },
	{ FNM_NOMATCH, "t[A-Z]st", "test", FNM_PATHNAME },

	/* separators */
	{ 0, "/test/dir", "/test/dir", FNM_PATHNAME },
	{ FNM_NOMATCH, "/test", "/test/", FNM_PATHNAME },
	{ FNM_NOMATCH, "/test/", "/test", FNM_PATHNAME },

	/* combinations */
	{ 0, "/t*/?[a-z]r", "/test/dir", FNM_PATHNAME },
	{ 0, "/t*/?[a-z]r", "/t/dur", FNM_PATHNAME },
	{ FNM_NOMATCH, "/t*/?[a-z]r", "/t/xx", FNM_PATHNAME },
	{ FNM_NOMATCH, "/t*?r", "/t/r", FNM_PATHNAME },

};

TEST(test_fnmatch) {
	const size_t len = sizeof(_data) / sizeof(_data[0]);
	for (int i = 0; i < len; i++) {
		const test_fnmatch_t* data = &_data[i];
		char failmsg[2000];

		if (data->result == 0) {
			snprintf(failmsg, sizeof(failmsg), "`%s' didn't match `%s' (with flags 0x%02x) but should.",
								data->pattern, data->string, data->flags);

			ASSERTEQ_W_MSG(data->result, fnmatch(data->pattern, data->string, data->flags), failmsg);
		} else {
			snprintf(failmsg, sizeof(failmsg), "`%s' matched `%s' (with flags 0x%02x) but shouldn't.\n",
							data->pattern, data->string, data->flags);

			ASSERTEQ_W_MSG(data->result, fnmatch(data->pattern, data->string, data->flags), failmsg);
		}
	}
}

