/////////////////////////////////////////////////////////////////////
// $Id: //ICXP/Src/Shared/Cross/Debug/db.h#9 $
//     (c) Relic Entertainment 2002
//
//! \file
//! \brief		Main header of the debug system
//! \ingroup	Debug
//!
//! System: \ref Debug
//!
//! >

#pragma once

// we should not be using 3rd party assertions
#ifdef Assert
#undef Assert
#endif

#ifdef assert
#undef assert
#endif

//	MFC generated code uses ASSERT
#ifndef __AFX_H__
	#ifdef ASSERT
	#undef ASSERT
	#endif
#endif

/////////////////////////////////////////////////////////////////////
//

#ifndef RELIC_LIB
    #ifdef DEBUG_DLL
	    #define DEBUG_API __declspec(dllexport)
    #else
	    #define DEBUG_API __declspec(dllimport)
    #endif
#else
    #define DEBUG_API /**/
#endif // RELIC_LIB

/////////////////////////////////////////////////////////////////////
// stuff internal to this dll
// DO NOT USE THESE!!!!!

DEBUG_API void dbTracefAux( const char* format, ... );
DEBUG_API void dbFatalfAux( const char* format, ... );
DEBUG_API void dbWarningfAux( unsigned long id, const char* format, ... );

#if !defined( RELIC_DEBUG )
	extern DEBUG_API const char*	g_dbFilename;
	extern DEBUG_API unsigned long	g_dbLineNum;

	inline void dbTraceInfoAux( const char* filename, int line )
	{
		g_dbFilename = filename;
		g_dbLineNum = line;
	}

	inline bool dbAlwaysReallyTrueAux()
	{
		static bool t = true;
		return t;
	}

#else
	DEBUG_API void dbAssertAux ( const char* file, int line, const char* expr );
	DEBUG_API void dbPrintfAux ( const char* format, ... );
	DEBUG_API void dbPrintfAux1( const char* file, int line, const char* format, ... );
	DEBUG_API void dbTraceInfoAux( const char* filename, int line );
	DEBUG_API bool dbAlwaysReallyTrueAux();

	//lint -function(__assert,dbAssertAux)

#endif

/////////////////////////////////////////////////////////////////////
// structured exception handling (WIN32)

extern "C"
{
	//lint -e{761}
	typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;
}

/////////////////////////////////////////////////////////////////////
// utilities

//! use this in a macro to generate a unique variable name
#define UNIQUE_VAR(V) V##__FILE__##__LINE__

//! use this to eliminate compiler warning generated when formal parameters or
//! local variables are not declared
#define UNREF_P(P) (P)

/////////////////////////////////////////////////////////////////////
// debug output

typedef void (*dbStringOutputCB)( const char* lines[], size_t count, unsigned long id );

//! register a dbPrintf output handler,
//! doesn't do anything when NDEBUG is defined
DEBUG_API void dbOutputHandlerAdd( dbStringOutputCB outputCB );

//! remove a previously registered dbPrintf output handler,
//! doesn't do anything when NDEBUG is defined
DEBUG_API void dbOutputHandlerRmv( dbStringOutputCB outputCB );

//! register a dbTracef & dbFatalf output handler
DEBUG_API void dbTraceHandlerAdd( dbStringOutputCB outputCB );

//! remove a previously registered dbTracef & dbFatalf output handler
DEBUG_API void dbTraceHandlerRmv( dbStringOutputCB outputCB );

//! register a dbWarningf output handler
DEBUG_API void dbWarningHandlerAdd( unsigned long id, dbStringOutputCB outputCB );

//! remove a previously registered dbWarningf output handler
DEBUG_API void dbWarningHandlerRmv( unsigned long id, dbStringOutputCB outputCB );

//! outputs a string to all registered handlers,
//! compiled out when NDEBUG is defined,
//! NOT THREAD-SAFE: use dbPrintf# if you need THREAD-SAFE
#if defined( RELIC_DEBUG )
	#define dbPrintf				dbTraceInfoAux(__FILE__,__LINE__),dbPrintfAux

	#define dbPrintf0(F)			dbPrintfAux1( __FILE__, __LINE__, (F) )
	#define dbPrintf1(F,A)			dbPrintfAux1( __FILE__, __LINE__, (F), (A) )
	#define dbPrintf2(F,A,B)		dbPrintfAux1( __FILE__, __LINE__, (F), (A), (B) )
	#define dbPrintf3(F,A,B,C)		dbPrintfAux1( __FILE__, __LINE__, (F), (A), (B), (C) )
	#define dbPrintf4(F,A,B,C,D)	dbPrintfAux1( __FILE__, __LINE__, (F), (A), (B), (C), (D) )
#else
	#define dbPrintf			//

	#define dbPrintf0			//
	#define dbPrintf1			//
	#define dbPrintf2			//
	#define dbPrintf3			//
	#define dbPrintf4			//
#endif

