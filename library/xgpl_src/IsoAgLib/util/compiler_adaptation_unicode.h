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

#if defined(WIN32)
#	include <tchar.h>
#else


//
// define basic types
//
#	if !defined(CHAR)
#		define CHAR char
#	endif
#	if !defined(WCHAR) && !defined(WIN32)
#		define WCHAR wchar_t  // (unsigned short)
#	endif
#	if !defined(_TCHAR_DEFINED)
#		define _TCHAR_DEFINED
#		if !defined(_UNICODE) && !defined(UNICODE)
#			define TCHAR char    	// ASCII
#		else
#			define TCHAR wchar_t	// Unicode
#		endif
#	endif

#endif // WIN32

//
// text macro(s)
//
#	ifdef _MSC_VER
#		pragma warning(disable:4005)
#	endif
#	if !defined(TEXT)
#		if !defined(_UNICODE) && !defined(UNICODE)
#			define TEXT(s) s		// ASCII
#		else
#			define TEXT(s) L##s		// Unicode
#		endif
#	endif

	
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


#if defined(WIN32) && defined(UNICODE)
#	include <windows.h>
#	if defined(__WXMSW__)
#		undef Yield		// only a 16-bit compatibility function
#	endif
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
#endif // WIN32 && UNICODE

#endif	//__UNICODE_HELPER_H__
