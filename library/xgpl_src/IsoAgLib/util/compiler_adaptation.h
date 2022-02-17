/*
  compiler_adaptation.h:
    defines to produce shorter symbol names durig compile (needed as
    workaround for buggy compilers)

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "compiler_adaptation_unicode.h" // defines TCHAR etc.

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#if ( (defined(GCC_VERSION) && (GCC_VERSION >= 40700)) || (defined(_MSC_VER) && (_MSC_VER >= 1600)) || (__cplusplus >= 201103L) )
#define ISOAGLIB_OVERRIDE override
#else
#define ISOAGLIB_OVERRIDE
#endif

#if defined(__TSW_CPP__)
  #define DEF_GET_POINTER_ADR_DEBUG_OUTPUT( pointer ) uint32_t( pointer )
#else
  #define DEF_GET_POINTER_ADR_DEBUG_OUTPUT( pointer ) pointer
#endif

#if defined(__TSW_CPP__)
/* include this header here cause they have to be included *before* some other stl
 * container header (e.g. vector>) when using Tasking 7.5r6.  */
#include <cstring>
#include <cstdio>
#include <cstdlib>
#endif


#ifndef __TSW_CPP__
 #define IRQ_FUNC_PAR ...
#else
 #define IRQ_FUNC_PAR
#endif


#undef CNAMESPACE
#if defined( __TSW_CPP_70__ )
 #define CNAMESPACE
#elif defined( _MSC_VER )
	#if _MSC_VER <= 1200
		#define CNAMESPACE
	#else
	 #define CNAMESPACE std
	#endif
#elif defined( __IAR_SYSTEMS_ICC__ )
	#define CNAMESPACE
#else
 #define CNAMESPACE std
#endif




#ifdef __IAR_SYSTEMS_ICC__
    // needed for AMS5
    #ifndef NULL
        #define NULL 0
    #endif
#endif


#if defined( __GNUC__ ) && __GNUC__ >= 4
  #define MALLOC_TEMPLATE(PAR) __gnu_cxx::malloc_allocator<PAR>
#else
  #define MALLOC_TEMPLATE(PAR) std::__malloc_alloc_template<0>
  #if defined( _MSC_VER )
    #define __STR2__(x) #x
    #define __STR1__(x) __STR2__(x)
    #define __LOC__ __FILE__ "("__STR1__(__LINE__)") : warning: "
  #endif
#endif


//==============================================================================
// WINCE
// The following section covers Windows CE specific definitions and global
// macros!
//==============================================================================
#ifdef _WIN32_WCE
  #ifndef WINCE
    #define WINCE
  #endif
  #ifndef WIN32
    #define WIN32
  #endif
#endif

// Windows CE does not support standard abort()!
#if defined(WINCE)
  #define MACRO_ISOAGLIB_ABORT() TerminateProcess(GetCurrentProcess(), 0)
  #define MACRO_ISOAGLIB_PERROR(x) printf("error: %s\n",x)
  #define MACRO_ISOAGLIB_TIMEGETTIME() GetTickCount()
#else
  #define MACRO_ISOAGLIB_ABORT() std::abort()
  #define MACRO_ISOAGLIB_PERROR(x) std::perror(x)
  #define MACRO_ISOAGLIB_TIMEGETTIME() timeGetTime()
#endif

#define MACRO_ISOAGLIB_MKTIME(x) std::mktime(x)
#define MACRO_ISOAGLIB_LOCALTIME(x) std::localtime(x)

#ifdef WINCE
  #include <winsock2.h>  // needs to be included BEFORE windows.h
  #include <windows.h>

  // sys/stat.h
  // define structure for returning status information
  #ifndef _STAT_DEFINED
  #define _STAT_DEFINED
  struct stat
  {
	//_dev_t st_dev;		// ID of device containing file
	//_ino_t st_ino;		// file serial number
	//unsigned short st_mode; // mode of file (see below)
	//short st_nlink;		// number of links to the file
	//short st_uid;			// user ID of file
	//short st_gid;			// group ID of file
	//_dev_t st_rdev;		// device ID (if file is character or block special)
	unsigned long st_size;	// file size in bytes (if file is a regular file)
	//time_t st_atime;		// time of last access
	//time_t st_mtime;		// time of last data modification
	//time_t st_ctime;		// time of last status change
  // -- unused: --
  //	blksize_t st_blksize; 	// a filesystem-specific preferred I/O block size for
							// this object.  In some filesystem types, this may
							// vary from file to file
  //	blkcnt_t st_blocks;		// number of blocks allocated for this object
  };

  static __inline int stat( const char* _fileName, struct stat* _stat )
  {
	WIN32_FIND_DATAA wfd;
	_stat->st_size = wfd.nFileSizeLow;
	HANDLE h = FindFirstFileA( _fileName, &wfd );
	if ( h != INVALID_HANDLE_VALUE)
	{
		CloseHandle(h);
		return 0; // success!
	}
	//TODO: set errno
	return -1;	// failure!
  }

  static __inline int stat( const wchar_t* _fileName, struct stat* _stat )
  {
	WIN32_FIND_DATAW wfd;
	HANDLE h = FindFirstFileW( _fileName, &wfd );
	if ( h != INVALID_HANDLE_VALUE)
	{
		_stat->st_size = wfd.nFileSizeLow;
		CloseHandle(h);
		return 0; // success!
	}
	_stat->st_size = 0;
	//TODO: set errno
	return -1;	// failure!
  }
#endif // _STAT_DEFINED
#endif	//WINCE
