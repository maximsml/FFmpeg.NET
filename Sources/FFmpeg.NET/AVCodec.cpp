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
// libavcodec
// Encoding/Decoding Library.
//////////////////////////////////////////////////////
#include "stdafx.h"
#include "AVCodec.h"
//////////////////////////////////////////////////////
//#include <libavcodec/bsf.h>
//////////////////////////////////////////////////////
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
//////////////////////////////////////////////////////
#pragma warning(disable: 4996) // marked as deprecated
//////////////////////////////////////////////////////
private struct AVBSFList {};
//////////////////////////////////////////////////////
FFmpeg::LibAVCodec::LibAVCodec()
{
}
//////////////////////////////////////////////////////
UInt32 FFmpeg::LibAVCodec::Version::get()
{
	return avcodec_version();
}

String^ FFmpeg::LibAVCodec::Configuration::get()
{
	auto p = avcodec_configuration();
	return p != nullptr ? gcnew String(p) : nullptr;
}

String^ FFmpeg::LibAVCodec::License::get()
{
	auto p = avcodec_license();
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
void FFmpeg::LibAVCodec::Register(FFmpeg::AVCodec^ _codec)
{
	AVBase::EnsureLibraryLoaded();
	if (_codec != nullptr && _codec->_Pointer != IntPtr::Zero)
	{
		s_bRegistered = true;
		VOID_API(AVCodec,avcodec_register,::AVCodec *codec)
		avcodec_register((::AVCodec*)_codec->_Pointer.ToPointer());
	}
}

void FFmpeg::LibAVCodec::RegisterAll()
{
	AVBase::EnsureLibraryLoaded();
	if (!s_bRegistered) 
	{
		s_bRegistered = true;
		VOID_API(AVCodec,avcodec_register_all)
		avcodec_register_all();
	}
}
//////////////////////////////////////////////////////
// AVCodecEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVCodec::AVCodecs::AVCodecEnumerator::AVCodecEnumerator()
	: m_pOpaque(IntPtr::Zero)
{

}

FFmpeg::AVCodec::AVCodecs::AVCodecEnumerator::~AVCodecEnumerator()
{
}

bool FFmpeg::AVCodec::AVCodecs::AVCodecEnumerator::MoveNext()
{
	AVBase::EnsureLibraryLoaded();
	const ::AVCodec * p = nullptr;
	void * opaque = m_pOpaque.ToPointer();
	LOAD_API(AVCodec,::AVCodec *,av_codec_next,const ::AVCodec*);
	LOAD_API(AVCodec,::AVCodec *,av_codec_iterate,void **);
	if (av_codec_iterate != nullptr)
	{
		p = av_codec_iterate(&opaque);
	}
	else
	{
		if (av_codec_next != nullptr)
		{
			p = av_codec_next((const ::AVCodec*)opaque);
			opaque = (void*)p;
		}
	}
	m_pOpaque = IntPtr(opaque);
	m_pCurrent = (p != nullptr) ? gcnew AVCodec((void*)p, nullptr) : nullptr;
	return (m_pCurrent != nullptr);
}
FFmpeg::AVCodec^ FFmpeg::AVCodec::AVCodecs::AVCodecEnumerator::Current::get()
{
	if (m_pOpaque == IntPtr::Zero)
	{
		MoveNext();
	}
	return m_pCurrent;
}
void FFmpeg::AVCodec::AVCodecs::AVCodecEnumerator::Reset()
{
	m_pOpaque = IntPtr::Zero;
}
Object^ FFmpeg::AVCodec::AVCodecs::AVCodecEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVCodecs
//////////////////////////////////////////////////////
FFmpeg::AVCodec::AVCodecs::AVCodecs()
{
	LibAVCodec::RegisterAll();
}

FFmpeg::AVCodec^ FFmpeg::AVCodec::AVCodecs::default::get(int index)
{
	if (index >= 0)
	{
		LOAD_API(AVCodec,::AVCodec *,av_codec_next,const ::AVCodec*);
		LOAD_API(AVCodec,::AVCodec *,av_codec_iterate,void **);
		if (av_codec_iterate)
		{
			void * opaque = nullptr;
			while (true)
			{
				auto p = av_codec_iterate(&opaque);
				if (!p) break;
				if (index-- == 0)
				{
					return gcnew AVCodec((void*)p, nullptr);
				}
			}
		} else
		if (av_codec_next)
		{
			::AVCodec * p = nullptr;
			while (true)
			{
				p = av_codec_next(p);
				if (!p) break;
				if (index-- == 0)
				{
					return gcnew AVCodec((void*)p, nullptr);
				}
			}
		}
	}
	return nullptr;
}
int FFmpeg::AVCodec::AVCodecs::Count::get()
{
	AVBase::EnsureLibraryLoaded();
	int count = 0;
	LOAD_API(AVCodec,::AVCodec *,av_codec_next,const ::AVCodec*);
	LOAD_API(AVCodec,::AVCodec *,av_codec_iterate,void **);

	if (count == 0 && av_codec_iterate)
	{
		void * opaque = nullptr;
		while (true)
		{
			auto p = av_codec_iterate(&opaque);
			if (!p) break;
			count++;
		}
	}
	if (count == 0 && av_codec_next)
	{
		::AVCodec * p = nullptr;
		while (true)
		{
			p = av_codec_next(p);
			if (!p) break;
			count++;
		}
	}
	return count;
}
//////////////////////////////////////////////////////
// AVCodec
//////////////////////////////////////////////////////
FFmpeg::AVCodec::AVCodec(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
int FFmpeg::AVCodec::_StructureSize::get()
{
	return sizeof(::AVCodec);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVCodec::name::get()
{ 
	auto p = ((::AVCodec*)m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}

String^ FFmpeg::AVCodec::long_name::get()
{ 
	auto p = ((::AVCodec*)m_pPointer)->long_name;
	return p != nullptr ? gcnew String(p) : nullptr;
}

FFmpeg::AVMediaType FFmpeg::AVCodec::type::get()
{
	return (FFmpeg::AVMediaType)((::AVCodec*)m_pPointer)->type;
}

FFmpeg::AVCodecID FFmpeg::AVCodec::id::get()
{
	return (FFmpeg::AVCodecID)((::AVCodec*)m_pPointer)->id;
}

FFmpeg::AVCodecCap FFmpeg::AVCodec::capabilities::get()
{
	return (FFmpeg::AVCodecCap)((::AVCodec*)m_pPointer)->capabilities;
}

array<FFmpeg::AVRational^>^ FFmpeg::AVCodec::supported_framerates::get() 
{ 
	List<FFmpeg::AVRational^>^ _array = nullptr;
	const ::AVRational * pRates = ((::AVCodec*)m_pPointer)->supported_framerates;
	if (pRates)
	{
		_array =  gcnew List<FFmpeg::AVRational^>();
		while (pRates->num != 0 && pRates->num != 0)
		{
			_array->Add(_CreateObject<AVRational>((void*)pRates++));
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}

array<FFmpeg::AVPixelFormat>^ FFmpeg::AVCodec::pix_fmts::get() 
{
	List<AVPixelFormat>^ _array = nullptr;
	const ::AVPixelFormat * _pointer = ((::AVCodec*)m_pPointer)->pix_fmts;
	if (_pointer)
	{
		_array =  gcnew List<AVPixelFormat>();
		while (*_pointer != -1)
		{
			_array->Add((AVPixelFormat)*_pointer++);
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}

array<int>^ FFmpeg::AVCodec::supported_samplerates::get() 
{
	List<int>^ _array = nullptr;
	const int * _pointer = ((::AVCodec*)m_pPointer)->supported_samplerates;
	if (_pointer)
	{
		_array =  gcnew List<int>();
		while (*_pointer != 0)
		{
			_array->Add((int)*_pointer++);
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}

array<FFmpeg::AVSampleFormat>^ FFmpeg::AVCodec::sample_fmts::get()
{ 
	List<AVSampleFormat>^ _array = nullptr;
	const ::AVSampleFormat * _pointer = ((::AVCodec*)m_pPointer)->sample_fmts;
	if (_pointer)
	{
		_array =  gcnew List<AVSampleFormat>();
		while (*_pointer != -1)
		{
			_array->Add((AVSampleFormat)*_pointer++);
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}

array<FFmpeg::AVChannelLayout>^ FFmpeg::AVCodec::channel_layouts::get()
{
	List<AVChannelLayout>^ _array = nullptr;
	const uint64_t * _pointer = ((::AVCodec*)m_pPointer)->channel_layouts;
	if (_pointer)
	{
		_array =  gcnew List<AVChannelLayout>();
		while (*_pointer != 0)
		{
			_array->Add((AVChannelLayout)*_pointer++);
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}

FFmpeg::AVClass^ FFmpeg::AVCodec::priv_class::get()
{
	auto p = ((::AVCodec*)m_pPointer)->priv_class;
	return _CreateObject<AVClass>((void*)p);
}

array<FFmpeg::AVProfile^>^ FFmpeg::AVCodec::profiles::get()
{
	List<FFmpeg::AVProfile^>^ _array = nullptr;
	const ::AVProfile * _pointer = ((::AVCodec*)m_pPointer)->profiles;
	if (_pointer)
	{
		_array =  gcnew List<FFmpeg::AVProfile^>();
		while (_pointer->profile != FF_PROFILE_UNKNOWN)
		{
			_array->Add(_CreateObject<AVProfile>((void*)_pointer++));
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}
int FFmpeg::AVCodec::max_lowres::get()
{
	//av_codec_get_max_lowres
	return ((::AVCodec*)m_pPointer)->max_lowres;
}

String^ FFmpeg::AVCodec::wrapper_name::get()
{
	auto p = ((::AVCodec*)m_pPointer)->wrapper_name;
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
array<FFmpeg::AVCodecHWConfig^>^ FFmpeg::AVCodec::hw_configs::get()
{
	List<FFmpeg::AVCodecHWConfig^>^ _array = gcnew List<FFmpeg::AVCodecHWConfig^>();
	int idx = 0;
	while (true)
	{
		auto p = avcodec_get_hw_config((::AVCodec*)m_pPointer,idx++);
		if (!p) break;
		_array->Add(_CreateObject<AVCodecHWConfig>((void*)p));
	}
	return _array->ToArray();
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVCodec::ToString()
{
	if (m_pPointer)
	{
		String^ _name;
		_name = long_name;

		if (String::IsNullOrEmpty(_name)) _name = name;
		if (!String::IsNullOrEmpty(_name))
		{
			return "[ AVCodec ] \"" + _name + "\"";
		}
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
FFmpeg::AVCodec^ FFmpeg::AVCodec::Next()
{
	return Next(this);
}

bool FFmpeg::AVCodec::IsDecoder()
{
	return (av_codec_is_decoder((const ::AVCodec *)m_pPointer) != 0);
}

bool FFmpeg::AVCodec::IsEncoder()
{
	return (av_codec_is_encoder((const ::AVCodec *)m_pPointer) != 0);
}

String^ FFmpeg::AVCodec::GetProfileName(FFmpeg::Profile _profile)
{
	auto p = av_get_profile_name((const ::AVCodec *)m_pPointer,(int)_profile);
	return p != nullptr ? gcnew String(p) : nullptr;
}

FFmpeg::AVCodecContext^ FFmpeg::AVCodec::AllocContext()
{
	return gcnew AVCodecContext(this);
}
//////////////////////////////////////////////////////
FFmpeg::AVCodec^ FFmpeg::AVCodec::Next(FFmpeg::AVCodec^ _prev)
{
	LibAVCodec::RegisterAll();

	const ::AVCodec * p = nullptr;
	LOAD_API(AVCodec,::AVCodec *,av_codec_next,const ::AVCodec*);
	LOAD_API(AVCodec,::AVCodec *,av_codec_iterate,void **);
	if (av_codec_next)
	{
		p = av_codec_next((const ::AVCodec*)(_prev != nullptr ? _prev->m_pPointer : nullptr));
	}
	else
	{
		if (av_codec_iterate)
		{
			void * opaque = nullptr;
			bool bMoveNext = (_prev != nullptr);
			do
			{
				p = av_codec_iterate(&opaque);
				if (p && bMoveNext)
				{
					bMoveNext = (p != _prev->_Pointer.ToPointer());
					continue;
				}
				break;
			}
			while (true);
		}
	}
	return p != nullptr ? gcnew AVCodec((void*)p,nullptr) : nullptr;
}

FFmpeg::AVCodec^ FFmpeg::AVCodec::FindDecoder(FFmpeg::AVCodecID _id)
{
	LibAVCodec::RegisterAll();
	const ::AVCodec * _pointer =  avcodec_find_decoder((::AVCodecID)_id);
	return _pointer != nullptr ? gcnew AVCodec((void*)_pointer,nullptr) : nullptr;
}

FFmpeg::AVCodec^ FFmpeg::AVCodec::FindDecoder(String^ _name)
{
	const ::AVCodec * _pointer =  nullptr;
	if (!String::IsNullOrEmpty(_name))
	{
		LibAVCodec::RegisterAll();
		char * szName = nullptr;
		szName = (char*)AllocString(_name).ToPointer();
		try
		{
			_pointer =  avcodec_find_decoder_by_name(szName);
		}
		finally
		{
			FreeMemory((IntPtr)szName);
		}
	}
	return _pointer != nullptr ? gcnew AVCodec((void*)_pointer,nullptr) : nullptr;
}

FFmpeg::AVCodec^ FFmpeg::AVCodec::FindEncoder(FFmpeg::AVCodecID _id)
{
	LibAVCodec::RegisterAll();
	const ::AVCodec * _pointer =  avcodec_find_encoder((::AVCodecID)_id);
	return _pointer != nullptr ? gcnew AVCodec((void*)_pointer,nullptr) : nullptr;
}

FFmpeg::AVCodec^ FFmpeg::AVCodec::FindEncoder(String^ _name)
{
	const ::AVCodec * _pointer =  nullptr;
	if (!String::IsNullOrEmpty(_name))
	{
		LibAVCodec::RegisterAll();
		char * szName = nullptr;
		szName = (char*)AllocString(_name).ToPointer();
		try
		{
			_pointer = avcodec_find_encoder_by_name(szName);
		}
		finally
		{
			FreeMemory((IntPtr)szName);
		}
	}
	return _pointer != nullptr ? gcnew AVCodec((void*)_pointer,nullptr) : nullptr;
}

int FFmpeg::AVCodec::GetBitsPerSample(FFmpeg::AVCodecID codec_id)
{
	return av_get_bits_per_sample((::AVCodecID)codec_id);
}

FFmpeg::AVCodecID FFmpeg::AVCodec::GetPCMCodec(FFmpeg::AVSampleFormat fmt, int be)
{
	return (AVCodecID)av_get_pcm_codec((::AVSampleFormat)fmt, be);
}

int FFmpeg::AVCodec::GetExactBitsPerSample(FFmpeg::AVCodecID codec_id)
{
	return av_get_exact_bits_per_sample((::AVCodecID)codec_id);
}

FFmpeg::AVMediaType FFmpeg::AVCodec::GetCodecType(FFmpeg::AVCodecID codec_id)
{
	return (AVMediaType)avcodec_get_type((::AVCodecID)codec_id);
}

String^ FFmpeg::AVCodec::GetCodecName(FFmpeg::AVCodecID id)
{
	auto p = avcodec_get_name((::AVCodecID)id);
	return p != nullptr ? gcnew String(p) : nullptr;
}

String^ FFmpeg::AVCodec::GetColorRangeName(AVColorRange range)
{
	auto p = av_color_range_name((::AVColorRange)range);
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::AVCodec::GetColorPrimariesName(AVColorPrimaries primaries)
{
	auto p = av_color_primaries_name((::AVColorPrimaries)primaries);
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::AVCodec::GetColorTransferName(AVColorTransferCharacteristic transfer)
{
	auto p = av_color_transfer_name((::AVColorTransferCharacteristic)transfer);
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::AVCodec::GetColorSpaceName(AVColorSpace space)
{
	auto p = av_color_space_name((::AVColorSpace)space);
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::AVCodec::GetChromaLocationName(AVChromaLocation location)
{
	auto p = av_chroma_location_name((::AVChromaLocation)location);
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
System::Drawing::Size^ FFmpeg::AVCodec::EnumToChromaPos(AVChromaLocation pos)
{
	int x = 0;
	int y = 0;
	avcodec_enum_to_chroma_pos(&x,&y,(::AVChromaLocation)pos);
	return gcnew System::Drawing::Size(x,y);
}

FFmpeg::AVChromaLocation FFmpeg::AVCodec::ChromaPosToEnum(System::Drawing::Size^ pos)
{
	return (AVChromaLocation)avcodec_chroma_pos_to_enum(pos->Width,pos->Height);
}
//////////////////////////////////////////////////////
// AVComponentDescriptor
//////////////////////////////////////////////////////
FFmpeg::AVComponentDescriptor::AVComponentDescriptor(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
int FFmpeg::AVComponentDescriptor::_StructureSize::get()
{
	return sizeof(::AVComponentDescriptor);
}
//////////////////////////////////////////////////////
UInt16 FFmpeg::AVComponentDescriptor::plane::get()
{
	return ((::AVComponentDescriptor*)m_pPointer)->plane;
}
UInt16 FFmpeg::AVComponentDescriptor::step::get()
{
	return ((::AVComponentDescriptor*)m_pPointer)->step;
}
UInt16 FFmpeg::AVComponentDescriptor::offset::get()
{
	return ((::AVComponentDescriptor*)m_pPointer)->offset;
}
UInt16 FFmpeg::AVComponentDescriptor::shift::get()
{
	return ((::AVComponentDescriptor*)m_pPointer)->shift;
}
UInt16 FFmpeg::AVComponentDescriptor::depth::get()
{
	return ((::AVComponentDescriptor*)m_pPointer)->depth;
}
//////////////////////////////////////////////////////
// AVPixFmtDescriptor
//////////////////////////////////////////////////////
FFmpeg::AVPixFmtDescriptor::AVPixFmtDescriptor(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
int FFmpeg::AVPixFmtDescriptor::_StructureSize::get()
{
	return sizeof(::AVPixFmtDescriptor);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVPixFmtDescriptor::ToString()
{
	return "[ AVPixFmtDescriptor ] \"" + name + "\"";
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVPixFmtDescriptor::name::get()
{
	auto p = ((::AVPixFmtDescriptor*)m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}
byte FFmpeg::AVPixFmtDescriptor::nb_components::get()
{
	return ((::AVPixFmtDescriptor*)m_pPointer)->nb_components;
}
byte FFmpeg::AVPixFmtDescriptor::log2_chroma_w::get()
{
	return ((::AVPixFmtDescriptor*)m_pPointer)->log2_chroma_w;
}
byte FFmpeg::AVPixFmtDescriptor::log2_chroma_h::get()
{
	return ((::AVPixFmtDescriptor*)m_pPointer)->log2_chroma_h;
}
FFmpeg::AV_PIX_FMT_FLAG FFmpeg::AVPixFmtDescriptor::flags::get()
{
	return (FFmpeg::AV_PIX_FMT_FLAG)((::AVPixFmtDescriptor*)m_pPointer)->flags;
}
array<FFmpeg::AVComponentDescriptor^>^ FFmpeg::AVPixFmtDescriptor::comp::get()
{
	::AVComponentDescriptor * p = ((::AVPixFmtDescriptor*)m_pPointer)->comp;
	List<FFmpeg::AVComponentDescriptor^>^ _list = gcnew List<FFmpeg::AVComponentDescriptor^>();
	for (int i = 0; i < 4; i++)
	{
		_list->Add(_CreateObject<AVComponentDescriptor>(&p[i]));
	}
	return _list->ToArray();
}
String^ FFmpeg::AVPixFmtDescriptor::alias::get()
{
	auto p = ((::AVPixFmtDescriptor*)m_pPointer)->alias;
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
int FFmpeg::AVPixFmtDescriptor::bits_per_pixel::get()
{
	return av_get_bits_per_pixel((::AVPixFmtDescriptor*)m_pPointer);
}
int FFmpeg::AVPixFmtDescriptor::padded_bits_per_pixel::get()
{
	return av_get_padded_bits_per_pixel((::AVPixFmtDescriptor*)m_pPointer);
}
FFmpeg::AVPixelFormat FFmpeg::AVPixFmtDescriptor::id::get()
{
	return (AVPixelFormat)av_pix_fmt_desc_get_id((::AVPixFmtDescriptor*)m_pPointer);
}
//////////////////////////////////////////////////////
FFmpeg::AVPixFmtDescriptor^ FFmpeg::AVPixFmtDescriptor::pix_fmt_desc_get(AVPixelFormat pix_fmt)
{
	auto p = av_pix_fmt_desc_get((::AVPixelFormat)pix_fmt);
	return p != nullptr ? gcnew FFmpeg::AVPixFmtDescriptor((void*)p,nullptr) : nullptr;
}

FFmpeg::AVPixFmtDescriptor^ FFmpeg::AVPixFmtDescriptor::Next(AVPixFmtDescriptor^ _desc)
{
	auto p = av_pix_fmt_desc_next((::AVPixFmtDescriptor*)(_desc != nullptr ? _desc->_Pointer.ToPointer() : nullptr));
	return p != nullptr ? gcnew FFmpeg::AVPixFmtDescriptor((void*)p,nullptr) : nullptr;
}
//////////////////////////////////////////////////////
// AVPixel
//////////////////////////////////////////////////////
System::Drawing::Size^ FFmpeg::AVPixel::get_chroma_sub_sample(AVPixelFormat pix_fmt)
{
	int h_shift = 0;
	int v_shift = 0;
	av_pix_fmt_get_chroma_sub_sample((::AVPixelFormat)pix_fmt,&h_shift,&v_shift);
	return gcnew System::Drawing::Size(h_shift,v_shift);
}

UInt32 FFmpeg::AVPixel::pix_fmt_to_codec_tag(AVPixelFormat pix_fmt)
{
	return avcodec_pix_fmt_to_codec_tag((::AVPixelFormat)pix_fmt);
}

String^ FFmpeg::AVPixel::get_pix_fmt_name(AVPixelFormat pix_fmt)
{
	auto p = av_get_pix_fmt_name((::AVPixelFormat)pix_fmt);
	return p != nullptr ? gcnew String(p) : nullptr;
}

int FFmpeg::AVPixel::pix_fmt_count_planes(AVPixelFormat pix_fmt)
{
	return av_pix_fmt_count_planes((::AVPixelFormat)pix_fmt);
}

FFmpeg::AVPixelFormat FFmpeg::AVPixel::pix_fmt_swap_endianness(AVPixelFormat pix_fmt)
{
	return (AVPixelFormat)av_pix_fmt_swap_endianness((::AVPixelFormat)pix_fmt);
}

FFmpeg::FFLoss FFmpeg::AVPixel::get_pix_fmt_loss(AVPixelFormat dst_pix_fmt,AVPixelFormat src_pix_fmt,bool has_alpha)
{
	return (FFLoss)av_get_pix_fmt_loss((::AVPixelFormat)dst_pix_fmt,(::AVPixelFormat)src_pix_fmt,has_alpha ? 1 : 0);
}

FFmpeg::AVPixelFormat FFmpeg::AVPixel::find_best_pix_fmt_of_list(array<AVPixelFormat>^ pix_fmt_list,AVPixelFormat src_pix_fmt,bool has_alpha, FFLoss % loss_ptr)
{
	int loss = (int)loss_ptr;
	::AVPixelFormat * list = (::AVPixelFormat *)av_malloc(sizeof(::AVPixelFormat) * ((pix_fmt_list != nullptr ? pix_fmt_list->Length : 0) + 1));
	try
	{
		int index = 0;
		if (pix_fmt_list != nullptr)
		{
			while (index < pix_fmt_list->Length) { list[index] = (::AVPixelFormat)pix_fmt_list[index++]; }
		}
		list[index] = AV_PIX_FMT_NONE;
		return (AVPixelFormat)avcodec_find_best_pix_fmt_of_list(list,(::AVPixelFormat)src_pix_fmt,has_alpha ? 1 : 0,&loss);
	}
	finally
	{
		av_free(list);
		loss_ptr = (FFLoss)loss;
	}
}

FFmpeg::AVPixelFormat FFmpeg::AVPixel::find_best_pix_fmt_of_2(AVPixelFormat dst_pix_fmt1,AVPixelFormat dst_pix_fmt2,AVPixelFormat src_pix_fmt,bool has_alpha,FFLoss % loss_ptr)
{
	int loss = (int)loss_ptr;
	try
	{
		return (AVPixelFormat)av_find_best_pix_fmt_of_2((::AVPixelFormat)dst_pix_fmt1,(::AVPixelFormat)dst_pix_fmt2,(::AVPixelFormat)src_pix_fmt,has_alpha ? 1 : 0,&loss);
	}
	finally
	{
		loss_ptr = (FFLoss)loss;
	}
}
//////////////////////////////////////////////////////
// AV
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
// RcOverride
//////////////////////////////////////////////////////
FFmpeg::RcOverride::RcOverride(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
FFmpeg::RcOverride::RcOverride()
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
}
//////////////////////////////////////////////////////
int FFmpeg::RcOverride::_StructureSize::get()
{
	return sizeof(::RcOverride);
}
//////////////////////////////////////////////////////
int FFmpeg::RcOverride::start_frame::get()
{
	return ((::RcOverride*)m_pPointer)->start_frame;
}
void FFmpeg::RcOverride::start_frame::set(int value)
{
	((::RcOverride*)m_pPointer)->start_frame = value;
}

int FFmpeg::RcOverride::end_frame::get()
{
	return ((::RcOverride*)m_pPointer)->end_frame;
}
void FFmpeg::RcOverride::end_frame::set(int value)
{
	((::RcOverride*)m_pPointer)->end_frame = value;
}

int FFmpeg::RcOverride::qscale::get()
{
	return ((::RcOverride*)m_pPointer)->qscale;
}
void FFmpeg::RcOverride::qscale::set(int value)
{
	((::RcOverride*)m_pPointer)->qscale = value;
}

float FFmpeg::RcOverride::quality_factor::get()
{
	return ((::RcOverride*)m_pPointer)->quality_factor;
}
void FFmpeg::RcOverride::quality_factor::set(float value)
{
	((::RcOverride*)m_pPointer)->quality_factor = value;
}
//////////////////////////////////////////////////////
// AVSubtitleRect
//////////////////////////////////////////////////////
FFmpeg::AVSubtitleRect::AVSubtitleRect(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}

FFmpeg::AVSubtitleRect::AVSubtitleRect()
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
}
//////////////////////////////////////////////////////
int FFmpeg::AVSubtitleRect::_StructureSize::get() 
{
	return sizeof(::AVSubtitleRect);
}
//////////////////////////////////////////////////////
int FFmpeg::AVSubtitleRect::x::get()
{
	return ((::AVSubtitleRect*)m_pPointer)->x;
}
void FFmpeg::AVSubtitleRect::x::set(int value)
{
	((::AVSubtitleRect*)m_pPointer)->x = value;
}
int FFmpeg::AVSubtitleRect::y::get()
{
	return ((::AVSubtitleRect*)m_pPointer)->y;
}
void FFmpeg::AVSubtitleRect::y::set(int value)
{
	((::AVSubtitleRect*)m_pPointer)->y = value;
}
int FFmpeg::AVSubtitleRect::w::get()
{
	return ((::AVSubtitleRect*)m_pPointer)->w;
}
void FFmpeg::AVSubtitleRect::w::set(int value)
{
	((::AVSubtitleRect*)m_pPointer)->w = value;
}
int FFmpeg::AVSubtitleRect::h::get()
{
	return ((::AVSubtitleRect*)m_pPointer)->h;
}
void FFmpeg::AVSubtitleRect::h::set(int value)
{
	((::AVSubtitleRect*)m_pPointer)->h = value;
}
int FFmpeg::AVSubtitleRect::nb_colors::get()
{
	return ((::AVSubtitleRect*)m_pPointer)->nb_colors;
}
void FFmpeg::AVSubtitleRect::nb_colors::set(int value)
{
	((::AVSubtitleRect*)m_pPointer)->nb_colors = value;
}
FFmpeg::AVPicture^ FFmpeg::AVSubtitleRect::pict::get()
{
#if FF_API_AVPICTURE
	return _CreateObject<AVPicture>(&((::AVSubtitleRect*)m_pPointer)->pict);
#else
	return nullptr;
#endif
}

FFmpeg::AVArray<IntPtr>^ FFmpeg::AVSubtitleRect::data::get()
{
	auto p = ((::AVSubtitleRect *)m_pPointer)->data;
	AVArray<IntPtr>^ _array = (AVArray<IntPtr>^)GetObject((IntPtr)p);
	if (_array) return _array;
	return gcnew AVArray<IntPtr>(p,this,4);
}

FFmpeg::AVArray<int>^ FFmpeg::AVSubtitleRect::linesize::get()
{
	auto p = ((::AVSubtitleRect *)m_pPointer)->linesize;
	AVArray<int>^ _array = (AVArray<int>^)GetObject((IntPtr)p);
	if (_array) return _array;
	return gcnew AVArray<int>(p,this,sizeof(int),4);
}

FFmpeg::AVSubtitleType FFmpeg::AVSubtitleRect::type::get()
{
	return (AVSubtitleType)((::AVSubtitleRect*)m_pPointer)->type;
}
void FFmpeg::AVSubtitleRect::type::set(AVSubtitleType value)
{
	((::AVSubtitleRect*)m_pPointer)->type = (::AVSubtitleType)value;
}
String^ FFmpeg::AVSubtitleRect::text::get()
{
	auto p = ((::AVSubtitleRect*)m_pPointer)->text;
	return p != nullptr ? gcnew String(p) : nullptr;
}
void FFmpeg::AVSubtitleRect::text::set(String^ value)
{
	((::AVSubtitleRect*)m_pPointer)->text = (char*)AllocString("text",value).ToPointer();
}
String^ FFmpeg::AVSubtitleRect::ass::get()
{
	auto p = ((::AVSubtitleRect*)m_pPointer)->ass;
	return p != nullptr ? gcnew String(p) : nullptr;
}
void FFmpeg::AVSubtitleRect::ass::set(String^ value)
{
	((::AVSubtitleRect*)m_pPointer)->ass = (char*)AllocString("ass",value).ToPointer();
}
int FFmpeg::AVSubtitleRect::flags::get()
{
	return ((::AVSubtitleRect*)m_pPointer)->flags;
}
void FFmpeg::AVSubtitleRect::flags::set(int value)
{
	((::AVSubtitleRect*)m_pPointer)->flags = value;
}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVSubtitleRect::GetClass()
{
	return gcnew AVClass((void*)avcodec_get_subtitle_rect_class(),nullptr);
}
//////////////////////////////////////////////////////
// AVSubtitle
//////////////////////////////////////////////////////
FFmpeg::AVSubtitle::AVSubtitle(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}

FFmpeg::AVSubtitle::AVSubtitle()
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
	m_pDescructor = (TFreeFN *)avsubtitle_free;
}
//////////////////////////////////////////////////////
int FFmpeg::AVSubtitle::_StructureSize::get() 
{
	return sizeof(::AVSubtitle);
}
//////////////////////////////////////////////////////
UInt16 FFmpeg::AVSubtitle::format::get()
{
	return ((::AVSubtitle*)m_pPointer)->format;
}
void FFmpeg::AVSubtitle::format::set(UInt16 value)
{
	((::AVSubtitle*)m_pPointer)->format = value;
}
UInt32 FFmpeg::AVSubtitle::start_display_time::get()
{
	return ((::AVSubtitle*)m_pPointer)->start_display_time;
}
void FFmpeg::AVSubtitle::start_display_time::set(UInt32 value)
{
	((::AVSubtitle*)m_pPointer)->start_display_time = value;
}
UInt32 FFmpeg::AVSubtitle::end_display_time::get()
{
	return ((::AVSubtitle*)m_pPointer)->end_display_time;
}
void FFmpeg::AVSubtitle::end_display_time::set(UInt32 value)
{
	((::AVSubtitle*)m_pPointer)->end_display_time = value;
}
array<FFmpeg::AVSubtitleRect^>^ FFmpeg::AVSubtitle::rects::get()
{
	List<FFmpeg::AVSubtitleRect^>^ _list = nullptr;
	::AVSubtitleRect** _pointer = ((::AVSubtitle*)m_pPointer)->rects;
	int _count = ((::AVSubtitle*)m_pPointer)->num_rects;
	if (_pointer && _count > 0)
	{
		_list = gcnew List<FFmpeg::AVSubtitleRect^>();
		for (int i = 0; i < _count; i++)
		{
			_list->Add(_CreateObject<AVSubtitleRect>(_pointer[i]));
		}
	}
	return _list != nullptr ? _list->ToArray() : nullptr;
}
void FFmpeg::AVSubtitle::rects::set(array<AVSubtitleRect^>^ value)
{
	{
		int nCount = ((::AVSubtitle*)m_pPointer)->num_rects;
		::AVSubtitleRect ** p = (::AVSubtitleRect **)((::AVSubtitle*)m_pPointer)->rects;
		if (p)
		{
			while (nCount-- > 0)
			{
				RemoveObject((IntPtr)*p++);
			}
		}
		((::AVSubtitle*)m_pPointer)->num_rects = 0;
		((::AVSubtitle*)m_pPointer)->rects = nullptr;
		FreeMemory("rects");
	}
	if (value != nullptr && value->Length > 0)
	{
		::AVSubtitleRect ** p = (::AVSubtitleRect **)AllocMemory("rects",value->Length * (sizeof(::AVSubtitleRect*))).ToPointer();
		if (p)
		{
			((::AVSubtitle*)m_pPointer)->rects = p;
			((::AVSubtitle*)m_pPointer)->num_rects = value->Length;
			for (int i = 0; i < value->Length; i++)
			{
				AddObject((IntPtr)*p,value[i]);
				*p++ = (::AVSubtitleRect*)value[i]->_Pointer.ToPointer();
			}
		}
	}
}
Int64 FFmpeg::AVSubtitle::pts::get()
{
	return ((::AVSubtitle*)m_pPointer)->pts;
}
void FFmpeg::AVSubtitle::pts::set(Int64 value)
{
	((::AVSubtitle*)m_pPointer)->pts = value;
}
//////////////////////////////////////////////////////
// AVBufferRef
//////////////////////////////////////////////////////
void buffer_data_free_cb(void *opaque, uint8_t *data)
{
	GCHandle _handle = GCHandle::FromIntPtr((IntPtr)opaque);
	if (_handle.IsAllocated)
	{
		((FFmpeg::AVBufferRef^)_handle.Target)->FreeCB((IntPtr)data);
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVBufferRef::AVBufferRef(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
	, m_pOpaque(nullptr)
	, m_pFreeCB(nullptr)
{
}
FFmpeg::AVBufferRef::AVBufferRef(int _size)
	: AVBase(nullptr,nullptr)
	, m_pOpaque(nullptr)
	, m_pFreeCB(nullptr)
{
	m_pPointer = av_buffer_alloc(_size);
	m_pFreep = (TFreeFNP*)av_buffer_unref;
}
FFmpeg::AVBufferRef::AVBufferRef(AVBufferRef^ _buffer)
	: AVBase(nullptr,nullptr)
	, m_pOpaque(nullptr)
	, m_pFreeCB(nullptr)
{
	m_pPointer = av_buffer_ref((::AVBufferRef*)_buffer->m_pPointer);
	m_pFreep = (TFreeFNP*)av_buffer_unref;
}
FFmpeg::AVBufferRef::AVBufferRef(IntPtr _data, int _size, AVBufferFreeCB^ free_cb, Object^ opaque)
	: AVBase(nullptr,nullptr)
	, m_pOpaque(opaque)
	, m_pFreeCB(free_cb)
{
	IntPtr _this = GCHandle::ToIntPtr(GCHandle::Alloc(this,GCHandleType::Weak));
	m_pPointer = av_buffer_create((uint8_t*)_data.ToPointer(),_size,buffer_data_free_cb,_this.ToPointer(),0);
	m_pFreep = (TFreeFNP*)av_buffer_unref;
}
//////////////////////////////////////////////////////
int FFmpeg::AVBufferRef::_StructureSize::get()
{
	return sizeof(::AVBufferRef);
}
//////////////////////////////////////////////////////
IntPtr FFmpeg::AVBufferRef::data::get()
{
	return (IntPtr)((::AVBufferRef*)m_pPointer)->data;
}

UInt64 FFmpeg::AVBufferRef::size::get()
{
	return ((::AVBufferRef*)m_pPointer)->size;
}
//////////////////////////////////////////////////////
int FFmpeg::AVBufferRef::RefCount::get()
{ 
	return av_buffer_get_ref_count((::AVBufferRef*)m_pPointer);
}

IntPtr FFmpeg::AVBufferRef::Opaque::get()
{
	return (IntPtr)av_buffer_get_opaque((::AVBufferRef*)m_pPointer);
}

bool FFmpeg::AVBufferRef::IsWritable::get()
{ 
	return 1 == av_buffer_is_writable((::AVBufferRef*)m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::AVBufferRef::MakeWritable()
{
	int _result;
	::AVBufferRef * _pointer = (::AVBufferRef *)m_pPointer;
	if (0 == (_result = av_buffer_make_writable(&_pointer)))
	{
		m_pPointer = _pointer;
	}
	return _result;
}

FFmpeg::AVRESULT FFmpeg::AVBufferRef::Realloc(int _size)
{
	int _result;
	::AVBufferRef * _pointer = (::AVBufferRef *)m_pPointer;
	if (0 == (_result = av_buffer_realloc(&_pointer,_size)))
	{
		m_pPointer = _pointer;
	}
	return _result;
}
//////////////////////////////////////////////////////
// ICloneable
Object^ FFmpeg::AVBufferRef::Clone()
{
	return gcnew AVBufferRef(this);
}
//////////////////////////////////////////////////////
void FFmpeg::AVBufferRef::FreeCB(IntPtr data)
{
	if (m_pFreeCB) m_pFreeCB(m_pOpaque,data);
}
//////////////////////////////////////////////////////
// AVCodecParameters
//////////////////////////////////////////////////////
FFmpeg::AVCodecParameters::AVCodecParameters(void * _pointer, AVBase^ _parent)
	: AVBase(_pointer, _parent)
{

}

FFmpeg::AVCodecParameters::AVCodecParameters()
	: AVBase(nullptr, nullptr)
{
	m_pPointer = avcodec_parameters_alloc();
	m_pFreep = (TFreeFNP*)avcodec_parameters_free;
}
FFmpeg::AVCodecParameters::AVCodecParameters(AVCodecContext^ codec)
	: AVCodecParameters()
{
	FromContext(codec);
}

FFmpeg::AVCodecParameters::AVCodecParameters(AVCodecParameters^ src)
	: AVCodecParameters()
{
	src->CopyTo(this);
}
//////////////////////////////////////////////////////
FFmpeg::AVMediaType FFmpeg::AVCodecParameters::codec_type::get()
{
	return (AVMediaType)((::AVCodecParameters*)m_pPointer)->codec_type;
}
void FFmpeg::AVCodecParameters::codec_type::set(AVMediaType value)
{
	((::AVCodecParameters*)m_pPointer)->codec_type = (::AVMediaType)value;
}
FFmpeg::AVCodecID FFmpeg::AVCodecParameters::codec_id::get()
{
	return (AVCodecID)((::AVCodecParameters*)m_pPointer)->codec_id;
}
void FFmpeg::AVCodecParameters::codec_id::set(AVCodecID value)
{
	((::AVCodecParameters*)m_pPointer)->codec_id = (::AVCodecID)value;
}
unsigned int FFmpeg::AVCodecParameters::codec_tag::get()
{
	return ((::AVCodecParameters*)m_pPointer)->codec_tag;
}
void FFmpeg::AVCodecParameters::codec_tag::set(unsigned int value)
{
	((::AVCodecParameters*)m_pPointer)->codec_tag = value;
}
IntPtr FFmpeg::AVCodecParameters::extradata::get()
{
	return IntPtr(((::AVCodecParameters*)m_pPointer)->extradata);
}
void FFmpeg::AVCodecParameters::extradata::set(IntPtr value)
{
	((::AVCodecParameters*)m_pPointer)->extradata = (uint8_t*)value.ToPointer();
}
int FFmpeg::AVCodecParameters::extradata_size::get()
{
	return ((::AVCodecParameters*)m_pPointer)->extradata_size;
}
void FFmpeg::AVCodecParameters::extradata_size::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->extradata_size = value;
}
int FFmpeg::AVCodecParameters::format::get()
{
	return ((::AVCodecParameters*)m_pPointer)->format;
}
void FFmpeg::AVCodecParameters::format::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->format = value;
}
Int64 FFmpeg::AVCodecParameters::bit_rate::get()
{
	return ((::AVCodecParameters*)m_pPointer)->bit_rate;
}
void FFmpeg::AVCodecParameters::bit_rate::set(Int64 value)
{
	((::AVCodecParameters*)m_pPointer)->bit_rate = value;
}
int FFmpeg::AVCodecParameters::bits_per_coded_sample::get()
{
	return ((::AVCodecParameters*)m_pPointer)->bits_per_coded_sample;
}
void FFmpeg::AVCodecParameters::bits_per_coded_sample::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->bits_per_coded_sample = value;
}
int FFmpeg::AVCodecParameters::bits_per_raw_sample::get()
{
	return ((::AVCodecParameters*)m_pPointer)->bits_per_raw_sample;
}
void FFmpeg::AVCodecParameters::bits_per_raw_sample::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->bits_per_raw_sample = value;
}
FFmpeg::Profile FFmpeg::AVCodecParameters::profile::get()
{
	return (Profile)((::AVCodecParameters*)m_pPointer)->profile;
}
void FFmpeg::AVCodecParameters::profile::set(Profile value)
{
	((::AVCodecParameters*)m_pPointer)->profile = (int)value;
}
int FFmpeg::AVCodecParameters::level::get()
{
	return ((::AVCodecParameters*)m_pPointer)->level;
}
void FFmpeg::AVCodecParameters::level::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->level = value;
}
int FFmpeg::AVCodecParameters::width::get()
{
	return ((::AVCodecParameters*)m_pPointer)->width;
}
void FFmpeg::AVCodecParameters::width::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->width = value;
}
int FFmpeg::AVCodecParameters::height::get()
{
	return ((::AVCodecParameters*)m_pPointer)->height;
}
void FFmpeg::AVCodecParameters::height::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->height = value;
}
FFmpeg::AVRational^ FFmpeg::AVCodecParameters::sample_aspect_ratio::get()
{
	return _CreateObject<AVRational>(&((::AVCodecParameters*)m_pPointer)->sample_aspect_ratio);
}
void FFmpeg::AVCodecParameters::sample_aspect_ratio::set(FFmpeg::AVRational^ value)
{
	if (value != nullptr)
	{
		((::AVCodecParameters*)m_pPointer)->sample_aspect_ratio.num = value->num;
		((::AVCodecParameters*)m_pPointer)->sample_aspect_ratio.den = value->den;
	}
}
FFmpeg::AVFieldOrder FFmpeg::AVCodecParameters::field_order::get()
{
	return (AVFieldOrder)((::AVCodecParameters*)m_pPointer)->field_order;
}
void FFmpeg::AVCodecParameters::field_order::set(FFmpeg::AVFieldOrder value)
{
	((::AVCodecParameters*)m_pPointer)->field_order = (::AVFieldOrder)value;
}
FFmpeg::AVColorRange FFmpeg::AVCodecParameters::color_range::get()
{
	return (AVColorRange)((::AVCodecParameters*)m_pPointer)->color_range;
}
void FFmpeg::AVCodecParameters::color_range::set(FFmpeg::AVColorRange value)
{
	((::AVCodecParameters*)m_pPointer)->color_range = (::AVColorRange)value;
}
FFmpeg::AVColorPrimaries FFmpeg::AVCodecParameters::color_primaries::get()
{
	return (AVColorPrimaries)((::AVCodecParameters*)m_pPointer)->color_primaries;
}
void FFmpeg::AVCodecParameters::color_primaries::set(FFmpeg::AVColorPrimaries value)
{
	((::AVCodecParameters*)m_pPointer)->color_primaries = (::AVColorPrimaries)value;
}
FFmpeg::AVColorTransferCharacteristic FFmpeg::AVCodecParameters::color_trc::get()
{
	return (AVColorTransferCharacteristic)((::AVCodecParameters*)m_pPointer)->color_trc;
}
void FFmpeg::AVCodecParameters::color_trc::set(FFmpeg::AVColorTransferCharacteristic value)
{
	((::AVCodecParameters*)m_pPointer)->color_trc = (::AVColorTransferCharacteristic)value;
}
FFmpeg::AVColorSpace FFmpeg::AVCodecParameters::color_space::get()
{
	return (AVColorSpace)((::AVCodecParameters*)m_pPointer)->color_space;
}
void FFmpeg::AVCodecParameters::color_space::set(FFmpeg::AVColorSpace value)
{
	((::AVCodecParameters*)m_pPointer)->color_space = (::AVColorSpace)value;
}
FFmpeg::AVChromaLocation FFmpeg::AVCodecParameters::chroma_location::get()
{
	return (AVChromaLocation)((::AVCodecParameters*)m_pPointer)->chroma_location;
}
void FFmpeg::AVCodecParameters::chroma_location::set(FFmpeg::AVChromaLocation value)
{
	((::AVCodecParameters*)m_pPointer)->chroma_location = (::AVChromaLocation)value;
}
int FFmpeg::AVCodecParameters::video_delay::get()
{
	return ((::AVCodecParameters*)m_pPointer)->video_delay;
}
void FFmpeg::AVCodecParameters::video_delay::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->video_delay = value;
}
FFmpeg::AVChannelLayout FFmpeg::AVCodecParameters::channel_layout::get()
{
	return (AVChannelLayout)((::AVCodecParameters*)m_pPointer)->channel_layout;
}
void FFmpeg::AVCodecParameters::channel_layout::set(FFmpeg::AVChannelLayout value)
{
	((::AVCodecParameters*)m_pPointer)->channel_layout = (uint64_t)value;
}
int FFmpeg::AVCodecParameters::channels::get()
{
	return ((::AVCodecParameters*)m_pPointer)->channels;
}
void FFmpeg::AVCodecParameters::channels::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->channels = value;
}
int FFmpeg::AVCodecParameters::sample_rate::get()
{
	return ((::AVCodecParameters*)m_pPointer)->sample_rate;
}
void FFmpeg::AVCodecParameters::sample_rate::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->sample_rate = value;
}
int FFmpeg::AVCodecParameters::block_align::get()
{
	return ((::AVCodecParameters*)m_pPointer)->block_align;
}
void FFmpeg::AVCodecParameters::block_align::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->block_align = value;
}
int FFmpeg::AVCodecParameters::frame_size::get()
{
	return ((::AVCodecParameters*)m_pPointer)->frame_size;
}
void FFmpeg::AVCodecParameters::frame_size::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->frame_size = value;
}
int FFmpeg::AVCodecParameters::initial_padding::get()
{
	return ((::AVCodecParameters*)m_pPointer)->initial_padding;
}
void FFmpeg::AVCodecParameters::initial_padding::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->initial_padding = value;
}
int FFmpeg::AVCodecParameters::trailing_padding::get()
{
	return ((::AVCodecParameters*)m_pPointer)->trailing_padding;
}
void FFmpeg::AVCodecParameters::trailing_padding::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->trailing_padding = value;
}
int FFmpeg::AVCodecParameters::seek_preroll::get()
{
	return ((::AVCodecParameters*)m_pPointer)->seek_preroll;
}
void FFmpeg::AVCodecParameters::seek_preroll::set(int value)
{
	((::AVCodecParameters*)m_pPointer)->seek_preroll = value;
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVCodecParameters::FromContext(AVCodecContext^ codec)
{
	return avcodec_parameters_from_context(
		((::AVCodecParameters*)m_pPointer),(::AVCodecContext*)codec->_Pointer.ToPointer());
}

FFmpeg::AVRESULT FFmpeg::AVCodecParameters::CopyTo(AVCodecContext^ context)
{
	return avcodec_parameters_to_context(
		(::AVCodecContext*)context->_Pointer.ToPointer(), (::AVCodecParameters*)m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::AVCodecParameters::CopyTo(AVCodecParameters^ dst)
{
	return avcodec_parameters_copy(
		(::AVCodecParameters*)dst->_Pointer.ToPointer(),((::AVCodecParameters*)m_pPointer));
}
//////////////////////////////////////////////////////
int FFmpeg::AVCodecParameters::GetAudioFrameDuration(int frame_bytes)
{
	return av_get_audio_frame_duration2((::AVCodecParameters*)m_pPointer, frame_bytes);
}
//////////////////////////////////////////////////////
System::Object ^ FFmpeg::AVCodecParameters::Clone()
{
	return gcnew AVCodecParameters(this);
}
//////////////////////////////////////////////////////
// AVCPBProperties
//////////////////////////////////////////////////////
FFmpeg::AVCPBProperties::AVCPBProperties(void * _pointer, AVBase^ _parent)
	: AVBase(_pointer, _parent)
	, m_nSize(sizeof(::AVCPBProperties))
{

}
//////////////////////////////////////////////////////
FFmpeg::AVCPBProperties::AVCPBProperties()
	: AVBase(nullptr, nullptr)
{
	size_t size = 0;
	m_pPointer = av_cpb_properties_alloc(&size);
	m_nSize = (int)size;
	m_pFree = av_free;
}
//////////////////////////////////////////////////////
int FFmpeg::AVCPBProperties::size::get()
{
	return m_nSize;
}
int FFmpeg::AVCPBProperties::_StructureSize::get() 
{
	return size::get();
}
//////////////////////////////////////////////////////
Int64 FFmpeg::AVCPBProperties::max_bitrate::get()
{
	return ((::AVCPBProperties*)m_pPointer)->max_bitrate;
}
Int64 FFmpeg::AVCPBProperties::min_bitrate::get()
{
	return ((::AVCPBProperties*)m_pPointer)->min_bitrate;
}
Int64 FFmpeg::AVCPBProperties::avg_bitrate::get()
{
	return ((::AVCPBProperties*)m_pPointer)->avg_bitrate;
}
Int64 FFmpeg::AVCPBProperties::buffer_size::get()
{
	return ((::AVCPBProperties*)m_pPointer)->buffer_size;
}
UInt64 FFmpeg::AVCPBProperties::vbv_delay::get()
{
	return ((::AVCPBProperties*)m_pPointer)->vbv_delay;
}
//////////////////////////////////////////////////////
// AVPacketSideData
//////////////////////////////////////////////////////
FFmpeg::AVPacketSideData::AVPacketSideData(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
int FFmpeg::AVPacketSideData::_StructureSize::get()
{
	return sizeof(::AVPacketSideData);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVPacketSideData::ToString()
{
	String^ _name = SideDataName(type);
	return String::IsNullOrEmpty(_name) ? __super::ToString() : ("[ AVPacketSideData ] \"" + _name + "\"");
}
//////////////////////////////////////////////////////
IntPtr FFmpeg::AVPacketSideData::data::get()
{
	return (IntPtr)((::AVPacketSideData*)m_pPointer)->data;
}
UInt64 FFmpeg::AVPacketSideData::size::get()
{
	return ((::AVPacketSideData*)m_pPointer)->size;
}
FFmpeg::AVPacketSideDataType FFmpeg::AVPacketSideData::type::get()
{
	return (FFmpeg::AVPacketSideDataType)((::AVPacketSideData*)m_pPointer)->type;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVPacketSideData::name::get()
{
	return SideDataName(type);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVPacketSideData::SideDataName(FFmpeg::AVPacketSideDataType _type)
{
	auto p = av_packet_side_data_name((::AVPacketSideDataType)_type);
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
// AVFrameSideData
//////////////////////////////////////////////////////
FFmpeg::AVFrameSideData::AVFrameSideData(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
int FFmpeg::AVFrameSideData::_StructureSize::get()
{
	return sizeof(::AVPacketSideData);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVFrameSideData::ToString()
{
	String^ _name = SideDataName(type);
	return String::IsNullOrEmpty(_name) ? __super::ToString() : ("[ AVFrameSideData ] \"" + _name + "\"");
}
//////////////////////////////////////////////////////
IntPtr FFmpeg::AVFrameSideData::data::get()
{
	return (IntPtr)((::AVPacketSideData*)m_pPointer)->data;
}
UInt64 FFmpeg::AVFrameSideData::size::get()
{
	return ((::AVFrameSideData*)m_pPointer)->size;
}
FFmpeg::AVFrameSideDataType FFmpeg::AVFrameSideData::type::get()
{
	return (FFmpeg::AVFrameSideDataType)((::AVFrameSideData*)m_pPointer)->type;
}

FFmpeg::AVDictionary^ FFmpeg::AVFrameSideData::metadata::get()
{
	auto p = ((::AVFrameSideData*)m_pPointer)->metadata;
	return _CreateObject<FFmpeg::AVDictionary>(p);
}

void FFmpeg::AVFrameSideData::metadata::set(AVDictionary^ value)
{
	AddObject((IntPtr)((::AVFrameSideData*)m_pPointer)->metadata,value);
	((::AVFrameSideData*)m_pPointer)->metadata = (::AVDictionary*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}

FFmpeg::AVBufferRef^ FFmpeg::AVFrameSideData::buf::get()
{
	auto p = ((::AVFrameSideData*)m_pPointer)->buf;
	return _CreateObject<FFmpeg::AVBufferRef>(p);
}

void FFmpeg::AVFrameSideData::buf::set(AVBufferRef^ value)
{
	AddObject((IntPtr)((::AVFrameSideData*)m_pPointer)->buf,value);
	((::AVFrameSideData*)m_pPointer)->buf = (::AVBufferRef*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVFrameSideData::SideDataName(FFmpeg::AVFrameSideDataType _type)
{
	auto p = av_frame_side_data_name((::AVFrameSideDataType)_type);
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
// AVPacket
//////////////////////////////////////////////////////
void packet_data_free_cb(void *opaque, uint8_t *data)
{
	GCHandle _handle = GCHandle::FromIntPtr((IntPtr)opaque);
	if (_handle.IsAllocated)
	{
		((FFmpeg::AVPacket^)_handle.Target)->FreeCB((IntPtr)data);
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVPacket::AVPacket(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
	, m_pOpaque(nullptr)
	, m_pFreeCB(nullptr)
{
}

FFmpeg::AVPacket::AVPacket()
	: AVBase(nullptr,nullptr)
	, m_pOpaque(nullptr)
	, m_pFreeCB(nullptr)
{
	m_pPointer = av_packet_alloc();
	m_pFreep = (TFreeFNP*)av_packet_free;
	m_pDescructor = (TFreeFN*)av_packet_unref;
	
	av_init_packet((::AVPacket*)m_pPointer);
	((::AVPacket*)m_pPointer)->data = nullptr;
	((::AVPacket*)m_pPointer)->size = 0;
}

FFmpeg::AVPacket::AVPacket(int _size)
	: AVBase(nullptr,nullptr)
	, m_pOpaque(nullptr)
	, m_pFreeCB(nullptr)
{
	m_pPointer = av_packet_alloc();
	m_pFreep = (TFreeFNP*)av_packet_free;
	m_pDescructor = (TFreeFN*)av_packet_unref;

	av_new_packet((::AVPacket*)m_pPointer,_size);
}

FFmpeg::AVPacket::AVPacket(IntPtr _data,int _size)
	: AVBase(nullptr,nullptr)
	, m_pOpaque(nullptr)
	, m_pFreeCB(nullptr)
{
	::AVPacket* p = av_packet_alloc();
	m_pFreep = (TFreeFNP*)av_packet_free;
	m_pDescructor = (TFreeFN*)av_packet_unref;

	av_init_packet(p);
	p->data = (uint8_t*)_data.ToPointer();
	p->size = _size;
	m_pPointer = p;
}

FFmpeg::AVPacket::AVPacket(IntPtr _data, int _size, AVBufferFreeCB^ free_cb, Object^ opaque)
	: AVBase(nullptr,nullptr)
	, m_pOpaque(opaque)
	, m_pFreeCB(free_cb)
{
	::AVPacket* p = av_packet_alloc();
	m_pFreep = (TFreeFNP*)av_packet_free;
	m_pDescructor = (TFreeFN*)av_packet_unref;

	av_init_packet(p);
	p->data = (uint8_t*)_data.ToPointer();
	p->size = _size;
	IntPtr _this = GCHandle::ToIntPtr(GCHandle::Alloc(this,GCHandleType::Weak));
	p->buf = av_buffer_create((uint8_t*)_data.ToPointer(),_size,packet_data_free_cb,_this.ToPointer(),0);
	m_pPointer = p;
}

FFmpeg::AVPacket::AVPacket(AVBufferRef^ buf)
	: AVBase(nullptr,nullptr)
	, m_pOpaque(nullptr)
	, m_pFreeCB(nullptr)
{
	::AVPacket* p = av_packet_alloc();
	::AVBufferRef* bufref = (::AVBufferRef*)buf->_Pointer.ToPointer();
	m_pFreep = (TFreeFNP*)av_packet_free;
	m_pDescructor = (TFreeFN*)av_packet_unref;
	av_init_packet(p);
	p->data = bufref->data;
	p->size = bufref->size;
	p->buf = bufref;
	m_pPointer = p;
	AddObject((IntPtr)bufref,buf);
}

FFmpeg::AVPacket::AVPacket(AVPacket^ _packet)
	: AVBase(nullptr,nullptr)
	, m_pOpaque(nullptr)
	, m_pFreeCB(nullptr)
{
	m_pPointer = av_packet_alloc();
	m_pFreep = (TFreeFNP*)av_packet_free;
	m_pDescructor = (TFreeFN*)av_packet_unref;

	av_packet_ref((::AVPacket*)m_pPointer,(::AVPacket*)_packet->m_pPointer);
}
//////////////////////////////////////////////////////
FFmpeg::AVPacket::~AVPacket()
{
	Free();
}
//////////////////////////////////////////////////////
int FFmpeg::AVPacket::_StructureSize::get()
{
	return sizeof(::AVPacket);
}
//////////////////////////////////////////////////////
FFmpeg::AVBufferRef^ FFmpeg::AVPacket::buf::get()
{
	return _CreateObject<FFmpeg::AVBufferRef>(((::AVPacket*)m_pPointer)->buf);
}
Int64 FFmpeg::AVPacket::pts::get()
{
	return ((::AVPacket*)m_pPointer)->pts;
}
void FFmpeg::AVPacket::pts::set(Int64 value)
{
	((::AVPacket*)m_pPointer)->pts = value;
}
Int64 FFmpeg::AVPacket::dts::get()
{
	return ((::AVPacket*)m_pPointer)->dts;
}
void FFmpeg::AVPacket::dts::set(Int64 value)
{
	((::AVPacket*)m_pPointer)->dts = value;
}
FFmpeg::AVMemPtr^ FFmpeg::AVPacket::data::get()
{
	return gcnew AVMemPtr((IntPtr)((::AVPacket*)m_pPointer)->data,((::AVPacket*)m_pPointer)->size);
}
void FFmpeg::AVPacket::data::set(AVMemPtr^ value)
{
	IntPtr p = value;
	if (((::AVPacket*)m_pPointer)->data != (uint8_t*)p.ToPointer())
	{
		av_buffer_unref(&((::AVPacket*)m_pPointer)->buf);
		((::AVPacket*)m_pPointer)->data = (uint8_t*)p.ToPointer();
		if (value != nullptr && value->size > 0)
		{
			((::AVPacket*)m_pPointer)->size = value->size;
		}
	}
}
int FFmpeg::AVPacket::size::get()
{
	return ((::AVPacket*)m_pPointer)->size;
}
void FFmpeg::AVPacket::size::set(int value)
{
	((::AVPacket*)m_pPointer)->size = value;
}
int FFmpeg::AVPacket::stream_index::get()
{
	return ((::AVPacket*)m_pPointer)->stream_index;
}
void FFmpeg::AVPacket::stream_index::set(int value)
{
	((::AVPacket*)m_pPointer)->stream_index = value;
}
FFmpeg::AvPktFlag FFmpeg::AVPacket::flags::get()
{
	return (FFmpeg::AvPktFlag)((::AVPacket*)m_pPointer)->flags;
}
void FFmpeg::AVPacket::flags::set(FFmpeg::AvPktFlag value)
{
	((::AVPacket*)m_pPointer)->flags = (int)value;
}
array<FFmpeg::AVPacketSideData^>^ FFmpeg::AVPacket::side_data::get()
{
	List<FFmpeg::AVPacketSideData^>^ _list = nullptr;
	::AVPacketSideData* _pointer = ((::AVPacket*)m_pPointer)->side_data;
	int _count = ((::AVPacket*)m_pPointer)->side_data_elems;
	if (_pointer && _count > 0)
	{
		_list = gcnew List<FFmpeg::AVPacketSideData^>();
		for (int i = 0; i < _count; i++)
		{
			_list->Add(_CreateObject<AVPacketSideData>(&_pointer[i]));
		}
	}
	return _list != nullptr ? _list->ToArray() : nullptr;
}
Int64 FFmpeg::AVPacket::duration::get()
{
	return ((::AVPacket*)m_pPointer)->duration;
}
void FFmpeg::AVPacket::duration::set(Int64 value)
{
	((::AVPacket*)m_pPointer)->duration = value;
}
Int64 FFmpeg::AVPacket::pos::get()
{
	return ((::AVPacket*)m_pPointer)->pos;
}
void FFmpeg::AVPacket::pos::set(Int64 value)
{
	((::AVPacket*)m_pPointer)->pos = value;
}
Int64 FFmpeg::AVPacket::convergence_duration::get()
{
#if FF_API_CONVERGENCE_DURATION
	return ((::AVPacket*)m_pPointer)->convergence_duration;
#else
	return ((::AVPacket*)m_pPointer)->duration;
#endif
}
void FFmpeg::AVPacket::convergence_duration::set(Int64 value)
{
#if FF_API_CONVERGENCE_DURATION
	((::AVPacket*)m_pPointer)->convergence_duration = value;
#else
	((::AVPacket*)m_pPointer)->duration = value;
#endif
}
//////////////////////////////////////////////////////
void FFmpeg::AVPacket::Free()
{
	if (m_pPointer)
	{
		av_packet_unref((::AVPacket*)m_pPointer);
	}
}
void FFmpeg::AVPacket::Shrink(int _size)
{
	av_shrink_packet((::AVPacket *)m_pPointer, _size);
}
FFmpeg::AVRESULT FFmpeg::AVPacket::Alloc(int _size)
{
	return Grow(_size);
}
FFmpeg::AVRESULT FFmpeg::AVPacket::Grow(int _size)
{
	return av_grow_packet((::AVPacket *)m_pPointer, _size);
}
void FFmpeg::AVPacket::RescaleTS(FFmpeg::AVRational^ tb_src, FFmpeg::AVRational^ tb_dst)
{
	::AVRational * _src = (::AVRational *)tb_src->_Pointer.ToPointer();
	::AVRational * _dst = (::AVRational *)tb_dst->_Pointer.ToPointer();
	av_packet_rescale_ts((::AVPacket *)m_pPointer,*_src,*_dst);
}
bool FFmpeg::AVPacket::CopyProps(FFmpeg::AVPacket^ _dest)
{
	return (0 == av_packet_copy_props((::AVPacket *)_dest->m_pPointer,(::AVPacket *)m_pPointer));
}
bool FFmpeg::AVPacket::CopySideData(FFmpeg::AVPacket^ _dest)
{
	LOAD_API(AVCodec,int,av_copy_packet_side_data,::AVPacket *,::AVPacket *);
	if (av_copy_packet_side_data)
	{
		return 0 == av_copy_packet_side_data((::AVPacket *)_dest->m_pPointer, (::AVPacket *)m_pPointer);
	}
	return CopyProps(_dest);
}
void FFmpeg::AVPacket::FreeSideData()
{
	av_packet_free_side_data((::AVPacket *)m_pPointer);
}
void FFmpeg::AVPacket::MoveTo(FFmpeg::AVPacket^ _dest)
{
	av_packet_move_ref((::AVPacket *)_dest->m_pPointer,(::AVPacket *)m_pPointer);
}
FFmpeg::AVPacketSideData^ FFmpeg::AVPacket::NewSideData(FFmpeg::AVPacketSideDataType _type, int _size)
{
	uint8_t * p = av_packet_new_side_data((::AVPacket *)m_pPointer,(::AVPacketSideDataType)_type,_size);
	if (p)
	{
		::AVPacketSideData* _pointer = ((::AVPacket*)m_pPointer)->side_data;
		int _count = ((::AVPacket*)m_pPointer)->side_data_elems;
		for (int i = 0; i < _count; i++)
		{
			if (_pointer[i].data == p)
			{
				return _CreateObject<AVPacketSideData>(&_pointer[i]);
			}
		}
	}
	return nullptr;
}

IntPtr FFmpeg::AVPacket::NewSideDataPtr(AVPacketSideDataType _type, int _size)
{
	return (IntPtr)av_packet_new_side_data((::AVPacket *)m_pPointer,(::AVPacketSideDataType)_type,_size);
}

IntPtr FFmpeg::AVPacket::GetSideDataPtr(AVPacketSideDataType _type, Int64 % _size)
{
#if LIBAVCODEC_VERSION_MAJOR > 58
	size_t nSize = 0;
#else
	int nSize = 0;
#endif
	uint8_t * p = av_packet_get_side_data((::AVPacket *)m_pPointer,(::AVPacketSideDataType)_type,&nSize);
	_size = (Int64)nSize;
	return (IntPtr)p;
}

bool FFmpeg::AVPacket::AddSideData(AVPacketSideDataType _type, IntPtr data, int _size)
{
	uint8_t * p = (uint8_t *)av_malloc(_size);
	memcpy(p,data.ToPointer(),_size);
	if (0 != av_packet_add_side_data((::AVPacket *)m_pPointer,(::AVPacketSideDataType)_type,p,_size))
	{
		av_free(p);
		return false;
	}
	return true;
}

bool FFmpeg::AVPacket::AddSideData(AVPacketSideDataType _type, array<byte>^ data)
{
	pin_ptr<byte> _data = &data[0];
	uint8_t * pData = (uint8_t *)_data;
	uint8_t * p = (uint8_t *)av_malloc(data->Length);
	memcpy(p,_data,data->Length);
	if (0 != av_packet_add_side_data((::AVPacket *)m_pPointer,(::AVPacketSideDataType)_type,p,data->Length))
	{
		av_free(p);
		return false;
	}
	return true;
}

bool FFmpeg::AVPacket::ShrinkSideData(FFmpeg::AVPacketSideDataType _type, int _size)
{
	return (0 == av_packet_shrink_side_data((::AVPacket *)m_pPointer,(::AVPacketSideDataType)_type,_size));
}
FFmpeg::AVPacketSideData^ FFmpeg::AVPacket::GetSideData(FFmpeg::AVPacketSideDataType _type)
{
#if LIBAVCODEC_VERSION_MAJOR > 58
	size_t _size = 0;
#else
	int _size = 0;
#endif
	uint8_t * p = av_packet_get_side_data((::AVPacket *)m_pPointer,(::AVPacketSideDataType)_type,&_size);
	if (p)
	{
		::AVPacketSideData* _pointer = ((::AVPacket*)m_pPointer)->side_data;
		int _count = ((::AVPacket*)m_pPointer)->side_data_elems;
		for (int i = 0; i < _count; i++)
		{
			if (_pointer[i].data == p)
			{
				return _CreateObject<AVPacketSideData>(&_pointer[i]);
			}
		}
	}
	return nullptr;
}
//////////////////////////////////////////////////////
bool FFmpeg::AVPacket::MakeWritable()
{
	return (0 == av_packet_make_writable((::AVPacket*)m_pPointer));
}
//////////////////////////////////////////////////////
bool FFmpeg::AVPacket::MergeSideData()
{
	DYNAMIC_DEF_API(AVCodec,int,true,av_packet_merge_side_data,::AVPacket *);
	return (0 == av_packet_merge_side_data((::AVPacket *)m_pPointer));
}
bool FFmpeg::AVPacket::SplitSideData()
{
	DYNAMIC_DEF_API(AVCodec,int,true,av_packet_split_side_data,::AVPacket *);
	return (0 == av_packet_split_side_data((::AVPacket *)m_pPointer));
}
//////////////////////////////////////////////////////
bool FFmpeg::AVPacket::Dump(String^ sFileName)
{
	return Dump(sFileName,false);
}
bool FFmpeg::AVPacket::Dump(String^ sFileName,bool bAppend)
{
	bool _result = false;
	wchar_t * path = (wchar_t *)AllocString(sFileName,true).ToPointer();
	try
	{
		HANDLE hFile = CreateFileW(path, FILE_GENERIC_WRITE,FILE_SHARE_READ,NULL,bAppend ? OPEN_ALWAYS : CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			SetFilePointer(hFile,0,NULL,FILE_END);
			DWORD dwResult;
			_result = (WriteFile(hFile,((::AVPacket*)m_pPointer)->data,((::AVPacket*)m_pPointer)->size,&dwResult,NULL) == TRUE);
			CloseHandle(hFile);
		}
	}
	finally
	{
		FreeMemory((IntPtr)path);
	}
	return _result;
}

bool FFmpeg::AVPacket::Dump(Stream^ _stream)
{
	array<byte>^ _array = ToArray();
	if (_array != nullptr)
	{
		_stream->Write(_array,0,_array->Length);
		return true;
	}
	return false;
}

array<byte>^ FFmpeg::AVPacket::ToArray()
{
	::AVPacket* _packet = ((::AVPacket*)m_pPointer);
	array<byte>^ _array = gcnew array<byte>(_packet->size);
	if (_packet->size > 0 && _packet->data != nullptr)
	{
		Marshal::Copy((IntPtr)_packet->data,_array,0,_packet->size);
	}
	return _array;
}

FFmpeg::AVPacket^ FFmpeg::AVPacket::FromArray(array<byte>^ _data)
{
	AVPacket^ _packet = gcnew AVPacket(_data->Length);
	Marshal::Copy(_data,0,(IntPtr)((::AVPacket*)_packet->m_pPointer)->data,_data->Length);
	return _packet;
}
FFmpeg::AVPacket^ FFmpeg::AVPacket::FromArray(array<int>^ _data)
{
	AVPacket^ _packet = gcnew AVPacket(_data->Length * sizeof(int));
	Marshal::Copy(_data,0,(IntPtr)((::AVPacket*)_packet->m_pPointer)->data,_data->Length);
	return _packet;
}
FFmpeg::AVPacket^ FFmpeg::AVPacket::FromArray(array<unsigned int>^ _data)
{
	AVPacket^ _packet = gcnew AVPacket(_data->Length * sizeof(unsigned int));
	pin_ptr<unsigned int> data = &_data[0];
	unsigned int * pData = (unsigned int *)data;
	memcpy(((::AVPacket*)_packet->m_pPointer)->data,pData,_data->Length * sizeof(unsigned int));
	return _packet;
}

FFmpeg::AVPacket^ FFmpeg::AVPacket::FromArray(array<unsigned short>^ _data)
{
	AVPacket^ _packet = gcnew AVPacket(_data->Length * sizeof(unsigned short));
	pin_ptr<unsigned short> data = &_data[0];
	short int * pData = (short int *)data;
	memcpy(((::AVPacket*)_packet->m_pPointer)->data,pData,_data->Length * sizeof(short int));
	return _packet;
}

FFmpeg::AVPacket^ FFmpeg::AVPacket::FromArray(array<Int64>^ _data)
{
	AVPacket^ _packet = gcnew AVPacket(_data->Length * sizeof(Int64));
	Marshal::Copy(_data,0,(IntPtr)((::AVPacket*)_packet->m_pPointer)->data,_data->Length);
	return _packet;
}
FFmpeg::AVPacket^ FFmpeg::AVPacket::FromArray(array<short>^ _data)
{
	AVPacket^ _packet = gcnew AVPacket(_data->Length * sizeof(short));
	Marshal::Copy(_data,0,(IntPtr)((::AVPacket*)_packet->m_pPointer)->data,_data->Length);
	return _packet;
}
FFmpeg::AVPacket^ FFmpeg::AVPacket::FromArray(array<float>^ _data)
{
	AVPacket^ _packet = gcnew AVPacket(_data->Length * sizeof(float));
	Marshal::Copy(_data,0,(IntPtr)((::AVPacket*)_packet->m_pPointer)->data,_data->Length);
	return _packet;
}
FFmpeg::AVPacket^ FFmpeg::AVPacket::FromArray(array<double>^ _data)
{
	AVPacket^ _packet = gcnew AVPacket(_data->Length * sizeof(double));
	Marshal::Copy(_data,0,(IntPtr)((::AVPacket*)_packet->m_pPointer)->data,_data->Length);
	return _packet;
}
//////////////////////////////////////////////////////
// ICloneable
Object^ FFmpeg::AVPacket::Clone() 
{
	return gcnew AVPacket(this);
}
//////////////////////////////////////////////////////
void FFmpeg::AVPacket::FreeCB(IntPtr data)
{
	if (m_pFreeCB) m_pFreeCB(m_pOpaque,data);
}
//////////////////////////////////////////////////////
// AVPicture
//////////////////////////////////////////////////////
int AVPicture_GetDataLinesize(void * p, void * opaque)
{
	if (opaque && p)
	{
		GCHandle _handle = GCHandle::FromIntPtr((IntPtr)opaque);
		if (_handle.IsAllocated)
		{
			FFmpeg::AVPicture^ target = ((FFmpeg::AVPicture^)_handle.Target);
			::AVPicture * picture = (::AVPicture *)target->_Pointer.ToPointer();
			int height = target->height; 
			if (height == 0) height = 1;
			if (height < 0) height *= -1;
			for (int i = 0; i < AV_NUM_DATA_POINTERS; i++)
			{
				if (picture->data[i] == p)
				{
					return (picture->linesize[i] * height);
				}
			}
		}
	}
	return 0;
}
//////////////////////////////////////////////////////
FFmpeg::AVPicture::AVPicture(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
	, m_nDataAllocated(0)
	, m_nNumDataPointers(NUM_DATA_POINTERS)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nFormat(AVPixelFormat::NONE)
{
}

FFmpeg::AVPicture::AVPicture(uint8_t ** _data,int* linesize, int cch, AVBase^ _parent)
	: AVBase(nullptr,_parent)
	, m_nNumDataPointers(NUM_DATA_POINTERS)
	, m_nDataAllocated(0)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nFormat(AVPixelFormat::NONE)
{
	AllocPointer(_StructureSize);
	m_nNumDataPointers = cch;
	for (int i = 0; i < cch; i++)
	{
		((::AVPicture *)m_pPointer)->data[i] = _data[i];
		((::AVPicture *)m_pPointer)->linesize[i] = linesize[i];
	}
}

FFmpeg::AVPicture::AVPicture()
	: AVBase(nullptr,nullptr)
	, m_nDataAllocated(0)
	, m_nNumDataPointers(NUM_DATA_POINTERS)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nFormat(AVPixelFormat::NONE)
{
	AllocPointer(_StructureSize);
}

FFmpeg::AVPicture::AVPicture(FFmpeg::AVPixelFormat pix_fmt, int width, int height)
	: AVBase(nullptr,nullptr)
	, m_nDataAllocated(0)
	, m_nNumDataPointers(NUM_DATA_POINTERS)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nFormat(AVPixelFormat::NONE)
{
	AllocPointer(_StructureSize);
	Alloc(pix_fmt, width, height);
}

FFmpeg::AVPicture::AVPicture(AVPixelFormat pix_fmt, int width, int height,int align)
	: AVBase(nullptr,nullptr)
	, m_nDataAllocated(0)
	, m_nNumDataPointers(NUM_DATA_POINTERS)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nFormat(AVPixelFormat::NONE)
{
	AllocPointer(_StructureSize);
	Alloc(pix_fmt, width, height, align);
}

FFmpeg::AVPicture::~AVPicture()
{
	Free();
}

FFmpeg::AVPicture::!AVPicture()
{
	Debug::Assert(m_nDataAllocated == 0);
}
//////////////////////////////////////////////////////
int FFmpeg::AVPicture::_StructureSize::get()
{
	return sizeof(::AVPicture);
}
//////////////////////////////////////////////////////
FFmpeg::AVArray<FFmpeg::AVMemPtr^>^ FFmpeg::AVPicture::data::get()
{
	auto p = ((::AVPicture *)m_pPointer)->data;
	AVArray<AVMemPtr^>^ _array = (AVArray<AVMemPtr^>^)GetObject((IntPtr)p);
	if (_array) return _array;

	IntPtr _this = GCHandle::ToIntPtr(GCHandle::Alloc(this,GCHandleType::Weak));
	return gcnew AVArray<AVMemPtr^>(((::AVPicture *)m_pPointer)->data,this,m_nNumDataPointers,
		(AVMemPtr::TGetSizeFN*)AVPicture_GetDataLinesize,_this.ToPointer());
}

FFmpeg::AVArray<int>^ FFmpeg::AVPicture::linesize::get()
{
	auto p = ((::AVPicture *)m_pPointer)->linesize;
	AVArray<int>^ _array = (AVArray<int>^)GetObject((IntPtr)p);
	if (_array) return _array;
	return gcnew AVArray<int>(((::AVPicture *)m_pPointer)->linesize,this,sizeof(int),m_nNumDataPointers);
}
//////////////////////////////////////////////////////
int FFmpeg::AVPicture::width::get()
{
	return m_nWidth;
}
void FFmpeg::AVPicture::width::set(int value)
{
	m_nWidth = value;
}

int FFmpeg::AVPicture::height::get()
{
	return m_nHeight;
}
void FFmpeg::AVPicture::height::set(int value)
{
	m_nHeight = value;
}
int FFmpeg::AVPicture::format::get()
{
	return m_nFormat;
}
void FFmpeg::AVPicture::format::set(int value)
{
	m_nFormat = value;
}
//////////////////////////////////////////////////////
int FFmpeg::AVPicture::Alloc(FFmpeg::AVPixelFormat pix_fmt, int width, int height)
{
	return Alloc(pix_fmt, width, height,1);
}

int FFmpeg::AVPicture::Alloc(AVPixelFormat pix_fmt, int width, int height,int align)
{
	Free();
	m_nDataAllocated = av_image_alloc(((::AVPicture *)m_pPointer)->data,((::AVPicture *)m_pPointer)->linesize,width, height,(::AVPixelFormat)pix_fmt,align);
	if (m_nDataAllocated >= 0)
	{
		m_nWidth = width;
		m_nHeight = height;
		m_nFormat = pix_fmt;
	}
	return m_nDataAllocated;
}

void FFmpeg::AVPicture::Free()
{
	if (m_nDataAllocated > 0 && m_pPointer)
	{
		LOAD_API(AVCodec,void,avpicture_free,const ::AVPicture*);
		if (avpicture_free)
		{
			avpicture_free((::AVPicture *)m_pPointer);
		}
		else
		{
			::AVPicture * picture = (::AVPicture *)m_pPointer;
			av_freep(&picture->data[0]);
		}
	}
	m_nDataAllocated = 0;
}

int FFmpeg::AVPicture::Fill(IntPtr ptr,FFmpeg::AVPixelFormat pix_fmt, int width, int height)
{
	Free();
	::AVPicture * p = (::AVPicture *)m_pPointer;
	int result = av_image_fill_arrays(p->data,p->linesize,(const uint8_t *)ptr.ToPointer(),(::AVPixelFormat)pix_fmt, width, height,1);
	if (result >= 0)
	{
		m_nWidth = width;
		m_nHeight = height;
		m_nFormat = pix_fmt;
	}
	return result;
}

int FFmpeg::AVPicture::Layout(FFmpeg::AVPixelFormat pix_fmt,int width, int height,IntPtr dest, int dest_size)
{
	::AVPicture * p = (::AVPicture *)m_pPointer;
	return av_image_copy_to_buffer(
		(unsigned char *)dest.ToPointer(), dest_size,
		(const uint8_t * const*)p->data, p->linesize,
		(::AVPixelFormat)pix_fmt, width, height, 1);
}

void FFmpeg::AVPicture::CopyTo(FFmpeg::AVPicture^ dst,FFmpeg::AVPixelFormat pix_fmt, int width, int height)
{
	auto _src = (::AVPicture*)m_pPointer;
	auto _dst = (::AVPicture*)dst->m_pPointer;
	av_image_copy(_dst->data, _dst->linesize, (const uint8_t **)_src->data,
		_src->linesize, (::AVPixelFormat)pix_fmt, width, height);
}

int FFmpeg::AVPicture::CopyTo(IntPtr dst,int dst_size,AVPixelFormat pix_fmt, int width, int height, int align)
{
	return av_image_copy_to_buffer((uint8_t *)dst.ToPointer(),dst_size,((::AVPicture*)m_pPointer)->data, ((::AVPicture*)m_pPointer)->linesize,(::AVPixelFormat)pix_fmt,width,height,align);
}

FFmpeg::AVRESULT FFmpeg::AVPicture::Crop(FFmpeg::AVPicture^ dst,FFmpeg::AVPixelFormat pix_fmt, int top_band, int left_band)
{
	DYNAMIC_DEF_API(AVCodec,int,AVERROR(ENOSYS),av_picture_crop,::AVPicture *, const ::AVPicture *,::AVPixelFormat , int , int );
	return av_picture_crop((::AVPicture*)dst->m_pPointer,(::AVPicture*)m_pPointer,(::AVPixelFormat)pix_fmt, top_band, left_band);
}

FFmpeg::AVRESULT FFmpeg::AVPicture::Pad(FFmpeg::AVPicture^ dst,FFmpeg::AVPixelFormat pix_fmt, int width, int height,
						   int padtop, int padbottom, int padleft, int padright, array<int>^ color)
{
	DYNAMIC_DEF_API(AVCodec,int,AVERROR(ENOSYS),av_picture_pad,::AVPicture *, const ::AVPicture *, int , int , ::AVPixelFormat ,
		int , int , int , int , int *);
	int _color[4] = {0,0,0,0};
	for (int i = 0; i < color->Length && i < 4; i++)
	{
		_color[i] = color[i];
	}
	return av_picture_pad((::AVPicture*)dst->m_pPointer,(::AVPicture*)m_pPointer, height, width,(::AVPixelFormat)pix_fmt,
		padtop, padbottom, padleft, padright, _color);
}

int FFmpeg::AVPicture::GetSize(FFmpeg::AVPixelFormat pix_fmt, int width, int height)
{
	return GetSize(pix_fmt, width,height,1);
}
int FFmpeg::AVPicture::GetSize(AVPixelFormat pix_fmt, int width, int height,int align)
{
	return av_image_get_buffer_size((::AVPixelFormat)pix_fmt, width, height,align);
}
void FFmpeg::AVPicture::FillMaxPixsteps(array<int>^ max_pixsteps,array<int>^ max_pixstep_comps,AVPixFmtDescriptor^ pixdesc)
{
	if (max_pixsteps->Length < 4) throw gcnew System::ArgumentException("max_pixsteps");
	if (max_pixstep_comps->Length < 4) throw gcnew System::ArgumentException("max_pixstep_comps");
	pin_ptr<int> p_max_pixsteps = &max_pixsteps[0];
	pin_ptr<int> p_max_pixstep_comps = &max_pixstep_comps[0];
	int * pn_max_pixsteps = (int *)p_max_pixsteps;
	int * pn_max_pixstep_comps = (int *)p_max_pixstep_comps;
	::AVPixFmtDescriptor * p = pixdesc != nullptr ? (::AVPixFmtDescriptor *)pixdesc->_Pointer.ToPointer() : nullptr;
	av_image_fill_max_pixsteps((int *)pn_max_pixsteps,(int *)pn_max_pixstep_comps,p);
}
int FFmpeg::AVPicture::GetLinesize(AVPixelFormat pix_fmt, int width, int plane)
{
	return av_image_get_linesize((::AVPixelFormat)pix_fmt,width,plane);
}
int FFmpeg::AVPicture::FillLinesizes(array<int>^ linesizes, AVPixelFormat pix_fmt, int width)
{
	if (linesizes->Length < 4) throw gcnew System::ArgumentException("linesizes");
	pin_ptr<int> p = &linesizes[0];
	int * pa = (int *)p;
	return av_image_fill_linesizes(pa,(::AVPixelFormat)pix_fmt,width);
}
int FFmpeg::AVPicture::FillLinesizes(AVArray<int>^ linesizes, AVPixelFormat pix_fmt, int width)
{
	if (linesizes->Count < 4) throw gcnew System::ArgumentException("linesizes");
	int * pa = (int *)linesizes->_Pointer.ToPointer();
	return av_image_fill_linesizes(pa, (::AVPixelFormat)pix_fmt, width);
}
int FFmpeg::AVPicture::FillPointers(AVArray<IntPtr>^ pointers, AVPixelFormat pix_fmt, int height, IntPtr ptr, AVArray<int>^ linesizes)
{
	if (pointers->Count < 4) throw gcnew System::ArgumentException("pointers");
	if (linesizes->Count < 4) throw gcnew System::ArgumentException("linesizes");
	int * pa = (int *)linesizes->_Pointer.ToPointer();
	uint8_t * ppa = (uint8_t *)pointers->_Pointer.ToPointer();
	return av_image_fill_pointers(&ppa, (::AVPixelFormat)pix_fmt, height, (uint8_t *)ptr.ToPointer(), pa);
}
int FFmpeg::AVPicture::FillPointers(array<IntPtr>^ pointers, AVPixelFormat pix_fmt, int height,IntPtr ptr,array<int>^ linesizes)
{
	if (pointers->Length < 4) throw gcnew System::ArgumentException("pointers");
	if (linesizes->Length < 4) throw gcnew System::ArgumentException("linesizes");
	pin_ptr<int> p = &linesizes[0];
	int * pa = (int *)p;
	pin_ptr<IntPtr> pp = &pointers[0];
	uint8_t * ppa = (uint8_t *)pp;
	return av_image_fill_pointers(&ppa, (::AVPixelFormat)pix_fmt,height,(uint8_t *)ptr.ToPointer(),pa);
}
bool FFmpeg::AVPicture::CheckSize(UInt32 w, UInt32 h)
{
	return 0 <= av_image_check_size(w,h,0,nullptr);
}
bool FFmpeg::AVPicture::CheckSAR(UInt32 w, UInt32 h, AVRational^ sar)
{
	return 0 == av_image_check_sar(w,h,*((::AVRational*)sar->_Pointer.ToPointer()));
}
void FFmpeg::AVPicture::CopyPlane(IntPtr dst, int dst_linesize,IntPtr src, int src_linesize,int bytewidth, int height)
{
	av_image_copy_plane((uint8_t*)dst.ToPointer(),dst_linesize,(const uint8_t *)src.ToPointer(),src_linesize,bytewidth,height);
}
//////////////////////////////////////////////////////
// AVPanScan
//////////////////////////////////////////////////////
FFmpeg::AVPanScan::AVPanScan(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
int FFmpeg::AVPanScan::_StructureSize::get() 
{
	return sizeof(::AVPanScan);
}
//////////////////////////////////////////////////////
int FFmpeg::AVPanScan::id::get()
{
	return ((::AVPanScan*)m_pPointer)->id;
}
void FFmpeg::AVPanScan::id::set(int value)
{ 
	((::AVPanScan*)m_pPointer)->id = value;
}
int FFmpeg::AVPanScan::width::get()
{
	return ((::AVPanScan*)m_pPointer)->width;
}
void FFmpeg::AVPanScan::width::set(int value)
{
	((::AVPanScan*)m_pPointer)->width = value;
}
int FFmpeg::AVPanScan::height::get()
{
	return ((::AVPanScan*)m_pPointer)->height;
}
void FFmpeg::AVPanScan::height::set(int value)
{
	((::AVPanScan*)m_pPointer)->height = value;
}

FFmpeg::AVPanScan::AVPosition^ FFmpeg::AVPanScan::position::get()
{
	return gcnew FFmpeg::AVPanScan::AVPosition(this);
}

short FFmpeg::AVPanScan::GetPosition(int x,int y)
{
	if (x < 0 || x > 2 || y < 0 || y > 1) throw gcnew ArgumentOutOfRangeException();
	return ((::AVPanScan*)m_pPointer)->position[x][y];
}
void FFmpeg::AVPanScan::SetPosition(int x,int y,short value)
{
	if (x < 0 || x > 2 || y < 0 || y > 1) throw gcnew ArgumentOutOfRangeException();
	((::AVPanScan*)m_pPointer)->position[x][y] = value;
}
//////////////////////////////////////////////////////
// AVImage
//////////////////////////////////////////////////////
FFmpeg::AVFrame::AVImage::AVImage(System::Drawing::Bitmap^ _image,AVFrame^ _frame, bool bDeleteBitmap)
	: AVBase(nullptr,_frame)
	, m_pData(nullptr)
	, m_bDeleteBitmap(bDeleteBitmap)
{
	m_pBitmap = _image;
	m_pData = m_pBitmap->LockBits(System::Drawing::Rectangle(0,0,m_pBitmap->Width,m_pBitmap->Height),
		System::Drawing::Imaging::ImageLockMode::ReadOnly,
#ifdef PixelFormat
#	undef PixelFormat
		System::Drawing::Imaging::PixelFormat::Format32bppPArgb	
#	define PixelFormat AVPixelFormat
#else
		System::Drawing::Imaging::PixelFormat::Format32bppPArgb	
#endif	
		);
}
FFmpeg::AVFrame::AVImage::~AVImage()
{
	if (m_pData)
	{
		m_pBitmap->UnlockBits(m_pData);
		delete m_pData;
		m_pData = nullptr;
	}
	if (m_pBitmap != nullptr)
	{
		if (m_bDeleteBitmap) delete m_pBitmap;
		m_pBitmap = nullptr;
	}
}
//////////////////////////////////////////////////////
System::Drawing::Imaging::BitmapData^ FFmpeg::AVFrame::AVImage::data::get()
{
	return m_pData;
}
//////////////////////////////////////////////////////
// AVFrame
//////////////////////////////////////////////////////
FFmpeg::AVFrame::AVFrame(void * _pointer,AVBase^ _parent)
	: AVPicture(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
FFmpeg::AVFrame::AVFrame()
	: AVPicture(nullptr,nullptr)
{
	m_pPointer = av_frame_alloc();
	if (m_pPointer != nullptr) m_pFreep = (TFreeFNP *)av_frame_free;
}
//////////////////////////////////////////////////////
int FFmpeg::AVFrame::_StructureSize::get()
{
	return sizeof(::AVFrame);
}
//////////////////////////////////////////////////////
IntPtr FFmpeg::AVFrame::extended_data::get()
{
	return (IntPtr)((::AVFrame*)m_pPointer)->extended_data;
}
//////////////////////////////////////////////////////
int FFmpeg::AVFrame::width::get()
{
	return ((::AVFrame*)m_pPointer)->width;
}
void FFmpeg::AVFrame::width::set(int value)
{
	((::AVFrame*)m_pPointer)->width = value;
}

int FFmpeg::AVFrame::height::get()
{
	return ((::AVFrame*)m_pPointer)->height;
}
void FFmpeg::AVFrame::height::set(int value)
{
	((::AVFrame*)m_pPointer)->height = value;
}

int FFmpeg::AVFrame::nb_samples::get()
{
	return ((::AVFrame*)m_pPointer)->nb_samples;
}
void FFmpeg::AVFrame::nb_samples::set(int value)
{
	((::AVFrame*)m_pPointer)->nb_samples = value;
}

int FFmpeg::AVFrame::format::get()
{
	return ((::AVFrame*)m_pPointer)->format;
}
void FFmpeg::AVFrame::format::set(int value)
{
	((::AVFrame*)m_pPointer)->format = value;
}

bool FFmpeg::AVFrame::key_frame::get()
{
	return ((::AVFrame*)m_pPointer)->key_frame == 1;
}
void FFmpeg::AVFrame::key_frame::set(bool value)
{
	((::AVFrame*)m_pPointer)->key_frame = value ? 1 : 0;
}

FFmpeg::AVPictureType FFmpeg::AVFrame::pict_type::get()
{
	return (FFmpeg::AVPictureType)((::AVFrame*)m_pPointer)->pict_type;
}
void FFmpeg::AVFrame::pict_type::set(AVPictureType value)
{
	((::AVFrame*)m_pPointer)->pict_type = (::AVPictureType)value;
}

FFmpeg::AVRational^ FFmpeg::AVFrame::sample_aspect_ratio::get()
{
	return _CreateObject<AVRational>((void*)&((::AVFrame*)m_pPointer)->sample_aspect_ratio);
}
void FFmpeg::AVFrame::sample_aspect_ratio::set(AVRational^ value)
{
	::AVRational _rational = { value != nullptr ? value->num : 0, value != nullptr ? value->den : 0 };
	((::AVFrame*)m_pPointer)->sample_aspect_ratio = _rational;
}

Int64 FFmpeg::AVFrame::pts::get() 
{
	return ((::AVFrame*)m_pPointer)->pts;
}

void FFmpeg::AVFrame::pts::set(Int64 value)
{
	((::AVFrame*)m_pPointer)->pts = value;
}

Int64 FFmpeg::AVFrame::pkt_pts::get()
{
#if FF_API_PKT_PTS
	return ((::AVFrame*)m_pPointer)->pkt_pts;
#else
	return ((::AVFrame*)m_pPointer)->pts;
#endif
}
void FFmpeg::AVFrame::pkt_pts::set(Int64 value)
{
#if FF_API_PKT_PTS
	((::AVFrame*)m_pPointer)->pkt_pts = value;
#else
	((::AVFrame*)m_pPointer)->pts = value;
#endif
}

Int64 FFmpeg::AVFrame::pkt_dts::get()
{
	return ((::AVFrame*)m_pPointer)->pkt_dts;
}
void FFmpeg::AVFrame::pkt_dts::set(Int64 value)
{
	((::AVFrame*)m_pPointer)->pkt_dts = value;
}

int FFmpeg::AVFrame::coded_picture_number::get()
{
	return ((::AVFrame*)m_pPointer)->coded_picture_number;
}
void FFmpeg::AVFrame::coded_picture_number::set(int value)
{
	((::AVFrame*)m_pPointer)->coded_picture_number = value;
}

int FFmpeg::AVFrame::display_picture_number::get()
{
	return ((::AVFrame*)m_pPointer)->display_picture_number;
}
void FFmpeg::AVFrame::display_picture_number::set(int value)
{
	((::AVFrame*)m_pPointer)->display_picture_number = value;
}

int FFmpeg::AVFrame::quality::get()
{
	return ((::AVFrame*)m_pPointer)->quality;
}
void FFmpeg::AVFrame::quality::set(int value)
{
	((::AVFrame*)m_pPointer)->quality = value;
}

IntPtr FFmpeg::AVFrame::opaque::get()
{
	return (IntPtr)((::AVFrame*)m_pPointer)->opaque;
}
void FFmpeg::AVFrame::opaque::set(IntPtr value)
{
	((::AVFrame*)m_pPointer)->opaque = value.ToPointer();
}

FFmpeg::AVArray<UInt64>^ FFmpeg::AVFrame::error::get()
{
#if FF_API_ERROR_FRAME
	auto p = ((::AVFrame *)m_pPointer)->error;
	AVArray<UInt64>^ _array = (AVArray<UInt64>^)GetObject((IntPtr)p);
	if (_array) return _array;
	return gcnew FFmpeg::AVArray<UInt64>(((::AVFrame*)m_pPointer)->error,this,sizeof(UInt64),AV_NUM_DATA_POINTERS);
#else
	return nullptr;
#endif
}

int FFmpeg::AVFrame::repeat_pict::get()
{
	return ((::AVFrame*)m_pPointer)->repeat_pict;
}
void FFmpeg::AVFrame::repeat_pict::set(int value)
{
	((::AVFrame*)m_pPointer)->repeat_pict = value;
}

int FFmpeg::AVFrame::interlaced_frame::get()
{
	return ((::AVFrame*)m_pPointer)->interlaced_frame;
}
void FFmpeg::AVFrame::interlaced_frame::set(int value)
{
	((::AVFrame*)m_pPointer)->interlaced_frame = value;
}

int FFmpeg::AVFrame::top_field_first::get()
{
	return ((::AVFrame*)m_pPointer)->top_field_first;
}
void FFmpeg::AVFrame::top_field_first::set(int value)
{
	((::AVFrame*)m_pPointer)->top_field_first = value;
}

int FFmpeg::AVFrame::palette_has_changed::get()
{
	return ((::AVFrame*)m_pPointer)->palette_has_changed;
}
void FFmpeg::AVFrame::palette_has_changed::set(int value)
{
	((::AVFrame*)m_pPointer)->palette_has_changed = value;
}

Int64 FFmpeg::AVFrame::reordered_opaque::get()
{
	return ((::AVFrame*)m_pPointer)->reordered_opaque;
}
void FFmpeg::AVFrame::reordered_opaque::set(Int64 value)
{
	((::AVFrame*)m_pPointer)->reordered_opaque = value;
}

int FFmpeg::AVFrame::sample_rate::get()
{
	DYNAMIC_DEF_API(AVUtil,int,((::AVFrame*)m_pPointer)->sample_rate,av_frame_get_sample_rate,::AVFrame *);
	return av_frame_get_sample_rate((::AVFrame*)m_pPointer);
}
void FFmpeg::AVFrame::sample_rate::set(int value)
{
	LOAD_API(AVUtil,void,av_frame_set_sample_rate,::AVFrame *,int);
	if (av_frame_set_sample_rate)
	{
		av_frame_set_sample_rate((::AVFrame*)m_pPointer, value);
	}
	else
	{
		((::AVFrame*)m_pPointer)->sample_rate = value;
	}
}

FFmpeg::AVChannelLayout FFmpeg::AVFrame::channel_layout::get()
{
	DYNAMIC_DEF_API(AVUtil,UInt64,((::AVFrame*)m_pPointer)->channel_layout,av_frame_get_channel_layout,::AVFrame *);
	return (FFmpeg::AVChannelLayout)av_frame_get_channel_layout((::AVFrame*)m_pPointer);
}
void FFmpeg::AVFrame::channel_layout::set(AVChannelLayout value)
{
	LOAD_API(AVUtil,void,av_frame_set_channel_layout,::AVFrame *,uint64_t);
	if (av_frame_set_channel_layout)
	{
		av_frame_set_channel_layout((::AVFrame*)m_pPointer, (uint64_t)value);
	}
	else
	{
		((::AVFrame*)m_pPointer)->channel_layout = value;
	}
}

FFmpeg::AVFrame::AVBufferRefArray^ FFmpeg::AVFrame::buf::get()
{
	return gcnew AVBufferRefArray(this);
}

array<FFmpeg::AVBufferRef^>^ FFmpeg::AVFrame::extended_buf::get()
{
	List<AVBufferRef^>^ _array = nullptr;
	int nCount = ((::AVFrame*)m_pPointer)->nb_extended_buf;
	::AVBufferRef ** _p = ((::AVFrame*)m_pPointer)->extended_buf;
	if (_p && nCount > 0)
	{
		_array =  gcnew List<AVBufferRef^>();
		while (nCount-- > 0)
		{
			_array->Add(_CreateObject<AVBufferRef>(*_p++));
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}

void FFmpeg::AVFrame::extended_buf::set(array<AVBufferRef^>^ value)
{
	::AVFrame* _frame = (::AVFrame*)m_pPointer;
	int nCount = _frame->nb_extended_buf;
	if (_frame->extended_buf)
	{
		while (nCount > 0)
		{
			 ::AVBufferRef * p = _frame->extended_buf[--nCount];
			 RemoveObject((IntPtr)p);
			 if (p) av_buffer_unref(&p);
		}
		_frame->nb_extended_buf = 0;
		av_freep(&_frame->extended_buf);
	}
	if (value != nullptr && value->Length > 0)
	{
		_frame->nb_extended_buf = value->Length;
		_frame->extended_buf = (::AVBufferRef **)av_mallocz_array(_frame->nb_extended_buf,sizeof(*_frame->extended_buf));
		for (int i = 0; i < _frame->nb_extended_buf;i++)
		{
			_frame->extended_buf[i] = (::AVBufferRef *)value[i]->_Pointer.ToPointer();
			AddObject(value[i]);
		}
	}
}

int FFmpeg::AVFrame::nb_extended_buf::get()
{
	return ((::AVFrame*)m_pPointer)->nb_extended_buf;
}

array<FFmpeg::AVFrameSideData^>^ FFmpeg::AVFrame::side_data::get()
{
	List<AVFrameSideData^>^ _array = nullptr;
	int nCount = ((::AVFrame*)m_pPointer)->nb_side_data;
	::AVFrameSideData ** _p = ((::AVFrame*)m_pPointer)->side_data;
	if (_p && nCount > 0)
	{
		_array =  gcnew List<AVFrameSideData^>();
		while (nCount-- > 0)
		{
			_array->Add(_CreateObject<AVFrameSideData>(*_p++));
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}

FFmpeg::AVFrameFlags FFmpeg::AVFrame::flags::get() 
{
	return (FFmpeg::AVFrameFlags)((::AVFrame*)m_pPointer)->flags;
}
void FFmpeg::AVFrame::flags::set(AVFrameFlags value)
{
	((::AVFrame*)m_pPointer)->flags = (int)value;
}	

FFmpeg::AVColorRange FFmpeg::AVFrame::color_range::get()
{
	DYNAMIC_DEF_API(AVUtil,::AVColorRange,(FFmpeg::AVColorRange)((::AVFrame*)m_pPointer)->color_range,av_frame_get_color_range,::AVFrame *);
	return (FFmpeg::AVColorRange)av_frame_get_color_range((::AVFrame*)m_pPointer);
}
void FFmpeg::AVFrame::color_range::set(AVColorRange value)
{
	LOAD_API(AVUtil,void,av_frame_set_color_range,::AVFrame *,::AVColorRange);
	if (av_frame_set_color_range)
	{
		av_frame_set_color_range((::AVFrame*)m_pPointer, (::AVColorRange)value);
	}
	else
	{
		((::AVFrame*)m_pPointer)->color_range = (::AVColorRange)value;
	}
}

FFmpeg::AVColorPrimaries FFmpeg::AVFrame::color_primaries::get()
{
	return (FFmpeg::AVColorPrimaries)((::AVFrame*)m_pPointer)->color_primaries;
}
void FFmpeg::AVFrame::color_primaries::set(AVColorPrimaries value)
{
	((::AVFrame*)m_pPointer)->color_primaries = (::AVColorPrimaries)value;
}

FFmpeg::AVColorTransferCharacteristic FFmpeg::AVFrame::color_trc::get()
{
	return (FFmpeg::AVColorTransferCharacteristic)((::AVFrame*)m_pPointer)->color_trc;
}
void FFmpeg::AVFrame::color_trc::set(AVColorTransferCharacteristic value)
{
	((::AVFrame*)m_pPointer)->color_trc = (::AVColorTransferCharacteristic)value;
}

FFmpeg::AVColorSpace FFmpeg::AVFrame::colorspace::get()
{
	DYNAMIC_DEF_API(AVUtil,::AVColorSpace ,(FFmpeg::AVColorSpace)((::AVFrame*)m_pPointer)->colorspace,av_frame_get_colorspace,::AVFrame *);
	return (FFmpeg::AVColorSpace)av_frame_get_colorspace((::AVFrame*)m_pPointer);
}
void FFmpeg::AVFrame::colorspace::set(AVColorSpace value)
{
	LOAD_API(AVUtil,void,av_frame_set_colorspace,::AVFrame *,int);
	if (av_frame_set_colorspace)
	{
		av_frame_set_colorspace((::AVFrame*)m_pPointer, (::AVColorSpace)value);
	}
	else
	{
		((::AVFrame*)m_pPointer)->colorspace = (::AVColorSpace)value;
	}
}

FFmpeg::AVChromaLocation FFmpeg::AVFrame::chroma_location::get()
{
	return (FFmpeg::AVChromaLocation)((::AVFrame*)m_pPointer)->chroma_location;
}
void FFmpeg::AVFrame::chroma_location::set(AVChromaLocation value)
{ 
	((::AVFrame*)m_pPointer)->chroma_location = (::AVChromaLocation)value;
}

Int64 FFmpeg::AVFrame::best_effort_timestamp::get()
{
	DYNAMIC_DEF_API(AVUtil,Int64,((::AVFrame*)m_pPointer)->best_effort_timestamp,av_frame_get_best_effort_timestamp,::AVFrame *);
	return av_frame_get_best_effort_timestamp((::AVFrame*)m_pPointer);
}
void FFmpeg::AVFrame::best_effort_timestamp::set(Int64 value)
{
	LOAD_API(AVUtil,void,av_frame_set_best_effort_timestamp,::AVFrame *,Int64);
	if (av_frame_set_best_effort_timestamp)
	{
		av_frame_set_best_effort_timestamp((::AVFrame*)m_pPointer, value);
	}
	else
	{
		((::AVFrame*)m_pPointer)->best_effort_timestamp = value;
	}
}

Int64 FFmpeg::AVFrame::pkt_pos::get()
{
	DYNAMIC_DEF_API(AVUtil,Int64,((::AVFrame*)m_pPointer)->pkt_pos,av_frame_get_pkt_pos,::AVFrame *);
	return av_frame_get_pkt_pos((::AVFrame*)m_pPointer);
}
void FFmpeg::AVFrame::pkt_pos::set(Int64 value)
{
	LOAD_API(AVUtil,void,av_frame_set_pkt_pos,::AVFrame *,Int64);
	if (av_frame_set_pkt_pos)
	{
		av_frame_set_pkt_pos((::AVFrame*)m_pPointer, value);
	}
	else
	{
		((::AVFrame*)m_pPointer)->pkt_pos = value;
	}
}

Int64 FFmpeg::AVFrame::pkt_duration::get()
{
	DYNAMIC_DEF_API(AVUtil,Int64,((::AVFrame*)m_pPointer)->pkt_duration,av_frame_get_pkt_duration,::AVFrame *);
	return av_frame_get_pkt_duration((::AVFrame*)m_pPointer);
}
void FFmpeg::AVFrame::pkt_duration::set(Int64 value)
{
	LOAD_API(AVUtil,void,av_frame_set_pkt_duration,::AVFrame *,Int64);
	if (av_frame_set_pkt_duration)
	{
		av_frame_set_pkt_duration((::AVFrame*)m_pPointer, value);
	}
	else
	{
		((::AVFrame*)m_pPointer)->pkt_duration = value;
	}
}

FFmpeg::AVDictionary^ FFmpeg::AVFrame::metadata::get()
{
	::AVDictionary* p = nullptr;
	LOAD_API(AVUtil,::AVDictionary*,av_frame_get_metadata,::AVFrame *);
	if (av_frame_get_metadata)
	{
		p = av_frame_get_metadata((::AVFrame*)m_pPointer);
	}
	else
	{
		::AVDictionary* p = ((::AVFrame*)m_pPointer)->metadata;
	}
	return _CreateObject<AVDictionary>(p);
}

void FFmpeg::AVFrame::metadata::set(FFmpeg::AVDictionary^ value)
{
	::AVDictionary* p = nullptr;
	LOAD_API(AVUtil,::AVDictionary*,av_frame_get_metadata,::AVFrame *);
	LOAD_API(AVUtil,void,av_frame_set_metadata,::AVFrame *,::AVDictionary*);
	if (av_frame_get_metadata)
	{
		p = av_frame_get_metadata((::AVFrame*)m_pPointer);
	}
	else
	{
		p = ((::AVFrame*)m_pPointer)->metadata;
	}
	AddObject((IntPtr)p,value);
	if (av_frame_set_metadata)
	{
		av_frame_set_metadata((::AVFrame*)m_pPointer, (::AVDictionary*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr));
	}
	else
	{
		((::AVFrame*)m_pPointer)->metadata = (::AVDictionary*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
	}
}

FFmpeg::FFDecodeError FFmpeg::AVFrame::decode_error_flags::get()
{
	DYNAMIC_DEF_API(AVUtil,int,(FFmpeg::FFDecodeError)((::AVFrame*)m_pPointer)->decode_error_flags,av_frame_get_decode_error_flags,::AVFrame *);
	return (FFmpeg::FFDecodeError)av_frame_get_decode_error_flags((::AVFrame*)m_pPointer);
}
void FFmpeg::AVFrame::decode_error_flags::set(FFDecodeError value)
{
	LOAD_API(AVUtil,void,av_frame_set_decode_error_flags,::AVFrame *,int);
	if (av_frame_set_decode_error_flags)
	{
		av_frame_set_decode_error_flags((::AVFrame*)m_pPointer, (int)value);
	}
	else
	{
		((::AVFrame*)m_pPointer)->decode_error_flags = (int)value;
	}
}

int FFmpeg::AVFrame::channels::get()
{
	DYNAMIC_DEF_API(AVUtil,int,((::AVFrame*)m_pPointer)->pkt_size,av_frame_get_channels,::AVFrame *);
	return av_frame_get_channels((::AVFrame*)m_pPointer);
}
void FFmpeg::AVFrame::channels::set(int value)
{
	LOAD_API(AVUtil,void,av_frame_set_channels,::AVFrame *,int);
	if (av_frame_set_channels)
	{
		av_frame_set_channels((::AVFrame*)m_pPointer, value);
	}
	else
	{
		((::AVFrame*)m_pPointer)->channels = value;
	}
}

int FFmpeg::AVFrame::pkt_size::get()
{
	DYNAMIC_DEF_API(AVUtil,int,((::AVFrame*)m_pPointer)->pkt_size,av_frame_get_pkt_size,::AVFrame *);
	return av_frame_get_pkt_size((::AVFrame*)m_pPointer);
}
void FFmpeg::AVFrame::pkt_size::set(int value)
{
	LOAD_API(AVUtil,void,av_frame_set_pkt_size,::AVFrame *,int);
	if (av_frame_set_pkt_size)
	{
		av_frame_set_pkt_size((::AVFrame*)m_pPointer, value);
	}
	else
	{
		((::AVFrame*)m_pPointer)->pkt_size = value;
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVBufferRef^ FFmpeg::AVFrame::GetBuf(int index)
{
	if (index >= 0 && index < AV_NUM_DATA_POINTERS)
	{
		return _CreateObject<AVBufferRef>(((::AVFrame*)m_pPointer)->buf[index]);
	}
	return nullptr;
}

void FFmpeg::AVFrame::SetBuf(int index,AVBufferRef^ value)
{
	if (index >= 0 && index < AV_NUM_DATA_POINTERS)
	{
		AddObject((IntPtr)((::AVFrame*)m_pPointer)->buf[index],value);
		((::AVFrame*)m_pPointer)->buf[index] = (::AVBufferRef*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
	}
}
int FFmpeg::AVFrame::Alloc(AVPixelFormat pix_fmt, int width, int height)
{
	return Alloc(pix_fmt, width, height,0);
}

int FFmpeg::AVFrame::Alloc(AVPixelFormat pix_fmt, int width, int height, int align)
{
	((::AVFrame*)m_pPointer)->width = width;
	((::AVFrame*)m_pPointer)->height = height;
	((::AVFrame*)m_pPointer)->format = (int)pix_fmt;
	return AllocateBuffers(align);
}

int FFmpeg::AVFrame::Alloc(AVSampleFormat sample_fmt, AVChannelLayout ch, int nb_samples)
{
	return Alloc(sample_fmt,ch,nb_samples,0);
}

int FFmpeg::AVFrame::Alloc(AVSampleFormat sample_fmt, AVChannelLayout ch, int nb_samples, int align)
{
	((::AVFrame*)m_pPointer)->nb_samples = nb_samples;
	((::AVFrame*)m_pPointer)->channel_layout = ch;
	((::AVFrame*)m_pPointer)->format = (int)sample_fmt;
	return AllocateBuffers(align);
}
//////////////////////////////////////////////////////
bool FFmpeg::AVFrame::IsWritable::get()
{
	return av_frame_is_writable((::AVFrame*)m_pPointer) == 1;
}

bool FFmpeg::AVFrame::MakeWritable()
{
	return av_frame_make_writable((::AVFrame*)m_pPointer) == 0;
}

void FFmpeg::AVFrame::Move(AVFrame^ dst)
{
	av_frame_move_ref((::AVFrame*)dst->m_pPointer,(::AVFrame*)m_pPointer);
	dst->m_pDescructor = m_pDescructor;
	dst->m_pFree = m_pFree;
	m_pDescructor = nullptr;
}

FFmpeg::AVRESULT FFmpeg::AVFrame::AllocateBuffers()
{
	return AllocateBuffers(0);
}
FFmpeg::AVRESULT FFmpeg::AVFrame::AllocateBuffers(int align)
{
	return av_frame_get_buffer((::AVFrame*)m_pPointer,align);
}

FFmpeg::AVRESULT FFmpeg::AVFrame::Copy(AVFrame^ dst)
{
	return av_frame_copy((::AVFrame*)dst->_Pointer.ToPointer(),(::AVFrame*)m_pPointer);
}

void FFmpeg::AVFrame::CopyTo(AVPicture^ dst)
{
	AVPicture::CopyTo(dst,(AVPixelFormat)format,width,height);
}

FFmpeg::AVRESULT FFmpeg::AVFrame::CopyProps(AVFrame^ dst)
{
	return av_frame_copy_props((::AVFrame*)dst->_Pointer.ToPointer(),(::AVFrame*)m_pPointer);
}

FFmpeg::AVBufferRef^ FFmpeg::AVFrame::GetPlaneBuffer(int plane)
{
	auto p = av_frame_get_plane_buffer((::AVFrame *)m_pPointer,plane);
	return _CreateObject<AVBufferRef>(p);
}

FFmpeg::AVFrameSideData^ FFmpeg::AVFrame::NewSideData(AVFrameSideDataType type,int size)
{
	auto p = av_frame_new_side_data((::AVFrame *)m_pPointer,(::AVFrameSideDataType)type,size);
	return _CreateObject<AVFrameSideData>(p);
}

FFmpeg::AVFrameSideData^ FFmpeg::AVFrame::GetSideData(AVFrameSideDataType type)
{
	auto p = av_frame_get_side_data((const ::AVFrame *)m_pPointer,(::AVFrameSideDataType)type);
	return _CreateObject<AVFrameSideData>(p);
}

void FFmpeg::AVFrame::RemoveSideData(AVFrameSideDataType type)
{
	av_frame_remove_side_data((::AVFrame*)m_pPointer,(::AVFrameSideDataType)type);
}

int FFmpeg::AVFrame::FillAudio(int nb_channels,AVSampleFormat sample_fmt,IntPtr buf,int buf_size, int align)
{
	return avcodec_fill_audio_frame((::AVFrame*)m_pPointer,nb_channels,
		(::AVSampleFormat)sample_fmt,(const uint8_t *)buf.ToPointer(),buf_size, align);
}
//////////////////////////////////////////////////////
void FFmpeg::AVFrame::Free()
{
	av_frame_unref((::AVFrame*)m_pPointer);
	m_pDescructor = nullptr;
	__super::Free();
}
//////////////////////////////////////////////////////
// ICloneable
Object^ FFmpeg::AVFrame::Clone()
{
	AVFrame^ _frame = gcnew AVFrame();
	av_frame_ref((::AVFrame*)_frame->m_pPointer,(::AVFrame*)m_pPointer);
	_frame->m_pDescructor = (TFreeFN *)av_frame_unref;
	return _frame;
}
//////////////////////////////////////////////////////
System::Drawing::Bitmap^ FFmpeg::AVFrame::ToBitmap()
{
	return ToBitmap(this);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVFrame::GetColorspaceName(AVColorSpace val)
{
	auto p = av_get_colorspace_name((::AVColorSpace)val);
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////

FFmpeg::AVFrame^ FFmpeg::AVFrame::FromImage(System::Drawing::Bitmap^ _image)
{
	return FromImage(_image, AVPixelFormat::BGRA);
}

FFmpeg::AVFrame^ FFmpeg::AVFrame::FromImage(System::Drawing::Bitmap^ _image, AVPixelFormat _format)
{
	return FromImage(_image, _format, true);
}

FFmpeg::AVFrame^ FFmpeg::AVFrame::FromImage(System::Drawing::Bitmap^ _image, bool bDeleteBitmap)
{
	return FromImage(_image,AVPixelFormat::BGRA, bDeleteBitmap);
}

FFmpeg::AVFrame^ FFmpeg::AVFrame::FromImage(System::Drawing::Bitmap^ _image,AVPixelFormat _format, bool bDeleteBitmap)
{
	if (_image == nullptr) return nullptr;
	AVFrame^ frame = gcnew AVFrame();
	AVImage ^ image = gcnew AVImage(_image,frame, bDeleteBitmap);
	::AVFrame * _frame = (::AVFrame *)frame->m_pPointer;
	_frame->linesize[0] = image->data->Stride;
	_frame->data[0] = (uint8_t *)image->data->Scan0.ToPointer();
	_frame->width = image->data->Width;
	_frame->height = image->data->Height;
	_frame->format = AV_PIX_FMT_BGRA;
	if (_format == AVPixelFormat::BGRA)
	{
		return frame;
	}
	else
	{
		try
		{
			return ConvertVideoFrame(frame,_format);
		}
		finally
		{
			delete frame;
		}
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVFrame^ FFmpeg::AVFrame::ConvertVideoFrame(AVFrame^ _source,AVPixelFormat _format)
{
	if (_source == nullptr || _source->_Pointer == IntPtr::Zero) return nullptr;
	
	if (_source->format == (int)_format)
	{
		return (FFmpeg::AVFrame^)_source->Clone();
	}
	AVFrame^ _dest = nullptr;
	::AVFrame* _frame = ((::AVFrame*)_source->_Pointer.ToPointer());
	if (_frame->width > 0 && _frame->height > 0 && _frame->data[0] != nullptr && _frame->linesize[0] != 0)
	{
		_dest = gcnew AVFrame();
		_source->CopyProps(_dest);
		_dest->Alloc(_format,_frame->width, _frame->height);
		_dest->format = (int)_format;
		_dest->width = _frame->width;
		_dest->height = _frame->height;
		SwsContext * _context = sws_getContext(_frame->width,_frame->height,(::AVPixelFormat)_frame->format,_frame->width,_frame->height,(::AVPixelFormat)_format,SWS_POINT,nullptr,nullptr,nullptr);
		try
		{
			::AVFrame* _dst_frame = ((::AVFrame*)_dest->_Pointer.ToPointer());
			sws_scale(_context,_frame->data,_frame->linesize, 0, _dst_frame->height,_dst_frame->data,_dst_frame->linesize);
		}
		finally
		{
			sws_freeContext(_context);
		}
	}
	return _dest;
}
//////////////////////////////////////////////////////
System::Drawing::Bitmap^ FFmpeg::AVFrame::ToBitmap(AVFrame^ _frame)
{
	AVFrame^ _target = ConvertVideoFrame(_frame,AVPixelFormat::BGRA);
	if (_target != nullptr)
	{
		System::Drawing::Bitmap^ _image = nullptr;
		try
		{
			::AVFrame* _source = ((::AVFrame*)_target->_Pointer.ToPointer());
			if (_source->width > 0 && _source->height > 0 && _source->data[0] != nullptr && _source->linesize[0] != 0)
			{
				_image = gcnew System::Drawing::Bitmap(_source->width,_source->height,_source->linesize[0],
#ifdef PixelFormat
#	undef PixelFormat
					System::Drawing::Imaging::PixelFormat::Format32bppPArgb	
#	define PixelFormat AVPixelFormat
#else
					System::Drawing::Imaging::PixelFormat::Format32bppPArgb	
#endif	
					,(IntPtr)_source->data[0]
					);
				return gcnew System::Drawing::Bitmap(_image);
			}
		}
		finally
		{
			if (_image != nullptr) delete _image;
			delete _target;
		}
	}
	return nullptr;
}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVFrame::GetClass()
{
	return gcnew AVClass((void*)avcodec_get_frame_class(),nullptr);
}
//////////////////////////////////////////////////////
// AVCodecHWConfig
//////////////////////////////////////////////////////
FFmpeg::AVCodecHWConfig::AVCodecHWConfig(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
FFmpeg::AVPixelFormat FFmpeg::AVCodecHWConfig::pix_fmt::get()
{
	return (AVPixelFormat)((::AVCodecHWConfig*)m_pPointer)->pix_fmt;
}
FFmpeg::AV_CODEC_HW_CONFIG_METHOD FFmpeg::AVCodecHWConfig::methods::get()
{
	return (AV_CODEC_HW_CONFIG_METHOD)((::AVCodecHWConfig*)m_pPointer)->methods;
}
FFmpeg::AVHWDeviceType FFmpeg::AVCodecHWConfig::device_type::get()
{
	return (AVHWDeviceType)((::AVCodecHWConfig*)m_pPointer)->device_type;
}
//////////////////////////////////////////////////////
// AVHWAccel
//////////////////////////////////////////////////////
FFmpeg::AVHWAccel::AVHWAccel(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
String^ FFmpeg::AVHWAccel::name::get()
{ 
	auto p = ((::AVHWAccel*)m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}

FFmpeg::AVMediaType FFmpeg::AVHWAccel::type::get()
{
	return (AVMediaType)((::AVHWAccel*)m_pPointer)->type;
}
FFmpeg::AVCodecID FFmpeg::AVHWAccel::id::get()
{
	return (AVCodecID)((::AVHWAccel*)m_pPointer)->type;
}
FFmpeg::AVPixelFormat FFmpeg::AVHWAccel::pix_fmt::get()
{
	return (AVPixelFormat)((::AVHWAccel*)m_pPointer)->type;
}
FFmpeg::AV_HWACCEL_CODEC_CAP FFmpeg::AVHWAccel::capabilities::get()
{
	return (AV_HWACCEL_CODEC_CAP)((::AVHWAccel*)m_pPointer)->capabilities;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVHWAccel::ToString()
{
	if (m_pPointer)
	{
		String^ _name = name;
		if (!String::IsNullOrEmpty(_name))
		{
			return "[ AVHWAccel ] \"" + _name +"\"";
		}
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
FFmpeg::AVHWAccel^ FFmpeg::AVHWAccel::Next()
{
	return Next(this);
}
//////////////////////////////////////////////////////
FFmpeg::AVHWAccel^ FFmpeg::AVHWAccel::Next(AVHWAccel^ hwaccel)
{
	DYNAMIC_DEF_API(AVCodec,::AVHWAccel *,nullptr,av_hwaccel_next,const ::AVHWAccel*);
	auto p = av_hwaccel_next((const ::AVHWAccel*)(hwaccel != nullptr ? hwaccel->m_pPointer : nullptr));
	return p != nullptr ? gcnew AVHWAccel(p,nullptr) : nullptr;
}
//////////////////////////////////////////////////////
// AVCodecContext
//////////////////////////////////////////////////////
int avcodec_handler_get_buffer2(::AVCodecContext *s, ::AVFrame *frame, int flags)
{
	int ret = -1;
	if (s && s->opaque)
	{
		GCHandle _handle = GCHandle::FromIntPtr((IntPtr)s->opaque);
		if (_handle.IsAllocated)
		{
			FFmpeg::AVFrame^ p = frame != nullptr ? gcnew FFmpeg::AVFrame(frame,nullptr) : nullptr;
			ret = ((FFmpeg::AVCodecContext^)_handle.Target)->GetBuffer2(p,flags);
			if (p != nullptr) delete p;
		}
	}
	return ret;
}

::AVPixelFormat avcodec_handler_get_format(::AVCodecContext *s,const ::AVPixelFormat * fmt)
{
	::AVPixelFormat ret = AV_PIX_FMT_NONE;
	if (s && s->opaque)
	{
		GCHandle _handle = GCHandle::FromIntPtr((IntPtr)s->opaque);
		if (_handle.IsAllocated)
		{
			List<FFmpeg::AVPixelFormat>^ list = gcnew List<FFmpeg::AVPixelFormat>();
			if (fmt)
			{
				int idx = 0;
				while (fmt[idx] != -1) { list->Add(fmt[idx++]); }
			}
			ret = ((FFmpeg::AVCodecContext^)_handle.Target)->GetFormat(list->ToArray());
			delete list;
		}
	}
	return ret;
}

void avcodec_handler_draw_horiz_band(::AVCodecContext *s,
	const ::AVFrame *src, int offset[AV_NUM_DATA_POINTERS],
	int y, int type, int height)
{
	if (s && s->opaque)
	{
		GCHandle _handle = GCHandle::FromIntPtr((IntPtr)s->opaque);
		if (_handle.IsAllocated)
		{
			array<int>^ _offset = gcnew array<int>(AV_NUM_DATA_POINTERS);
			for (int i = 0; i < AV_NUM_DATA_POINTERS; i++) _offset[i] = offset[i];
			FFmpeg::AVFrame^ p = src != nullptr ? gcnew FFmpeg::AVFrame((void*)src,nullptr) : nullptr;
			((FFmpeg::AVCodecContext^)_handle.Target)->DrawHorizBand(p,_offset,y,type,height);
			delete _offset;
			if (p != nullptr) delete p;
		}
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVCodecContext::AVCodecContext(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
	, m_pGetBuffer2(nullptr)
	, m_pDrawHorizBand(nullptr)
	, m_pGetFormat(nullptr)
	, m_pOpaque(IntPtr::Zero)
	, m_pGetFormatNative(IntPtr::Zero)
	, m_pDrawHorizBandNative(IntPtr::Zero)
	, m_pGetBuffer2Native(IntPtr::Zero)
{
	if (m_pPointer)
	{
		m_pGetFormatNative = (IntPtr)((::AVCodecContext*)m_pPointer)->get_format;
		m_pDrawHorizBandNative = (IntPtr)((::AVCodecContext*)m_pPointer)->draw_horiz_band;
		m_pGetBuffer2Native = (IntPtr)((::AVCodecContext*)m_pPointer)->get_buffer2;
	}
}

FFmpeg::AVCodecContext::AVCodecContext(AVCodec^ _codec)
	: AVBase(nullptr,nullptr)
	, m_pGetBuffer2(nullptr)
	, m_pDrawHorizBand(nullptr)
	, m_pGetFormat(nullptr)
	, m_pOpaque(IntPtr::Zero)
	, m_pGetFormatNative(IntPtr::Zero)
	, m_pDrawHorizBandNative(IntPtr::Zero)
	, m_pGetBuffer2Native(IntPtr::Zero)
{
	LibAVCodec::RegisterAll();
	m_pPointer = avcodec_alloc_context3(_codec != nullptr ? (::AVCodec*)_codec->_Pointer.ToPointer() : nullptr);
	if (m_pPointer)
	{
		m_pFreep = (TFreeFNP*)avcodec_free_context;
		m_pGetFormatNative = (IntPtr)((::AVCodecContext*)m_pPointer)->get_format;
		m_pDrawHorizBandNative = (IntPtr)((::AVCodecContext*)m_pPointer)->draw_horiz_band;
		m_pGetBuffer2Native = (IntPtr)((::AVCodecContext*)m_pPointer)->get_buffer2;
	}
}

//////////////////////////////////////////////////////
String^ FFmpeg::AVCodecContext::ToString()
{
	char buf[1024] = {0};
	avcodec_string(buf,_countof(buf),(::AVCodecContext*)m_pPointer,0);
	if (strlen(buf)) return "[ AVCodecContext ] \"" + gcnew String(buf) + "\"";
	return Object::ToString();
}
//////////////////////////////////////////////////////
int FFmpeg::AVCodecContext::_StructureSize::get()
{
	return sizeof(::AVCodecContext);
}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVCodecContext::av_class::get()
{
	auto p = ((::AVCodecContext*)m_pPointer)->av_class;
	return _CreateObject<FFmpeg::AVClass>((void*)p);
}

int FFmpeg::AVCodecContext::log_level_offset::get()
{
	return ((::AVCodecContext*)m_pPointer)->log_level_offset;
}
void FFmpeg::AVCodecContext::log_level_offset::set(int value)
{
	((::AVCodecContext*)m_pPointer)->log_level_offset = value;
}

FFmpeg::AVMediaType FFmpeg::AVCodecContext::codec_type::get()
{
	return (AVMediaType)((::AVCodecContext*)m_pPointer)->codec_type;
}
void FFmpeg::AVCodecContext::codec_type::set(AVMediaType value)
{
	((::AVCodecContext*)m_pPointer)->codec_type = (::AVMediaType)value;
}

FFmpeg::AVCodec^ FFmpeg::AVCodecContext::codec::get()
{
	auto p = ((::AVCodecContext*)m_pPointer)->codec;
	return _CreateObject<FFmpeg::AVCodec>((void*)p);
}
void FFmpeg::AVCodecContext::codec::set(AVCodec^ value)
{
	AddObject((IntPtr)((void*)((::AVCodecContext*)m_pPointer)->codec),value);
	((::AVCodecContext*)m_pPointer)->codec = (::AVCodec*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}

FFmpeg::AVCodecID FFmpeg::AVCodecContext::codec_id::get()
{
	return (AVCodecID)((::AVCodecContext*)m_pPointer)->codec_id;
}

void FFmpeg::AVCodecContext::codec_id::set(AVCodecID value)
{
	((::AVCodecContext*)m_pPointer)->codec_id = (::AVCodecID)value;
}

UInt32 FFmpeg::AVCodecContext::codec_tag::get()
{
	return ((::AVCodecContext*)m_pPointer)->codec_tag;
}
void FFmpeg::AVCodecContext::codec_tag::set(UInt32 value)
{
	((::AVCodecContext*)m_pPointer)->codec_tag = value;
}

IntPtr FFmpeg::AVCodecContext::priv_data::get()
{
	return (IntPtr)((::AVCodecContext*)m_pPointer)->priv_data;
}

IntPtr FFmpeg::AVCodecContext::opaque::get()
{
	Threading::Monitor::Enter(this);
	try
	{
		if (m_pGetBuffer2 || m_pDrawHorizBand || m_pGetFormat)
		{
			return m_pOpaque;
		}
		else
		{
			return (IntPtr)((::AVCodecContext*)m_pPointer)->opaque;
		}
	}
	finally
	{
		Threading::Monitor::Exit(this);
	}
}
void FFmpeg::AVCodecContext::opaque::set(IntPtr value)
{
	Threading::Monitor::Enter(this);
	try
	{
		if (m_pGetBuffer2 || m_pDrawHorizBand || m_pGetFormat)
		{
			m_pOpaque = value;
		}
		else
		{
			((::AVCodecContext*)m_pPointer)->opaque = value.ToPointer();
		}
	}
	finally
	{
		Threading::Monitor::Exit(this);
	}
}

Int64 FFmpeg::AVCodecContext::bit_rate::get()
{
	return ((::AVCodecContext*)m_pPointer)->bit_rate;
}
void FFmpeg::AVCodecContext::bit_rate::set(Int64  value)
{
	((::AVCodecContext*)m_pPointer)->bit_rate = value;
}

int FFmpeg::AVCodecContext::bit_rate_tolerance::get()
{
	return ((::AVCodecContext*)m_pPointer)->bit_rate_tolerance;
}
void FFmpeg::AVCodecContext::bit_rate_tolerance::set(int value)
{
	((::AVCodecContext*)m_pPointer)->bit_rate_tolerance = value;
}

int FFmpeg::AVCodecContext::global_quality::get()
{
	return ((::AVCodecContext*)m_pPointer)->global_quality;
}
void FFmpeg::AVCodecContext::global_quality::set(int value)
{
	((::AVCodecContext*)m_pPointer)->global_quality = value;
}

int FFmpeg::AVCodecContext::compression_level::get()
{
	return ((::AVCodecContext*)m_pPointer)->compression_level;
}
void FFmpeg::AVCodecContext::compression_level::set(int value)
{
	((::AVCodecContext*)m_pPointer)->compression_level = value;
}

FFmpeg::AVCodecFlag FFmpeg::AVCodecContext::flags::get()
{
	return (AVCodecFlag)((::AVCodecContext*)m_pPointer)->flags;
}
void FFmpeg::AVCodecContext::flags::set(AVCodecFlag value)
{
	((::AVCodecContext*)m_pPointer)->flags = (int)value;
}

FFmpeg::AVCodecFlag2 FFmpeg::AVCodecContext::flags2::get()
{
	return (AVCodecFlag2)((::AVCodecContext*)m_pPointer)->flags2;
}
void FFmpeg::AVCodecContext::flags2::set(AVCodecFlag2 value)
{
	((::AVCodecContext*)m_pPointer)->flags2 = (int)value;
}

array<byte>^ FFmpeg::AVCodecContext::extra_data::get()
{
	List<byte>^ _array = nullptr;
	int nCount = ((::AVCodecContext*)m_pPointer)->extradata_size;
	uint8_t * p = (uint8_t *)((::AVCodecContext*)m_pPointer)->extradata;
	if (p && nCount > 0)
	{
		_array =  gcnew List<byte>();
		while (nCount-- > 0)
		{
			_array->Add(*p++);
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}
void FFmpeg::AVCodecContext::extra_data::set(array<byte>^ value)
{
	if (value != nullptr && value->Length > 0)
	{
		av_freep(&((::AVCodecContext*)m_pPointer)->extradata);
		((::AVCodecContext*)m_pPointer)->extradata_size = value->Length;
		((::AVCodecContext*)m_pPointer)->extradata = (uint8_t*)av_malloc(value->Length + AV_INPUT_BUFFER_PADDING_SIZE);
		Marshal::Copy(value,0,(IntPtr)((::AVCodecContext*)m_pPointer)->extradata,(int)((::AVCodecContext*)m_pPointer)->extradata_size);
	}
	else
	{
		av_freep(&((::AVCodecContext*)m_pPointer)->extradata);
		((::AVCodecContext*)m_pPointer)->extradata_size = 0;
		((::AVCodecContext*)m_pPointer)->extradata = nullptr;
	}
}

FFmpeg::AVRational^ FFmpeg::AVCodecContext::time_base::get()
{
	return _CreateObject<AVRational>((void*)&((::AVCodecContext*)m_pPointer)->time_base);
}
void FFmpeg::AVCodecContext::time_base::set(AVRational^ value)
{
	::AVRational _rational = { value != nullptr ? value->num : 0, value != nullptr ? value->den : 0 };
	((::AVCodecContext*)m_pPointer)->time_base = _rational;
}

int FFmpeg::AVCodecContext::ticks_per_frame::get()
{
	return ((::AVCodecContext*)m_pPointer)->ticks_per_frame;
}
int FFmpeg::AVCodecContext::delay::get()
{
	return ((::AVCodecContext*)m_pPointer)->delay;
}
int FFmpeg::AVCodecContext::width::get()
{
	return ((::AVCodecContext*)m_pPointer)->width;
}
void FFmpeg::AVCodecContext::width::set(int value)
{
	((::AVCodecContext*)m_pPointer)->width = value;
}

int FFmpeg::AVCodecContext::height::get()
{
	return ((::AVCodecContext*)m_pPointer)->height;
}
void FFmpeg::AVCodecContext::height::set(int value)
{ 
	((::AVCodecContext*)m_pPointer)->height = value;
}

int FFmpeg::AVCodecContext::coded_width::get()
{
	return ((::AVCodecContext*)m_pPointer)->coded_width;
}
void FFmpeg::AVCodecContext::coded_width::set(int value)
{
	((::AVCodecContext*)m_pPointer)->coded_width = value;
}

int FFmpeg::AVCodecContext::coded_height::get()
{
	return ((::AVCodecContext*)m_pPointer)->coded_height;
}
void FFmpeg::AVCodecContext::coded_height::set(int value)
{
	((::AVCodecContext*)m_pPointer)->coded_height = value;
}

int FFmpeg::AVCodecContext::gop_size::get()
{
	return ((::AVCodecContext*)m_pPointer)->gop_size;
}
void FFmpeg::AVCodecContext::gop_size::set(int value)
{
	((::AVCodecContext*)m_pPointer)->gop_size = value;
}

FFmpeg::AVPixelFormat FFmpeg::AVCodecContext::pix_fmt::get()
{
	return (AVPixelFormat)((::AVCodecContext*)m_pPointer)->pix_fmt;
}
void FFmpeg::AVCodecContext::pix_fmt::set(AVPixelFormat value)
{
	((::AVCodecContext*)m_pPointer)->pix_fmt = (::AVPixelFormat)value;
}

FFmpeg::DrawHorizBandHandler^ FFmpeg::AVCodecContext::draw_horiz_band::get()
{
	Threading::Monitor::Enter(this);
	try
	{
		return m_pDrawHorizBand;
	}
	finally
	{
		Threading::Monitor::Exit(this);
	}
}
void FFmpeg::AVCodecContext::draw_horiz_band::set(DrawHorizBandHandler^ value)
{
	if (m_pDrawHorizBand != value)
	{
		Threading::Monitor::Enter(this);
		try
		{
			if (value == nullptr)
			{
				if (!m_pGetFormat && !m_pGetBuffer2)
				{
					((::AVCodecContext *)m_pPointer)->opaque = m_pOpaque.ToPointer();
				}
				typedef void (draw_horiz_band_t)(::AVCodecContext *s,
					const ::AVFrame *src, int offset[AV_NUM_DATA_POINTERS],
					int y, int type, int height);
				((::AVCodecContext *)m_pPointer)->draw_horiz_band = (draw_horiz_band_t*)m_pDrawHorizBandNative.ToPointer();
			}
			else
			{
				if (m_pDrawHorizBand == nullptr)
				{
					m_pDrawHorizBandNative = (IntPtr)((::AVCodecContext *)m_pPointer)->draw_horiz_band;
				}
				if (!m_pGetFormat && !m_pGetBuffer2)
				{
					m_pOpaque = (IntPtr)((::AVCodecContext *)m_pPointer)->opaque;
					if (!m_ThisPtr.IsAllocated)
					{
						m_ThisPtr = GCHandle::Alloc(this, GCHandleType::Weak);
					}
					((::AVCodecContext *)m_pPointer)->opaque = (void*)GCHandle::ToIntPtr(m_ThisPtr);
				}
				((::AVCodecContext *)m_pPointer)->draw_horiz_band = avcodec_handler_draw_horiz_band;
			}
			m_pDrawHorizBand = value;
		}
		finally
		{
			Threading::Monitor::Exit(this);
		}
	}
}

FFmpeg::GetFormatHandler^ FFmpeg::AVCodecContext::get_format::get()
{
	Threading::Monitor::Enter(this);
	try
	{
		return m_pGetFormat;
	}
	finally
	{
		Threading::Monitor::Exit(this);
	}
}
void FFmpeg::AVCodecContext::get_format::set(GetFormatHandler^ value)
{
	if (m_pGetFormat != value)
	{
		Threading::Monitor::Enter(this);
		try
		{
			if (value == nullptr)
			{
				if (!m_pDrawHorizBand && !m_pGetBuffer2)
				{
					((::AVCodecContext *)m_pPointer)->opaque = m_pOpaque.ToPointer();
				}
				typedef ::AVPixelFormat (get_format_t)(::AVCodecContext *,const ::AVPixelFormat * );
				((::AVCodecContext *)m_pPointer)->get_format = (get_format_t*)m_pGetFormatNative.ToPointer();
			}
			else
			{
				if (m_pGetFormat == nullptr)
				{
					m_pGetFormatNative = (IntPtr)((::AVCodecContext *)m_pPointer)->get_format;
				}
				if (!m_pDrawHorizBand && !m_pGetBuffer2)
				{
					m_pOpaque = (IntPtr)((::AVCodecContext *)m_pPointer)->opaque;
					if (!m_ThisPtr.IsAllocated)
					{
						m_ThisPtr = GCHandle::Alloc(this, GCHandleType::Weak);
					}
					((::AVCodecContext *)m_pPointer)->opaque = (void*)GCHandle::ToIntPtr(m_ThisPtr);
				}
				((::AVCodecContext *)m_pPointer)->get_format = avcodec_handler_get_format;
			}
			m_pGetFormat = value;
		}
		finally
		{
			Threading::Monitor::Exit(this);
		}
	}
}

int FFmpeg::AVCodecContext::max_b_frames::get()
{
	return ((::AVCodecContext*)m_pPointer)->max_b_frames;
}

void FFmpeg::AVCodecContext::max_b_frames::set(int value)
{
	((::AVCodecContext*)m_pPointer)->max_b_frames = value;
}

float FFmpeg::AVCodecContext::b_quant_factor::get()
{
	return ((::AVCodecContext*)m_pPointer)->b_quant_factor;
}
void FFmpeg::AVCodecContext::b_quant_factor::set(float value)
{
	((::AVCodecContext*)m_pPointer)->b_quant_factor = value;
}

int FFmpeg::AVCodecContext::b_frame_strategy::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "b_strategy", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->b_frame_strategy;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::b_frame_strategy::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "b_strategy", value, 0)) 
	{ 
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->b_frame_strategy = value;
#endif
	}
}

float FFmpeg::AVCodecContext::b_quant_offset::get()
{
	return ((::AVCodecContext*)m_pPointer)->b_quant_offset;
}
void FFmpeg::AVCodecContext::b_quant_offset::set(float value)
{
	((::AVCodecContext*)m_pPointer)->b_quant_offset = value;
}

int FFmpeg::AVCodecContext::has_b_frames::get()
{
	return ((::AVCodecContext*)m_pPointer)->has_b_frames;
}
int FFmpeg::AVCodecContext::mpeg_quant::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "mpeg_quant", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->mpeg_quant;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::mpeg_quant::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "mpeg_quant", value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->mpeg_quant = value;
#endif
	}
}

float FFmpeg::AVCodecContext::i_quant_factor::get()
{
	return ((::AVCodecContext*)m_pPointer)->i_quant_factor;
}
void FFmpeg::AVCodecContext::i_quant_factor::set(float value)
{
	((::AVCodecContext*)m_pPointer)->i_quant_factor = value;
}

float FFmpeg::AVCodecContext::i_quant_offset::get()
{
	return ((::AVCodecContext*)m_pPointer)->i_quant_offset;
}
void FFmpeg::AVCodecContext::i_quant_offset::set(float value)
{
	((::AVCodecContext*)m_pPointer)->i_quant_offset = value;
}

float FFmpeg::AVCodecContext::lumi_masking::get()
{
	return ((::AVCodecContext*)m_pPointer)->lumi_masking;
}
void FFmpeg::AVCodecContext::lumi_masking::set(float value)
{
	((::AVCodecContext*)m_pPointer)->lumi_masking = value;
}

float FFmpeg::AVCodecContext::temporal_cplx_masking::get()
{
	return ((::AVCodecContext*)m_pPointer)->temporal_cplx_masking;
}
void FFmpeg::AVCodecContext::temporal_cplx_masking::set(float value)
{
	((::AVCodecContext*)m_pPointer)->temporal_cplx_masking = value;
}

float FFmpeg::AVCodecContext::spatial_cplx_masking::get()
{
	return ((::AVCodecContext*)m_pPointer)->spatial_cplx_masking;
}
void FFmpeg::AVCodecContext::spatial_cplx_masking::set(float value)
{
	((::AVCodecContext*)m_pPointer)->spatial_cplx_masking = value;
}

float FFmpeg::AVCodecContext::p_masking::get()
{
	return ((::AVCodecContext*)m_pPointer)->p_masking;
}
void FFmpeg::AVCodecContext::p_masking::set(float value)
{
	((::AVCodecContext*)m_pPointer)->p_masking = value;
}

float FFmpeg::AVCodecContext::dark_masking::get()
{
	return ((::AVCodecContext*)m_pPointer)->dark_masking;
}
void FFmpeg::AVCodecContext::dark_masking::set(float value)
{
	((::AVCodecContext*)m_pPointer)->dark_masking = value;
}

int FFmpeg::AVCodecContext::slice_count::get()
{
	return ((::AVCodecContext*)m_pPointer)->slice_count;
}
void FFmpeg::AVCodecContext::slice_count::set(int value)
{
	((::AVCodecContext*)m_pPointer)->slice_count = value;
}

FFmpeg::FFPred FFmpeg::AVCodecContext::prediction_method::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "pred", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->prediction_method;
#endif
	}
	return (FFPred)val;
}
void FFmpeg::AVCodecContext::prediction_method::set(FFPred value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "pred", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->prediction_method = (int)value;
#endif
	}
}

IntPtr FFmpeg::AVCodecContext::slice_offset::get()
{
	return (IntPtr)((::AVCodecContext*)m_pPointer)->slice_offset;
}
void FFmpeg::AVCodecContext::slice_offset::set(IntPtr value)
{
	((::AVCodecContext*)m_pPointer)->slice_offset = (int*)value.ToPointer();
}

FFmpeg::AVRational^ FFmpeg::AVCodecContext::sample_aspect_ratio::get()
{
	return _CreateObject<AVRational>((void*)&((::AVCodecContext*)m_pPointer)->sample_aspect_ratio);
}

void FFmpeg::AVCodecContext::sample_aspect_ratio::set(AVRational^ value)
{
	::AVRational _rational = { value != nullptr ? value->num : 0, value != nullptr ? value->den : 0 };
	((::AVCodecContext*)m_pPointer)->sample_aspect_ratio = _rational;
}

FFmpeg::FFCmp FFmpeg::AVCodecContext::me_cmp::get()
{
	return (FFCmp)((::AVCodecContext*)m_pPointer)->me_cmp;
}
void FFmpeg::AVCodecContext::me_cmp::set(FFCmp value)
{
	((::AVCodecContext*)m_pPointer)->me_cmp = (int)value;
}

FFmpeg::FFCmp FFmpeg::AVCodecContext::me_sub_cmp::get()
{
	return (FFCmp)((::AVCodecContext*)m_pPointer)->me_sub_cmp;
}
void FFmpeg::AVCodecContext::me_sub_cmp::set(FFCmp value)
{
	((::AVCodecContext*)m_pPointer)->me_sub_cmp = (int)value;
}

FFmpeg::FFCmp FFmpeg::AVCodecContext::mb_cmp::get()
{
	return (FFCmp)((::AVCodecContext*)m_pPointer)->mb_cmp;
}
void FFmpeg::AVCodecContext::mb_cmp::set(FFCmp value)
{
	((::AVCodecContext*)m_pPointer)->mb_cmp = (int)value;
}

FFmpeg::FFCmp FFmpeg::AVCodecContext::ildct_cmp::get()
{
	return (FFCmp)((::AVCodecContext*)m_pPointer)->ildct_cmp;
}
void FFmpeg::AVCodecContext::ildct_cmp::set(FFCmp value)
{
	((::AVCodecContext*)m_pPointer)->ildct_cmp = (int)value;
}

int FFmpeg::AVCodecContext::dia_size::get()
{
	return ((::AVCodecContext*)m_pPointer)->dia_size;
}
void FFmpeg::AVCodecContext::dia_size::set(int value)
{
	((::AVCodecContext*)m_pPointer)->dia_size = value;
}

int FFmpeg::AVCodecContext::last_predictor_count::get()
{
	return ((::AVCodecContext*)m_pPointer)->last_predictor_count;
}
void FFmpeg::AVCodecContext::last_predictor_count::set(int value)
{
	((::AVCodecContext*)m_pPointer)->last_predictor_count = value;
}

int FFmpeg::AVCodecContext::pre_me::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "preme", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->pre_me;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::pre_me::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "preme", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->pre_me = (int)value;
#endif
	}
}

FFmpeg::FFCmp FFmpeg::AVCodecContext::me_pre_cmp::get()
{
	return (FFCmp)((::AVCodecContext*)m_pPointer)->me_pre_cmp;
}
void FFmpeg::AVCodecContext::me_pre_cmp::set(FFCmp value)
{
	((::AVCodecContext*)m_pPointer)->me_pre_cmp = (int)value;
}

int FFmpeg::AVCodecContext::pre_dia_size::get()
{
	return ((::AVCodecContext*)m_pPointer)->pre_dia_size;
}
void FFmpeg::AVCodecContext::pre_dia_size::set(int value)
{
	((::AVCodecContext*)m_pPointer)->pre_dia_size = value;
}

int FFmpeg::AVCodecContext::me_subpel_quality::get()
{
	return ((::AVCodecContext*)m_pPointer)->pre_dia_size;
}
void FFmpeg::AVCodecContext::me_subpel_quality::set(int value)
{
	((::AVCodecContext*)m_pPointer)->pre_dia_size = value;
}

FFmpeg::FFDtg FFmpeg::AVCodecContext::dtg_active_format::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"dtg_active_format",0,&val))
	{
		return (FFDtg)val;
	}
	return FFDtg::None;
}

bool FFmpeg::AVCodecContext::IsOpened::get()
{
	return avcodec_is_open((::AVCodecContext*)m_pPointer) > 0;
}

int FFmpeg::AVCodecContext::me_range::get()
{
	return ((::AVCodecContext*)m_pPointer)->me_range;
}
void FFmpeg::AVCodecContext::me_range::set(int value)
{
	((::AVCodecContext*)m_pPointer)->me_range = value;
}

FFmpeg::SliceFlag FFmpeg::AVCodecContext::slice_flags::get()
{
	return (SliceFlag)((::AVCodecContext*)m_pPointer)->slice_flags;
}
void FFmpeg::AVCodecContext::slice_flags::set(SliceFlag value)
{
	((::AVCodecContext*)m_pPointer)->slice_flags = (int)value;
} 

FFmpeg::FFMbDecision FFmpeg::AVCodecContext::mb_decision::get()
{
	return (FFMbDecision)((::AVCodecContext*)m_pPointer)->mb_decision;
}
void FFmpeg::AVCodecContext::mb_decision::set(FFMbDecision value)
{
	((::AVCodecContext*)m_pPointer)->mb_decision = (int)value;
} 

IntPtr FFmpeg::AVCodecContext::intra_matrix::get()
{
	return (IntPtr)((::AVCodecContext*)m_pPointer)->intra_matrix;
}
void FFmpeg::AVCodecContext::intra_matrix::set(IntPtr value)
{
	((::AVCodecContext*)m_pPointer)->intra_matrix = (uint16_t*)value.ToPointer();
} 

IntPtr FFmpeg::AVCodecContext::inter_matrix::get()
{
	return (IntPtr)((::AVCodecContext*)m_pPointer)->inter_matrix;
}
void FFmpeg::AVCodecContext::inter_matrix::set(IntPtr value)
{
	((::AVCodecContext*)m_pPointer)->inter_matrix = (uint16_t*)value.ToPointer();
} 

int FFmpeg::AVCodecContext::scenechange_threshold::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "sc_threshold", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->scenechange_threshold;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::scenechange_threshold::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "sc_threshold", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->scenechange_threshold = (int)value;
#endif
	}
} 

int FFmpeg::AVCodecContext::noise_reduction::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "nr", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->noise_reduction;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::noise_reduction::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "nr", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->noise_reduction = (int)value;
#endif
	}
} 

int FFmpeg::AVCodecContext::intra_dc_precision::get()
{
	return ((::AVCodecContext*)m_pPointer)->intra_dc_precision;
}
void FFmpeg::AVCodecContext::intra_dc_precision::set(int value)
{
	((::AVCodecContext*)m_pPointer)->intra_dc_precision = value;
} 

int FFmpeg::AVCodecContext::skip_top::get()
{
	return ((::AVCodecContext*)m_pPointer)->skip_top;
}
void FFmpeg::AVCodecContext::skip_top::set(int value)
{
	((::AVCodecContext*)m_pPointer)->skip_top = value;
} 

int FFmpeg::AVCodecContext::skip_bottom::get()
{
	return ((::AVCodecContext*)m_pPointer)->skip_bottom;
}
void FFmpeg::AVCodecContext::skip_bottom::set(int value)
{
	((::AVCodecContext*)m_pPointer)->skip_bottom = value;
} 

int FFmpeg::AVCodecContext::mb_lmin::get()
{
	return ((::AVCodecContext*)m_pPointer)->mb_lmin;
}
void FFmpeg::AVCodecContext::mb_lmin::set(int value)
{
	((::AVCodecContext*)m_pPointer)->mb_lmin = value;
} 

int FFmpeg::AVCodecContext::mb_lmax::get()
{
	return ((::AVCodecContext*)m_pPointer)->mb_lmax;
}
void FFmpeg::AVCodecContext::mb_lmax::set(int value)
{
	((::AVCodecContext*)m_pPointer)->mb_lmax = value;
} 

int FFmpeg::AVCodecContext::me_penalty_compensation::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "mepc", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->me_penalty_compensation;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::me_penalty_compensation::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "mepc", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->me_penalty_compensation = (int)value;
#endif
	}
} 

int FFmpeg::AVCodecContext::bidir_refine::get()
{
	return ((::AVCodecContext*)m_pPointer)->bidir_refine;
}
void FFmpeg::AVCodecContext::bidir_refine::set(int value)
{
	((::AVCodecContext*)m_pPointer)->bidir_refine = value;
} 

int FFmpeg::AVCodecContext::brd_scale::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "brd_scale", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->brd_scale;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::brd_scale::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "brd_scale", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->brd_scale = (int)value;
#endif
	}
} 

int FFmpeg::AVCodecContext::keyint_min::get()
{
	return ((::AVCodecContext*)m_pPointer)->keyint_min;
}
void FFmpeg::AVCodecContext::keyint_min::set(int value)
{
	((::AVCodecContext*)m_pPointer)->keyint_min = value;
} 

int FFmpeg::AVCodecContext::refs::get()
{
	return ((::AVCodecContext*)m_pPointer)->refs;
}
void FFmpeg::AVCodecContext::refs::set(int value)
{
	((::AVCodecContext*)m_pPointer)->refs = value;
} 

int FFmpeg::AVCodecContext::chromaoffset::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "chromaoffset", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->chromaoffset;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::chromaoffset::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "chromaoffset", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->chromaoffset = (int)value;
#endif
	}
} 

int FFmpeg::AVCodecContext::mv0_threshold::get()
{
	return ((::AVCodecContext*)m_pPointer)->mv0_threshold;
}
void FFmpeg::AVCodecContext::mv0_threshold::set(int value)
{
	((::AVCodecContext*)m_pPointer)->mv0_threshold = value;
} 

int FFmpeg::AVCodecContext::b_sensitivity::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "b_sensitivity", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->b_sensitivity;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::b_sensitivity::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "b_sensitivity", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->b_sensitivity = (int)value;
#endif
	}
} 

FFmpeg::AVColorPrimaries FFmpeg::AVCodecContext::color_primaries::get()
{
	return (AVColorPrimaries)((::AVCodecContext*)m_pPointer)->color_primaries;
}
void FFmpeg::AVCodecContext::color_primaries::set(AVColorPrimaries value)
{
	((::AVCodecContext*)m_pPointer)->color_primaries = (::AVColorPrimaries)value;
} 

FFmpeg::AVColorTransferCharacteristic FFmpeg::AVCodecContext::color_trc::get()
{
	return (AVColorTransferCharacteristic)((::AVCodecContext*)m_pPointer)->color_trc;
}
void FFmpeg::AVCodecContext::color_trc::set(AVColorTransferCharacteristic value)
{
	((::AVCodecContext*)m_pPointer)->color_trc = (::AVColorTransferCharacteristic)value;
} 

FFmpeg::AVColorSpace FFmpeg::AVCodecContext::colorspace::get()
{
	return (AVColorSpace)((::AVCodecContext*)m_pPointer)->colorspace;
}
void FFmpeg::AVCodecContext::colorspace::set(AVColorSpace value)
{
	((::AVCodecContext*)m_pPointer)->colorspace = (::AVColorSpace)value;
} 

FFmpeg::AVColorRange FFmpeg::AVCodecContext::color_range::get()
{
	return (AVColorRange)((::AVCodecContext*)m_pPointer)->color_range;
}
void FFmpeg::AVCodecContext::color_range::set(AVColorRange value)
{
	((::AVCodecContext*)m_pPointer)->color_range = (::AVColorRange)value;
} 

FFmpeg::AVChromaLocation FFmpeg::AVCodecContext::chroma_sample_location::get()
{
	return (AVChromaLocation)((::AVCodecContext*)m_pPointer)->chroma_sample_location;
}
void FFmpeg::AVCodecContext::chroma_sample_location::set(AVChromaLocation value)
{
	((::AVCodecContext*)m_pPointer)->chroma_sample_location = (::AVChromaLocation)value;
} 

int FFmpeg::AVCodecContext::slices::get()
{
	return ((::AVCodecContext*)m_pPointer)->slices;
}
void FFmpeg::AVCodecContext::slices::set(int value)
{
	((::AVCodecContext*)m_pPointer)->slices = value;
}

FFmpeg::AVFieldOrder FFmpeg::AVCodecContext::field_order::get()
{
	return (AVFieldOrder)((::AVCodecContext*)m_pPointer)->field_order;
}
void FFmpeg::AVCodecContext::field_order::set(AVFieldOrder value)
{
	((::AVCodecContext*)m_pPointer)->field_order = (::AVFieldOrder)value;
}

int FFmpeg::AVCodecContext::sample_rate::get()
{
	return ((::AVCodecContext*)m_pPointer)->sample_rate;
}
void FFmpeg::AVCodecContext::sample_rate::set(int value)
{
	((::AVCodecContext*)m_pPointer)->sample_rate = value;
}

int FFmpeg::AVCodecContext::channels::get()
{
	return ((::AVCodecContext*)m_pPointer)->channels;
}
void FFmpeg::AVCodecContext::channels::set(int value)
{
	((::AVCodecContext*)m_pPointer)->channels = value;
}

FFmpeg::AVSampleFormat FFmpeg::AVCodecContext::sample_fmt::get()
{
	return (AVSampleFormat)((::AVCodecContext*)m_pPointer)->sample_fmt;
}
void FFmpeg::AVCodecContext::sample_fmt::set(AVSampleFormat value)
{
	((::AVCodecContext*)m_pPointer)->sample_fmt = (::AVSampleFormat)value;
}

int FFmpeg::AVCodecContext::frame_size::get()
{
	return ((::AVCodecContext*)m_pPointer)->frame_size;
}
int FFmpeg::AVCodecContext::frame_number::get()
{
	return ((::AVCodecContext*)m_pPointer)->frame_number;
}

int FFmpeg::AVCodecContext::block_align::get()
{
	return ((::AVCodecContext*)m_pPointer)->block_align;
}
void FFmpeg::AVCodecContext::block_align::set(int value)
{
	((::AVCodecContext*)m_pPointer)->block_align = value;
}

int FFmpeg::AVCodecContext::cutoff::get()
{
	return ((::AVCodecContext*)m_pPointer)->cutoff;
}
void FFmpeg::AVCodecContext::cutoff::set(int value)
{
	((::AVCodecContext*)m_pPointer)->cutoff = value;
}

FFmpeg::AVChannelLayout FFmpeg::AVCodecContext::channel_layout::get()
{
	return (AVChannelLayout)((::AVCodecContext*)m_pPointer)->channel_layout;
}
void FFmpeg::AVCodecContext::channel_layout::set(AVChannelLayout value)
{
	((::AVCodecContext*)m_pPointer)->channel_layout = (uint64_t)value;
}

FFmpeg::AVChannelLayout FFmpeg::AVCodecContext::request_channel_layout::get()
{
	return (AVChannelLayout)((::AVCodecContext*)m_pPointer)->request_channel_layout;
}
void FFmpeg::AVCodecContext::request_channel_layout::set(AVChannelLayout value)
{
	((::AVCodecContext*)m_pPointer)->request_channel_layout = (uint64_t)value;
}

FFmpeg::AVAudioServiceType FFmpeg::AVCodecContext::audio_service_type::get()
{
	return (AVAudioServiceType)((::AVCodecContext*)m_pPointer)->audio_service_type;
}
void FFmpeg::AVCodecContext::audio_service_type::set(AVAudioServiceType value)
{
	((::AVCodecContext*)m_pPointer)->audio_service_type = (::AVAudioServiceType)value;
}

FFmpeg::AVSampleFormat FFmpeg::AVCodecContext::request_sample_fmt::get()
{
	return (AVSampleFormat)((::AVCodecContext*)m_pPointer)->request_sample_fmt;
}
void FFmpeg::AVCodecContext::request_sample_fmt::set(AVSampleFormat value)
{
	((::AVCodecContext*)m_pPointer)->request_sample_fmt = (::AVSampleFormat)value;
}

FFmpeg::GetBuffer2Handler^ FFmpeg::AVCodecContext::get_buffer2::get()
{
	Threading::Monitor::Enter(this);
	try
	{
		return m_pGetBuffer2;
	}
	finally
	{
		Threading::Monitor::Exit(this);
	}
}
void FFmpeg::AVCodecContext::get_buffer2::set(GetBuffer2Handler^ value)
{
	if (m_pGetBuffer2 != value)
	{
		Threading::Monitor::Enter(this);
		try
		{
			if (value == nullptr)
			{
				if (!m_pDrawHorizBand && !m_pGetFormat)
				{
					((::AVCodecContext *)m_pPointer)->opaque = m_pOpaque.ToPointer();
				}
				typedef int (get_buffer2_t)(::AVCodecContext *, ::AVFrame *, int );
				((::AVCodecContext *)m_pPointer)->get_buffer2 = (get_buffer2_t*)m_pGetBuffer2Native.ToPointer();
			}
			else
			{
				if (m_pGetBuffer2 == nullptr)
				{
					m_pGetBuffer2Native = (IntPtr)((::AVCodecContext *)m_pPointer)->get_buffer2;
				}
				if (!m_pDrawHorizBand && !m_pGetFormat)
				{
					m_pOpaque = (IntPtr)((::AVCodecContext *)m_pPointer)->opaque;
					if (!m_ThisPtr.IsAllocated)
					{
						m_ThisPtr = GCHandle::Alloc(this, GCHandleType::Weak);
					}
					((::AVCodecContext *)m_pPointer)->opaque = (void*)GCHandle::ToIntPtr(m_ThisPtr);
				}
				((::AVCodecContext *)m_pPointer)->get_buffer2 = avcodec_handler_get_buffer2;
			}
			m_pGetBuffer2 = value;
		}
		finally
		{
			Threading::Monitor::Exit(this);
		}
	}
}

int FFmpeg::AVCodecContext::refcounted_frames::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "refcounted_frames", 0, &val))
	{
#if (LIBAVCODEC_VERSION_MAJOR < 59)
		val = ((::AVCodecContext*)m_pPointer)->refcounted_frames;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::refcounted_frames::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "refcounted_frames", (int64_t)value, 0))
	{
#if (LIBAVCODEC_VERSION_MAJOR < 59)
		((::AVCodecContext*)m_pPointer)->refcounted_frames = (int)value;
#endif
	}
}

float FFmpeg::AVCodecContext::qcompress::get()
{
	return ((::AVCodecContext*)m_pPointer)->qcompress;
}
void FFmpeg::AVCodecContext::qcompress::set(float value)
{
	((::AVCodecContext*)m_pPointer)->qcompress = value;
}

float FFmpeg::AVCodecContext::qblur::get()
{
	return ((::AVCodecContext*)m_pPointer)->qblur;
}
void FFmpeg::AVCodecContext::qblur::set(float value)
{
	((::AVCodecContext*)m_pPointer)->qblur = value;
}

int FFmpeg::AVCodecContext::qmin::get()
{
	return ((::AVCodecContext*)m_pPointer)->qmin;
}
void FFmpeg::AVCodecContext::qmin::set(int value)
{
	((::AVCodecContext*)m_pPointer)->qmin = value;
}

int FFmpeg::AVCodecContext::qmax::get()
{
	return ((::AVCodecContext*)m_pPointer)->qmax;
}
void FFmpeg::AVCodecContext::qmax::set(int value)
{
	((::AVCodecContext*)m_pPointer)->qmax = value;
}

int FFmpeg::AVCodecContext::max_qdiff::get()
{
	return ((::AVCodecContext*)m_pPointer)->max_qdiff;
}
void FFmpeg::AVCodecContext::max_qdiff::set(int value)
{
	((::AVCodecContext*)m_pPointer)->max_qdiff = value;
}

int FFmpeg::AVCodecContext::rc_buffer_size::get()
{
	return ((::AVCodecContext*)m_pPointer)->rc_buffer_size;
}
void FFmpeg::AVCodecContext::rc_buffer_size::set(int value)
{
	((::AVCodecContext*)m_pPointer)->rc_buffer_size = value;
}

array<FFmpeg::RcOverride^>^ FFmpeg::AVCodecContext::rc_override::get()
{
	List<RcOverride^>^ _array = nullptr;
	int nCount = ((::AVCodecContext*)m_pPointer)->rc_override_count;
	::RcOverride * _p = ((::AVCodecContext*)m_pPointer)->rc_override;
	if (_p && nCount > 0)
	{
		_array =  gcnew List<FFmpeg::RcOverride^>();
		while (nCount-- > 0)
		{
			_array->Add(_CreateObject<RcOverride>(_p++));
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}

void FFmpeg::AVCodecContext::rc_override::set(array<RcOverride^>^ value)
{
	if (value != nullptr && value->Length > 0)
	{
		::RcOverride * p = (::RcOverride *)AllocMemory("rc_override",value->Length * sizeof(::RcOverride)).ToPointer();
		((::AVCodecContext*)m_pPointer)->rc_override = p;
		((::AVCodecContext*)m_pPointer)->rc_override_count = value->Length;
		for (int i = 0; i < value->Length; i++)
		{
			memcpy(&p[i],value[i]->_Pointer.ToPointer(),sizeof(::RcOverride));
		}
	}
	else
	{
		FreeMemory("rc_override");
		((::AVCodecContext*)m_pPointer)->rc_override_count = 0;
		((::AVCodecContext*)m_pPointer)->rc_override = nullptr;
	}
}

Int64 FFmpeg::AVCodecContext::rc_max_rate::get()
{
	return ((::AVCodecContext*)m_pPointer)->rc_max_rate;
}
void FFmpeg::AVCodecContext::rc_max_rate::set(Int64 value)
{
	((::AVCodecContext*)m_pPointer)->rc_max_rate = value;
}

Int64 FFmpeg::AVCodecContext::rc_min_rate::get()
{
	return ((::AVCodecContext*)m_pPointer)->rc_min_rate;
}
void FFmpeg::AVCodecContext::rc_min_rate::set(Int64 value)
{
	((::AVCodecContext*)m_pPointer)->rc_min_rate = value;
}

float FFmpeg::AVCodecContext::rc_max_available_vbv_use::get()
{
	return ((::AVCodecContext*)m_pPointer)->rc_max_available_vbv_use;
}
void FFmpeg::AVCodecContext::rc_max_available_vbv_use::set(float value)
{
	((::AVCodecContext*)m_pPointer)->rc_max_available_vbv_use = value;
}

float FFmpeg::AVCodecContext::rc_min_vbv_overflow_use::get()
{
	return ((::AVCodecContext*)m_pPointer)->rc_min_vbv_overflow_use;
}
void FFmpeg::AVCodecContext::rc_min_vbv_overflow_use::set(float value)
{
	((::AVCodecContext*)m_pPointer)->rc_min_vbv_overflow_use = value;
}

int FFmpeg::AVCodecContext::rc_initial_buffer_occupancy::get()
{
	return ((::AVCodecContext*)m_pPointer)->rc_initial_buffer_occupancy;
}
void FFmpeg::AVCodecContext::rc_initial_buffer_occupancy::set(int value)
{
	((::AVCodecContext*)m_pPointer)->rc_initial_buffer_occupancy = value;
}

FFmpeg::FFCoderType FFmpeg::AVCodecContext::coder_type::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "coder", 0, &val))
	{
#if FF_API_CODER_TYPE
		val = ((::AVCodecContext*)m_pPointer)->coder_type;
#endif
	}
	return (FFCoderType)val;
}
void FFmpeg::AVCodecContext::coder_type::set(FFCoderType value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "coder", (int64_t)value, 0))
	{
#if FF_API_CODER_TYPE
		((::AVCodecContext*)m_pPointer)->coder_type = (int)value;
#endif
	}
}

int FFmpeg::AVCodecContext::context_model::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "context", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->context_model;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::context_model::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "context", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->context_model = (int)value;
#endif
	}
}

int FFmpeg::AVCodecContext::frame_skip_threshold::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "skip_threshold", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->frame_skip_threshold;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::frame_skip_threshold::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "skip_threshold", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->frame_skip_threshold = value;
#endif
	}
}

int FFmpeg::AVCodecContext::frame_skip_factor::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "skip_factor", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->frame_skip_factor;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::frame_skip_factor::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "skip_factor", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->frame_skip_factor = value;
#endif
	}
}

int FFmpeg::AVCodecContext::frame_skip_exp::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "skip_exp", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->frame_skip_exp;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::frame_skip_exp::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "skip_exp", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->frame_skip_exp = value;
#endif
	}
}

int FFmpeg::AVCodecContext::frame_skip_cmp::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "skipcmp", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->frame_skip_cmp;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::frame_skip_cmp::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "skipcmp", (int64_t)value, 0))
	{
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->frame_skip_cmp = value;
#endif
	}
}

int FFmpeg::AVCodecContext::trellis::get()
{
	return ((::AVCodecContext*)m_pPointer)->trellis;
}
void FFmpeg::AVCodecContext::trellis::set(int value)
{
	((::AVCodecContext*)m_pPointer)->trellis = value;
}

int FFmpeg::AVCodecContext::min_prediction_order::get()
{
#if FF_API_PRIVATE_OPT
	return ((::AVCodecContext*)m_pPointer)->min_prediction_order;
#endif
	__int64 val = 0;
	av_opt_get_int(m_pPointer, "min_prediction_order", 0, &val);
	return (int)val;
}
void FFmpeg::AVCodecContext::min_prediction_order::set(int value)
{
#if FF_API_PRIVATE_OPT
	((::AVCodecContext*)m_pPointer)->min_prediction_order = value;
#endif
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "min_prediction_order", value, 0)) { }
}

int FFmpeg::AVCodecContext::max_prediction_order::get()
{
#if FF_API_PRIVATE_OPT
	return ((::AVCodecContext*)m_pPointer)->max_prediction_order;
#endif
	__int64 val = 0;
	av_opt_get_int(m_pPointer, "max_prediction_order", 0, &val);
	return (int)val;
}
void FFmpeg::AVCodecContext::max_prediction_order::set(int value)
{
#if FF_API_PRIVATE_OPT
	((::AVCodecContext*)m_pPointer)->max_prediction_order = value;
#endif
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "max_prediction_order", value, 0)) { }
}

Int64 FFmpeg::AVCodecContext::timecode_frame_start::get()
{
#if FF_API_PRIVATE_OPT
	return ((::AVCodecContext*)m_pPointer)->timecode_frame_start;
#endif
	__int64 val = 0;
	av_opt_get_int(m_pPointer, "timecode_frame_start", 0, &val);
	return (int)val;
}
void FFmpeg::AVCodecContext::timecode_frame_start::set(Int64 value)
{
#if FF_API_PRIVATE_OPT
	((::AVCodecContext*)m_pPointer)->timecode_frame_start = value;
#endif
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "timecode_frame_start", value, 0)) { }
}
int FFmpeg::AVCodecContext::rtp_payload_size::get()
{
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "ps", 0, &val))
	{
#if FF_API_PRIVATE_OPT
		val = ((::AVCodecContext*)m_pPointer)->rtp_payload_size;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::rtp_payload_size::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "ps", value, 0)) 
	{ 
#if FF_API_PRIVATE_OPT
		((::AVCodecContext*)m_pPointer)->rtp_payload_size = value;
#endif
	}
}

int FFmpeg::AVCodecContext::mv_bits::get()
{
#if FF_API_STAT_BITS
	return ((::AVCodecContext*)m_pPointer)->mv_bits;
#endif
	__int64 val = 0;
	av_opt_get_int(m_pPointer, "mv_bits", 0, &val);
	return (int)val;

}
int FFmpeg::AVCodecContext::header_bits::get()
{
#if FF_API_STAT_BITS
	return ((::AVCodecContext*)m_pPointer)->header_bits;
#endif
	__int64 val = 0;
	av_opt_get_int(m_pPointer, "header_bits", 0, &val);
	return (int)val;
}
int FFmpeg::AVCodecContext::i_tex_bits::get()
{
#if FF_API_STAT_BITS
	return ((::AVCodecContext*)m_pPointer)->i_tex_bits;
#endif
	__int64 val = 0;
	av_opt_get_int(m_pPointer, "i_tex_bits", 0, &val);
	return (int)val;
}
int FFmpeg::AVCodecContext::p_tex_bits::get()
{
#if FF_API_STAT_BITS
	return ((::AVCodecContext*)m_pPointer)->p_tex_bits;
#endif
	__int64 val = 0;
	av_opt_get_int(m_pPointer, "p_tex_bits", 0, &val);
	return (int)val;
}
int FFmpeg::AVCodecContext::i_count::get()
{
#if FF_API_STAT_BITS
	return ((::AVCodecContext*)m_pPointer)->i_count;
#endif
	__int64 val = 0;
	av_opt_get_int(m_pPointer, "i_count", 0, &val);
	return (int)val;
}
int FFmpeg::AVCodecContext::p_count::get()
{
#if FF_API_STAT_BITS
	return ((::AVCodecContext*)m_pPointer)->p_count;
#endif
	__int64 val = 0;
	av_opt_get_int(m_pPointer, "p_count", 0, &val);
	return (int)val;
}
int FFmpeg::AVCodecContext::skip_count::get()
{
#if FF_API_STAT_BITS
	return ((::AVCodecContext*)m_pPointer)->skip_count;
#endif
	__int64 val = 0;
	av_opt_get_int(m_pPointer, "skip_count", 0, &val);
	return (int)val;
}
int FFmpeg::AVCodecContext::misc_bits::get()
{
#if FF_API_STAT_BITS
	return ((::AVCodecContext*)m_pPointer)->misc_bits;
#endif
	__int64 val = 0;
	av_opt_get_int(m_pPointer, "misc_bits", 0, &val);
	return (int)val;
}
int FFmpeg::AVCodecContext::frame_bits::get()
{
#if FF_API_STAT_BITS
	return ((::AVCodecContext*)m_pPointer)->frame_bits;
#endif
	__int64 val = 0;
	av_opt_get_int(m_pPointer, "frame_bits", 0, &val);
	return (int)val;
}
IntPtr FFmpeg::AVCodecContext::stats_out::get()
{
	return (IntPtr)((::AVCodecContext*)m_pPointer)->stats_out;
}
IntPtr FFmpeg::AVCodecContext::stats_in::get()
{
	return (IntPtr)((::AVCodecContext*)m_pPointer)->stats_in;
}
void FFmpeg::AVCodecContext::stats_in::set(IntPtr value)
{
	((::AVCodecContext*)m_pPointer)->stats_in = (char*)value.ToPointer();
}
FFmpeg::FFBug FFmpeg::AVCodecContext::workaround_bugs::get()
{
	return (FFBug)((::AVCodecContext*)m_pPointer)->workaround_bugs;
}
void FFmpeg::AVCodecContext::workaround_bugs::set(FFBug value)
{
	((::AVCodecContext*)m_pPointer)->workaround_bugs = (int)value;
}
FFmpeg::FFCompliance FFmpeg::AVCodecContext::strict_std_compliance::get()
{
	return (FFCompliance)((::AVCodecContext*)m_pPointer)->strict_std_compliance;
}
void FFmpeg::AVCodecContext::strict_std_compliance::set(FFCompliance value)
{
	((::AVCodecContext*)m_pPointer)->strict_std_compliance = (int)value;
}
FFmpeg::ErrorConcealment FFmpeg::AVCodecContext::error_concealment::get()
{
	return (ErrorConcealment)((::AVCodecContext*)m_pPointer)->error_concealment;
}
void FFmpeg::AVCodecContext::error_concealment::set(ErrorConcealment value)
{
	((::AVCodecContext*)m_pPointer)->error_concealment = (int)value;
}
FFmpeg::FFDebug FFmpeg::AVCodecContext::debug::get()
{
	return (FFDebug)((::AVCodecContext*)m_pPointer)->debug;
}
void FFmpeg::AVCodecContext::debug::set(FFDebug value)
{
	((::AVCodecContext*)m_pPointer)->debug = (int)value;
}
FFmpeg::FFDebugMV FFmpeg::AVCodecContext::debug_mv::get()
{
	return (FFDebugMV)((::AVCodecContext*)m_pPointer)->debug_mv;
}
void FFmpeg::AVCodecContext::debug_mv::set(FFDebugMV value)
{
	((::AVCodecContext*)m_pPointer)->debug_mv = (int)value;
}
FFmpeg::ErrorRecognition FFmpeg::AVCodecContext::err_recognition::get()
{
	return (ErrorRecognition)((::AVCodecContext*)m_pPointer)->err_recognition;
}
void FFmpeg::AVCodecContext::err_recognition::set(ErrorRecognition value)
{
	((::AVCodecContext*)m_pPointer)->err_recognition = (int)value;
}
Int64 FFmpeg::AVCodecContext::reordered_opaque::get()
{
	return ((::AVCodecContext*)m_pPointer)->reordered_opaque;
}
void FFmpeg::AVCodecContext::reordered_opaque::set(Int64 value)
{
	((::AVCodecContext*)m_pPointer)->reordered_opaque = value;
}
FFmpeg::AVHWAccel^ FFmpeg::AVCodecContext::hwaccel::get()
{
	return _CreateObject<AVHWAccel>((void*)((::AVCodecContext*)m_pPointer)->hwaccel);
}
IntPtr FFmpeg::AVCodecContext::hwaccel_context::get()
{
	return (IntPtr)((::AVCodecContext*)m_pPointer)->hwaccel_context;
}
void FFmpeg::AVCodecContext::hwaccel_context::set(IntPtr value)
{
	((::AVCodecContext*)m_pPointer)->hwaccel_context = value.ToPointer();
}

FFmpeg::AVArray<UInt64>^ FFmpeg::AVCodecContext::error::get()
{
	auto p = ((::AVCodecContext *)m_pPointer)->error;
	AVArray<UInt64>^ _array = (AVArray<UInt64>^)GetObject((IntPtr)p);
	if (_array) return _array;
	return gcnew FFmpeg::AVArray<UInt64>(((::AVCodecContext*)m_pPointer)->error,this,sizeof(UInt64),AV_NUM_DATA_POINTERS);
}

FFmpeg::FFDct FFmpeg::AVCodecContext::dct_algo::get()
{
	return (FFDct)((::AVCodecContext*)m_pPointer)->dct_algo;
}
void FFmpeg::AVCodecContext::dct_algo::set(FFDct value)
{
	((::AVCodecContext*)m_pPointer)->dct_algo = (int)value;
}
FFmpeg::FFIdct FFmpeg::AVCodecContext::idct_algo::get()
{
	return (FFIdct)((::AVCodecContext*)m_pPointer)->idct_algo;
}
void FFmpeg::AVCodecContext::idct_algo::set(FFIdct value)
{
	((::AVCodecContext*)m_pPointer)->idct_algo = (int)value;
}
int FFmpeg::AVCodecContext::bits_per_coded_sample::get()
{
	return ((::AVCodecContext*)m_pPointer)->bits_per_coded_sample;
}
void FFmpeg::AVCodecContext::bits_per_coded_sample::set(int value)
{
	((::AVCodecContext*)m_pPointer)->bits_per_coded_sample = value;
}
int FFmpeg::AVCodecContext::bits_per_raw_sample::get()
{
	return ((::AVCodecContext*)m_pPointer)->bits_per_raw_sample;
}
void FFmpeg::AVCodecContext::bits_per_raw_sample::set(int value)
{
	((::AVCodecContext*)m_pPointer)->bits_per_raw_sample = value;
}
int FFmpeg::AVCodecContext::lowres::get()
{
	LOAD_API(AVCodec,int,av_codec_get_lowres,const ::AVCodecContext*);
	if (av_codec_get_lowres)
		return av_codec_get_lowres((::AVCodecContext*)m_pPointer);
	__int64 val = 0;
	if (AVERROR_OPTION_NOT_FOUND == av_opt_get_int(m_pPointer, "lowres", 0, &val))
	{
#if FF_API_LOWRES
		val = ((::AVCodecContext*)m_pPointer)->lowres;
#endif
	}
	return (int)val;
}
void FFmpeg::AVCodecContext::lowres::set(int value)
{
	LOAD_API(AVCodec,void,av_codec_set_lowres,const ::AVCodecContext*,int);
	if (av_codec_set_lowres)
		av_codec_set_lowres((::AVCodecContext*)m_pPointer,value);
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer, "lowres", (int64_t)value, 0))
	{
#if FF_API_LOWRES
		((::AVCodecContext*)m_pPointer)->lowres = value;
#endif
	}
}

FFmpeg::AVFrame^ FFmpeg::AVCodecContext::coded_frame::get()
{
	FFmpeg::AVFrame^ frame = nullptr;
#if FF_API_CODED_FRAME
	frame = _CreateObject<AVFrame>((void*)((::AVCodecContext*)m_pPointer)->coded_frame);
#endif
	return frame;
}
int FFmpeg::AVCodecContext::thread_count::get()
{
	return ((::AVCodecContext*)m_pPointer)->thread_count;
}
void FFmpeg::AVCodecContext::thread_count::set(int value)
{
	((::AVCodecContext*)m_pPointer)->thread_count = value;
}
FFmpeg::ThreadType FFmpeg::AVCodecContext::thread_type::get()
{
	return (ThreadType)((::AVCodecContext*)m_pPointer)->thread_type;
}
void FFmpeg::AVCodecContext::thread_type::set(ThreadType value)
{
	((::AVCodecContext*)m_pPointer)->thread_type = (int)value;
}
FFmpeg::ThreadType FFmpeg::AVCodecContext::active_thread_type::get()
{
	return (ThreadType)((::AVCodecContext*)m_pPointer)->active_thread_type;
}
int FFmpeg::AVCodecContext::thread_safe_callbacks::get()
{
	return ((::AVCodecContext*)m_pPointer)->thread_safe_callbacks;
}
void FFmpeg::AVCodecContext::thread_safe_callbacks::set(int value)
{
	((::AVCodecContext*)m_pPointer)->thread_safe_callbacks = value;
}
int FFmpeg::AVCodecContext::nsse_weight::get()
{
	return ((::AVCodecContext*)m_pPointer)->nsse_weight;
}
void FFmpeg::AVCodecContext::nsse_weight::set(int value)
{
	((::AVCodecContext*)m_pPointer)->nsse_weight = value;
}
FFmpeg::Profile FFmpeg::AVCodecContext::profile::get()
{
	return (Profile)((::AVCodecContext*)m_pPointer)->profile;
}
void FFmpeg::AVCodecContext::profile::set(Profile value)
{
	((::AVCodecContext*)m_pPointer)->profile = (int)value;
}
int FFmpeg::AVCodecContext::level::get()
{
	return ((::AVCodecContext*)m_pPointer)->level;
}
void FFmpeg::AVCodecContext::level::set(int value)
{
	((::AVCodecContext*)m_pPointer)->level = value;
}
FFmpeg::AVDiscard FFmpeg::AVCodecContext::skip_loop_filter::get()
{
	return (AVDiscard)((::AVCodecContext*)m_pPointer)->skip_loop_filter;
}
void FFmpeg::AVCodecContext::skip_loop_filter::set(AVDiscard value)
{
	((::AVCodecContext*)m_pPointer)->skip_loop_filter = (::AVDiscard)value;
}
FFmpeg::AVDiscard FFmpeg::AVCodecContext::skip_idct::get()
{
	return (AVDiscard)((::AVCodecContext*)m_pPointer)->skip_idct;
}
void FFmpeg::AVCodecContext::skip_idct::set(AVDiscard value)
{
	((::AVCodecContext*)m_pPointer)->skip_idct = (::AVDiscard)value;
}
FFmpeg::AVDiscard FFmpeg::AVCodecContext::skip_frame::get()
{
	return (AVDiscard)((::AVCodecContext*)m_pPointer)->skip_frame;
}
void FFmpeg::AVCodecContext::skip_frame::set(AVDiscard value)
{
	((::AVCodecContext*)m_pPointer)->skip_frame = (::AVDiscard)value;
}
array<byte>^ FFmpeg::AVCodecContext::subtitle_header::get()
{
	List<byte>^ _array = nullptr;
	int nCount = ((::AVCodecContext*)m_pPointer)->subtitle_header_size;
	uint8_t * p = (uint8_t *)((::AVCodecContext*)m_pPointer)->subtitle_header;
	if (p && nCount > 0)
	{
		_array =  gcnew List<byte>();
		while (nCount-- > 0)
		{
			_array->Add(*p++);
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}
void FFmpeg::AVCodecContext::subtitle_header::set(array<byte>^ value)
{
	if (value != nullptr && value->Length > 0)
	{
		((::AVCodecContext*)m_pPointer)->subtitle_header_size = value->Length;
		((::AVCodecContext*)m_pPointer)->subtitle_header = 
			(uint8_t *)AllocMemory("subtitle_header",value->Length).ToPointer();
		Marshal::Copy(value,0,
			(IntPtr)((::AVCodecContext*)m_pPointer)->subtitle_header,value->Length);
	}
	else
	{
		FreeMemory("subtitle_header");
		((::AVCodecContext*)m_pPointer)->subtitle_header_size = 0;
		((::AVCodecContext*)m_pPointer)->subtitle_header = nullptr;
	}
}

UInt64 FFmpeg::AVCodecContext::vbv_delay::get()
{
#if FF_API_VBV_DELAY
	return ((::AVCodecContext*)m_pPointer)->vbv_delay;
#endif
	return 0;
}
int FFmpeg::AVCodecContext::side_data_only_packets::get()
{
#if FF_API_SIDEDATA_ONLY_PKT
	return ((::AVCodecContext*)m_pPointer)->side_data_only_packets;
#endif
	return 0;
}
void FFmpeg::AVCodecContext::side_data_only_packets::set(int value)
{
#if FF_API_SIDEDATA_ONLY_PKT
	((::AVCodecContext*)m_pPointer)->side_data_only_packets = value;
#endif
}
int FFmpeg::AVCodecContext::initial_padding::get()
{
	return ((::AVCodecContext*)m_pPointer)->initial_padding;
}
FFmpeg::AVRational^ FFmpeg::AVCodecContext::framerate::get()
{
	return _CreateObject<AVRational>((void*)&((::AVCodecContext*)m_pPointer)->framerate);
}

void FFmpeg::AVCodecContext::framerate::set(AVRational^ value)
{
	((::AVCodecContext*)m_pPointer)->framerate.num = value->num;
	((::AVCodecContext*)m_pPointer)->framerate.den = value->den;
}

FFmpeg::AVPixelFormat FFmpeg::AVCodecContext::sw_pix_fmt::get()
{
	return (AVPixelFormat)((::AVCodecContext*)m_pPointer)->sw_pix_fmt;
}
FFmpeg::AVRational^ FFmpeg::AVCodecContext::pkt_timebase::get()
{
	::AVRational _rational = {0,0};
	LOAD_API(AVCodec,::AVRational,av_codec_get_pkt_timebase,const ::AVCodecContext*);
	if (av_codec_get_pkt_timebase)
	{
		_rational = av_codec_get_pkt_timebase((::AVCodecContext*)m_pPointer);
	}
	else
	{
		if (AVERROR_OPTION_NOT_FOUND == av_opt_get_q(m_pPointer, "pkt_timebase", 0, &_rational))
		{
			_rational = ((::AVCodecContext*)m_pPointer)->pkt_timebase;
		}
	}
	return gcnew AVRational(*((Int64*)&_rational));	
}
void FFmpeg::AVCodecContext::pkt_timebase::set(AVRational^ value)
{
	::AVRational _rational = { value != nullptr ? value->num : 0, value != nullptr ? value->den : 0 };
	LOAD_API(AVCodec,void,av_codec_set_pkt_timebase,const ::AVCodecContext*,::AVRational q);
	if (av_codec_set_pkt_timebase)
	{
		av_codec_set_pkt_timebase((::AVCodecContext*)m_pPointer, _rational);
	}
	else
	{
		if (AVERROR_OPTION_NOT_FOUND == av_opt_set_q(m_pPointer, "pkt_timebase", _rational, 0))
		{
			((::AVCodecContext*)m_pPointer)->pkt_timebase = _rational;
		}
	}
}
FFmpeg::AVCodecDescriptor^ FFmpeg::AVCodecContext::codec_descriptor::get()
{
	const ::AVCodecDescriptor * p = nullptr;
	LOAD_API(AVCodec,::AVCodecDescriptor *,av_codec_get_codec_descriptor,const ::AVCodecContext*);
	if (av_codec_get_codec_descriptor)
	{
		p = av_codec_get_codec_descriptor((::AVCodecContext*)m_pPointer);
	}
	else
	{
		p = ((::AVCodecContext*)m_pPointer)->codec_descriptor;
	}
	return _CreateObject<AVCodecDescriptor>((void*)p);
}

void FFmpeg::AVCodecContext::codec_descriptor::set(AVCodecDescriptor^ value)
{
	const ::AVCodecDescriptor * old = nullptr;
	const ::AVCodecDescriptor * p = (const ::AVCodecDescriptor *)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
	LOAD_API(AVCodec,void,av_codec_set_codec_descriptor,const ::AVCodecContext*,const ::AVCodecDescriptor *);
	LOAD_API(AVCodec,::AVCodecDescriptor *,av_codec_get_codec_descriptor,const ::AVCodecContext*);
	if (av_codec_get_codec_descriptor)
	{
		old = av_codec_get_codec_descriptor((::AVCodecContext*)m_pPointer);
	}
	else
	{
		old = ((::AVCodecContext*)m_pPointer)->codec_descriptor;
	}
	if (p != old)
	{
		RemoveObject((IntPtr)(void*)old);
		if (p != nullptr)
		{
			AddObject((IntPtr)(void*)p,value);
			if (av_codec_set_codec_descriptor)
			{
				av_codec_set_codec_descriptor((::AVCodecContext*)m_pPointer,p);
			}
			else
			{
				((::AVCodecContext*)m_pPointer)->codec_descriptor = p;
			}
		}
	}
}

Int64 FFmpeg::AVCodecContext::pts_correction_num_faulty_pts::get()
{
	return ((::AVCodecContext*)m_pPointer)->pts_correction_num_faulty_pts;
}
Int64 FFmpeg::AVCodecContext::pts_correction_num_faulty_dts::get()
{
	return ((::AVCodecContext*)m_pPointer)->pts_correction_num_faulty_dts;
}
Int64 FFmpeg::AVCodecContext::pts_correction_last_pts::get()
{
	return ((::AVCodecContext*)m_pPointer)->pts_correction_last_pts;
}
Int64 FFmpeg::AVCodecContext::pts_correction_last_dts::get()
{
	return ((::AVCodecContext*)m_pPointer)->pts_correction_last_dts;
}
String^ FFmpeg::AVCodecContext::sub_charenc::get()
{
	auto p = ((::AVCodecContext*)m_pPointer)->sub_charenc;
	return p != nullptr ? gcnew String(p) : nullptr;
}
void FFmpeg::AVCodecContext::sub_charenc::set(String^ value)
{
	if (value && value->Length)
	{
		((::AVCodecContext*)m_pPointer)->sub_charenc = (char*)AllocString("sub_charenc",value).ToPointer();
	}
	else
	{
		((::AVCodecContext*)m_pPointer)->sub_charenc = nullptr;
		FreeMemory("sub_charenc");
	}
}

FFmpeg::FF_SUB_CHARENC_MODE FFmpeg::AVCodecContext::sub_charenc_mode::get()
{
	return (FF_SUB_CHARENC_MODE)((::AVCodecContext*)m_pPointer)->sub_charenc_mode;
}
int FFmpeg::AVCodecContext::skip_alpha::get()
{
	return ((::AVCodecContext*)m_pPointer)->skip_alpha;
}
void FFmpeg::AVCodecContext::skip_alpha::set(int value)
{
	((::AVCodecContext*)m_pPointer)->skip_alpha = value;
}
int FFmpeg::AVCodecContext::seek_preroll::get()
{
	DYNAMIC_DEF_API(AVCodec,int,((::AVCodecContext*)m_pPointer)->seek_preroll,av_codec_get_seek_preroll,::AVCodecContext*);
	return av_codec_get_seek_preroll((::AVCodecContext*)m_pPointer);
}
void FFmpeg::AVCodecContext::seek_preroll::set(int value)
{
	LOAD_API(AVCodec,void,av_codec_set_seek_preroll,::AVCodecContext*,int);
	if (av_codec_set_seek_preroll)
	{
		av_codec_set_seek_preroll((::AVCodecContext*)m_pPointer, value);
	}
	else
	{
		((::AVCodecContext*)m_pPointer)->seek_preroll = value;
	}
	
}
IntPtr FFmpeg::AVCodecContext::chroma_intra_matrix::get()
{
	DYNAMIC_DEF_API(AVCodec,uint16_t*,(IntPtr)((::AVCodecContext*)m_pPointer)->chroma_intra_matrix,av_codec_get_chroma_intra_matrix,::AVCodecContext*);
	return (IntPtr)av_codec_get_chroma_intra_matrix((::AVCodecContext*)m_pPointer);
}
void FFmpeg::AVCodecContext::chroma_intra_matrix::set(IntPtr value)
{
	LOAD_API(AVCodec,void,av_codec_set_chroma_intra_matrix,::AVCodecContext*,uint16_t *);
	if (av_codec_set_chroma_intra_matrix)
	{
		av_codec_set_chroma_intra_matrix((::AVCodecContext*)m_pPointer, (uint16_t *)value.ToPointer());
	}
	else
	{
		((::AVCodecContext*)m_pPointer)->chroma_intra_matrix = (uint16_t *)value.ToPointer();
	}
}
String^ FFmpeg::AVCodecContext::dump_separator::get()
{
	uint8_t * val = nullptr;
	if (0 >= av_opt_get(m_pPointer,"dump_separator",0,&val))
	{
		if (val)
		{
			try
			{
				return gcnew String((char*)val);
			}
			finally
			{
				av_free(val);
			}
		}
	}
	return nullptr;
}
void FFmpeg::AVCodecContext::dump_separator::set(String^ value)
{
	char * _temp = nullptr;
	try
	{
		_temp = (char*)AllocString(value).ToPointer();
		av_opt_set(m_pPointer,"dump_separator",_temp,0);
	}
	finally
	{
		FreeMemory((IntPtr)_temp);
	}
}
String^ FFmpeg::AVCodecContext::codec_whitelist::get()
{
	uint8_t * val = nullptr;
	if (0 >= av_opt_get(m_pPointer,"codec_whitelist",0,&val))
	{
		if (val)
		{
			try
			{
				return gcnew String((char*)val);
			}
			finally
			{
				av_free(val);
			}
		}
	}
	return nullptr;
}
void FFmpeg::AVCodecContext::codec_whitelist::set(String^ value)
{
	char * _temp = nullptr;
	try
	{
		_temp = (char*)AllocString(value).ToPointer();
		av_opt_set(m_pPointer,"codec_whitelist",_temp,0);
	}
	finally
	{
		FreeMemory((IntPtr)_temp);
	}
}

FFmpeg::FF_CODEC_PROPERTY FFmpeg::AVCodecContext::properties::get()
{
	return (FF_CODEC_PROPERTY)((::AVCodecContext*)m_pPointer)->properties;
}
array<FFmpeg::AVPacketSideData^>^ FFmpeg::AVCodecContext::coded_side_data::get()
{
	List<AVPacketSideData^>^ list = gcnew List<AVPacketSideData^>();
	try
	{
		auto p = ((::AVCodecContext*)m_pPointer);
		for (int i = 0; i < p->nb_coded_side_data; i++)
		{
			list->Add(_CreateObject<AVPacketSideData>(&p->coded_side_data[i]));
		}
		return list->ToArray();
	}
	finally
	{
		delete list;
	}
}
FFmpeg::AVBufferRef^ FFmpeg::AVCodecContext::hw_frames_ctx::get() 
{
	return _CreateObject<AVBufferRef>(((::AVCodecContext*)m_pPointer)->hw_frames_ctx);
}
void FFmpeg::AVCodecContext::hw_frames_ctx::set(FFmpeg::AVBufferRef^ value)
{
	AddObject((IntPtr)((::AVCodecContext*)m_pPointer)->hw_frames_ctx,value);
	((::AVCodecContext*)m_pPointer)->hw_frames_ctx = (::AVBufferRef*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}
FFmpeg::FF_SUB_TEXT_FMT FFmpeg::AVCodecContext::sub_text_format::get()
{
	return (FF_SUB_TEXT_FMT)((::AVCodecContext*)m_pPointer)->sub_text_format;
} 
void FFmpeg::AVCodecContext::sub_text_format::set(FF_SUB_TEXT_FMT value)
{
	((::AVCodecContext*)m_pPointer)->sub_text_format = (int)value;
} 
int FFmpeg::AVCodecContext::trailing_padding::get()
{
	return ((::AVCodecContext*)m_pPointer)->trailing_padding;
} 
Int64 FFmpeg::AVCodecContext::max_pixels::get()
{
	return ((::AVCodecContext*)m_pPointer)->max_pixels;
}
void FFmpeg::AVCodecContext::max_pixels::set(Int64 value)
{
	((::AVCodecContext*)m_pPointer)->max_pixels = value;
} 
FFmpeg::AVBufferRef^ FFmpeg::AVCodecContext::hw_device_ctx::get()
{
	return _CreateObject<AVBufferRef>(((::AVCodecContext*)m_pPointer)->hw_device_ctx);
}
void FFmpeg::AVCodecContext::hw_device_ctx::set(AVBufferRef^ value)
{
	AddObject((IntPtr)((::AVCodecContext*)m_pPointer)->hw_device_ctx,value);
	((::AVCodecContext*)m_pPointer)->hw_device_ctx = (::AVBufferRef*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}
FFmpeg::AV_HWACCEL_FLAG FFmpeg::AVCodecContext::hwaccel_flags::get()
{
	return (AV_HWACCEL_FLAG)((::AVCodecContext*)m_pPointer)->hwaccel_flags;
}
void FFmpeg::AVCodecContext::hwaccel_flags::set(AV_HWACCEL_FLAG value)
{
	((::AVCodecContext*)m_pPointer)->hwaccel_flags = (int)value;
}
bool FFmpeg::AVCodecContext::apply_cropping::get()
{
	return ((::AVCodecContext*)m_pPointer)->apply_cropping != 0;
}
void FFmpeg::AVCodecContext::apply_cropping::set(bool value)
{
	((::AVCodecContext*)m_pPointer)->apply_cropping = (value ? 1 : 0);
}
int FFmpeg::AVCodecContext::extra_hw_frames::get()
{
	return ((::AVCodecContext*)m_pPointer)->extra_hw_frames;
} 
void FFmpeg::AVCodecContext::extra_hw_frames::set(int value)
{
	((::AVCodecContext*)m_pPointer)->extra_hw_frames = value;
} 
int FFmpeg::AVCodecContext::discard_damaged_percentage::get()
{
	return ((::AVCodecContext*)m_pPointer)->discard_damaged_percentage;
} 
void FFmpeg::AVCodecContext::discard_damaged_percentage::set(int value)
{
	((::AVCodecContext*)m_pPointer)->discard_damaged_percentage = value;
} 
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVCodecContext::CopyTo(AVCodecContext^ dest)
{
	LOAD_API(AVCodec,int,avcodec_copy_context,::AVCodecContext*,::AVCodecContext*);
	if (avcodec_copy_context)
	{
		return avcodec_copy_context((::AVCodecContext*)dest->_Pointer.ToPointer(), (::AVCodecContext*)m_pPointer);
	}
	else
	{
		int ret = AVERROR(ENOMEM);
		::AVCodecParameters * parameters = avcodec_parameters_alloc();
		if (parameters)
		{
			try
			{
				ret = avcodec_parameters_from_context(parameters, ((::AVCodecContext*)m_pPointer));
				if (ret >= 0)
				{
					ret = avcodec_parameters_to_context((::AVCodecContext*)dest->_Pointer.ToPointer(),parameters);
				}
			}
			finally
			{
				avcodec_parameters_free(&parameters);
			}
		}
		return ret;
	}
}

FFmpeg::AVRESULT FFmpeg::AVCodecContext::Open(AVCodec^ codec)
{
	return Open(codec,nullptr);
}

FFmpeg::AVRESULT FFmpeg::AVCodecContext::Open(AVCodec^ codec, AVDictionary^ options)
{
	AVRESULT _result;
	::AVDictionary * _dict = options != nullptr ? (::AVDictionary *)options->_Pointer.ToPointer() : nullptr;
	try
	{
		_result = avcodec_open2((::AVCodecContext*)m_pPointer,(::AVCodec*)(codec != nullptr ? codec->_Pointer.ToPointer() : nullptr),&_dict);
		if (options)
		{
			options->ChangePointer(_dict);
			_dict = nullptr;
		}
	}
	finally
	{
		if (_dict) av_dict_free(&_dict);
	}
	return _result;
}

FFmpeg::AVRESULT FFmpeg::AVCodecContext::Close()
{
	return avcodec_close((::AVCodecContext*)m_pPointer);
}

void FFmpeg::AVCodecContext::Flush()
{
	avcodec_flush_buffers((::AVCodecContext*)m_pPointer);
}
void FFmpeg::AVCodecContext::AlignDimentions(int % _width,int % _height)
{
	int width = _width;
	int height = _height;
	try
	{
		avcodec_align_dimensions(((::AVCodecContext*)m_pPointer),&width,&height);
	}
	finally
	{
		_width = width;
		_height = height;
	}
}

void FFmpeg::AVCodecContext::AlignDimentions(int % _width,int % _height,array<int>^ % linesize_align)
{
	int width = _width;
	int height = _height;
	int line[AV_NUM_DATA_POINTERS];
	memset(line,0x00,sizeof(line));
	try
	{
		avcodec_align_dimensions2(((::AVCodecContext*)m_pPointer),&width,&height,line);
		List<int>^ _list = gcnew List<int>();
		for(int i = 0; i < _countof(line); i++) { _list->Add(line[i]); }
		linesize_align = _list->ToArray();
	}
	finally
	{
		_width = width;
		_height = height;
	}
}

int FFmpeg::AVCodecContext::GetAudioFrameDuration(int frame_bytes)
{
	return av_get_audio_frame_duration((::AVCodecContext*)m_pPointer, frame_bytes);
}

FFmpeg::AVRESULT FFmpeg::AVCodecContext::SetParameters(AVCodecParameters^ par)
{
	return avcodec_parameters_to_context((::AVCodecContext*)m_pPointer, (::AVCodecParameters*)par->_Pointer.ToPointer());
}

int FFmpeg::AVCodecContext::DecodeAudio(AVFrame^ frame,bool % got_frame_ptr,AVPacket^ avpkt)
{
	int i_got_frame_ptr = 0;
	try
	{
		LOAD_API(AVCodec,int,avcodec_decode_audio4,::AVCodecContext*,::AVFrame *,int *, const ::AVPacket *);
		if (avcodec_decode_audio4)
		{
			return avcodec_decode_audio4((::AVCodecContext*)m_pPointer, (::AVFrame*)frame->_Pointer.ToPointer(), &i_got_frame_ptr, (const ::AVPacket*)(avpkt != nullptr ? avpkt->_Pointer.ToPointer() : nullptr));
		}
		else
		{
			::AVPacket* p = nullptr;
			int pkt_size = 0;
			if (avpkt != nullptr)
			{
				p = (::AVPacket*)avpkt->_Pointer.ToPointer();
				pkt_size = p->size;
			}
			int ret = avcodec_send_packet((::AVCodecContext*)m_pPointer,p);
			if (ret >= 0)
			{
				ret = avcodec_receive_frame((::AVCodecContext*)m_pPointer,(::AVFrame*)frame->_Pointer.ToPointer());
				if (ret == 0)
				{
					i_got_frame_ptr = 1;
					ret = pkt_size;
				}
				if (ret == AVERROR(EAGAIN))
				{
					ret = pkt_size;
				}
			}
			return ret;
		}
	}
	finally
	{
		got_frame_ptr = (i_got_frame_ptr != 0);
		if (got_frame_ptr) frame->m_pDescructor = (TFreeFN*)av_frame_unref;
	}
}

int FFmpeg::AVCodecContext::DecodeVideo(AVFrame^ frame,bool % got_picture_ptr,AVPacket^ avpkt)
{
	int i_got_picture_ptr = 0;
	try
	{
		LOAD_API(AVCodec,int,avcodec_decode_video2,::AVCodecContext*,::AVFrame *,int *, const ::AVPacket *);
		if (avcodec_decode_video2)
		{
			return avcodec_decode_video2((::AVCodecContext*)m_pPointer, (::AVFrame*)frame->_Pointer.ToPointer(), &i_got_picture_ptr, (const ::AVPacket*)(avpkt != nullptr ? avpkt->_Pointer.ToPointer() : nullptr));
		}
		else
		{
			::AVPacket* p = nullptr;
			int pkt_size = 0;
			if (avpkt != nullptr)
			{
				p = (::AVPacket*)avpkt->_Pointer.ToPointer();
				pkt_size = p->size;
			}
			int ret = avcodec_send_packet((::AVCodecContext*)m_pPointer,p);
			if (ret >= 0)
			{
				ret = avcodec_receive_frame((::AVCodecContext*)m_pPointer,(::AVFrame*)frame->_Pointer.ToPointer());
				if (ret == 0)
				{
					i_got_picture_ptr = 1;
					ret = pkt_size;
				}
				if (ret == AVERROR(EAGAIN))
				{
					ret = pkt_size;
				}
			}
			return ret;
		}
	}
	finally
	{
		got_picture_ptr = (i_got_picture_ptr != 0);
		if (got_picture_ptr) frame->m_pDescructor = (TFreeFN*)av_frame_unref;
	}
}

FFmpeg::AVRESULT FFmpeg::AVCodecContext::DecodeSubtitle(AVSubtitle^ sub,bool % got_sub_ptr,AVPacket^ avpkt)
{
	int i_got_sub_ptr = 0;
	try
	{
		if (sub->m_pDescructor) sub->m_pDescructor(sub->_Pointer.ToPointer());
		return avcodec_decode_subtitle2((::AVCodecContext*)m_pPointer,(::AVSubtitle*)sub->_Pointer.ToPointer(),&i_got_sub_ptr,(::AVPacket*)(avpkt != nullptr ? avpkt->_Pointer.ToPointer() : nullptr));
	}
	finally
	{
		got_sub_ptr = (i_got_sub_ptr != 0);
		sub->m_pDescructor = got_sub_ptr ? (TFreeFN*)avsubtitle_free : nullptr;
	}
}

FFmpeg::AVRESULT FFmpeg::AVCodecContext::EncodeAudio(AVPacket^ avpkt,AVFrame^ frame,bool % got_packet_ptr)
{
	int i_got_packet_ptr = 0;
	try
	{
		LOAD_API(AVCodec,int,avcodec_encode_audio2,::AVCodecContext*,::AVPacket *,const ::AVFrame *,int *);
		if (avcodec_encode_audio2)
		{
			return avcodec_encode_audio2((::AVCodecContext *)m_pPointer, (::AVPacket *)avpkt->_Pointer.ToPointer(), (const ::AVFrame *)(frame != nullptr ? frame->_Pointer.ToPointer() : nullptr), &i_got_packet_ptr);
		}
		else
		{
			::AVFrame* p = nullptr;
			if (frame != nullptr)
			{
				p = (::AVFrame*)frame->_Pointer.ToPointer();
			}
			bool again = false;
			int ret = 0;
			while (true)
			{
				ret = avcodec_send_frame((::AVCodecContext*)m_pPointer, p);
				if (!again)
				{
					again = (ret == AVERROR(EAGAIN));
					if (ret >= 0 || ret == AVERROR(EAGAIN))
					{
						ret = avcodec_receive_packet((::AVCodecContext*)m_pPointer, (::AVPacket*)avpkt->_Pointer.ToPointer());
						if (ret == 0)
						{
							i_got_packet_ptr = 1;
						}
						if (ret == AVERROR(EAGAIN))
						{
							ret = 0;
						}
						if (again) continue;
					}
				}
				break;
			}
			return ret;
		}
	}
	finally
	{
		got_packet_ptr = (i_got_packet_ptr != 0);
	}
}

FFmpeg::AVRESULT FFmpeg::AVCodecContext::EncodeVideo(AVPacket^ avpkt,AVFrame^ frame,bool % got_packet_ptr)
{
	int i_got_packet_ptr = 0;
	try
	{
		LOAD_API(AVCodec,int,avcodec_encode_video2,::AVCodecContext*,::AVPacket *,const ::AVFrame *,int *);
		if (avcodec_encode_video2)
		{
			return avcodec_encode_video2((::AVCodecContext *)m_pPointer, (::AVPacket *)avpkt->_Pointer.ToPointer(), (const ::AVFrame *)(frame != nullptr ? frame->_Pointer.ToPointer() : nullptr), &i_got_packet_ptr);
		}
		else
		{
			::AVFrame* p = nullptr;
			if (frame != nullptr)
			{
				p = (::AVFrame*)frame->_Pointer.ToPointer();
			}
			bool again = false;
			int ret = 0;
			while (true)
			{
				ret = avcodec_send_frame((::AVCodecContext*)m_pPointer, p);
				if (!again)
				{
					again = (ret == AVERROR(EAGAIN));
					if (ret >= 0 || ret == AVERROR(EAGAIN))
					{
						ret = avcodec_receive_packet((::AVCodecContext*)m_pPointer, (::AVPacket*)avpkt->_Pointer.ToPointer());
						if (ret == 0)
						{
							i_got_packet_ptr = 1;
						}
						if (ret == AVERROR(EAGAIN))
						{
							ret = 0;
						}
						if (again) continue;
					}
				}
				break;
			}
			return ret;
		}
	}
	finally
	{
		got_packet_ptr = (i_got_packet_ptr != 0);
	}
}

FFmpeg::AVRESULT FFmpeg::AVCodecContext::EncodeSubtitle(IntPtr buf,int buf_size,AVSubtitle^ sub)
{
	return avcodec_encode_subtitle((::AVCodecContext *)m_pPointer,(uint8_t *)buf.ToPointer(),buf_size,(const ::AVSubtitle *)(sub != nullptr ? sub->_Pointer.ToPointer() : nullptr));
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVCodecContext::SendPacket(AVPacket^ avpkt)
{
	return avcodec_send_packet((::AVCodecContext*)m_pPointer,avpkt != nullptr ? (::AVPacket*)avpkt->_Pointer.ToPointer() : nullptr);
}
FFmpeg::AVRESULT FFmpeg::AVCodecContext::ReceiveFrame(AVFrame^ frame)
{
	return avcodec_receive_frame((::AVCodecContext*)m_pPointer,(::AVFrame*)frame->_Pointer.ToPointer());
}
FFmpeg::AVRESULT FFmpeg::AVCodecContext::SendFrame(AVFrame^ frame)
{
	return avcodec_send_frame((::AVCodecContext*)m_pPointer,frame != nullptr ? (::AVFrame*)frame->_Pointer.ToPointer() : nullptr);
}
FFmpeg::AVRESULT FFmpeg::AVCodecContext::ReceivePacket(AVPacket^ avpkt)
{
	return avcodec_receive_packet((::AVCodecContext*)m_pPointer,(::AVPacket*)avpkt->_Pointer.ToPointer());
}
//////////////////////////////////////////////////////
FFmpeg::AVPixelFormat FFmpeg::AVCodecContext::default_get_format(array<AVPixelFormat>^ fmt)
{
	AVPixelFormat _result = AVPixelFormat::NONE;
	int count = (fmt != nullptr ? fmt->Length : 0) + 1;
	::AVPixelFormat * p = (::AVPixelFormat *)av_malloc(count * sizeof(::AVPixelFormat));
	if (p)
	{
		try
		{
			for (int i = 0; i < count - 1; i++)
			{
				p[i] = (::AVPixelFormat)fmt[i];
			}
			p[count - 1] = AV_PIX_FMT_NONE;
			_result = (AVPixelFormat)avcodec_default_get_format((::AVCodecContext*)m_pPointer,p);
		}
		finally
		{
			av_free(p);
		}
	}
	return _result;
}

FFmpeg::AVRESULT FFmpeg::AVCodecContext::default_get_buffer2(AVFrame^ frame, int flags)
{
	return avcodec_default_get_buffer2(((::AVCodecContext*)m_pPointer),frame != nullptr ? (::AVFrame*)frame->_Pointer.ToPointer() : nullptr,flags);
}
//////////////////////////////////////////////////////
void FFmpeg::AVCodecContext::DrawHorizBand(AVFrame^ src, array<int>^ offset, int y, int type, int height)
{
	DrawHorizBandHandler^ _handler;
	Threading::Monitor::Enter(this);
	_handler = m_pDrawHorizBand;
	Threading::Monitor::Exit(this);
	if (_handler)
	{
		_handler(this,src,offset, y, type, height);
	}
}

FFmpeg::AVPixelFormat FFmpeg::AVCodecContext::GetFormat(array<AVPixelFormat>^ fmt)
{
	GetFormatHandler^ _handler;
	Threading::Monitor::Enter(this);
	_handler = m_pGetFormat;
	Threading::Monitor::Exit(this);
	if (_handler)
	{
		return _handler(this,fmt);
	}
	return AV_PIX_FMT_NONE;
}

FFmpeg::AVRESULT FFmpeg::AVCodecContext::GetBuffer2(AVFrame^ frame, int flags)
{
	GetBuffer2Handler^ _handler;
	Threading::Monitor::Enter(this);
	_handler = m_pGetBuffer2;
	Threading::Monitor::Exit(this);
	if (_handler)
	{
		return _handler(this,frame,flags);
	}
	return AVERROR(ENOSYS);
}
//////////////////////////////////////////////////////
// ICloneable
Object^ FFmpeg::AVCodecContext::Clone()
{
	AVCodecContext^ dest = gcnew AVCodecContext(nullptr);
	CopyTo(dest);
	return dest;
}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVCodecContext::GetClass()
{
	return gcnew AVClass((void*)avcodec_get_class(),nullptr);
}
//////////////////////////////////////////////////////
// AVCodecParser::AVCodecParsers
//////////////////////////////////////////////////////
FFmpeg::AVCodecParser::AVCodecParsers::AVCodecParsers()
{
	LibAVCodec::RegisterAll();
}
FFmpeg::AVCodecParser^ FFmpeg::AVCodecParser::AVCodecParsers::default::get(int index)
{
	if (index >= 0)
	{
		LOAD_API(AVCodec,::AVCodecParser *,av_parser_next,const ::AVCodecParser*);
		LOAD_API(AVCodec,::AVCodecParser *,av_parser_iterate,void **);
		if (av_parser_iterate)
		{
			void * opaque = nullptr;
			while (true)
			{
				auto p = av_parser_iterate(&opaque);
				if (!p) break;
				if (index-- == 0)
				{
					return gcnew AVCodecParser((void*)p, nullptr);
				}
			}
		} else
			if (av_parser_next)
			{
				::AVCodecParser * p = nullptr;
				while (true)
				{
					p = av_parser_next(p);
					if (!p) break;
					if (index-- == 0)
					{
						return gcnew AVCodecParser((void*)p, nullptr);
					}
				}
			}
	}
	return nullptr;
}
int FFmpeg::AVCodecParser::AVCodecParsers::Count::get()
{
	int count = 0;
	LOAD_API(AVCodec,::AVCodecParser *,av_parser_next,const ::AVCodecParser*);
	LOAD_API(AVCodec,::AVCodecParser *,av_parser_iterate,void **);

	if (count == 0 && av_parser_iterate)
	{
		void * opaque = nullptr;
		while (true)
		{
			auto p = av_parser_iterate(&opaque);
			if (!p) break;
			count++;
		}
	}
	if (count == 0 && av_parser_next)
	{
		::AVCodecParser * p = nullptr;
		while (true)
		{
			p = av_parser_next(p);
			if (!p) break;
			count++;
		}
	}
	return count;
}
//////////////////////////////////////////////////////
// AVCodecParser::Parsers::Enumerator
//////////////////////////////////////////////////////
FFmpeg::AVCodecParser::AVCodecParsers::Enumerator::Enumerator()
	: m_pOpaque(IntPtr::Zero)
{
	LibAVCodec::RegisterAll();
}
FFmpeg::AVCodecParser::AVCodecParsers::Enumerator::~Enumerator()
{
}
//////////////////////////////////////////////////////
// IEnumerator
bool FFmpeg::AVCodecParser::AVCodecParsers::Enumerator::MoveNext()
{
	const ::AVCodecParser * p = nullptr;
	void * opaque = m_pOpaque.ToPointer();
	LOAD_API(AVCodec,::AVCodecParser *,av_parser_next,const ::AVCodecParser*);
	LOAD_API(AVCodec,::AVCodecParser *,av_parser_iterate,void **);
	if (av_parser_iterate)
	{
		p = av_parser_iterate(&opaque);
	}
	else
	{
		if (av_parser_next)
		{
			p = av_parser_next((const ::AVCodecParser*)opaque);
			opaque = (void*)p;
		}
	}
	m_pOpaque = IntPtr(opaque);
	m_pCurrent = (p != nullptr) ? gcnew AVCodecParser((void*)p, nullptr) : nullptr;
	return (m_pCurrent != nullptr);
}

FFmpeg::AVCodecParser^ FFmpeg::AVCodecParser::AVCodecParsers::Enumerator::Current::get()
{
	if (m_pOpaque == IntPtr::Zero)
	{
		MoveNext();
	}
	return m_pCurrent;
}

void FFmpeg::AVCodecParser::AVCodecParsers::Enumerator::Reset()
{
	m_pOpaque = IntPtr::Zero;
}

Object^ FFmpeg::AVCodecParser::AVCodecParsers::Enumerator::CurrentObject::get()
{
	return Current::get();
}

//////////////////////////////////////////////////////
// AVCodecParser
//////////////////////////////////////////////////////
FFmpeg::AVCodecParser::AVCodecParser(void * _pointer, AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
array<FFmpeg::AVCodecID>^ FFmpeg::AVCodecParser::codec_ids::get()
{
	List<AVCodecID>^ _array = nullptr;
	const int * _pointer = ((::AVCodecParser*)m_pPointer)->codec_ids;
	if (_pointer)
	{
		int idx = 0;
		_array =  gcnew List<AVCodecID>();
		while (idx++ < 5 && *_pointer > 0)
		{
			_array->Add((AVCodecID)*_pointer++);
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}
//////////////////////////////////////////////////////
FFmpeg::AVCodecParser^ FFmpeg::AVCodecParser::Next(AVCodecParser^ _current)
{
	LibAVCodec::RegisterAll();
	::AVCodecParser * p = nullptr;
	LOAD_API(AVCodec,::AVCodecParser *,av_parser_next,const ::AVCodecParser*);
	LOAD_API(AVCodec,::AVCodecParser *,av_parser_iterate,void **);
	if (av_parser_next)
	{
		p = av_parser_next(_current != nullptr ? (::AVCodecParser*)_current->_Pointer.ToPointer() : nullptr);
	}
	else
	{
		if (av_parser_iterate)
		{
			void * opaque = nullptr;
			bool bMoveNext = (_current != nullptr);
			do
			{
				p = av_parser_iterate(&opaque);
				if (p && bMoveNext)
				{
					bMoveNext = (p != _current->_Pointer.ToPointer());
					continue;
				}
				break;
			}
			while (true);
		}
	}
	return p != nullptr ? gcnew AVCodecParser((void*)p, nullptr) : nullptr;
}

void FFmpeg::AVCodecParser::Register(AVCodecParser^ _parser)
{
	LibAVCodec::RegisterAll();
	LOAD_API(AVCodec,void,av_register_codec_parser,::AVCodecParser *);
	if (av_register_codec_parser)
	{
		av_register_codec_parser((::AVCodecParser*)_parser->_Pointer.ToPointer());
	}
}
//////////////////////////////////////////////////////
// AVCodecParserContext
//////////////////////////////////////////////////////
FFmpeg::AVCodecParserContext::AVCodecParserContext(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
FFmpeg::AVCodecParserContext::AVCodecParserContext(AVCodecID  codec_id)
	: AVBase(nullptr,nullptr)
{
	m_pPointer = av_parser_init((int)codec_id);
	if (m_pPointer)
	{
		m_pFree = (TFreeFN*)av_parser_close;
	}
}
//////////////////////////////////////////////////////
IntPtr FFmpeg::AVCodecParserContext::priv_data::get()
{
	return (IntPtr)((::AVCodecParserContext*)m_pPointer)->priv_data;
}
FFmpeg::AVCodecParser^ FFmpeg::AVCodecParserContext::parser::get()
{
	return _CreateObject<AVCodecParser>((void*)((::AVCodecParserContext*)m_pPointer)->parser);
}
Int64 FFmpeg::AVCodecParserContext::frame_offset::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->frame_offset;
}
Int64 FFmpeg::AVCodecParserContext::cur_offset::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->cur_offset;
}
Int64 FFmpeg::AVCodecParserContext::next_frame_offset::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->next_frame_offset;
}
FFmpeg::AVPictureType FFmpeg::AVCodecParserContext::pict_type::get()
{
	return (AVPictureType)((::AVCodecParserContext*)m_pPointer)->pict_type;
}
int FFmpeg::AVCodecParserContext::repeat_pict::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->repeat_pict;
}
Int64 FFmpeg::AVCodecParserContext::pts::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->pts;
}
Int64 FFmpeg::AVCodecParserContext::dts::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->dts;
}
Int64 FFmpeg::AVCodecParserContext::last_pts::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->last_pts;
}
Int64 FFmpeg::AVCodecParserContext::last_dts::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->last_dts;
}
int FFmpeg::AVCodecParserContext::fetch_timestamp::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->fetch_timestamp;
}
int FFmpeg::AVCodecParserContext::cur_frame_start_index::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->cur_frame_start_index;
}
array<Int64>^ FFmpeg::AVCodecParserContext::cur_frame_offset::get()
{
	List<Int64>^ _list = gcnew List<Int64>();
	int64_t * p = ((::AVCodecParserContext*)m_pPointer)->cur_frame_offset;
	for (int i = 0; i < AV_PARSER_PTS_NB; i++) _list->Add(p[i]);
	return _list->ToArray();
}
array<Int64>^ FFmpeg::AVCodecParserContext::cur_frame_pts::get()
{
	List<Int64>^ _list = gcnew List<Int64>();
	int64_t * p = ((::AVCodecParserContext*)m_pPointer)->cur_frame_pts;
	for (int i = 0; i < AV_PARSER_PTS_NB; i++) _list->Add(p[i]);
	return _list->ToArray();
}
array<Int64>^ FFmpeg::AVCodecParserContext::cur_frame_dts::get()
{
	List<Int64>^ _list = gcnew List<Int64>();
	int64_t * p = ((::AVCodecParserContext*)m_pPointer)->cur_frame_dts;
	for (int i = 0; i < AV_PARSER_PTS_NB; i++) _list->Add(p[i]);
	return _list->ToArray();
}
FFmpeg::AVParserFlags FFmpeg::AVCodecParserContext::flags::get()
{
	return (AVParserFlags)((::AVCodecParserContext*)m_pPointer)->flags;
}
Int64 FFmpeg::AVCodecParserContext::offset::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->offset;
}
array<Int64>^ FFmpeg::AVCodecParserContext::cur_frame_end::get()
{
	List<Int64>^ _list = gcnew List<Int64>();
	int64_t * p = ((::AVCodecParserContext*)m_pPointer)->cur_frame_end;
	for (int i = 0; i < AV_PARSER_PTS_NB; i++) _list->Add(p[i]);
	return _list->ToArray();
}
int FFmpeg::AVCodecParserContext::key_frame::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->key_frame;
}
Int64 FFmpeg::AVCodecParserContext::convergence_duration::get()
{
#if FF_API_CONVERGENCE_DURATION
	return ((::AVCodecParserContext*)m_pPointer)->convergence_duration;
#else
	return ((::AVCodecParserContext*)m_pPointer)->duration;
#endif
}
int FFmpeg::AVCodecParserContext::dts_sync_point::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->dts_sync_point;
}
int FFmpeg::AVCodecParserContext::dts_ref_dts_delta::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->dts_ref_dts_delta;
}
int FFmpeg::AVCodecParserContext::pts_dts_delta::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->pts_dts_delta;
}
array<Int64>^ FFmpeg::AVCodecParserContext::cur_frame_pos::get()
{
	List<Int64>^ _list = gcnew List<Int64>();
	int64_t * p = ((::AVCodecParserContext*)m_pPointer)->cur_frame_pos;
	for (int i = 0; i < AV_PARSER_PTS_NB; i++) _list->Add(p[i]);
	return _list->ToArray();
}
Int64 FFmpeg::AVCodecParserContext::pos::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->pos;
}
Int64 FFmpeg::AVCodecParserContext::last_pos::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->last_pos;
}
int FFmpeg::AVCodecParserContext::duration::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->duration;
}
FFmpeg::AVFieldOrder FFmpeg::AVCodecParserContext::field_order::get()
{
	return (AVFieldOrder)((::AVCodecParserContext*)m_pPointer)->field_order;
}
FFmpeg::AVPictureStructure FFmpeg::AVCodecParserContext::picture_structure::get()
{
	return (AVPictureStructure)((::AVCodecParserContext*)m_pPointer)->picture_structure;
}
int FFmpeg::AVCodecParserContext::output_picture_number::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->output_picture_number;
}
int FFmpeg::AVCodecParserContext::width::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->width;
}
int FFmpeg::AVCodecParserContext::height::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->height;
}
int FFmpeg::AVCodecParserContext::coded_width::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->coded_width;
}
int FFmpeg::AVCodecParserContext::coded_height::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->coded_height;
}
int FFmpeg::AVCodecParserContext::format::get()
{
	return ((::AVCodecParserContext*)m_pPointer)->format;
}
//////////////////////////////////////////////////////
int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
                     IntPtr % poutbuf, int % poutbuf_size,
                     IntPtr buf, int buf_size,
                     Int64 pts, Int64 dts,
                     Int64 pos)
{
	uint8_t * out_buf = poutbuf != IntPtr::Zero ? (uint8_t *)poutbuf.ToPointer() : nullptr;
	int outbuf_size = poutbuf_size;
	try
	{
		return av_parser_parse2((::AVCodecParserContext*)m_pPointer,(::AVCodecContext*)avctx->_Pointer.ToPointer(),
					poutbuf != IntPtr::Zero ? &out_buf : nullptr,&outbuf_size,(const uint8_t *)buf.ToPointer(),buf_size,pts, dts,pos);
	}
	finally
	{
		poutbuf_size = outbuf_size;
	}
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
		IntPtr % poutbuf, int % poutbuf_size,
		array<byte>^ buf,
		Int64 pts, Int64 dts,
		Int64 pos)
{
	pin_ptr<byte> _data = &buf[0];
	uint8_t * pData = (uint8_t *)_data;
	return Parse(avctx,poutbuf,poutbuf_size,(IntPtr)pData,buf->Length,pts,dts,pos);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	AVPacket^ pkt,
	array<byte>^ buf,
	Int64 pts, Int64 dts,
	Int64 pos)
{
	pin_ptr<byte> _data = &buf[0];
	uint8_t * pData = (uint8_t *)_data;

	return Parse(avctx,pkt,(IntPtr)pData,buf->Length,pts,dts,pos);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	AVPacket^ pkt,
	IntPtr buf, int buf_size,
	Int64 pts, Int64 dts,
	Int64 pos)
{
	return av_parser_parse2((::AVCodecParserContext*)m_pPointer,
		(::AVCodecContext*)avctx->_Pointer.ToPointer(),
		pkt != nullptr ? &((::AVPacket*)pkt->_Pointer.ToPointer())->data : nullptr,
		pkt != nullptr ? &((::AVPacket*)pkt->_Pointer.ToPointer())->size : nullptr,
		(const uint8_t *)buf.ToPointer(),
		buf_size,pts,dts,pos);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	IntPtr % poutbuf, int % poutbuf_size,
	IntPtr buf, int buf_size,
	Int64 pts, Int64 dts)
{
	return Parse(avctx,poutbuf,poutbuf_size,buf, buf_size,pts, dts,0);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	IntPtr % poutbuf, int % poutbuf_size,
	array<byte>^ buf,
	Int64 pts, Int64 dts)
{
	return Parse(avctx,poutbuf,poutbuf_size,buf,pts, dts,0);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	AVPacket^ pkt,
	array<byte>^ buf,
	Int64 pts, Int64 dts)
{
	return Parse(avctx,pkt,buf,pts, dts,0);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	AVPacket^ pkt,
	IntPtr buf, int buf_size,
	Int64 pts, Int64 dts)
{
	return Parse(avctx,pkt,buf,buf_size,pts, dts,0);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	IntPtr % poutbuf, int % poutbuf_size,
	IntPtr buf, int buf_size,
	Int64 pos)
{
	return Parse(avctx,poutbuf,poutbuf_size,buf, buf_size,AV_NOPTS_VALUE, AV_NOPTS_VALUE,pos);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	IntPtr % poutbuf, int % poutbuf_size,
	array<byte>^ buf,
	Int64 pos)
{
	return Parse(avctx,poutbuf,poutbuf_size,buf, AV_NOPTS_VALUE, AV_NOPTS_VALUE,pos);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	AVPacket^ pkt,
	array<byte>^ buf,
	Int64 pos)
{
	return Parse(avctx,pkt,buf, AV_NOPTS_VALUE, AV_NOPTS_VALUE,pos);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	AVPacket^ pkt,
	IntPtr buf, int buf_size,
	Int64 pos)
{
	return Parse(avctx,pkt,buf,buf_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE,pos);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	IntPtr % poutbuf, int % poutbuf_size,
	IntPtr buf, int buf_size)
{
	return Parse(avctx,poutbuf,poutbuf_size,buf,buf_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE,0);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	IntPtr % poutbuf, int % poutbuf_size,
	array<byte>^ buf)
{
	return Parse(avctx,poutbuf,poutbuf_size,buf,AV_NOPTS_VALUE, AV_NOPTS_VALUE,0);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	AVPacket^ pkt,
	array<byte>^ buf)
{
	return Parse(avctx,pkt,buf,AV_NOPTS_VALUE, AV_NOPTS_VALUE,0);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,
	AVPacket^ pkt,
	IntPtr buf, int buf_size)
{
	return Parse(avctx,pkt,buf,buf_size,AV_NOPTS_VALUE, AV_NOPTS_VALUE,0);
}

