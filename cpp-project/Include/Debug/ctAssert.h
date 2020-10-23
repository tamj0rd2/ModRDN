////////////////////////////////////////////////////////////////////
// ctAssert.h
//
// Dominic Mathieu 
// 2000-10-24
//       (c) relic entertainment inc. 
//
//	*

#pragma once

#ifdef Assert
#undef Assert
#endif

#ifdef assert
#undef assert
#endif

/////////////////////////////////////////////////////////////////////
// ctAssert

// this is a compile-time assert, that generates no code in release build

// if you use it, expect these errors
//	   error C2514: 'COMPILE_TIME_ASSERT_FAILED<0>' : class has no constructors

template <bool b> class COMPILE_TIME_ASSERT_FAILED;

template <> class COMPILE_TIME_ASSERT_FAILED< true  >
{
};

#define ctAssert(n) COMPILE_TIME_ASSERT_FAILED< (n) >()

/////////////////////////////////////////////////////////////////////
// ctSizeof

// this is a compile-time sizeof that will print to the error message window

template <size_t n> class COMPILE_TIME_SIZEOF;

#define ctSizeof(n) COMPILE_TIME_SIZEOF< sizeof(n) >()
