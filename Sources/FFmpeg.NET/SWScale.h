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
// libswscale
// Color conversion and scaling library.
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
public enum class SwsFlags : int
{
	None = 0,
	FastBilinear = 1,
	Bilinear = 2,
	Bicubic = 4,
	X = 8,
	Point = 0x10,
	Area = 0x20,
	Bicublin = 0x40,
	Gauss = 0x80,
	Sinc = 0x100,
	Lanczos = 0x200,
	Spline = 0x400,

	SRC_V_CHR_DROP_MASK = 0x30000,
	SRC_V_CHR_DROP_SHIFT = 16,
	PARAM_DEFAULT = 123456,
	PRINT_INFO = 0x1000,
	FULL_CHR_H_INT = 0x2000,
	FULL_CHR_H_INP = 0x4000,
	DIRECT_BGR = 0x8000,
	ACCURATE_RND = 0x40000,
	BITEXACT = 0x80000,
	ERROR_DIFFUSION = 0x800000,
};
//////////////////////////////////////////////////////
//static const float Sws_MAX_REDUCE_CUTOFF = 0.002f;
//////////////////////////////////////////////////////
// SwsCC
public enum class SwsCC : int
{
	ITU709 = 1,
	FCC = 4,
	ITU601 = 5,
	ITU624 = 5,
	SMPTE170M = 5,
	SMPTE240M = 7,
	DEFAULT = 5,
	BT2020 = 9,
};
//////////////////////////////////////////////////////
// SwsDither
public enum class SwsDither
{
	NONE = 0,
	AUTO,
	BAYER,
	ED,
	A_DITHER,
	X_DITHER,
	NB,
};
//////////////////////////////////////////////////////
#pragma endregion 
//////////////////////////////////////////////////////
#pragma region Classes
//////////////////////////////////////////////////////
// LibSWScale
public ref class LibSWScale
{
private:
	LibSWScale();
public:
	// Return the LIBSWSCALE_VERSION_INT constant.
	static property UInt32 Version { UInt32 get(); }
	/// Return the libswscale build-time configuration.
	static property String^ Configuration { String^ get(); }
	/// Return the libswscale license.
	static property String^ License { String^ get(); }
};
//////////////////////////////////////////////////////
// SwsVector
public ref class SwsVector : public AVArray<double>
						   , public ICloneable
{
internal:
	SwsVector(void * _pointer,AVBase ^ _parent);
public:
	SwsVector(int length);
	SwsVector(array<double>^ vector);
public:
	///< pointer to the list of coefficients
	property AVArray<double>^ coeff { AVArray<double>^ get(); }
	///< number of coefficients in the vector
	property int length { int get(); }
public:
	array<double>^ ToArray();
public:
	void Scale(double scalar);
	void Normalize(double height);
	void Conv(SwsVector^ b);
	void Add(SwsVector^ b);
	void Sub(SwsVector^ b);
	void Shift(int shift);
public:
	static SwsVector^ operator + (SwsVector^ a,SwsVector^ b);
	static SwsVector^ operator - (SwsVector^ a,SwsVector^ b);
	static SwsVector^ operator * (SwsVector^ a,double scalar);
public:
	// ICloneable
	virtual Object^ Clone();
public:
	/// Return a normalized Gaussian curve used to filter stuff
	/// quality = 3 is high quality, lower is lower quality.
	static SwsVector^ GetGaussianVec(double variance, double quality);

	/// Allocate and return a vector with length coefficients, all
	/// with the same value c.
	static SwsVector^ GetConstVec(double c, int length);

	/// Allocate and return a vector with just one coefficient, with
	/// value 1.0.
	static SwsVector^ GetIdentityVec(void);
};
//////////////////////////////////////////////////////
// SwsFilter
public ref class SwsFilter : public AVBase
{
public:
	SwsFilter();
	SwsFilter(float lumaGBlur, float chromaGBlur,
		float lumaSharpen, float chromaSharpen,
		float chromaHShift, float chromaVShift,
		int verbose);
	SwsFilter(SwsVector^ lh,SwsVector^ lv,SwsVector^ ch,SwsVector^ cv);
public:
	property int _StructureSize { virtual int get() override; }
public:
	property SwsVector^ lumH { SwsVector^ get(); void set(SwsVector^); }
	property SwsVector^ lumV { SwsVector^ get(); void set(SwsVector^); }
	property SwsVector^ chrH { SwsVector^ get(); void set(SwsVector^); }
	property SwsVector^ chrV { SwsVector^ get(); void set(SwsVector^); }
};
//////////////////////////////////////////////////////
// SwsContext
public ref class SwsContext : public AVBase
{
public:
	/// Get the AVClass for swsContext. It can be used in combination with
	/// AV_OPT_SEARCH_FAKE_OBJ for examining options.
	static property AVClass^ Class { AVClass^ get(); }
public:
	/// Allocate an empty SwsContext. This must be filled and passed to
	/// sws_init_context(). For filling see AVOptions, options.c and
	/// sws_setColorspaceDetails().
	SwsContext();
	/// Allocate and return an SwsContext. You need it to perform
	/// scaling/conversion operations using sws_scale().
	///
	/// @param srcW the width of the source image
	/// @param srcH the height of the source image
	/// @param srcFormat the source image format
	/// @param dstW the width of the destination image
	/// @param dstH the height of the destination image
	/// @param dstFormat the destination image format
	/// @param flags specify which algorithm and options to use for rescaling
	/// @return a pointer to an allocated context, or NULL in case of error
	/// @note this function is to be removed after a saner alternative is
	///       written
	SwsContext(int srcW, int srcH, AVPixelFormat srcFormat,
		int dstW, int dstH, AVPixelFormat dstFormat,
		SwsFlags flags, SwsFilter^ srcFilter,
		SwsFilter^ dstFilter,double^ param);

	SwsContext(int srcW, int srcH, AVPixelFormat srcFormat,
		int dstW, int dstH, AVPixelFormat dstFormat,
		SwsFlags flags);

	SwsContext(AVFrame^ src,
		AVFrame^ dst,
		SwsFlags flags);

	SwsContext(AVCodecContext^ src,
		AVCodecContext^ dst,
		SwsFlags flags);
private:
	void Init(int srcW, int srcH, AVPixelFormat srcFormat,
		int dstW, int dstH, AVPixelFormat dstFormat,
		SwsFlags flags, SwsFilter^ srcFilter,
		SwsFilter^ dstFilter,double^ param);
public:
	property SwsFlags Flags { SwsFlags get(); void set(SwsFlags); }
	property int srcW { int get(); void set(int); }
	property int srcH { int get(); void set(int); }
	property int dstW { int get(); void set(int); }
	property int dstH { int get(); void set(int); }
	property AVPixelFormat srcFormat { AVPixelFormat get(); void set(AVPixelFormat); }
	property AVPixelFormat dstFormat { AVPixelFormat get(); void set(AVPixelFormat); }
	property SwsDither Dither { SwsDither get(); void set(SwsDither); }
	property bool Gamma { bool get(); void set(bool); }
public:
	/// Initialize the swscaler context sws_context.
	///
	/// @return zero or positive value on success, a negative value on
	/// error
	AVRESULT Init(SwsFilter^ srcFilter,SwsFilter^ dstFilter);

	/// Scale the image slice in srcSlice and put the resulting scaled
	/// slice in the image in dst. A slice is a sequence of consecutive
	/// rows in an image.
	///
	/// Slices have to be provided in sequential order, either in
	/// top-bottom or bottom-top order. If slices are provided in
	/// non-sequential order the behavior of the function is undefined.
	///
	/// @param srcSlice  the array containing the pointers to the planes of
	///                  the source slice
	/// @param srcStride the array containing the strides for each plane of
	///                  the source image
	/// @param srcSliceY the position in the source image of the slice to
	///                  process, that is the number (counted starting from
	///                  zero) in the image of the first row of the slice
	/// @param srcSliceH the height of the source slice, that is the number
	///                  of rows in the slice
	/// @param dst       the array containing the pointers to the planes of
	///                  the destination image
	/// @param dstStride the array containing the strides for each plane of
	///                  the destination image
	/// @return          the height of the output slice
	int Scale(array<IntPtr>^ srcSlice,
		array<int>^ srcStride, int srcSliceY, int srcSliceH,
		array<IntPtr>^ dst,array<int>^ dstStride);

	int Scale(AVPicture^ src,int srcSliceY, int srcSliceH,array<IntPtr>^ dst,array<int>^ dstStride);
	int Scale(array<IntPtr>^ srcSlice,
		array<int>^ srcStride,int srcSliceY, int srcSliceH,AVPicture^ dst);

	int Scale(AVPicture^ src,int srcSliceY, int srcSliceH,AVPicture^ dst);
public:
	/// @param dstRange flag indicating the while-black range of the output (1=jpeg / 0=mpeg)
	/// @param srcRange flag indicating the while-black range of the input (1=jpeg / 0=mpeg)
	/// @param table the yuv2rgb coefficients describing the output yuv space, normally ff_yuv2rgb_coeffs[x]
	/// @param inv_table the yuv2rgb coefficients describing the input yuv space, normally ff_yuv2rgb_coeffs[x]
	/// @param brightness 16.16 fixed point brightness correction
	/// @param contrast 16.16 fixed point contrast correction
	/// @param saturation 16.16 fixed point saturation correction
	/// @return false if not supported
	bool SetColorspaceDetails(array<int>^ inv_table,
		int srcRange, array<int>^ table, int dstRange,
		int brightness, int contrast, int saturation);

	/// @return false if not supported
	bool GetColorspaceDetails(array<int>^ % inv_table,
		int* srcRange,array<int>^ % table, int* dstRange,
		int* brightness, int* contrast, int* saturation);
public:
	/// Return a positive value if pix_fmt is a supported input format, 0
	/// otherwise.
	static bool IsSupportedInput(AVPixelFormat pix_fmt);

	/// Return a positive value if pix_fmt is a supported output format, 0
	/// otherwise.
	static bool IsSupportedOutput(AVPixelFormat pix_fmt);

	/// @param[in]  pix_fmt the pixel format
	/// @return a positive value if an endianness conversion for pix_fmt is
	/// supported, 0 otherwise.
	static bool IsSupportedEndiannessConversion(AVPixelFormat pix_fmt);
public:
	/// Convert an 8-bit paletted frame into a frame with a color depth of 32 bits.
	///
	/// The output frame will have the same packed format as the palette.
	///
	/// @param src        source frame buffer
	/// @param dst        destination frame buffer
	/// @param num_pixels number of pixels to convert
	/// @param palette    array with [256] entries, which must match color arrangement (RGB or BGR) of src
	static void ConvertPalette8ToPacked32(IntPtr src, IntPtr dst, int num_pixels, IntPtr palette);

	/// Convert an 8-bit paletted frame into a frame with a color depth of 24 bits.
	///
	/// With the palette format "ABCD", the destination frame ends up with the format "ABC".
	///
	/// @param src        source frame buffer
	/// @param dst        destination frame buffer
	/// @param num_pixels number of pixels to convert
	/// @param palette    array with [256] entries, which must match color arrangement (RGB or BGR) of src
	static void ConvertPalette8ToPacked24(IntPtr src, IntPtr dst, int num_pixels, IntPtr palette);
};
//////////////////////////////////////////////////////
#pragma endregion 
//////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////