int FFmpeg::AVCodecParserContext::Parse(AVCodecContext^ avctx,AVPacket^ out_pkt,AVPacket^ in_pkt)
{
	__int64 pts = AV_NOPTS_VALUE;
	__int64 dts = AV_NOPTS_VALUE;
	uint8_t * buf = nullptr;
	int buf_size = 0;
	if (in_pkt != nullptr)
	{
		::AVPacket * p = (::AVPacket *)in_pkt->_Pointer.ToPointer();
		if (p)
		{
			pts = p->pts;
			dts = p->dts;
			buf = p->data;
			buf_size = p->size;
		}
	}
	return Parse(avctx,out_pkt,(IntPtr)buf,buf_size,pts,dts,pos);
}

int FFmpeg::AVCodecParserContext::Change(AVCodecContext^ avctx,
                     IntPtr % poutbuf, int % poutbuf_size,
                     IntPtr buf, int buf_size, int keyframe)
{
	uint8_t * out_buf = poutbuf != IntPtr::Zero ? (uint8_t *)poutbuf.ToPointer() : nullptr;
	int outbuf_size = poutbuf_size;
	try
	{
		LOAD_API(AVCodec,int,av_parser_change,::AVCodecParserContext *,::AVCodecContext *,uint8_t **, int *,const uint8_t *, int , int );
		if (av_parser_change)
		{
			return av_parser_change((::AVCodecParserContext*)m_pPointer, (::AVCodecContext*)avctx->_Pointer.ToPointer(),
				poutbuf != IntPtr::Zero ? &out_buf : nullptr, &outbuf_size, (const uint8_t *)buf.ToPointer(), buf_size, keyframe);
		}
		return AVRESULT(ENOSYS);
	}
	finally
	{
		poutbuf_size = outbuf_size;
	}
}
//////////////////////////////////////////////////////
// AVBitStreamFilter::AVBitStreamFilters
//////////////////////////////////////////////////////
FFmpeg::AVBitStreamFilter::AVBitStreamFilters::AVBitStreamFilters()
{
	LibAVCodec::RegisterAll();
}
FFmpeg::AVBitStreamFilter^ FFmpeg::AVBitStreamFilter::AVBitStreamFilters::default::get(int index)
{
	if (index >= 0)
	{
		LOAD_API(AVCodec,::AVBitStreamFilter *,av_bitstream_filter_next,const ::AVBitStreamFilter*);
		LOAD_API(AVCodec,::AVBitStreamFilter *,av_bsf_iterate,void **);
		if (av_bsf_iterate)
		{
			void * opaque = nullptr;
			while (true)
			{
				auto p = av_bsf_iterate(&opaque);
				if (!p) break;
				if (index-- == 0)
				{
					return gcnew AVBitStreamFilter((void*)p, nullptr);
				}
			}
		} else
			if (av_bitstream_filter_next)
			{
				::AVBitStreamFilter * p = nullptr;
				while (true)
				{
					p = av_bitstream_filter_next(p);
					if (!p) break;
					if (index-- == 0)
					{
						return gcnew AVBitStreamFilter((void*)p, nullptr);
					}
				}
			}
	}
	return nullptr;
}
int FFmpeg::AVBitStreamFilter::AVBitStreamFilters::Count::get()
{
	int count = 0;
	LOAD_API(AVCodec,::AVBitStreamFilter *,av_bitstream_filter_next,const ::AVBitStreamFilter*);
	LOAD_API(AVCodec,::AVBitStreamFilter *,av_bsf_iterate,void **);

	if (count == 0 && av_bsf_iterate)
	{
		void * opaque = nullptr;
		while (true)
		{
			auto p = av_bsf_iterate(&opaque);
			if (!p) break;
			count++;
		}
	}
	if (count == 0 && av_bitstream_filter_next)
	{
		::AVBitStreamFilter * p = nullptr;
		while (true)
		{
			p = av_bitstream_filter_next(p);
			if (!p) break;
			count++;
		}
	}
	return count;
}
//////////////////////////////////////////////////////
// AVBitStreamFilterEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVBitStreamFilter::AVBitStreamFilters::AVBitStreamFilterEnumerator::AVBitStreamFilterEnumerator()
	: m_pOpaque(IntPtr::Zero)
{
	LibAVCodec::RegisterAll();
}
FFmpeg::AVBitStreamFilter::AVBitStreamFilters::AVBitStreamFilterEnumerator::~AVBitStreamFilterEnumerator()
{
}
//////////////////////////////////////////////////////
// IEnumerator
bool FFmpeg::AVBitStreamFilter::AVBitStreamFilters::AVBitStreamFilterEnumerator::MoveNext()
{
	const ::AVBitStreamFilter * p = nullptr;
	void * opaque = m_pOpaque.ToPointer();
	LOAD_API(AVCodec,::AVBitStreamFilter *,av_bitstream_filter_next,const ::AVBitStreamFilter*);
	LOAD_API(AVCodec,::AVBitStreamFilter *,av_bsf_iterate,void **);
	if (av_bsf_iterate)
	{
		p = av_bsf_iterate(&opaque);
	}
	else
	{
		if (av_bitstream_filter_next)
		{
			p = av_bitstream_filter_next((const ::AVBitStreamFilter*)opaque);
			opaque = (void*)p;
		}
	}
	m_pOpaque = IntPtr(opaque);
	m_pCurrent = (p != nullptr) ? gcnew AVBitStreamFilter((void*)p, nullptr) : nullptr;
	return (m_pCurrent != nullptr);
}

