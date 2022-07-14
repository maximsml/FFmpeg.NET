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
#include "stdafx.h"
#include "Postproc.h"
//////////////////////////////////////////////////////
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
//////////////////////////////////////////////////////
#pragma warning(disable: 4996) // marked as deprecated
//////////////////////////////////////////////////////
// LibPostproc
//////////////////////////////////////////////////////
FFmpeg::LibPostproc::LibPostproc()
{
}
//////////////////////////////////////////////////////
UInt32 FFmpeg::LibPostproc::Version::get()
{
	AVBase::EnsureLibraryLoaded();
	DYNAMIC_DEF_API(Postproc,UInt32,0,postproc_version);
	return postproc_version();
}
String^ FFmpeg::LibPostproc::Configuration::get()
{
	AVBase::EnsureLibraryLoaded();
	DYNAMIC_DEF_API(Postproc,char *,nullptr,postproc_configuration);
	auto p = postproc_configuration();
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::LibPostproc::License::get()
{
	AVBase::EnsureLibraryLoaded();
	DYNAMIC_DEF_API(Postproc,char *,nullptr,postproc_license);
	auto p = postproc_license();
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
String^ FFmpeg::LibPostproc::Help::get()
{
	AVBase::EnsureLibraryLoaded();
	DYNAMIC_DEF_SYM(Postproc,char *,nullptr,pp_help);
	auto p = pp_help;
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
// PPMode
//////////////////////////////////////////////////////
FFmpeg::PPMode::PPMode(void * _pointer, FFmpeg::AVBase ^ _parent)
	: AVBase(_pointer, _parent)
{
}
//////////////////////////////////////////////////////
FFmpeg::PPMode^ FFmpeg::PPMode::GetModeByNameAndQuality(String^ name, int quality)
{
	AVBase::EnsureLibraryLoaded();
	char * szName = nullptr;
	try
	{
		szName = (char*)AllocString(name).ToPointer();
		DYNAMIC_DEF_API(Postproc,pp_mode *,nullptr,pp_get_mode_by_name_and_quality,const char *name, int quality);
		auto p = pp_get_mode_by_name_and_quality(szName,quality);
		PPMode^ mode = nullptr;
		if (p != nullptr)
		{
			mode = gcnew PPMode(p,nullptr);
			if (mode)
			{
				VOID_API(Postproc,pp_free_mode,pp_mode *mode)
				mode->m_pFree = pp_free_mode;
			}
		}
		return mode;
	}
	finally
	{
		FreeMemory((IntPtr)szName);
	}
}
//////////////////////////////////////////////////////
// PPContext
//////////////////////////////////////////////////////
FFmpeg::PPContext::PPContext(int width, int height, PPFlags flags)
	: AVBase(nullptr,nullptr)
{
	pp_context * p = nullptr;
	PTR_API(Postproc,pp_get_context,int,int,int)
	p = pp_get_context(width, height,(int)flags);
	if (!p) throw gcnew System::ArgumentException();
	m_pPointer = p;
	VOID_API(Postproc,pp_free_context,pp_context *)
	m_pFree = pp_free_context;
}
//////////////////////////////////////////////////////
void FFmpeg::PPContext::Postprocess(array<IntPtr>^ src, array<int>^ srcStride,
	array<IntPtr>^ dst, array<int>^ dstStride,
	int horizontalSize, int verticalSize,
	IntPtr QP_store, int QP_stride,
	PPMode^ mode, AVPictureType pict_type)
{
	pin_ptr<int> ss = &srcStride[0];
	pin_ptr<int> ds = &dstStride[0];
	uint8_t * d_src[3] = {0,0,0};
	uint8_t * d_dst[3] = {0,0,0};
	for (int i = 0; i < 3; i++)
	{
		d_src[i] = (uint8_t * )src[i].ToPointer();
		d_dst[i] = (uint8_t * )dst[i].ToPointer();
	}
	VOID_API(Postproc,pp_postprocess,const uint8_t * src[3], const int srcStride[3],
		uint8_t * dst[3], const int dstStride[3],
		int horizontalSize, int verticalSize,
		const int8_t *QP_store,  int QP_stride,
		pp_mode *mode, pp_context *ppContext, int pict_type)
	pp_postprocess((const uint8_t **)d_src,ss,d_dst,ds,
		horizontalSize, verticalSize,(const int8_t*)QP_store.ToPointer(),QP_stride,
		mode->_Pointer.ToPointer(),m_pPointer,(int)pict_type
	);
}

void FFmpeg::PPContext::Postprocess(AVPicture^ src, AVPicture^ dst,
	int horizontalSize, int verticalSize,
	IntPtr QP_store, int QP_stride,
	PPMode^ mode, AVPictureType pict_type)
{
	::AVPicture* _src = (::AVPicture*)src->_Pointer.ToPointer();
	::AVPicture* _dst = (::AVPicture*)dst->_Pointer.ToPointer();

	VOID_API(Postproc,pp_postprocess,const uint8_t * src[3], const int srcStride[3],
		uint8_t * dst[3], const int dstStride[3],
		int horizontalSize, int verticalSize,
		const int8_t *QP_store,  int QP_stride,
		pp_mode *mode, pp_context *ppContext, int pict_type)
	pp_postprocess((const uint8_t **)_src->data,_src->linesize,_dst->data,_dst->linesize,
		horizontalSize, verticalSize,(const int8_t*)QP_store.ToPointer(),QP_stride,
		mode->_Pointer.ToPointer(),m_pPointer,(int)pict_type
	);
}

void FFmpeg::PPContext::Postprocess(array<IntPtr>^ src, array<int>^ srcStride,
	array<IntPtr>^ dst, array<int>^ dstStride,
	int horizontalSize, int verticalSize,
	array<SByte>^ QP_store,
	PPMode^ mode, AVPictureType pict_type)
{
	pin_ptr<signed char> qp = nullptr;
	int QP_stride = 0;
	if (QP_store != nullptr)
	{
		qp = &QP_store[0];
		QP_stride = QP_store->Length;
	}
	Postprocess(src,srcStride,dst,dstStride,horizontalSize, verticalSize,(IntPtr)qp,QP_stride, mode, pict_type);
}

void FFmpeg::PPContext::Postprocess(AVPicture^ src, AVPicture^ dst,
	int horizontalSize, int verticalSize,
	array<SByte>^ QP_store,
	PPMode^ mode, AVPictureType pict_type)
{
	pin_ptr<signed char> qp = nullptr;
	int QP_stride = 0;
	if (QP_store != nullptr)
	{
		qp = &QP_store[0];
		QP_stride = QP_store->Length;
	}
	Postprocess(src,dst,horizontalSize, verticalSize,(IntPtr)qp,QP_stride, mode, pict_type);
}

void FFmpeg::PPContext::Postprocess(array<IntPtr>^ src, array<int>^ srcStride,
	array<IntPtr>^ dst, array<int>^ dstStride,
	int horizontalSize, int verticalSize,
	PPMode^ mode, AVPictureType pict_type)
{
	Postprocess(src,srcStride,dst,dstStride,horizontalSize,verticalSize,nullptr,mode,pict_type);
}

void FFmpeg::PPContext::Postprocess(AVPicture^ src, AVPicture^ dst,
	int horizontalSize, int verticalSize,
	PPMode^ mode, AVPictureType pict_type)
{
	Postprocess(src,dst,horizontalSize,verticalSize,nullptr,mode,pict_type);
}

void FFmpeg::PPContext::Postprocess(array<IntPtr>^ src, array<int>^ srcStride,
	array<IntPtr>^ dst, array<int>^ dstStride,
	int horizontalSize, int verticalSize,
	PPMode^ mode)
{
	Postprocess(src,srcStride,dst,dstStride,horizontalSize, verticalSize,mode,AVPictureType::NONE);
}

void FFmpeg::PPContext::Postprocess(AVPicture^ src, AVPicture^ dst,
	int horizontalSize, int verticalSize,
	PPMode^ mode)
{
	Postprocess(src,dst,horizontalSize, verticalSize,mode,AVPictureType::NONE);
}
//////////////////////////////////////////////////////