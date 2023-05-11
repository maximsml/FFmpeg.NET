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
#include "stdafx.h"
#include "SWScale.h"
//////////////////////////////////////////////////////
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
//////////////////////////////////////////////////////
#pragma warning(disable: 4996) // marked as deprecated
//////////////////////////////////////////////////////
struct SwsContext {};
//////////////////////////////////////////////////////
// LibSWScale
//////////////////////////////////////////////////////
FFmpeg::LibSWScale::LibSWScale()
{
}
//////////////////////////////////////////////////////
UInt32 FFmpeg::LibSWScale::Version::get()
{
	return swscale_version();
}
String^ FFmpeg::LibSWScale::Configuration::get()
{
	auto p = swscale_configuration();
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::LibSWScale::License::get()
{
	auto p = swscale_license();
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
// SwsVector
//////////////////////////////////////////////////////
FFmpeg::SwsVector::SwsVector(void * _pointer,AVBase ^ _parent)
	: AVArray(_pointer,_parent,0,0)
{
	m_nItemSize = sizeof(double);
	m_nCount = ((::SwsVector*)m_pPointer)->length;
}

FFmpeg::SwsVector::SwsVector(int length)
	: AVArray(nullptr,nullptr,0,0)
{
	m_nItemSize = sizeof(double);
	m_nCount = length;
	m_pPointer = sws_allocVec(length);
	m_pFree = (TFreeFN *)sws_freeVec;
}

FFmpeg::SwsVector::SwsVector(array<double>^ vector)
	: AVArray(nullptr,nullptr,0,0)
{
	m_nItemSize = sizeof(double);
	m_nCount = vector->Length;
	m_pPointer = sws_allocVec(vector->Length);
	m_pFree = (TFreeFN *)sws_freeVec;
	auto p = ((::SwsVector *)m_pPointer)->coeff;
	for (int i = 0; i < vector->Length; i++)
	{
		p[i] = vector[i];
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVArray<double>^ FFmpeg::SwsVector::coeff::get()
{
	auto p = ((::SwsVector *)m_pPointer)->coeff;
	AVArray<double>^ _array = (AVArray<double>^)GetObject((IntPtr)p);
	if (_array) return _array;
	return gcnew AVArray<double>(((::SwsVector*)m_pPointer)->coeff,this,sizeof(double),((::SwsVector*)m_pPointer)->length);
}

int FFmpeg::SwsVector::length::get()
{
	return ((::SwsVector*)m_pPointer)->length;
}
//////////////////////////////////////////////////////
array<double>^ FFmpeg::SwsVector::ToArray()
{
	List<double>^ _list = gcnew List<double>();
	for (int i = 0; i < ((::SwsVector*)m_pPointer)->length; i++)
	{
		_list->Add(((::SwsVector*)m_pPointer)->coeff[i]);
	}
	return _list->ToArray();
}
//////////////////////////////////////////////////////
void FFmpeg::SwsVector::Scale(double scalar)
{
	sws_scaleVec((::SwsVector*)m_pPointer,scalar);
}
void FFmpeg::SwsVector::Normalize(double height)
{
	sws_normalizeVec((::SwsVector*)m_pPointer,height);
}
void FFmpeg::SwsVector::Conv(SwsVector^ b)
{
	LOAD_API(Swscale,void,sws_convVec,::SwsVector *,::SwsVector *);
	if (sws_convVec)
	{
		sws_convVec((::SwsVector*)m_pPointer,(::SwsVector*)b->_Pointer.ToPointer());
	}
	else
	{
		::SwsVector * a = (::SwsVector *)m_pPointer;
		::SwsVector * v = (::SwsVector *)b->_Pointer.ToPointer();
		int length = a->length + v->length - 1;
		::SwsVector * vec = sws_allocVec(length);
		if (vec)
		{
			memset(vec->coeff,0x00,vec->length * sizeof(vec->coeff[0]));
			for (int i = 0; i < a->length; i++) {
				for (int j = 0; j < v->length; j++) {
					vec->coeff[i + j] += a->coeff[i] * v->coeff[j];
				}
			}
			av_free(a->coeff);
			a->coeff  = vec->coeff;
			a->length = vec->length;
			av_free(vec);
		}
	}
	
}
void FFmpeg::SwsVector::Add(SwsVector^ b)
{
	LOAD_API(Swscale,void,sws_addVec,::SwsVector *,::SwsVector *);
	if (sws_addVec)
	{
		sws_addVec((::SwsVector*)m_pPointer, (::SwsVector*)b->_Pointer.ToPointer());
	}
	else
	{
		::SwsVector * a = (::SwsVector *)m_pPointer;
		::SwsVector * v = (::SwsVector *)b->_Pointer.ToPointer();
		int length = a->length > v->length ? a->length : v->length;
		::SwsVector * vec = sws_allocVec(length);
		if (vec)
		{
			memset(vec->coeff,0x00,vec->length * sizeof(vec->coeff[0]));
			for (int i = 0; i < a->length; i++)
				vec->coeff[i + (length - 1) / 2 - (a->length - 1) / 2] += a->coeff[i];
			for (int i = 0; i < v->length; i++)
				vec->coeff[i + (length - 1) / 2 - (v->length - 1) / 2] += v->coeff[i];
			av_free(a->coeff);
			a->coeff  = vec->coeff;
			a->length = vec->length;
			av_free(vec);
		}
	}
}
void FFmpeg::SwsVector::Sub(SwsVector^ b)
{
	LOAD_API(Swscale,void,sws_subVec,::SwsVector *,::SwsVector *);
	if (sws_subVec)
	{
		sws_subVec((::SwsVector*)m_pPointer, (::SwsVector*)b->_Pointer.ToPointer());
	}
	else
	{
		::SwsVector * a = (::SwsVector *)m_pPointer;
		::SwsVector * v = (::SwsVector *)b->_Pointer.ToPointer();
		int length = a->length > v->length ? a->length : v->length;
		::SwsVector * vec = sws_allocVec(length);
		if (vec)
		{
			memset(vec->coeff,0x00,vec->length * sizeof(vec->coeff[0]));
			for (int i = 0; i < a->length; i++)
				vec->coeff[i + (length - 1) / 2 - (a->length - 1) / 2] += a->coeff[i];
			for (int i = 0; i < v->length; i++)
				vec->coeff[i + (length - 1) / 2 - (v->length - 1) / 2] -= v->coeff[i];
			av_free(a->coeff);
			a->coeff  = vec->coeff;
			a->length = vec->length;
			av_free(vec);
		}
	}
}
void FFmpeg::SwsVector::Shift(int shift)
{
	LOAD_API(Swscale,void,sws_shiftVec,::SwsVector *,int);
	if (sws_shiftVec)
	{
		sws_shiftVec((::SwsVector*)m_pPointer, shift);
	}
	else
	{
		::SwsVector * a = (::SwsVector *)m_pPointer;
		int length = a->length + Math::Abs(shift) * 2;
		::SwsVector * vec = sws_allocVec(length);
		if (vec)
		{
			memset(vec->coeff,0x00,vec->length * sizeof(vec->coeff[0]));
			for (int i = 0; i < a->length; i++) {
				vec->coeff[i + (length    - 1) / 2 -
					(a->length - 1) / 2 - shift] = a->coeff[i];
			}
			av_free(a->coeff);
			a->coeff  = vec->coeff;
			a->length = vec->length;
			av_free(vec);
		}
	}
}
//////////////////////////////////////////////////////
FFmpeg::SwsVector^ FFmpeg::SwsVector::operator + (FFmpeg::SwsVector^ a, FFmpeg::SwsVector^ b)
{
	a->Add(b);
	return a;
}

FFmpeg::SwsVector^ FFmpeg::SwsVector::operator - (FFmpeg::SwsVector^ a, FFmpeg::SwsVector^ b)
{
	a->Sub(b);
	return a;
}

FFmpeg::SwsVector^ FFmpeg::SwsVector::operator * (SwsVector^ a,double scalar)
{
	a->Scale(scalar);
	return a;
}
//////////////////////////////////////////////////////
Object^ FFmpeg::SwsVector::Clone()
{
	LOAD_API(Swscale,::SwsVector *,sws_cloneVec,::SwsVector *);
	::SwsVector * v = nullptr;
	if (sws_cloneVec)
	{
		v = sws_cloneVec((::SwsVector*)m_pPointer);
	}
	else
	{
		::SwsVector* src = (::SwsVector*)m_pPointer;
		v = sws_allocVec(src->length);
		if (v)
		{
			memcpy(v->coeff, src->coeff, v->length * sizeof(v->coeff[0]));
		}
	}
	if (!v) return nullptr;
	SwsVector^ _vector = gcnew SwsVector(v,nullptr);
	_vector->m_pFree = (TFreeFN *)sws_freeVec;
	return _vector;
}
//////////////////////////////////////////////////////
FFmpeg::SwsVector^ FFmpeg::SwsVector::GetGaussianVec(double variance, double quality)
{
	SwsVector^ _vector = gcnew SwsVector(sws_getGaussianVec(variance,quality),nullptr);
	_vector->m_pFree = (TFreeFN *)sws_freeVec;
	return _vector;
}
FFmpeg::SwsVector^ FFmpeg::SwsVector::GetConstVec(double c, int length)
{
	LOAD_API(Swscale,::SwsVector *,sws_getConstVec,double , int);
	::SwsVector * v = nullptr;
	if (sws_getConstVec)
	{
		v = sws_getConstVec(c, length);
	}
	else
	{
		v = sws_allocVec(length);
		if (v)
		{
			for (int i = 0; i < v->length; i++)
			{
				v->coeff[i] = c;
			}
		}
	}
	if (!v) return nullptr;
	SwsVector^ _vector = gcnew SwsVector(v,nullptr);
	_vector->m_pFree = (TFreeFN *)sws_freeVec;
	return _vector;
}
FFmpeg::SwsVector^ FFmpeg::SwsVector::GetIdentityVec(void)
{
	LOAD_API(Swscale,::SwsVector *,sws_getIdentityVec);
	::SwsVector * v = nullptr;
	if (sws_getIdentityVec)
	{
		v = sws_getIdentityVec();
	}
	else
	{
		v = sws_allocVec(1);
		if (v)
		{
			v->coeff[0] = 1.0;
		}
	}
	if (!v) return nullptr;
	SwsVector^ _vector = gcnew SwsVector(v,nullptr);
	_vector->m_pFree = (TFreeFN *)sws_freeVec;
	return _vector;
}
//////////////////////////////////////////////////////
// SwsFilter
//////////////////////////////////////////////////////
FFmpeg::SwsFilter::SwsFilter()
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
}

FFmpeg::SwsFilter::SwsFilter(float lumaGBlur, float chromaGBlur,
		  float lumaSharpen, float chromaSharpen,
		  float chromaHShift, float chromaVShift,
		  int verbose)
	: AVBase(nullptr,nullptr)
{
	m_pPointer = sws_getDefaultFilter(lumaGBlur, chromaGBlur,lumaSharpen, chromaSharpen,chromaHShift, chromaVShift,verbose);
	if (m_pPointer)
	{
		m_pFree = (TFreeFN *)sws_freeFilter;
	}
}

FFmpeg::SwsFilter::SwsFilter(SwsVector^ lh, SwsVector^ lv, SwsVector^ ch, SwsVector^ cv)
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
	FFmpeg::SwsFilter::lumH::set(lh);
	FFmpeg::SwsFilter::lumV::set(lv);
	FFmpeg::SwsFilter::chrH::set(ch);
	FFmpeg::SwsFilter::chrV::set(cv);
}
//////////////////////////////////////////////////////
int FFmpeg::SwsFilter::_StructureSize::get()
{
	return sizeof(::SwsFilter);
}
//////////////////////////////////////////////////////
FFmpeg::SwsVector^ FFmpeg::SwsFilter::lumH::get()
{
	return _CreateObject<SwsVector>(((::SwsFilter*)m_pPointer)->lumH);
}
void FFmpeg::SwsFilter::lumH::set(SwsVector^ value)
{
	AddObject((IntPtr)((::SwsFilter*)m_pPointer)->lumH,value);
	((::SwsFilter*)m_pPointer)->lumH = (::SwsVector*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}

FFmpeg::SwsVector^ FFmpeg::SwsFilter::lumV::get()
{
	return _CreateObject<SwsVector>(((::SwsFilter*)m_pPointer)->lumV);
}
void FFmpeg::SwsFilter::lumV::set(SwsVector^ value)
{
	AddObject((IntPtr)((::SwsFilter*)m_pPointer)->lumV,value);
	((::SwsFilter*)m_pPointer)->lumV = (::SwsVector*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}

FFmpeg::SwsVector^ FFmpeg::SwsFilter::chrH::get()
{
	return _CreateObject<SwsVector>(((::SwsFilter*)m_pPointer)->chrH);
}
void FFmpeg::SwsFilter::chrH::set(SwsVector^ value)
{
	AddObject((IntPtr)((::SwsFilter*)m_pPointer)->chrH,value);
	((::SwsFilter*)m_pPointer)->chrH = (::SwsVector*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}

FFmpeg::SwsVector^ FFmpeg::SwsFilter::chrV::get()
{
	return _CreateObject<SwsVector>(((::SwsFilter*)m_pPointer)->chrV);
}
void FFmpeg::SwsFilter::chrV::set(SwsVector^ value)
{
	AddObject((IntPtr)((::SwsFilter*)m_pPointer)->chrV,value);
	((::SwsFilter*)m_pPointer)->chrV = (::SwsVector*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}
//////////////////////////////////////////////////////
// SwsContext
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::SwsContext::Class::get()
{
	auto p = sws_get_class();
	return p != nullptr ? gcnew AVClass((void*)p,nullptr) : nullptr;
}
//////////////////////////////////////////////////////
FFmpeg::SwsContext::SwsContext()
	: AVBase(nullptr,nullptr)
{
	m_pPointer = sws_alloc_context();
	m_pFree = (TFreeFN *)sws_freeContext;
}
//////////////////////////////////////////////////////
FFmpeg::SwsContext::SwsContext(int srcW, int srcH, AVPixelFormat srcFormat,
		   int dstW, int dstH, AVPixelFormat dstFormat,
		   SwsFlags flags, SwsFilter^ srcFilter,
		   SwsFilter^ dstFilter,double^ param)
	: AVBase(nullptr,nullptr)
{
	Init(srcW,srcH,srcFormat,dstW,dstH,dstFormat,flags,srcFilter,dstFilter,param);
}
FFmpeg::SwsContext::SwsContext(int srcW, int srcH, AVPixelFormat srcFormat,
		   int dstW, int dstH, AVPixelFormat dstFormat,
		   SwsFlags flags)
	: AVBase(nullptr,nullptr)
{
	Init(srcW,srcH,srcFormat,dstW,dstH,dstFormat,flags,nullptr,nullptr,nullptr);
}
FFmpeg::SwsContext::SwsContext(AVFrame^ src,AVFrame^ dst,SwsFlags flags)
	: AVBase(nullptr,nullptr)
{
	Init(src->width,src->height,(AVPixelFormat)src->format,dst->width,dst->height,(AVPixelFormat)dst->format,flags,nullptr,nullptr,nullptr);
}

FFmpeg::SwsContext::SwsContext(AVCodecContext^ src,AVCodecContext^ dst,SwsFlags flags)
	: AVBase(nullptr,nullptr)
{
	Init(src->width,src->height,src->pix_fmt,dst->width,dst->height,dst->pix_fmt,flags,nullptr,nullptr,nullptr);
}
//////////////////////////////////////////////////////
void FFmpeg::SwsContext::Init(int srcW, int srcH, AVPixelFormat srcFormat,
		  int dstW, int dstH, AVPixelFormat dstFormat,
		  SwsFlags flags, SwsFilter^ srcFilter,
		  SwsFilter^ dstFilter,double^ param)
{
	if (m_pPointer && m_pFree)
	{
		m_pFree(m_pPointer);
		m_pPointer = nullptr;
	}
	pin_ptr< double > p = param == nullptr ? nullptr : &*param;
	m_pPointer = sws_getContext(srcW,srcH,(::AVPixelFormat)srcFormat,dstW,dstH,(::AVPixelFormat)dstFormat,
		(int)flags,srcFilter != nullptr ? (::SwsFilter*)srcFilter->_Pointer.ToPointer() : nullptr,
		dstFilter != nullptr ? (::SwsFilter*)dstFilter->_Pointer.ToPointer() : nullptr,p);
	m_pFree = (TFreeFN *)sws_freeContext;
}
//////////////////////////////////////////////////////
FFmpeg::SwsFlags FFmpeg::SwsContext::Flags::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"sws_flags",0,&val))
	{
		return (SwsFlags)val;
	}
	return SwsFlags::None;
}

void FFmpeg::SwsContext::Flags::set(SwsFlags value)
{
	av_opt_set_int(m_pPointer,"sws_flags",(int64_t)value,0);
}

int FFmpeg::SwsContext::srcW::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"srcw",0,&val))
	{
		return (int)val;
	}
	return 0;
}
void FFmpeg::SwsContext::srcW::set(int value)
{
	av_opt_set_int(m_pPointer,"srcw",(int64_t)value,0);
}

