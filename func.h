#pragma once

#define CAT_(A, B) A##B
#define CAT(A, B) CAT_(A, B)

#define STRINGIFY_2(a) #a
#define STRINGIFY(a) STRINGIFY_2(a)

#define SECTION(X) __pragma(code_seg(X STRINGIFY(__COUNTER__)))