FFmpeg::AVBitStreamFilter^ FFmpeg::AVBitStreamFilter::AVBitStreamFilters::AVBitStreamFilterEnumerator::Current::get()
{
	if (m_pOpaque == IntPtr::Zero)
	{
		MoveNext();
	}
	return m_pCurrent;
}

void FFmpeg::AVBitStreamFilter::AVBitStreamFilters::AVBitStreamFilterEnumerator::Reset()
{
	m_pOpaque = IntPtr::Zero;
}

Object^ FFmpeg::AVBitStreamFilter::AVBitStreamFilters::AVBitStreamFilterEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVBitStreamFilter
//////////////////////////////////////////////////////
FFmpeg::AVBitStreamFilter::AVBitStreamFilter(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
String^ FFmpeg::AVBitStreamFilter::name::get()
{
	auto p = ((::AVBitStreamFilter*)m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}

array<FFmpeg::AVCodecID>^ FFmpeg::AVBitStreamFilter::codec_ids::get()
{
	auto p = ((::AVBitStreamFilter*)m_pPointer)->codec_ids;
	if (p == nullptr) return nullptr;
	List<AVCodecID>^ list = gcnew List<AVCodecID>();
	try
	{
		while (*p != AV_CODEC_ID_NONE)
		{
			list->Add((AVCodecID)*p++);
		}
		return list->ToArray();
	}
	finally
	{
		delete list;
	}
}

FFmpeg::AVClass^ FFmpeg::AVBitStreamFilter::priv_class::get()
{
	return _CreateObject<AVClass>((void*)((::AVBitStreamFilter*)m_pPointer)->priv_class);
}

String^ FFmpeg::AVBitStreamFilter::ToString()
{
	if (m_pPointer)
	{
		String^ _name = name::get();
		if (!String::IsNullOrEmpty(_name))
		{
			return "[ AVBitStreamFilter ] \"" + _name +"\"";
		}
	}
	return Object::ToString();
}

FFmpeg::AVBitStreamFilter::AVBitStreamFilters^ FFmpeg::AVBitStreamFilter::Filters::get()
{
	return gcnew AVBitStreamFilters();
}
//////////////////////////////////////////////////////
void FFmpeg::AVBitStreamFilter::Register(AVBitStreamFilter^ bsf)
{
	LOAD_API(AVCodec,void,av_register_bitstream_filter,const ::AVBitStreamFilter*);
	if (av_register_bitstream_filter)
	{
		av_register_bitstream_filter(bsf != nullptr ? (::AVBitStreamFilter*)bsf->_Pointer.ToPointer() : nullptr);
	}
}
FFmpeg::AVBitStreamFilter^ FFmpeg::AVBitStreamFilter::Next(AVBitStreamFilter^ f)
{
	::AVBitStreamFilter * p = nullptr;
	LOAD_API(AVCodec,::AVBitStreamFilter*,av_bitstream_filter_next,const ::AVBitStreamFilter*);
	LOAD_API(AVCodec,::AVBitStreamFilter *,av_bsf_iterate,void **);
	if (av_bitstream_filter_next)
	{
		p = av_bitstream_filter_next(f != nullptr ? (::AVBitStreamFilter*)f->_Pointer.ToPointer() : nullptr);
	}
	else
	{
		if (av_bsf_iterate)
		{
			void * opaque = nullptr;
			bool bMoveNext = (f != nullptr);
			do
			{
				p = av_bsf_iterate(&opaque);
				if (p && bMoveNext)
				{
					bMoveNext = (p != f->_Pointer.ToPointer());
					continue;
				}
				break;
			}
			while (true);
		}
	}
	return p != nullptr ? gcnew AVBitStreamFilter((void*)p, nullptr) : nullptr;
}
FFmpeg::AVBitStreamFilter^ FFmpeg::AVBitStreamFilter::GetByName(String^ value)
{
	if (String::IsNullOrEmpty(value)) return nullptr;
	LOAD_API(AVCodec,const ::AVBitStreamFilter*,av_bsf_get_by_name,const char*);
	if (av_bsf_get_by_name)
	{
		char * _name = nullptr;
		try
		{
			_name = (char *)AllocString(value).ToPointer();
			auto p = av_bsf_get_by_name(_name);
			return p != nullptr ? gcnew AVBitStreamFilter((void*)p, nullptr) : nullptr;
		}
		finally
		{
			FreeMemory((IntPtr)_name);
		}
	}
	else
	{
		auto filters = gcnew AVBitStreamFilters();
		try
		{
			for (int i = 0; i < filters->Count; i++)
			{
				if (filters[i]->name == value)
				{
					return filters[i];
				}
			}
		}
		finally
		{
			delete filters;
		}
		return nullptr;
	}
}
//////////////////////////////////////////////////////
// AVBitStreamFilterContext
//////////////////////////////////////////////////////
FFmpeg::AVBitStreamFilterContext::AVBitStreamFilterContext(void * _pointer, AVBase^ _parent)
	: AVBase(_pointer, _parent)
{
}

FFmpeg::AVBitStreamFilterContext::AVBitStreamFilterContext(String^ name)
	:AVBase(nullptr,nullptr)
{
	LibAVCodec::RegisterAll();
	char * _name = nullptr;
	try
	{
		LOAD_API(AVCodec,::AVBitStreamFilter*,av_bitstream_filter_init,const char*);
		LOAD_API(AVCodec,void,av_bitstream_filter_close,::AVBitStreamFilterContext *);
		if (av_bitstream_filter_init && av_bitstream_filter_close)
		{
			_name = (char *)AllocString(name).ToPointer();
			m_pPointer = av_bitstream_filter_init(_name);
			if (m_pPointer)
			{
				m_pFree = (TFreeFN*)av_bitstream_filter_close;
			}
		}
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}
//////////////////////////////////////////////////////
IntPtr FFmpeg::AVBitStreamFilterContext::priv_data::get()
{
	return (IntPtr)((::AVBitStreamFilterContext*)m_pPointer)->priv_data;
}
FFmpeg::AVBitStreamFilter^ FFmpeg::AVBitStreamFilterContext::filter::get()
{
	return _CreateObject<AVBitStreamFilter>((void*)((::AVBitStreamFilterContext*)m_pPointer)->filter);
}
FFmpeg::AVCodecParserContext^ FFmpeg::AVBitStreamFilterContext::parser::get()
{
	return _CreateObject<AVCodecParserContext>(((::AVBitStreamFilterContext*)m_pPointer)->parser);
}
FFmpeg::AVBitStreamFilterContext^ FFmpeg::AVBitStreamFilterContext::next::get()
{
	return _CreateObject<AVBitStreamFilterContext>(((::AVBitStreamFilterContext*)m_pPointer)->next);
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVBitStreamFilterContext::Filter(AVCodecContext^ avctx, String^ args,
                               IntPtr % poutbuf, int % poutbuf_size,
                               IntPtr buf, int buf_size, int keyframe)
{
	uint8_t * out_buf = poutbuf != IntPtr::Zero ? (uint8_t *)poutbuf.ToPointer() : nullptr;
	int outbuf_size = poutbuf_size;
	char * _args = nullptr;
	try
	{
		LOAD_API(AVCodec,int,av_bitstream_filter_filter,::AVBitStreamFilterContext *,
			::AVCodecContext *, const char *,uint8_t **, int *,const uint8_t *, int , int );
		if (av_bitstream_filter_filter)
		{
			_args = (char *)AllocString(args).ToPointer();
			return av_bitstream_filter_filter((::AVBitStreamFilterContext*)m_pPointer, (::AVCodecContext*)avctx->_Pointer.ToPointer(),
				_args, poutbuf != IntPtr::Zero ? &out_buf : nullptr, &outbuf_size, (const uint8_t *)buf.ToPointer(), buf_size, keyframe);
		}
		return AVRESULT(ENOSYS);
	}
	finally
	{
		poutbuf_size = outbuf_size;
		FreeMemory((IntPtr)_args);
	}
}
//////////////////////////////////////////////////////
// AVBSFContext
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVBSFContext::Class::get()
{
	DYNAMIC_DEF_API(AVCodec,const ::AVClass *,nullptr,av_bsf_get_class);
	auto p = av_bsf_get_class();
	return p != nullptr ? gcnew AVClass((void*)p, nullptr) : nullptr;
}
//////////////////////////////////////////////////////
FFmpeg::AVBSFContext::AVBSFContext(void * _pointer, AVBase^ _parent)
	: AVBase(_pointer, _parent)
{

}
//////////////////////////////////////////////////////
FFmpeg::AVBSFContext::AVBSFContext(AVBitStreamFilter^ filter)
	: AVBase(nullptr, nullptr)
{
	LOAD_API(AVCodec,void,av_bsf_free,::AVBSFContext**);
	LOAD_API(AVCodec,int,av_bsf_alloc,::AVBitStreamFilter*,::AVBSFContext**);
	if (av_bsf_alloc)
	{
		::AVBSFContext * p = nullptr;
		if (0 <= av_bsf_alloc((::AVBitStreamFilter*)(filter->_Pointer.ToPointer()), &p))
		{
			m_pPointer = p;
			m_pFreep = (TFreeFNP*)av_bsf_free;
		}
	}
}
FFmpeg::AVBSFContext::AVBSFContext(AVBSFList^ lst)
	: AVBase(nullptr, nullptr)
{
	LOAD_API(AVCodec,void,av_bsf_free,::AVBSFContext**);
	LOAD_API(AVCodec,int,av_bsf_list_finalize,::AVBSFList**,::AVBSFContext**);
	if (av_bsf_list_finalize)
	{
		::AVBSFContext * p = nullptr;
		::AVBSFList* l = (::AVBSFList*)lst->_Pointer.ToPointer();
		auto result = av_bsf_list_finalize(&l, &p);
		if (p)
		{
			lst->_Pointer = IntPtr::Zero;
			m_pPointer = p;
			m_pFreep = (TFreeFNP*)av_bsf_free;
		}
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVBSFContext::av_class::get()
{ 
	return _CreateObject<AVClass>((void*)((::AVBSFContext *)m_pPointer)->av_class);
}
FFmpeg::AVBitStreamFilter^ FFmpeg::AVBSFContext::filter::get()
{
	return _CreateObject<AVBitStreamFilter>((void*)((::AVBSFContext *)m_pPointer)->filter);
}
IntPtr FFmpeg::AVBSFContext::priv_data::get()
{ 
	return (IntPtr)((::AVBSFContext *)m_pPointer)->priv_data;
}
FFmpeg::AVCodecParameters^ FFmpeg::AVBSFContext::par_in::get()
{ 
	return _CreateObject<AVCodecParameters>(((::AVBSFContext *)m_pPointer)->par_in);
}
FFmpeg::AVCodecParameters^ FFmpeg::AVBSFContext::par_out::get()
{ 
	return _CreateObject<AVCodecParameters>(((::AVBSFContext *)m_pPointer)->par_out);
}
FFmpeg::AVRational^ FFmpeg::AVBSFContext::time_base_in::get()
{ 
	return _CreateObject<AVRational>(&((::AVBSFContext *)m_pPointer)->time_base_in);
}
void FFmpeg::AVBSFContext::time_base_in::set(AVRational^ value)
{
	if (value != nullptr)
	{
		((::AVBSFContext *)m_pPointer)->time_base_in.num = value->num;
		((::AVBSFContext *)m_pPointer)->time_base_in.den = value->den;
	}
}

FFmpeg::AVRational^ FFmpeg::AVBSFContext::time_base_out::get()
{
	return _CreateObject<AVRational>(&((::AVBSFContext *)m_pPointer)->time_base_out);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVBSFContext::ToString()
{
	if (m_pPointer && ((::AVBSFContext *)m_pPointer)->filter)
	{
		String^ _name = gcnew String(((::AVBSFContext *)m_pPointer)->filter->name);
		if (!String::IsNullOrEmpty(_name))
		{
			return "[ AVBSFContext ] \"" + _name +"\"";
		}
	}
	return __super::ToString();
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVBSFContext::Init()
{
	DYNAMIC_DEF_API(AVCodec,int,AVERROR(ENOSYS),av_bsf_init,::AVBSFContext *);
	return av_bsf_init((::AVBSFContext*)m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::AVBSFContext::SendPacket(AVPacket^ pkt)
{
	DYNAMIC_DEF_API(AVCodec,int,AVERROR(ENOSYS),av_bsf_send_packet,::AVBSFContext *,::AVPacket*);
	return av_bsf_send_packet((::AVBSFContext*)m_pPointer,pkt != nullptr ? (::AVPacket*)pkt->_Pointer.ToPointer() : nullptr);
}

FFmpeg::AVRESULT FFmpeg::AVBSFContext::ReceivePacket(AVPacket^ pkt)
{
	DYNAMIC_DEF_API(AVCodec,int,AVERROR(ENOSYS),av_bsf_receive_packet,::AVBSFContext *,::AVPacket*);
	return av_bsf_receive_packet((::AVBSFContext*)m_pPointer,(::AVPacket*)pkt->_Pointer.ToPointer());
}

void FFmpeg::AVBSFContext::Flush()
{
	VOID_API(AVCodec,av_bsf_flush,::AVBSFContext*)
	av_bsf_flush((::AVBSFContext*)m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::AVBSFContext::Parse(String^ str, AVBSFContext^ % bsf)
{
	DYNAMIC_DEF_API(AVCodec,int,AVERROR(ENOSYS),av_bsf_list_parse_str,char*,::AVBSFContext **);
	LOAD_API(AVCodec,void,av_bsf_free,::AVBSFContext**);
	::AVBSFContext * p = nullptr;
	char * _str = (char *)AllocString(str).ToPointer();
	try
	{
		auto result = av_bsf_list_parse_str(_str,&p);
		if (p)
		{
			bsf = gcnew AVBSFContext(p,nullptr);
			bsf->m_pFreep = (TFreeFNP*)av_bsf_free;
		}
		return result;
	}
	finally
	{
		if (str) FreeMemory((IntPtr)_str);
	}
}

FFmpeg::AVRESULT FFmpeg::AVBSFContext::get_null_filter(AVBSFContext^ % bsf)
{
	DYNAMIC_DEF_API(AVCodec,int,AVERROR(ENOSYS),av_bsf_get_null_filter,::AVBSFContext **);
	LOAD_API(AVCodec,void,av_bsf_free,::AVBSFContext**);
	::AVBSFContext * p = nullptr;
	int result = av_bsf_get_null_filter(&p);
	if (p)
	{
		bsf = gcnew AVBSFContext(p,nullptr);
		bsf->m_pFreep = (TFreeFNP*)av_bsf_free;
	}
	return result;
}
//////////////////////////////////////////////////////
// AVBSFList
//////////////////////////////////////////////////////
FFmpeg::AVBSFList::AVBSFList()
	: AVBase(nullptr,nullptr)
{
	LOAD_API(AVCodec,::AVBSFList *,av_bsf_list_alloc);
	LOAD_API(AVCodec,void,av_bsf_list_free,::AVBSFList **);
	if (av_bsf_list_alloc)
	{
		m_pPointer = av_bsf_list_alloc();
		if (m_pPointer)
		{
			m_pFreep = (TFreeFNP*)av_bsf_list_free;
		}
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVBSFList::Append(AVBSFContext^ bsf)
{
	DYNAMIC_DEF_API(AVCodec,int,AVERROR(ENOSYS),av_bsf_list_append,::AVBSFList *, ::AVBSFContext *);
	return av_bsf_list_append(((::AVBSFList*)m_pPointer),(::AVBSFContext*)bsf->_Pointer.ToPointer());
}
FFmpeg::AVRESULT FFmpeg::AVBSFList::Append(String^ bsf_name)
{
	return Append(bsf_name,nullptr);
}
FFmpeg::AVRESULT FFmpeg::AVBSFList::Append(String^ bsf_name, AVDictionary^ options)
{
	DYNAMIC_DEF_API(AVCodec,int,AVERROR(ENOSYS),av_bsf_list_append2,::AVBSFList *,char *, ::AVDictionary **);
	AVRESULT _result;
	char * _bsf_name = (char *)AllocString(bsf_name).ToPointer();
	::AVDictionary * _dict = options != nullptr ? (::AVDictionary *)options->_Pointer.ToPointer() : nullptr;
	try
	{
		_result = av_bsf_list_append2((::AVBSFList*)m_pPointer,_bsf_name,&_dict);
		if (options)
		{
			options->ChangePointer(_dict);
			_dict = nullptr;
		}
	}
	finally
	{
		if (_dict) av_dict_free(&_dict);
		if (_bsf_name) FreeMemory((IntPtr)_bsf_name);
	}
	return _result;
}
//////////////////////////////////////////////////////