int FFmpeg::SwsContext::srcH::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"srch",0,&val))
	{
		return (int)val;
	}
	return 0;
}
void FFmpeg::SwsContext::srcH::set(int value)
{
	av_opt_set_int(m_pPointer,"srch",(int64_t)value,0);
}

int FFmpeg::SwsContext::dstW::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"dstw",0,&val))
	{
		return (int)val;
	}
	return 0;
}
void FFmpeg::SwsContext::dstW::set(int value)
{
	av_opt_set_int(m_pPointer,"dstw",(int64_t)value,0);
}
int FFmpeg::SwsContext::dstH::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"dsth",0,&val))
	{
		return (int)val;
	}
	return 0;
}
void FFmpeg::SwsContext::dstH::set(int value)
{
	av_opt_set_int(m_pPointer,"dsth",(int64_t)value,0);
}

FFmpeg::AVPixelFormat FFmpeg::SwsContext::srcFormat::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"src_format",0,&val))
	{
		return (AVPixelFormat)val;
	}
	return AVPixelFormat::NONE;
}
void FFmpeg::SwsContext::srcFormat::set(AVPixelFormat value)
{
	av_opt_set_int(m_pPointer,"src_format",(int64_t)value,0);
}
FFmpeg::AVPixelFormat FFmpeg::SwsContext::dstFormat::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"dst_format",0,&val))
	{
		return (AVPixelFormat)val;
	}
	return AVPixelFormat::NONE;
}
void FFmpeg::SwsContext::dstFormat::set(AVPixelFormat value)
{
	av_opt_set_int(m_pPointer,"dst_format",(int64_t)value,0);
}
FFmpeg::SwsDither FFmpeg::SwsContext::Dither::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"sws_dither",0,&val))
	{
		return (SwsDither)val;
	}
	return SwsDither::NONE;
}
void FFmpeg::SwsContext::Dither::set(SwsDither value)
{
	av_opt_set_int(m_pPointer,"sws_dither",(int64_t)value,0);
}
bool FFmpeg::SwsContext::Gamma::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"gamma",0,&val))
	{
		return val != 0 ? true : false;
	}
	return false;
}
void FFmpeg::SwsContext::Gamma::set(bool value)
{
	av_opt_set_int(m_pPointer,"gamma",(int64_t)(value ? 1 : 0),0);
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::SwsContext::Init(SwsFilter^ srcFilter,SwsFilter^ dstFilter)
{
	return sws_init_context((::SwsContext *)m_pPointer, 
		srcFilter != nullptr ? (::SwsFilter*)srcFilter->_Pointer.ToPointer() : nullptr,
		dstFilter != nullptr ? (::SwsFilter*)dstFilter->_Pointer.ToPointer() : nullptr);
}
//////////////////////////////////////////////////////
int FFmpeg::SwsContext::Scale(array<IntPtr>^ srcSlice,
							  array<int>^ srcStride, int srcSliceY, int srcSliceH,
							  array<IntPtr>^ dst,array<int>^ dstStride)
{
	uint8_t * srcData[4] = {0,0,0,0};
	int srcLinesize[4] = {0,0,0,0};

	uint8_t * dstData[4] = {0,0,0,0};
	int dstLinesize[4] = {0,0,0,0};
	for (int i = 0; i < srcSlice->Length && i < _countof(srcData); i++) { srcData[i] = (uint8_t *)srcSlice[i].ToPointer(); }
	for (int i = 0; i < dst->Length && i < _countof(dstData); i++) { dstData[i] = (uint8_t *)dst[i].ToPointer(); }
	for (int i = 0; i < srcStride->Length && i < _countof(srcLinesize); i++) { srcLinesize[i] = srcStride[i]; }
	for (int i = 0; i < dstStride->Length && i < _countof(dstLinesize); i++) { dstLinesize[i] = dstStride[i]; }

	return sws_scale((::SwsContext *)m_pPointer,srcData,srcLinesize,srcSliceY,srcSliceH,dstData,dstLinesize);
}

int FFmpeg::SwsContext::Scale(AVArray<IntPtr>^ srcSlice,
	AVArray<int>^ srcStride, int srcSliceY, int srcSliceH,
	array<IntPtr>^ dst, array<int>^ dstStride)
{
	uint8_t * srcData[4] = {0,0,0,0};
	int srcLinesize[4] = {0,0,0,0};

	uint8_t * dstData[4] = {0,0,0,0};
	int dstLinesize[4] = {0,0,0,0};
	for (int i = 0; i < srcSlice->Count && i < _countof(srcData); i++) { srcData[i] = (uint8_t *)srcSlice[i].ToPointer(); }
	for (int i = 0; i < dst->Length && i < _countof(dstData); i++) { dstData[i] = (uint8_t *)dst[i].ToPointer(); }
	for (int i = 0; i < srcStride->Count && i < _countof(srcLinesize); i++) { srcLinesize[i] = srcStride[i]; }
	for (int i = 0; i < dstStride->Length && i < _countof(dstLinesize); i++) { dstLinesize[i] = dstStride[i]; }

	return sws_scale((::SwsContext *)m_pPointer,srcData,srcLinesize,srcSliceY,srcSliceH,dstData,dstLinesize);
}

int FFmpeg::SwsContext::Scale(array<IntPtr>^ srcSlice,
	array<int>^ srcStride, int srcSliceY, int srcSliceH,
	AVArray<IntPtr>^ dst, AVArray<int>^ dstStride)
{
	uint8_t * srcData[4] = {0,0,0,0};
	int srcLinesize[4] = {0,0,0,0};

	uint8_t * dstData[4] = {0,0,0,0};
	int dstLinesize[4] = {0,0,0,0};
	for (int i = 0; i < srcSlice->Length && i < _countof(srcData); i++) { srcData[i] = (uint8_t *)srcSlice[i].ToPointer(); }
	for (int i = 0; i < dst->Count && i < _countof(dstData); i++) { dstData[i] = (uint8_t *)dst[i].ToPointer(); }
	for (int i = 0; i < srcStride->Length && i < _countof(srcLinesize); i++) { srcLinesize[i] = srcStride[i]; }
	for (int i = 0; i < dstStride->Count && i < _countof(dstLinesize); i++) { dstLinesize[i] = dstStride[i]; }

	return sws_scale((::SwsContext *)m_pPointer,srcData,srcLinesize,srcSliceY,srcSliceH,dstData,dstLinesize);
}

int FFmpeg::SwsContext::Scale(AVArray<IntPtr>^ srcSlice,
	AVArray<int>^ srcStride, int srcSliceY, int srcSliceH,
	AVArray<IntPtr>^ dst, AVArray<int>^ dstStride)
{
	uint8_t * srcData[4] = {0,0,0,0};
	int srcLinesize[4] = {0,0,0,0};

	uint8_t * dstData[4] = {0,0,0,0};
	int dstLinesize[4] = {0,0,0,0};
	for (int i = 0; i < srcSlice->Count && i < _countof(srcData); i++) { srcData[i] = (uint8_t *)srcSlice[i].ToPointer(); }
	for (int i = 0; i < dst->Count && i < _countof(dstData); i++) { dstData[i] = (uint8_t *)dst[i].ToPointer(); }
	for (int i = 0; i < srcStride->Count && i < _countof(srcLinesize); i++) { srcLinesize[i] = srcStride[i]; }
	for (int i = 0; i < dstStride->Count && i < _countof(dstLinesize); i++) { dstLinesize[i] = dstStride[i]; }

	return sws_scale((::SwsContext *)m_pPointer,srcData,srcLinesize,srcSliceY,srcSliceH,dstData,dstLinesize);
}

int FFmpeg::SwsContext::Scale(AVPicture^ src, int srcSliceY, int srcSliceH, array<IntPtr>^ dst, array<int>^ dstStride)
{
	uint8_t * dstData[4] = {0,0,0,0};
	int dstLinesize[4] = {0,0,0,0};
	for (int i = 0; i < dst->Length && i < _countof(dstData); i++) { dstData[i] = (uint8_t *)dst[i].ToPointer(); }
	for (int i = 0; i < dstStride->Length && i < _countof(dstLinesize); i++) { dstLinesize[i] = dstStride[i]; }
	::AVPicture* _src = (::AVPicture*)src->_Pointer.ToPointer();
	return sws_scale((::SwsContext *)m_pPointer,_src->data,_src->linesize,srcSliceY,srcSliceH,dstData,dstLinesize);
}

int FFmpeg::SwsContext::Scale(array<IntPtr>^ srcSlice, array<int>^ srcStride, int srcSliceY, int srcSliceH, AVPicture^ dst)
{
	uint8_t * srcData[4] = {0,0,0,0};
	int srcLinesize[4] = {0,0,0,0};
	for (int i = 0; i < srcSlice->Length && i < _countof(srcData); i++) { srcData[i] = (uint8_t *)srcSlice[i].ToPointer(); }
	for (int i = 0; i < srcStride->Length && i < _countof(srcLinesize); i++) { srcLinesize[i] = srcStride[i]; }
	::AVPicture* _dst = (::AVPicture*)dst->_Pointer.ToPointer();
	return sws_scale((::SwsContext *)m_pPointer,srcData,srcLinesize,srcSliceY,srcSliceH,_dst->data,_dst->linesize);
}

int FFmpeg::SwsContext::Scale(AVPicture^ src,int srcSliceY, int srcSliceH,AVPicture^ dst)
{
	::AVPicture* _src = (::AVPicture*)src->_Pointer.ToPointer();
	::AVPicture* _dst = (::AVPicture*)dst->_Pointer.ToPointer();
	return sws_scale((::SwsContext *)m_pPointer,_src->data,_src->linesize,srcSliceY,srcSliceH,_dst->data,_dst->linesize);
}
//////////////////////////////////////////////////////
bool FFmpeg::SwsContext::SetColorspaceDetails(array<int>^ inv_table,
						  int srcRange, array<int>^ table, int dstRange,
						  int brightness, int contrast, int saturation)
{
	int _inv_table[4] = {0,0,0,0};
	int _table[4] = {0,0,0,0};
	if (inv_table != nullptr) 
	{
		for (int i = 0; i < inv_table->Length; i++) { _inv_table[i] = inv_table[i]; }
	}
	if (table != nullptr) 
	{
		for (int i = 0; i < table->Length; i++) { _table[i] = table[i]; }
	}
	return (-1 != sws_setColorspaceDetails((::SwsContext*)m_pPointer,_inv_table,srcRange,_table, dstRange,brightness, contrast, saturation));
}
bool FFmpeg::SwsContext::GetColorspaceDetails(array<int>^ % inv_table,
						  int* srcRange,array<int>^ % table, int* dstRange,
						  int* brightness, int* contrast, int* saturation)
{
	int _inv_table[4] = {0,0,0,0};
	int _table[4] = {0,0,0,0};
	int _srcRange,_dstRange,_brightness,_contrast,_saturation;
	int _result = sws_getColorspaceDetails((::SwsContext*)m_pPointer,(int**)&_inv_table,&_srcRange,(int**)&_table,&_dstRange,&_brightness,&_contrast,&_saturation);
	if (_result != -1)
	{
		List<int>^ _list = gcnew List<int>();
		for (int i = 0; i < 4; i++) _list->Add(_inv_table[i]);
		inv_table = _list->ToArray();
		_list->Clear();
		if (srcRange) *srcRange = _srcRange;
		for (int i = 0; i < 4; i++) _list->Add(_table[i]);
		table = _list->ToArray();
		_list->Clear();
		if (dstRange) *dstRange = _dstRange;
		if (brightness) *brightness = _brightness;
		if (contrast) *contrast = _contrast;
		if (saturation) *saturation = _saturation;
	}
	return (_result != -1);
}
//////////////////////////////////////////////////////
bool FFmpeg::SwsContext::IsSupportedInput(AVPixelFormat pix_fmt)
{
	return sws_isSupportedInput((::AVPixelFormat)pix_fmt) != 0;
}
bool FFmpeg::SwsContext::IsSupportedOutput(AVPixelFormat pix_fmt)
{
	return sws_isSupportedOutput((::AVPixelFormat)pix_fmt) != 0;
}
bool FFmpeg::SwsContext::IsSupportedEndiannessConversion(AVPixelFormat pix_fmt)
{
	return sws_isSupportedEndiannessConversion((::AVPixelFormat)pix_fmt) != 0;
}
//////////////////////////////////////////////////////
void FFmpeg::SwsContext::ConvertPalette8ToPacked32(IntPtr src, IntPtr dst, int num_pixels, IntPtr palette)
{
	sws_convertPalette8ToPacked32((const uint8_t *)src.ToPointer(),(uint8_t *)dst.ToPointer(), num_pixels,(const uint8_t *)palette.ToPointer());
}

void FFmpeg::SwsContext::ConvertPalette8ToPacked24(IntPtr src, IntPtr dst, int num_pixels, IntPtr palette)
{
	sws_convertPalette8ToPacked24((const uint8_t *)src.ToPointer(),(uint8_t *)dst.ToPointer(), num_pixels,(const uint8_t *)palette.ToPointer());
}
//////////////////////////////////////////////////////