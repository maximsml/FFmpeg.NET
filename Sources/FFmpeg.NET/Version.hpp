//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper
// Copyright (C) 2018-2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
//
// The FFmpeg.NET Wrapper of ffmpeg libraries is provided "as is" 
// without warranty of any kind; without even the implied warranty 
// of merchantability or fitness for a particular purpose. 
//
// FFmpeg.NET Wrapper library is free for non-commercial usage
// Usage terms described in attached license text file
//
///////////////////////////////////////////////////////
// Common Version Definitions
///////////////////////////////////////////////////////
#ifndef VER_PRODUCTMAJORVERSION
#	define VER_PRODUCTMAJORVERSION	1
#endif
///////////////////////////////////////////////////////
#ifndef VER_PRODUCTMINORVERSION
#	define VER_PRODUCTMINORVERSION	0
#endif
///////////////////////////////////////////////////////
#ifndef VER_PRODUCTREVISION
#	define VER_PRODUCTREVISION		0
#endif
///////////////////////////////////////////////////////
#ifndef VER_PRODUCTBUILD
#	define VER_PRODUCTBUILD			0
#endif
///////////////////////////////////////////////////////
#ifndef VER_PRODUCTVERSION
#	define VER_PRODUCTVERSION	     VER_PRODUCTMAJORVERSION,VER_PRODUCTMINORVERSION,VER_PRODUCTREVISION,VER_PRODUCTBUILD
#endif
///////////////////////////////////////////////////////
#ifndef	MAKE_STRING
#	define MAKE_STRING(x) #x
#endif
#ifndef	_STR
#	define _STR(x) MAKE_STRING(x)
#endif
///////////////////////////////////////////////////////
// Version
#ifndef VER_PRODUCTVERSION_STR
#	ifdef VER_PRODUCTVERSION
#		define VER_PRODUCTVERSION_STR		_STR(VER_PRODUCTVERSION)
#	endif
#endif
///////////////////////////////////////////////////////
// File Name
#ifndef VER_INTERNALNAME_STR
#	ifdef VER_ORIGINALFILENAME_STR
#		define VER_INTERNALNAME_STR        VER_ORIGINALFILENAME_STR
#	endif
#else
#	ifndef VER_ORIGINALFILENAME_STR
#		define VER_ORIGINALFILENAME_STR    VER_INTERNALNAME_STR
#	endif
#endif
///////////////////////////////////////////////////////
#ifndef VER_COMPANYNAME_STR
#	define VER_COMPANYNAME_STR "Company"
#endif
///////////////////////////////////////////////////////
#ifndef VER_LEGALCOPYRIGHT_YEARS
#	define VER_LEGALCOPYRIGHT_YEARS "2010"
#endif
///////////////////////////////////////////////////////
#ifndef VER_LEGALCOPYRIGHT_STR
#	define VER_LEGALCOPYRIGHT_STR "Copyright \251 " VER_COMPANYNAME_STR " Corporation " VER_LEGALCOPYRIGHT_YEARS ". All rights reserved."
#endif
///////////////////////////////////////////////////////
#ifndef VER_FILETYPE
#	define VER_FILETYPE                VFT_UNKNOWN
#endif
///////////////////////////////////////////////////////
#ifndef VER_FILESUBTYPE
#	define VER_FILESUBTYPE             VFT2_UNKNOWN
#endif
///////////////////////////////////////////////////////
#ifndef VER_PRODUCTNAME_STR
#	define VER_PRODUCTNAME_STR  VER_COMPANYNAME_STR " \256 " VER_LEGALCOPYRIGHT_YEARS " v. " VER_PRODUCTVERSION_STR
#endif
///////////////////////////////////////////////////////
#include <common.ver>
///////////////////////////////////////////////////////
