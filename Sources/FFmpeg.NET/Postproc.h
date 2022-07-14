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
// libpostproc 
// Video postprocessing library.
//////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////
using namespace System;
//////////////////////////////////////////////////////
#include "AVCodec.h"
//////////////////////////////////////////////////////
namespace FFmpeg {
//////////////////////////////////////////////////////
#pragma region Enums
//////////////////////////////////////////////////////
[Flags]
public enum class PPFlags : unsigned
{
	None		= 0x00000000,
	Cpu_MMX		= 0x80000000,
	Cpu_MMX2	= 0x20000000,
	Cpu_3DNOW	= 0x40000000,
	Cpu_ALTIVEC	= 0x10000000,
	Cpu_AUTO	= 0x00080000,
	Format_420	= (0x00000011|0x00000008),
	Format_422	= (0x00000001|0x00000008),
	Format_411	= (0x00000002|0x00000008),
	Format_444	= (0x00000000|0x00000008),
	Format_440	= (0x00000010|0x00000008),
};
//////////////////////////////////////////////////////
#pragma endregion 
//////////////////////////////////////////////////////
#pragma region Classes
//////////////////////////////////////////////////////
// LibPostproc
public ref class LibPostproc
{
private:
	LibPostproc();
public:
	// Return the LIBPOSTPROC_VERSION_INT constant.
	static property UInt32 Version { UInt32 get(); }
	/// Return the libpostproc build-time configuration.
	static property String^ Configuration { String^ get(); }
	/// Return the libpostproc license.
	static property String^ License { String^ get(); }
public:
	///< a simple help text
	static property String^ Help { String^ get(); }
};
//////////////////////////////////////////////////////
// PPMode
public ref class PPMode : public AVBase
{
public:
	static const int QUALITY_MAX = 6;
internal:
	PPMode(void * _pointer,AVBase ^ _parent);
public:
	/// Return a pp_mode or NULL if an error occurred.
	///
	/// @param name    the string after "-pp" on the command line
	/// @param quality a number from 0 to QUALITY_MAX
	static PPMode^ GetModeByNameAndQuality(String^ name, int quality);
};
//////////////////////////////////////////////////////
// PPContext
public ref class PPContext : public AVBase
{
public:
	PPContext(int width, int height, PPFlags flags);
public:
	void Postprocess(array<IntPtr>^ src,array<int>^ srcStride, 
		array<IntPtr>^ dst,array<int>^ dstStride,
		int horizontalSize, int verticalSize,
		IntPtr QP_store, int QP_stride,
		PPMode^ mode, AVPictureType pict_type);

	void Postprocess(AVPicture^ src,AVPicture^ dst,
		int horizontalSize, int verticalSize,
		IntPtr QP_store, int QP_stride,
		PPMode^ mode, AVPictureType pict_type);

	void Postprocess(array<IntPtr>^ src,array<int>^ srcStride, 
		array<IntPtr>^ dst,array<int>^ dstStride,
		int horizontalSize, int verticalSize,
		array<SByte>^ QP_store,
		PPMode^ mode, AVPictureType pict_type);

	void Postprocess(AVPicture^ src,AVPicture^ dst,
		int horizontalSize, int verticalSize,
		array<SByte>^ QP_store,
		PPMode^ mode, AVPictureType pict_type);

	void Postprocess(array<IntPtr>^ src,array<int>^ srcStride, 
		array<IntPtr>^ dst,array<int>^ dstStride,
		int horizontalSize, int verticalSize,
		PPMode^ mode, AVPictureType pict_type);

	void Postprocess(AVPicture^ src,AVPicture^ dst,
		int horizontalSize, int verticalSize,
		PPMode^ mode, AVPictureType pict_type);

	void Postprocess(array<IntPtr>^ src,array<int>^ srcStride, 
		array<IntPtr>^ dst,array<int>^ dstStride,
		int horizontalSize, int verticalSize,
		PPMode^ mode);

	void Postprocess(AVPicture^ src,AVPicture^ dst,
		int horizontalSize, int verticalSize,
		PPMode^ mode);
};
//////////////////////////////////////////////////////
#pragma endregion 
//////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////
