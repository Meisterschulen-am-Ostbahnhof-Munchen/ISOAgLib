//==============================================================================
//  File:           unicode_helper.h
//  Author:         Florian Mücke <f.muecke@osb-ag.de>
//  Created:        04.07.2008 15:59:05 (fmu)
//  Modified:       $Date$
//  Copyright:      2008, OSB AG
//  Description:    Helper functions for multibyte/widechar handling 
//==============================================================================

#ifndef __UNICODE_HELPER_H__
#define __UNICODE_HELPER_H__

#ifdef WIN32
#	include <windows.h>
#endif

//
// basic types
//
#define CHAR char
#define WCHAR wchar_t  // (unsigned short)
#if !defined(_UNICODE) && !defined(UNICODE)
#	define TCHAR CHAR    // ASCII
#else
#	define TCHAR WCHAR   // Unicode
#endif


//
// text macro(s)
//
#if !defined(__T)
#	if !defined(_UNICODE) && !defined(UNICODE)
#		define __T(s) s       // ASCII
#	else
#		define __T(s) L##s    // Unicode
#	endif
#endif

#if !defined(TEXT)
#	define TEXT(s) __T(s)
#endif

#if !defined(_UNICODE) && !defined(UNICODE)
#	define STD_TSTRING	std::string
#else
#	define STD_TSTRING	std::wstring
#endif

//
// define string functions for (non) unicode handling
//

#if defined(_UNICODE) || defined(UNICODE)
#	define STRLEN  wcslen
#	define STRCHR  wcschr
#	define STRRCHR wcsrchr
#	define STRCAT  wcscat
#	define STRCMP  wcscmp
#	define STRDUP  wcsdup
#	define STRCPY  wcscpy
#else
#	define STRLEN  strlen
#	define STRCHR  strchr
#	define STRRCHR strrchr
#	define STRCAT  strcat
#	define STRCMP  strcmp
#	define STRDUP  strdup
#	define STRCPY  strcpy
#endif


#ifdef WIN32
// strSize must include space for terminating zero
inline bool wc2mb( LPWSTR lpwStr, LPSTR lpStr, size_t strSize, int cp = CP_ACP )
{
	size_t size = wcslen( lpwStr );
	if( strSize <= size )
		return false;
	WideCharToMultiByte( cp, 0, lpwStr, -1, lpStr, size, NULL, NULL );
	return true;
}

// wstrSize must include space for terminating zero
inline bool mb2wc( LPSTR lpStr, LPWSTR lpwStr, size_t wstrSize, int cp = CP_ACP )
{
	size_t size = strlen( lpStr );
	if( wstrSize <= size )
		return false;
	MultiByteToWideChar( cp, 0, lpStr, -1, lpwStr, size );
	return true;
}
#endif

#endif	//__UNICODE_HELPER_H__
