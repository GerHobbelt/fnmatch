#ifndef _TEST_H_
#define _TEST_H_

#define __FZ_VALIDATE_BOOL_FOR_ASSERT_DEFINED__ 1
#define FZ_DO_NOT_VALIDATE_BOOL_FOR_ASSERT 1
#define GTEST_DONT_DEFINE_TEST 1
#include "gtest/gtest.h"

#include <string.h>
#include <stdint.h>

using namespace testing;

#define TEST(name)		GTEST_TEST(fnmatch_testsuite, name)

#define ASSERT(x) GTEST_EXPECT_TRUE_W_MSG(x,"`" #x "' is not true!")
#define ASSERTEQ(a,b) EXPECT_EQ(a,b)
#define ASSERTNE(a,b) EXPECT_NE(a,b)
#define ASSERTLT(a,b) EXPECT_LT(a,b)
#define ASSERTLE(a,b) EXPECT_LE(a,b)
#define ASSERTGT(a,b) EXPECT_GT(a,b)
#define ASSERTGE(a,b) EXPECT_GE(a,b)

#define ASSERTEQ_W_MSG(a,b,msg) EXPECT_EQ(a,b) << msg
#define ASSERTNE_W_MSG(a,b,msg) EXPECT_NE(a,b) << msg
#define ASSERTLT_W_MSG(a,b,msg) EXPECT_LT(a,b) << msg
#define ASSERTLE_W_MSG(a,b,msg) EXPECT_LE(a,b) << msg
#define ASSERTGT_W_MSG(a,b,msg) EXPECT_GT(a,b) << msg
#define ASSERTGE_W_MSG(a,b,msg) EXPECT_GE(a,b) << msg

#define ASSERTSTRCMP(a,op,b,msg) GTEST_ASSERT_TRUE(((a)==(b)) || ((a)!=NULL) && ((b)!=NULL) && strcmp((a),(b)) op 0) << msg
#define ASSERTSTREQ(a,b) ASSERTSTRCMP(a,==,b, "Strings `" #a "' and `" #b "' are not equal!")
#define ASSERTSTRLT(a,b) ASSERTSTRCMP(a,<,b, "String `" #a "' is not `less than' string `" #b "'!")
#define ASSERTSTRGT(a,b) ASSERTSTRCMP(a,>,b, "String `" #a "' is not `greater than' string `" #b "'!")

#endif
