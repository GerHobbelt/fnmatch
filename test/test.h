#ifndef _TEST_H_
#define _TEST_H_

#include "gtest/gtest.h"

#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERTBASE(x,...) do if( !(x) ) { FAIL(__VA_ARGS__); } while( 0 )
#define ASSERT(x,...) ASSERTBASE(x,"`" #x "' is not true!\n" __VA_ARGS__)
#define ASSERTEQ(a,b,...) ASSERTBASE((a) == (b),"`" #a "' does not equal `" #b "'!\n" __VA_ARGS__)
#define ASSERTNE(a,b,...) ASSERTBASE((a) != (b),"`" #a "' does equal `" #b "'!\n" __VA_ARGS__)
#define ASSERTLT(a,b,...) ASSERTBASE((a) < (b),"`" #a "' is not less than `" #b "'!\n" __VA_ARGS__)
#define ASSERTLE(a,b,...) ASSERTBASE((a) <= (b),"`" #a "' is greater than `" #b "'!\n" __VA_ARGS__)
#define ASSERTGT(a,b,...) ASSERTBASE((a) > (b),"`" #a "' is not greater than`" #b "'!\n" __VA_ARGS__)
#define ASSERTGE(a,b,...) ASSERTBASE((a) >= (b),"`" #a "' is less than`" #b "'!\n" __VA_ARGS__)
#define ASSERTSTRCMP(a,op,b) (((a)==(b)) || ((a)!=NULL) && ((b)!=NULL) && strcmp((a),(b)) op 0)
#define ASSERTSTREQ(a,b,...) ASSERTBASE(ASSERTSTRCMP(a,==,b),"Strings `" #a "' and `" #b "' are not equal!\n" __VA_ARGS__)
#define ASSERTSTRLT(a,b,...) ASSERTBASE(ASSERTSTRCMP(a,<,b),"String `" #a "' is not `less than' string `" #b "'!\n" __VA_ARGS__)
#define ASSERTSTRGT(a,b,...) ASSERTBASE(ASSERTSTRCMP(a,>,b),"String `" #a "' is not `greater than' string `" #b "'!\n" __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