//! outputs a string to all registered handlers, but only ONCE
//! compiled out when NDEBUG is defined
#if defined( RELIC_DEBUG )
	#define dbPrintfOnce(F)		\
		do                                                                 \
		{                                                                  \
			static bool UNIQUE_VAR(__dbPrintfOnce) = true;				   \
																		   \
			if( UNIQUE_VAR(__dbPrintfOnce) )							   \
			{															   \
				UNIQUE_VAR(__dbPrintfOnce) = false;						   \
																		   \
				dbPrintfAux1( __FILE__, __LINE__, (F) );				   \
			}															   \
																		   \
			break;														   \
		}                                                                  \
		while( dbAlwaysReallyTrueAux() ) /* this is never evaluated because of the break on the previous line */

#else
	#define dbPrintfOnce(F)		//
#endif

//! outputs a string to all registered handlers,
//! works in RELEASE & DEBUG mode
#define dbTracef	dbTracefAux

//! outputs a string through dbTracef, then halts the application,
//! works in RELEASE & DEBUG mode
#define dbFatalf	dbTraceInfoAux(__FILE__,__LINE__),dbFatalfAux

//! reports a user-targeted warning (generally for load time errors),
//! works in RELEASE & DEBUG mode
#define dbWarningf dbWarningfAux

//! validate an assertion, halts the application if false,
#define dbAssert(expr)                        \
	do                                          \
	{                                           \
		if (!(expr))                              \
		{                                         \
			dbFatalf("Bad assertion: %s", #expr);   \
			__asm { int 3};                         \
		}                                         \
                                              \
		break;                                    \
	} while (dbAlwaysReallyTrueAux()) /* this is never evaluated because of the break on the previous line */


//! always halts the application,
//! compiled out when NDEBUG is defined
#if defined( RELIC_DEBUG )
	#define dbBreak()	\
	do                                                                 \
	{                                                                  \
		dbAssertAux( __FILE__, __LINE__, "dbBreak" ); __asm { int 3 }; \
		break;                                                         \
	}                                                                  \
	while( dbAlwaysReallyTrueAux() ) /* this is never evaluated because of the break on the previous line */
#else
	#define dbBreak() //
#endif

//! convert a code address to a source location, as a string
DEBUG_API bool	dbConvertAddress
	(
	const long	codeAddress,
	char*		buf,
	size_t		buflen
	);

//! convert a code address to a source location
DEBUG_API bool	dbConvertAddress
	(
	const long		codeAddress,
	char			codeModl[ 256 ],
	char			codeFunc[ 256 ],
	char			codeFile[ 256 ],
	unsigned long&	codeLine
	);

/////////////////////////////////////////////////////////////////////
//	Desc  : fill 'FuncAddrArray' with the address of the functions in the callstack,
//        : up to a maximum depth of 'maxDepth',
//        : skipping the first 'numSkip' functions
//        : THREAD-SAFE
//
//		  : NOTE: 'fast' only works in debug
//	Result:
//	Params: FuncAddrArray - out parm, must be able to hold ( maxDepth - numSkip ) elements
//	Author: Dom
//
DEBUG_API bool	dbDoStackTrace
	(
	long*	FuncAddrArray,
	size_t	maxDepth,
	size_t	numSkip,
	bool	fast
	);

/////////////////////////////////////////////////////////////////////
//	Desc  : similar to the standard stack trace,
//        : it should only be used when you have trapped an exception and
//        : would like the call stack at the point where the exception was thrown
//	Result:
//	Params: FuncAddrArray - out parm, must be able to hold ( maxDepth - numSkip ) elements
//	Author: Dom
//
DEBUG_API bool	dbDoStackTrace
	(
	long*	FuncAddrArray,
	size_t	maxDepth,
	size_t	numSkip,
	const EXCEPTION_POINTERS* data
	);

/////////////////////////////////////////////////////////////////////
//	Desc  : This will write out a mini-dump file that can be loaded in a debugger
//        : it should only be used when you have trapped an exception and
//        : would like the program context at that point
//	Result:
//	Params: Exception information
//	Author: Drew
//
DEBUG_API bool	dbSaveMiniDump
	(
	const char* filename,
	const EXCEPTION_POINTERS* data
	);

/////////////////////////////////////////////////////////////////////
// stack trace object

class DEBUG_API dbTraceObj
{
// types
public:
	enum { MAX_DEPTH = 32 };

// construction
public:
	 dbTraceObj();
	 dbTraceObj( const dbTraceObj& );
	 dbTraceObj& operator= ( const dbTraceObj& );
	 bool operator== ( const dbTraceObj& ) const;

	~dbTraceObj();

	static dbTraceObj GenerateTrace( size_t numSkip );

// interface
public:
	const char*	FunctionAt( size_t index ) const;
	size_t		Depth() const;

	const long	GetTraceAddress( size_t index ) const;

// fields
private:
	size_t			m_depth;

	long			m_trace[ MAX_DEPTH ];

	mutable char*	m_text [ MAX_DEPTH ];
	mutable bool	m_textValid;

// implementation
private:
	void Release();
};

/////////////////////////////////////////////////////////////////////
//	Desc  :
//        : THREAD-SAFE
//	Result:
//	Params:
//	Author: Dom
//
DEBUG_API void	dbPrintStackTrace( const dbTraceObj& traceObj, const char* header );

/////////////////////////////////////////////////////////////////////
//	Desc  : check whether the current process is run under a debugger
//        : THREAD-SAFE
//	Result:
//	Params:
//	Author: Dom
//
DEBUG_API bool	dbIsDebuggerPresent();

