#pragma once

#define COMPILER_GCC 0
#define COMPILER_CLANG 0

#if (defined(__GNUC__) || defined(_MSC_VER)) && defined(__clang__)
#    undef COMPILER_CLANG
#    define COMPILER_CLANG 1
#elif defined(__GNUC__)
#    undef COMPILER_GCC
#    define COMPILER_GCC 1
#else
#    error "Unsupported compiler!"
#endif
