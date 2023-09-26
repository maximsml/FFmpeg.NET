//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper
// Copyright (C) 2018 Maxim Kartavenkov (mkartavenkov@gmail.com)
//
// The FFmpeg.NET Wrapper of ffmpeg libraries is provided "as is" 
// without warranty of any kind; without even the implied warranty 
// of merchantability or fitness for a particular purpose. 
//
// FFmpeg.NET Wrapper library is free for non-commercial usage
// Usage terms described in attached license text file
//
// Assemblly Settings File
//////////////////////////////////////////////////////
#include "stdafx.h"
#include "./Version.rc"
//////////////////////////////////////////////////////
#define NET_VERSION	     VER_PRODUCTMAJORVERSION.VER_PRODUCTMINORVERSION.VER_PRODUCTREVISION.VER_PRODUCTBUILD
#define NET_VERSION_STR  _STR(NET_VERSION)
//////////////////////////////////////////////////////
#ifdef  _DEBUG
#define BUILD_CONFIGURATION "Debug"
#else
#define BUILD_CONFIGURATION "Release"
#endif
//////////////////////////////////////////////////////
#ifdef _WIN64
#define BUILD_PLATFORM "x64"
#else
#define BUILD_PLATFORM "x86"
#endif
//////////////////////////////////////////////////////
#define BUILD_STR BUILD_CONFIGURATION "-" BUILD_PLATFORM
//////////////////////////////////////////////////////
using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;
//////////////////////////////////////////////////////
[assembly:AssemblyTitleAttribute(VER_FILEDESCRIPTION_STR)];
[assembly:AssemblyDescriptionAttribute(VER_FILEDESCRIPTION_STR)];
[assembly:AssemblyConfigurationAttribute(BUILD_STR)];
[assembly:AssemblyCompanyAttribute(VER_COMPANYNAME_STR)];
[assembly:AssemblyProductAttribute(VER_PRODUCTNAME_STR)];
[assembly:AssemblyCopyrightAttribute(_STR(VER_LEGALCOPYRIGHT_STR))];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];
[assembly:AssemblyVersionAttribute(NET_VERSION_STR)];
[assembly:ComVisible(false)];
[assembly:CLSCompliantAttribute(true)];
//////////////////////////////////////////////////////
//[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];
// 
// Version History
// 
// 1.0.2207.0201 - Initial Version
// 1.0.2207.2301 - Added Version Information
// 1.0.2305.1101:
// - Added Hash Algorithms
// - AVCodecContext Extradata Fix
// 1.0.2309.1401:
// - Added Support of ffmpeg versions 4.4.1, 5.1.2, 6.0.0
// - Added Video Capture Example
// - Added Streaming Example
// - Added RTSP Receiving Example
