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
// libavformat 
// I/O and Muxing/Demuxing Library.
//////////////////////////////////////////////////////
#include "stdafx.h"
#include "AVFormat.h"
#include "AVDevice.h"
//////////////////////////////////////////////////////
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
//////////////////////////////////////////////////////
#pragma warning(disable: 4996) // marked as deprecated
//////////////////////////////////////////////////////
// LibAVFormat
//////////////////////////////////////////////////////
FFmpeg::LibAVFormat::LibAVFormat()
{
}
//////////////////////////////////////////////////////
UInt32 FFmpeg::LibAVFormat::Version::get()
{
	return avformat_version();
}
String^ FFmpeg::LibAVFormat::Configuration::get()
{
	auto p = avformat_configuration();
	return p != nullptr ? gcnew String(p) : nullptr;
}

String^ FFmpeg::LibAVFormat::License::get()
{
	auto p = avformat_license();
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
void FFmpeg::LibAVFormat::RegisterAll()
{
	AVBase::EnsureLibraryLoaded();
	if (m_bRegistered) return;
	m_bRegistered = true;
	VOID_API(AVFormat,av_register_all)
	av_register_all();
	LibAVDevice::RegisterAll();
}

void FFmpeg::LibAVFormat::RegisterInputFormat(AVInputFormat^ format)
{
	AVBase::EnsureLibraryLoaded();
	VOID_API(AVFormat,av_register_input_format,::AVInputFormat*)
	av_register_input_format((::AVInputFormat*)format->_Pointer.ToPointer());
}
void FFmpeg::LibAVFormat::RegisterOutputFormat(AVOutputFormat^ format)
{
	AVBase::EnsureLibraryLoaded();
	VOID_API(AVFormat,av_register_output_format,::AVOutputFormat*)
	av_register_output_format((::AVOutputFormat*)format->_Pointer.ToPointer());
}
//////////////////////////////////////////////////////
int FFmpeg::LibAVFormat::NetworkInit()
{
	return avformat_network_init();
}

int FFmpeg::LibAVFormat::NetworkDeinit()
{
	return avformat_network_deinit();
}
//////////////////////////////////////////////////////
// AVIOProtocolEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVIOProtocolEnum::AVIOProtocolEnumerator::AVIOProtocolEnumerator(bool _output)
	: m_pOpaque(nullptr)
	, m_bOutput(_output)
	, m_pCurrent(nullptr)
{
}
FFmpeg::AVIOProtocolEnum::AVIOProtocolEnumerator::~AVIOProtocolEnumerator()
{
}
//////////////////////////////////////////////////////
// IEnumerator
bool FFmpeg::AVIOProtocolEnum::AVIOProtocolEnumerator::MoveNext()
{
	void * opaque = m_pOpaque;
	m_pCurrent = (void*)avio_enum_protocols((void**)&opaque,m_bOutput ? 1 : 0);
	m_pOpaque = opaque;
	return m_pCurrent != nullptr;
}

String^ FFmpeg::AVIOProtocolEnum::AVIOProtocolEnumerator::Current::get()
{
	if (m_pCurrent == nullptr)
	{
		if (!MoveNext()) return nullptr;
	}
	return m_pCurrent != nullptr ? gcnew String((const char *)m_pCurrent) : nullptr;
}

void FFmpeg::AVIOProtocolEnum::AVIOProtocolEnumerator::Reset()
{
	m_pOpaque = nullptr;
	m_pCurrent = nullptr;
}

Object^ FFmpeg::AVIOProtocolEnum::AVIOProtocolEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVIOProtocolEnum
//////////////////////////////////////////////////////
FFmpeg::AVIOProtocolEnum::AVIOProtocolEnum(bool _output)
	: m_bOutput(_output)
{
	LibAVFormat::RegisterAll();
}
//////////////////////////////////////////////////////
// IEnumerable
System::Collections::IEnumerator^ FFmpeg::AVIOProtocolEnum::GetEnumerator()
{
	return gcnew AVIOProtocolEnumerator(m_bOutput);
}

System::Collections::Generic::IEnumerator<String^>^ FFmpeg::AVIOProtocolEnum::GetEnumeratorGeneric()
{
	return gcnew AVIOProtocolEnumerator(m_bOutput);
}
//////////////////////////////////////////////////////
// AVIOInterruptDesc
//////////////////////////////////////////////////////
FFmpeg::AVIOInterruptDesc::AVIOInterruptDesc(AVIOInterruptCB^ _callback,Object^ _opaque)
	: m_pCallback(_callback)
	, m_pOpaque(_opaque)
{
}
//////////////////////////////////////////////////////
FFmpeg::AVIOInterruptCB^ FFmpeg::AVIOInterruptDesc::callback::get()
{
	return m_pCallback;
}
Object^ FFmpeg::AVIOInterruptDesc::opaque::get()
{
	return m_pOpaque;
}
//////////////////////////////////////////////////////
// AVIOContext
//////////////////////////////////////////////////////
int AVIOContext_read_packet(void *opaque, uint8_t *buf, int buf_size)
{
	GCHandle _handle = GCHandle::FromIntPtr((IntPtr)opaque);
	if (_handle.IsAllocated)
	{
		return ((FFmpeg::AVIOContext^)_handle.Target)->ReadCB((IntPtr)buf,buf_size);
	}
	return -1;
}

int AVIOContext_write_packet(void *opaque, uint8_t *buf, int buf_size)
{
	GCHandle _handle = GCHandle::FromIntPtr((IntPtr)opaque);
	if (_handle.IsAllocated)
	{
		return ((FFmpeg::AVIOContext^)_handle.Target)->WriteCB((IntPtr)buf,buf_size);
	}
	return -1;
}

int64_t AVIOContext_seek(void *opaque, int64_t offset, int whence)
{
	GCHandle _handle = GCHandle::FromIntPtr((IntPtr)opaque);
	if (_handle.IsAllocated)
	{
		return ((FFmpeg::AVIOContext^)_handle.Target)->SeekCB(offset, whence);
	}
	return -1;
}

int Generic_interrupt_callback(void *opaque)
{
	GCHandle _handle = GCHandle::FromIntPtr((IntPtr)opaque);
	if (_handle.IsAllocated)
	{
		FFmpeg::AVIOInterruptDesc^ _desc = ((FFmpeg::AVIOInterruptDesc^)_handle.Target);
		if (_desc->callback)
		{
			if (_desc->callback(_desc->opaque))
			{
				return 1;
			}
		}
	}
	return 0;
}
//////////////////////////////////////////////////////
FFmpeg::AVIOContext::AVIOContext(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
	, m_pSeek(nullptr)
	, m_pRead(nullptr)
	, m_pWrite(nullptr)
	, m_pInterruptCB(nullptr)
	, m_pOpaque(nullptr)
	, m_pBuffer(nullptr)
{
}

FFmpeg::AVIOContext::AVIOContext(String^ _url, FFmpeg::AvioFlag _flags)
	: AVBase(nullptr,nullptr)
	, m_pSeek(nullptr)
	, m_pRead(nullptr)
	, m_pWrite(nullptr)
	, m_pInterruptCB(nullptr)
	, m_pOpaque(nullptr)
	, m_pBuffer(nullptr)
{
	AVDictionary^ _dictionary = gcnew AVDictionary();
	Open(_url, _flags, nullptr, _dictionary);
	delete _dictionary;
}

FFmpeg::AVIOContext::AVIOContext(String^ _url, FFmpeg::AvioFlag _flags, FFmpeg::AVIOInterruptCB^ int_cb,Object^ opaque)
	: AVBase(nullptr,nullptr)
	, m_pSeek(nullptr)
	, m_pRead(nullptr)
	, m_pWrite(nullptr)
	, m_pInterruptCB(nullptr)
	, m_pOpaque(nullptr)
	, m_pBuffer(nullptr)
{
	AVDictionary^ _dictionary = gcnew AVDictionary();
	Open(_url, _flags, int_cb != nullptr ? gcnew AVIOInterruptDesc(int_cb, opaque) : nullptr, _dictionary);
	delete _dictionary;
}

FFmpeg::AVIOContext::AVIOContext(String^ _url, FFmpeg::AvioFlag _flags, FFmpeg::AVIOInterruptCB^ int_cb,Object^ opaque,FFmpeg::AVDictionary^ options)
	: AVBase(nullptr,nullptr)
	, m_pSeek(nullptr)
	, m_pRead(nullptr)
	, m_pWrite(nullptr)
	, m_pInterruptCB(nullptr)
	, m_pOpaque(nullptr)
	, m_pBuffer(nullptr)
{
	Open(_url, _flags, int_cb != nullptr ? gcnew AVIOInterruptDesc(int_cb, opaque) : nullptr, options);
}

FFmpeg::AVIOContext::AVIOContext(AVMemPtr^ buf, int write_flag, Object^ opaque, AVIOPacketCB^ read_packet, AVIOPacketCB^ write_packet, AVIOSeekCB^ seek)
	: AVBase(nullptr,nullptr)
	, m_pOpaque(opaque)
	, m_pSeek(seek)
	, m_pRead(read_packet)
	, m_pWrite(write_packet)
	, m_pInterruptCB(nullptr)
	, m_pBuffer(buf)
{
	if (m_pBuffer) m_pBuffer->AddRef();
	IntPtr _this = GCHandle::ToIntPtr(GCHandle::Alloc(this,GCHandleType::Weak));
	m_pPointer = avio_alloc_context((unsigned char*)buf->m_pPointer,buf->size,write_flag,_this.ToPointer(),
		read_packet != nullptr ? AVIOContext_read_packet : nullptr,
		write_packet != nullptr ? AVIOContext_write_packet : nullptr,
		(seek != nullptr || write_packet != nullptr) ? AVIOContext_seek : nullptr);
	if (m_pPointer)
	{
		m_pFreep = (TFreeFNP*)avio_context_free;
	}
}
//////////////////////////////////////////////////////
void FFmpeg::AVIOContext::_FreeObjects()
{
	if (m_pBuffer) m_pBuffer->Release();
	m_pBuffer = nullptr;
	__super::_FreeObjects();
}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVIOContext::av_class::get()
{
	auto p = ((::AVIOContext*)m_pPointer)->av_class;
	return _CreateObject<FFmpeg::AVClass>((void*)p);
}
IntPtr FFmpeg::AVIOContext::buffer::get()
{
	return (IntPtr)((::AVIOContext*)m_pPointer)->buffer;
}
int FFmpeg::AVIOContext::buffer_size::get()
{
	return ((::AVIOContext*)m_pPointer)->buffer_size;
}
IntPtr FFmpeg::AVIOContext::buf_ptr::get()
{
	return (IntPtr)((::AVIOContext*)m_pPointer)->buf_ptr;
}
IntPtr FFmpeg::AVIOContext::buf_end::get()
{
	return (IntPtr)((::AVIOContext*)m_pPointer)->buf_end;
}
Int64 FFmpeg::AVIOContext::pos::get()
{
	return ((::AVIOContext*)m_pPointer)->pos;
}
bool FFmpeg::AVIOContext::must_flush::get()
{
	return true;//((::AVIOContext*)m_pPointer)->must_flush != 0;
}
bool FFmpeg::AVIOContext::eof_reached::get()
{
	return ((::AVIOContext*)m_pPointer)->eof_reached != 0;
}
void FFmpeg::AVIOContext::eof_reached::set(bool b)
{
	((::AVIOContext*)m_pPointer)->eof_reached = b ? 1 : 0;
}
bool FFmpeg::AVIOContext::write_flag::get()
{
	return ((::AVIOContext*)m_pPointer)->write_flag != 0;
}
int FFmpeg::AVIOContext::max_packet_size::get()
{
	return ((::AVIOContext*)m_pPointer)->buffer_size;
}
UInt32 FFmpeg::AVIOContext::checksum::get()
{
	return ((::AVIOContext*)m_pPointer)->checksum;
}
IntPtr FFmpeg::AVIOContext::checksum_ptr::get()
{
	return (IntPtr)((::AVIOContext*)m_pPointer)->checksum_ptr;
}
int FFmpeg::AVIOContext::error::get()
{
	return ((::AVIOContext*)m_pPointer)->error;
}
bool FFmpeg::AVIOContext::seekable::get()
{
	return ((::AVIOContext*)m_pPointer)->seekable != 0;
}
Int64 FFmpeg::AVIOContext::maxsize::get()
{
#if LIBAVFORMAT_VERSION_MAJOR * 100 + LIBAVFORMAT_VERSION_MINOR > 5904
	return 0;
#else
	return ((::AVIOContext*)m_pPointer)->maxsize;
#endif
}
bool FFmpeg::AVIOContext::direct::get()
{
	return ((::AVIOContext*)m_pPointer)->direct != 0;
}
Int64 FFmpeg::AVIOContext::bytes_read::get()
{
	return ((::AVIOContext*)m_pPointer)->bytes_read;
}

Int64 FFmpeg::AVIOContext::bytes_written::get()
{
#if FF_API_AVIOCONTEXT_WRITTEN
	return ((::AVIOContext*)m_pPointer)->written;
#else
	return ((::AVIOContext*)m_pPointer)->bytes_written;
#endif
}

int FFmpeg::AVIOContext::seek_count::get()
{
#if LIBAVFORMAT_VERSION_MAJOR * 100 + LIBAVFORMAT_VERSION_MINOR > 5904
	return 0;
#else
	return ((::AVIOContext*)m_pPointer)->seek_count;
#endif
}
int FFmpeg::AVIOContext::writeout_count::get()
{
#if LIBAVFORMAT_VERSION_MAJOR * 100 + LIBAVFORMAT_VERSION_MINOR > 5904
	return 0;
#else
	return ((::AVIOContext*)m_pPointer)->writeout_count;
#endif
}
int FFmpeg::AVIOContext::orig_buffer_size::get()
{
#if LIBAVFORMAT_VERSION_MAJOR * 100 + LIBAVFORMAT_VERSION_MINOR > 5904
	return 0;
#else
	return ((::AVIOContext*)m_pPointer)->orig_buffer_size;
#endif
}
//////////////////////////////////////////////////////
void FFmpeg::AVIOContext::w8(int b)
{
	avio_w8((::AVIOContext*)m_pPointer,b);
}
void FFmpeg::AVIOContext::write(IntPtr buf, int size)
{
	avio_write((::AVIOContext*)m_pPointer,(const unsigned char *)buf.ToPointer(),size);
}
void FFmpeg::AVIOContext::wl64(UInt64 val)
{
	avio_wl64((::AVIOContext*)m_pPointer,val);
}
void FFmpeg::AVIOContext::wb64(UInt64 val)
{
	avio_wb64((::AVIOContext*)m_pPointer,val);
}
void FFmpeg::AVIOContext::wl32(UInt32 val)
{
	avio_wl32((::AVIOContext*)m_pPointer,val);
}
void FFmpeg::AVIOContext::wb32(UInt32 val)
{
	avio_wb32((::AVIOContext*)m_pPointer,val);
}
void FFmpeg::AVIOContext::wl24(UInt32 val)
{
	avio_wl24((::AVIOContext*)m_pPointer,val);
}
void FFmpeg::AVIOContext::wb24(UInt32 val)
{
	avio_wb24((::AVIOContext*)m_pPointer,val);
}
void FFmpeg::AVIOContext::wl16(UInt32 val)
{
	avio_wl16((::AVIOContext*)m_pPointer,val);
}
void FFmpeg::AVIOContext::wb16(UInt32 val)
{
	avio_wb16((::AVIOContext*)m_pPointer,val);
}
int FFmpeg::AVIOContext::put_str(String^ str)
{
	char * _temp = nullptr;
	try
	{
		_temp = (char*)AllocString(str).ToPointer();
		return avio_put_str((::AVIOContext*)m_pPointer,_temp);
	}
	finally
	{
		FreeMemory((IntPtr)_temp);
	}
}
int FFmpeg::AVIOContext::put_str16le(String^ str)
{
	char * _temp = nullptr;
	try
	{
		_temp = (char*)AllocString(str).ToPointer();
		return avio_put_str16le((::AVIOContext*)m_pPointer,_temp);
	}
	finally
	{
		FreeMemory((IntPtr)_temp);
	}
}
int FFmpeg::AVIOContext::put_str16be(String^ str)
{
	char * _temp = nullptr;
	try
	{
		_temp = (char*)AllocString(str).ToPointer();
		return avio_put_str16be((::AVIOContext*)m_pPointer,_temp);
	}
	finally
	{
		FreeMemory((IntPtr)_temp);
	}
}

void FFmpeg::AVIOContext::write_marker(Int64 time, AVIODataMarkerType type)
{
	avio_write_marker((::AVIOContext*)m_pPointer,time,(::AVIODataMarkerType)type);

}
//////////////////////////////////////////////////////
Int64 FFmpeg::AVIOContext::seek(Int64 offset, AVSeek whence)
{
	return avio_seek((::AVIOContext*)m_pPointer,offset,(int)whence);
}
Int64 FFmpeg::AVIOContext::skip(Int64 offset)
{
	return avio_skip((::AVIOContext*)m_pPointer,offset);
}
Int64 FFmpeg::AVIOContext::tell()
{
	return avio_tell((::AVIOContext*)m_pPointer);
}
Int64 FFmpeg::AVIOContext::size()
{
	return avio_size((::AVIOContext*)m_pPointer);
}
bool FFmpeg::AVIOContext::feof()
{
	return 0 != avio_feof((::AVIOContext*)m_pPointer);
}
void FFmpeg::AVIOContext::flush()
{
	avio_flush((::AVIOContext*)m_pPointer);
}
//////////////////////////////////////////////////////
int FFmpeg::AVIOContext::read(IntPtr buf, int size)
{
	return avio_read((::AVIOContext*)m_pPointer,(unsigned char *)buf.ToPointer(),size);
}
int FFmpeg::AVIOContext::read_partial(IntPtr buf, int size)
{
	return avio_read_partial((::AVIOContext*)m_pPointer,(unsigned char *)buf.ToPointer(),size);
}

int FFmpeg::AVIOContext::r8()
{
	return avio_r8((::AVIOContext*)m_pPointer);
}
UInt32 FFmpeg::AVIOContext::rl16()
{
	return avio_rl16((::AVIOContext*)m_pPointer);
}
UInt32 FFmpeg::AVIOContext::rl24()
{
	return avio_rl24((::AVIOContext*)m_pPointer);
}
UInt32 FFmpeg::AVIOContext::rl32()
{
	return avio_rl32((::AVIOContext*)m_pPointer);
}
UInt64 FFmpeg::AVIOContext::rl64()
{
	return avio_rl64((::AVIOContext*)m_pPointer);
}
UInt32 FFmpeg::AVIOContext::rb16()
{
	return avio_rb16((::AVIOContext*)m_pPointer);
}
UInt32 FFmpeg::AVIOContext::rb24()
{
	return avio_rb24((::AVIOContext*)m_pPointer);
}
UInt32 FFmpeg::AVIOContext::rb32()
{
	return avio_rb32((::AVIOContext*)m_pPointer);
}
UInt64 FFmpeg::AVIOContext::rb64()
{
	return avio_rb64((::AVIOContext*)m_pPointer);
}
int FFmpeg::AVIOContext::get_str(int maxlen, String^ % str)
{
	int _result = 0;
	char * buf = nullptr;
	try
	{
		buf = (char *)AllocMemory(maxlen).ToPointer();
		_result = avio_get_str((::AVIOContext*)m_pPointer,maxlen,buf,maxlen);
		str = _result > 0 ? gcnew String(buf) : nullptr;
	}
	finally
	{
		FreeMemory((IntPtr)buf);
	}
	return _result;
}
int FFmpeg::AVIOContext::get_str16le(int maxlen, String^ % str)
{
	int _result = 0;
	char * buf = nullptr;
	try
	{
		buf = (char *)AllocMemory(maxlen).ToPointer();
		_result = avio_get_str16le((::AVIOContext*)m_pPointer,maxlen,buf,maxlen);
		str = _result > 0 ? gcnew String(buf) : nullptr;
	}
	finally
	{
		FreeMemory((IntPtr)buf);
	}
	return _result;
}
int FFmpeg::AVIOContext::get_str16be(int maxlen, String^ % str)
{
	int _result = 0;
	char * buf = nullptr;
	try
	{
		buf = (char *)AllocMemory(maxlen).ToPointer();
		_result = avio_get_str16be((::AVIOContext*)m_pPointer,maxlen,buf,maxlen);
		str = _result > 0 ? gcnew String(buf) : nullptr;
	}
	finally
	{
		FreeMemory((IntPtr)buf);
	}
	return _result;
}
int FFmpeg::AVIOContext::pause(bool _pause)
{
	return avio_pause((::AVIOContext*)m_pPointer,_pause ? 1 : 0);
}
Int64 FFmpeg::AVIOContext::seek_time(int stream_index,Int64 timestamp, AVSeekFlag flags)
{
	return avio_seek_time((::AVIOContext*)m_pPointer,stream_index,timestamp,(int)flags);
}
//////////////////////////////////////////////////////
int FFmpeg::AVIOContext::get_packet(AVPacket^ pkt, int size)
{
	return av_get_packet((::AVIOContext*)m_pPointer,(::AVPacket*)pkt->_Pointer.ToPointer(),size);
}
//////////////////////////////////////////////////////
int FFmpeg::AVIOContext::append_packet(AVPacket^ pkt, int size)
{
	return av_append_packet((::AVIOContext*)m_pPointer,(::AVPacket*)pkt->_Pointer.ToPointer(),size);
}
//////////////////////////////////////////////////////
int FFmpeg::AVIOContext::accept(AVIOContext^ % c)
{
	::AVIOContext * p = nullptr;
	int result = avio_accept((::AVIOContext*)m_pPointer,&p);
	if (p)
	{
		c =  gcnew AVIOContext(p,nullptr);
		c->m_pFreep = (TFreeFNP *)avio_closep;
	}
	return result;
}

int FFmpeg::AVIOContext::handshake()
{
	return avio_handshake((::AVIOContext*)m_pPointer);
}
//////////////////////////////////////////////////////
bool FFmpeg::AVIOContext::Open(String^ _url, FFmpeg::AvioFlag _flags, FFmpeg::AVIOInterruptDesc^ int_cb,FFmpeg::AVDictionary^ options)
{
	if (String::IsNullOrEmpty(_url)) return false;

	IntPtr _ptr = (int_cb != nullptr ? GCHandle::ToIntPtr(GCHandle::Alloc(int_cb,GCHandleType::Weak)) : IntPtr::Zero);
	::AVIOInterruptCB _cb = { Generic_interrupt_callback, _ptr.ToPointer() };
	m_pInterruptCB = int_cb;

	char * szUrl = nullptr;
	bool _result = false;
	::AVDictionary * _dict = options != nullptr ? (::AVDictionary *)options->_Pointer.ToPointer() : nullptr;
	::AVIOContext * _context = nullptr;
	try
	{
		if (m_pPointer != nullptr)
		{
			if (m_pDescructor)
			{
				m_pDescructor(m_pPointer);
			}
			if (m_pFree)
			{
				m_pFree(m_pPointer);
			}
			if (m_pFreep)
			{
				void* _ptr = m_pPointer;
				m_pFreep(&_ptr);
			}
		}
		m_pPointer = nullptr;
		szUrl = (char*)AllocString(_url).ToPointer();
		_result = (0 <= avio_open2(&_context,szUrl,(int)_flags,int_cb != nullptr ? &_cb : nullptr, &_dict));
		if (options)
		{
			options->ChangePointer(_dict);
			_dict = nullptr;
		}
	}
	finally
	{
		FreeMemory((IntPtr)szUrl);
		if (_dict) av_dict_free(&_dict);
		m_pPointer = _context;
	}
	m_pFreep = _result ? (TFreeFNP *)avio_closep : nullptr;
	m_pFree = nullptr;
	m_pDescructor = nullptr;

	return _result;
}
//////////////////////////////////////////////////////
int FFmpeg::AVIOContext::ReadCB(IntPtr buf, int buf_size)
{
	if (m_pRead)
	{
		if (m_pBuffer != nullptr && m_pBuffer->m_pPointer != buf.ToPointer())
		{
			m_pBuffer->m_pPointer = buf.ToPointer();
		}
		return m_pRead(m_pOpaque,buf,buf_size);
	}
	return -1;
}

int FFmpeg::AVIOContext::WriteCB(IntPtr buf, int buf_size)
{
	if (m_pWrite)
	{
		if (m_pBuffer != nullptr && m_pBuffer->m_pPointer != buf.ToPointer())
		{
			m_pBuffer->m_pPointer = buf.ToPointer();
		}
		return m_pWrite(m_pOpaque,buf,buf_size);
	}
	return -1;
}

int FFmpeg::AVIOContext::SeekCB(Int64 offset, int whence)
{
	if (m_pSeek)
	{
		return m_pSeek(m_pOpaque,offset, (AVSeek)whence);
	}
	return -1;
}
//////////////////////////////////////////////////////
FFmpeg::AvioFlag FFmpeg::AVIOContext::Check(String^ _url)
{
	return Check(_url, FFmpeg::AvioFlag::READ_WRITE);
}

FFmpeg::AvioFlag FFmpeg::AVIOContext::Check(String^ _url, FFmpeg::AvioFlag _flags)
{
	LibAVFormat::RegisterAll();
	char * szUrl = nullptr;
	try
	{
		szUrl = (char*)AllocString(_url).ToPointer();
		return (FFmpeg::AvioFlag)avio_check(szUrl, (int)_flags);
	}
	finally
	{
		FreeMemory((IntPtr)szUrl);
	}
}

String^ FFmpeg::AVIOContext::FindProtocolName(String^ _url)
{
	LibAVFormat::RegisterAll();
	char * szUrl = nullptr;
	try
	{
		szUrl = (char*)AllocString(_url).ToPointer();
		const char * p = avio_find_protocol_name(szUrl);
		return p != nullptr ? gcnew String(p) : nullptr;
	}
	finally
	{
		FreeMemory((IntPtr)szUrl);
	}
}

FFmpeg::AVIOProtocolEnum^ FFmpeg::AVIOContext::EnumProtocols(bool bOutput)
{
	return gcnew AVIOProtocolEnum(bOutput);
}
//////////////////////////////////////////////////////
typedef struct AVCodecTag
{
	AVCodecID id;
	unsigned int tag;
}AVCodecTag;
//////////////////////////////////////////////////////
// AVCodecTag
//////////////////////////////////////////////////////
FFmpeg::AVCodecTag::AVCodecTag(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVCodecTag::ToString()
{
	String^ _result = AVCodec::GetCodecName(id);
	if (String::IsNullOrEmpty(_result) || _result->Equals("unknown_codec", StringComparison::InvariantCultureIgnoreCase))
	{
		array<byte>^ _data = BitConverter::GetBytes((UInt32)id);
		_result = "";
		for (int i = 0; i < _data->Length; i++)
		{
			if (_data[i] < 32 || _data[i] > 128) return __super::ToString();
			_result += (char)_data[i];
		}
	}
	return "[ AVCodecTag ] \"" + _result + "\"";
}
int FFmpeg::AVCodecTag::_StructureSize::get()
{
	return sizeof(::AVCodecTag);
}
//////////////////////////////////////////////////////
FFmpeg::AVCodecID FFmpeg::AVCodecTag::id::get()
{
	return (AVCodecID)((::AVCodecTag*)m_pPointer)->id;
}
UInt32 FFmpeg::AVCodecTag::tag::get()
{
	return ((::AVCodecTag*)m_pPointer)->tag;
}
String^ FFmpeg::AVCodecTag::name::get()
{
	return AVCodec::GetCodecName(id);
}
//////////////////////////////////////////////////////
FFmpeg::AVCodecID FFmpeg::AVCodecTag::GetCodecID(array<AVCodecTag^>^ tags, int tag)
{
	LibAVFormat::RegisterAll();
	::AVCodecTag ** _tags = nullptr;
	try
	{
		_tags = (::AVCodecTag **)AllocMemory(sizeof(::AVCodecTag*) * tags->Length).ToPointer();
		for (int i = 0; i < tags->Length; i++) { _tags[i] = (::AVCodecTag*)tags[i]->_Pointer.ToPointer(); }
		return (AVCodecID)av_codec_get_id(_tags, tag);
	}
	finally
	{
		FreeMemory((IntPtr)_tags);
	}
}

int FFmpeg::AVCodecTag::GetCodecTag(array<AVCodecTag^>^ tags, AVCodecID id)
{
	LibAVFormat::RegisterAll();
	::AVCodecTag ** _tags = nullptr;
	try
	{
		_tags = (::AVCodecTag **)AllocMemory(sizeof(::AVCodecTag*) * tags->Length).ToPointer();
		for (int i = 0; i < tags->Length; i++) { _tags[i] = (::AVCodecTag*)tags[i]->_Pointer.ToPointer(); }
		return av_codec_get_tag(_tags, (::AVCodecID)id);
	}
	finally
	{
		FreeMemory((IntPtr)_tags);
	}
}

array<FFmpeg::AVCodecTag^>^ FFmpeg::AVCodecTag::GetRiffVideoTags()
{
	LibAVFormat::RegisterAll();
	const struct ::AVCodecTag *table[] = { avformat_get_riff_video_tags(), nullptr };
	List<FFmpeg::AVCodecTag^>^ _list = nullptr;
	try
	{
		_list = gcnew List<FFmpeg::AVCodecTag^>();
		const struct ::AVCodecTag ** p = table;
		while (*p != nullptr)
		{
			_list->Add(_CreateChildObject<AVCodecTag>((void*)*p++,nullptr));
		}
		return _list->ToArray();
	}
	finally
	{
		if (_list != nullptr) delete _list;
	}
}
array<FFmpeg::AVCodecTag^>^ FFmpeg::AVCodecTag::GetRiffAudioTags()
{
	LibAVFormat::RegisterAll();
	const struct ::AVCodecTag *table[] = { avformat_get_riff_audio_tags(), 0 };
	List<FFmpeg::AVCodecTag^>^ _list = nullptr;
	try
	{
		_list = gcnew List<FFmpeg::AVCodecTag^>();
		const struct ::AVCodecTag ** p = table;
		while (*p != nullptr)
		{
			_list->Add(_CreateChildObject<AVCodecTag>((void*)*p++,nullptr));
		}
		return _list->ToArray();
	}
	finally
	{
		if (_list != nullptr) delete _list;
	}
}
array<FFmpeg::AVCodecTag^>^ FFmpeg::AVCodecTag::GetMovVideoTags()
{
	LibAVFormat::RegisterAll();
	const struct ::AVCodecTag *table[] = { avformat_get_mov_video_tags(), 0 };
	List<FFmpeg::AVCodecTag^>^ _list = nullptr;
	try
	{
		_list = gcnew List<FFmpeg::AVCodecTag^>();
		const struct ::AVCodecTag ** p = table;
		while (*p != nullptr)
		{
			_list->Add(_CreateChildObject<AVCodecTag>((void*)*p++,nullptr));
		}
		return _list->ToArray();
	}
	finally
	{
		if (_list != nullptr) delete _list;
	}
}
array<FFmpeg::AVCodecTag^>^ FFmpeg::AVCodecTag::GetMovAudioTags()
{
	LibAVFormat::RegisterAll();
	const struct ::AVCodecTag *table[] = { avformat_get_mov_audio_tags(), 0 };
	List<FFmpeg::AVCodecTag^>^ _list = nullptr;
	try
	{
		_list = gcnew List<FFmpeg::AVCodecTag^>();
		const struct ::AVCodecTag ** p = table;
		while (*p != nullptr)
		{
			_list->Add(_CreateChildObject<AVCodecTag>((void*)*p++,nullptr));
		}
		return _list->ToArray();
	}
	finally
	{
		if (_list != nullptr) delete _list;
	}
}
//////////////////////////////////////////////////////
// AVOutputFormatEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVOutputFormat::AVOutputFormats::Enumerator::Enumerator()
	: m_pCurrent(nullptr)
	, m_pOpaque(IntPtr::Zero)
{
}
FFmpeg::AVOutputFormat::AVOutputFormats::Enumerator::~Enumerator()
{
}
//////////////////////////////////////////////////////
// IEnumerator
bool FFmpeg::AVOutputFormat::AVOutputFormats::Enumerator::MoveNext()
{
	const ::AVOutputFormat * p = nullptr;
	void * opaque = m_pOpaque.ToPointer();
	LOAD_API(AVFormat,::AVOutputFormat *,av_oformat_next,void *);
	LOAD_API(AVFormat,::AVOutputFormat *,av_muxer_iterate,void **);
	if (av_muxer_iterate)
	{
		p = av_muxer_iterate(&opaque);
	}
	else
	{
		if (av_oformat_next)
		{
			p = av_oformat_next(opaque);
			opaque = (void*)p;
		}
	}
	m_pOpaque = IntPtr(opaque);
	m_pCurrent = p != nullptr ? gcnew AVOutputFormat((void*)p,nullptr) : nullptr;
	return m_pCurrent != nullptr;
}

FFmpeg::AVOutputFormat^ FFmpeg::AVOutputFormat::AVOutputFormats::Enumerator::Current::get()
{
	if (m_pOpaque == IntPtr::Zero)
	{
		if (!MoveNext()) return nullptr;
	}
	return m_pCurrent;
}

void FFmpeg::AVOutputFormat::AVOutputFormats::Enumerator::Reset()
{
	m_pOpaque = IntPtr::Zero;
}

Object^ FFmpeg::AVOutputFormat::AVOutputFormats::Enumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVOutputFormat::AVOutputFormats
//////////////////////////////////////////////////////
FFmpeg::AVOutputFormat::AVOutputFormats::AVOutputFormats()
{
	LibAVFormat::RegisterAll();
}
//////////////////////////////////////////////////////
FFmpeg::AVOutputFormat^ FFmpeg::AVOutputFormat::AVOutputFormats::default::get(int index)
{
	if (index >= 0)
	{
		LOAD_API(AVFormat,::AVOutputFormat *,av_oformat_next,void *);
		LOAD_API(AVFormat,::AVOutputFormat *,av_muxer_iterate,void **);

		if (av_muxer_iterate)
		{
			void * opaque = nullptr;
			while (true)
			{
				auto p = av_muxer_iterate(&opaque);
				if (!p) break;
				if (index-- == 0)
				{
					return gcnew AVOutputFormat((void*)p, nullptr);
				}
			}
		} else
		if (av_oformat_next)
		{
			::AVOutputFormat * p = nullptr;
			while (true)
			{
				p = av_oformat_next(p);
				if (!p) break;
				if (index-- == 0)
				{
					return gcnew AVOutputFormat((void*)p, nullptr);
				}
			}
		}
	}
	return nullptr;
}
int FFmpeg::AVOutputFormat::AVOutputFormats::Count::get()
{
	int count = 0;
	LOAD_API(AVFormat,::AVOutputFormat *,av_oformat_next,void *);
	LOAD_API(AVFormat,::AVOutputFormat *,av_muxer_iterate,void **);
	if (count == 0 && av_muxer_iterate)
	{
		void * opaque = nullptr;
		while (true)
		{
			auto p = av_muxer_iterate(&opaque);
			if (!p) break;
			count++;
		}
	}
	if (count == 0 && av_oformat_next)
	{
		::AVOutputFormat * p = nullptr;
		while (true)
		{
			p = av_oformat_next(p);
			if (!p) break;
			count++;
		}
	}
	return count;
}
//////////////////////////////////////////////////////
// AVOutputFormat
//////////////////////////////////////////////////////
FFmpeg::AVOutputFormat::AVOutputFormat(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVOutputFormat::ToString()
{
	if (m_pPointer)
	{
		String^ _name;
		if (String::IsNullOrEmpty(_name)) _name = long_name;
		if (String::IsNullOrEmpty(_name)) _name = name;
		if (!String::IsNullOrEmpty(_name))
		{
			return "[ AVOutputFormat ] \"" +_name + "\"";
		}
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
int FFmpeg::AVOutputFormat::_StructureSize::get() 
{
	return sizeof(::AVOutputFormat);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVOutputFormat::name::get()
{ 
	auto p = ((::AVOutputFormat*)m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}

String^ FFmpeg::AVOutputFormat::long_name::get()
{
	auto p = ((::AVOutputFormat*)m_pPointer)->long_name;
	return p != nullptr ? gcnew String(p) : nullptr;
}

String^ FFmpeg::AVOutputFormat::mime_type::get()
{
	auto p = ((::AVOutputFormat*)m_pPointer)->mime_type;
	return p != nullptr ? gcnew String(p) : nullptr;
}

String^ FFmpeg::AVOutputFormat::extensions::get()
{ 
	auto p = ((::AVOutputFormat*)m_pPointer)->extensions;
	return p != nullptr ? gcnew String(p) : nullptr;
}

FFmpeg::AVCodecID FFmpeg::AVOutputFormat::audio_codec::get()
{
	return (FFmpeg::AVCodecID)((::AVOutputFormat*)m_pPointer)->audio_codec;
}

FFmpeg::AVCodecID FFmpeg::AVOutputFormat::video_codec::get()
{
	return (FFmpeg::AVCodecID)((::AVOutputFormat*)m_pPointer)->video_codec;
}

FFmpeg::AVCodecID FFmpeg::AVOutputFormat::subtitle_codec::get()
{ 
	return (FFmpeg::AVCodecID)((::AVOutputFormat*)m_pPointer)->subtitle_codec;
}

FFmpeg::AVfmt FFmpeg::AVOutputFormat::flags::get()
{
	return (FFmpeg::AVfmt)((::AVOutputFormat*)m_pPointer)->flags;
}

FFmpeg::AVClass^ FFmpeg::AVOutputFormat::priv_class::get()
{
	auto p = ((::AVOutputFormat*)m_pPointer)->priv_class;
	return _CreateObject<FFmpeg::AVClass>((void*)p);
}

array<FFmpeg::AVCodecTag^>^ FFmpeg::AVOutputFormat::codec_tag::get()
{
	List<FFmpeg::AVCodecTag^>^ _list = nullptr;
	::AVCodecTag ** p = (::AVCodecTag **)((::AVOutputFormat*)m_pPointer)->codec_tag;
	if (p != nullptr && p[0]->id != 0)
	{
		_list = gcnew List<FFmpeg::AVCodecTag^>();
		::AVCodecTag * pp = p[0];
		while (pp->id != 0)
		{
			_list->Add(_CreateObject<AVCodecTag>(pp++));
		}
	}
	return _list != nullptr ? _list->ToArray() : nullptr;
}

FFmpeg::AVCodecID FFmpeg::AVOutputFormat::data_codec::get()
{ 
#if LIBAVFORMAT_VERSION_MAJOR < 60
	return (FFmpeg::AVCodecID)((::AVOutputFormat*)m_pPointer)->data_codec;
#else
	return FFmpeg::AVCodecID::NONE;
#endif
}
//////////////////////////////////////////////////////
FFmpeg::AVFormatContext^ FFmpeg::AVOutputFormat::OpenContext()
{
	return AVFormatContext::OpenOutput(this,nullptr,nullptr);
}

FFmpeg::AVCodecID FFmpeg::AVOutputFormat::GuessCodec(String^ short_name, String^ filename, String^ mime_type, FFmpeg::AVMediaType type)
{
	char * _short_name = nullptr;
	char * _filename= nullptr;
	char * _mime_type= nullptr;
	try
	{
		_short_name = (char*)AllocString(short_name).ToPointer();
		_filename = (char*)AllocString(filename).ToPointer();
		_mime_type = (char*)AllocString(mime_type).ToPointer();
		return (FFmpeg::AVCodecID)av_guess_codec((::AVOutputFormat*)m_pPointer, _short_name, _filename, _mime_type, (::AVMediaType)type);
	}
	finally
	{
		FreeMemory((IntPtr)_short_name);
		FreeMemory((IntPtr)_filename);
		FreeMemory((IntPtr)_mime_type);
	}
}

bool FFmpeg::AVOutputFormat::QueryCodec(AVCodecID codec_id,FFCompliance std_compliance)
{
	return 1 == avformat_query_codec((::AVOutputFormat*)m_pPointer,(::AVCodecID)codec_id,(int)std_compliance);
}
//////////////////////////////////////////////////////
int FFmpeg::AVOutputFormat::transfer_internal_stream_timing_info(AVStream^ ost, AVStream^ ist, AVTimebaseSource copy_tb)
{
	return avformat_transfer_internal_stream_timing_info((::AVOutputFormat*)m_pPointer,
		(::AVStream *)ost->_Pointer.ToPointer(), (const ::AVStream *)ist->_Pointer.ToPointer(),
		(::AVTimebaseSource)copy_tb);
}
//////////////////////////////////////////////////////
FFmpeg::AVOutputFormat^ FFmpeg::AVOutputFormat::Next()
{
	return Next(this);
}
FFmpeg::AVOutputFormat^ FFmpeg::AVOutputFormat::Next(AVOutputFormat^ f)
{
	FFmpeg::LibAVFormat::RegisterAll();
	const ::AVOutputFormat * p = nullptr;
	LOAD_API(AVFormat,::AVOutputFormat *,av_oformat_next,const ::AVOutputFormat*);
	LOAD_API(AVFormat,::AVOutputFormat *,av_muxer_iterate,void **);
	if (av_oformat_next)
	{
		p = av_oformat_next(f != nullptr ? (const ::AVOutputFormat*)(f->_Pointer.ToPointer()) : nullptr);
	}
	else
	{
		if (av_muxer_iterate)
		{
			void * opaque = nullptr;
			bool bMoveNext = (f != nullptr);
			do
			{
				p = av_muxer_iterate(&opaque);
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
	return p != nullptr ? gcnew AVOutputFormat((void*)p,nullptr) : nullptr;
}
//////////////////////////////////////////////////////
FFmpeg::AVOutputFormat^ FFmpeg::AVOutputFormat::GuessFormat(String^ short_name, String^ filename, String^ mime_type)
{
	LibAVFormat::RegisterAll();
	char * _short_name = nullptr;
	char * _filename= nullptr;
	char * _mime_type= nullptr;
	try
	{
		_short_name = (char*)AllocString(short_name).ToPointer();
		_filename = (char*)AllocString(filename).ToPointer();
		_mime_type = (char*)AllocString(mime_type).ToPointer();
		auto p = av_guess_format(_short_name, _filename, _mime_type);
		return p != nullptr ? gcnew AVOutputFormat((void*)p,nullptr) : nullptr;
	}
	finally
	{
		FreeMemory((IntPtr)_short_name);
		FreeMemory((IntPtr)_filename);
		FreeMemory((IntPtr)_mime_type);
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVOutputFormat::AVOutputFormats^ FFmpeg::AVOutputFormat::Formats::get()
{
	return gcnew AVOutputFormats();
}
//////////////////////////////////////////////////////
// AVProbeData
//////////////////////////////////////////////////////
FFmpeg::AVProbeData::AVProbeData(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
FFmpeg::AVProbeData::AVProbeData()
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
}
//////////////////////////////////////////////////////
int FFmpeg::AVProbeData::_StructureSize::get()
{
	return sizeof(::AVProbeData);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVProbeData::filename::get()
{
	auto p = ((::AVProbeData*)m_pPointer)->filename;
	return p != nullptr ? gcnew String(p) : nullptr;
}

void FFmpeg::AVProbeData::filename::set(String^ _name)
{
	((::AVProbeData*)m_pPointer)->filename = (char*)AllocString("filename",_name).ToPointer();
}

IntPtr FFmpeg::AVProbeData::buf::get()
{
	return (IntPtr)((::AVProbeData*)m_pPointer)->buf;
}

void FFmpeg::AVProbeData::buf::set(IntPtr _value)
{
	((::AVProbeData*)m_pPointer)->buf = (unsigned char *)_value.ToPointer();
}

int FFmpeg::AVProbeData::buf_size::get()
{
	return ((::AVProbeData*)m_pPointer)->buf_size;
}

void FFmpeg::AVProbeData::buf_size::set(int _value)
{
	((::AVProbeData*)m_pPointer)->buf_size = _value;
}

String^ FFmpeg::AVProbeData::mime_type::get()
{
	auto p = ((::AVProbeData*)m_pPointer)->mime_type;
	return p != nullptr ? gcnew String(p) : nullptr;
}

void FFmpeg::AVProbeData::mime_type::set(String^ _name)
{
	((::AVProbeData*)m_pPointer)->mime_type = (char*)AllocString("mime_type",_name).ToPointer();
}

//////////////////////////////////////////////////////
// AVInputFormats
//////////////////////////////////////////////////////
FFmpeg::AVInputFormat::AVInputFormats::AVInputFormats()
{
	LibAVFormat::RegisterAll();
}

FFmpeg::AVInputFormat^ FFmpeg::AVInputFormat::AVInputFormats::default::get(int index)
{
	if (index >= 0)
	{
		LOAD_API(AVFormat,::AVInputFormat *,av_iformat_next,void *);
		LOAD_API(AVFormat,::AVInputFormat *,av_demuxer_iterate,void **);

		if (av_demuxer_iterate)
		{
			void * opaque = nullptr;
			while (true)
			{
				auto p = av_demuxer_iterate(&opaque);
				if (!p) break;
				if (index-- == 0)
				{
					return gcnew AVInputFormat((void*)p, nullptr);
				}
			}
		} else
		if (av_iformat_next)
		{
			::AVInputFormat * p = nullptr;
			while (true)
			{
				p = av_iformat_next(p);
				if (!p) break;
				if (index-- == 0)
				{
					return gcnew AVInputFormat((void*)p, nullptr);
				}
			}
		}
	}
	return nullptr;
}
int FFmpeg::AVInputFormat::AVInputFormats::Count::get()
{
	int count = 0;

	LOAD_API(AVFormat,::AVInputFormat *,av_iformat_next,void *);
	LOAD_API(AVFormat,::AVInputFormat *,av_demuxer_iterate,void **);

	if (count == 0 && av_demuxer_iterate)
	{
		void * opaque = nullptr;
		while (true)
		{
			auto p = av_demuxer_iterate(&opaque);
			if (!p) break;
			count++;
		}
	}
	if (count == 0 && av_iformat_next)
	{
		::AVInputFormat * p = nullptr;
		while (true)
		{
			p = av_iformat_next(p);
			if (!p) break;
			count++;
		}
	}
	return count;
}
//////////////////////////////////////////////////////
// AVInputFormatEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVInputFormat::AVInputFormats::AVInputFormatEnumerator::AVInputFormatEnumerator()
	: m_pOpaque(IntPtr::Zero)
{
	FFmpeg::LibAVFormat::RegisterAll();
}
FFmpeg::AVInputFormat::AVInputFormats::AVInputFormatEnumerator::~AVInputFormatEnumerator()
{
}
//////////////////////////////////////////////////////
// IEnumerator
bool FFmpeg::AVInputFormat::AVInputFormats::AVInputFormatEnumerator::MoveNext()
{
	const ::AVInputFormat * p = nullptr;
	void * opaque = m_pOpaque.ToPointer();
	LOAD_API(AVFormat,::AVInputFormat *,av_iformat_next,void *);
	LOAD_API(AVFormat,::AVInputFormat *,av_demuxer_iterate,void **);
	if (av_demuxer_iterate)
	{
		p = av_demuxer_iterate(&opaque);
	}
	else
	{
		if (av_iformat_next)
		{
			p = av_iformat_next(opaque);
			opaque = (void*)p;
		}
	}
	m_pOpaque = IntPtr(opaque);
	m_pCurrent = p != nullptr ? gcnew AVInputFormat((void*)p,nullptr) : nullptr;
	return (m_pCurrent != nullptr);
}

FFmpeg::AVInputFormat^ FFmpeg::AVInputFormat::AVInputFormats::AVInputFormatEnumerator::Current::get()
{
	if (m_pOpaque == IntPtr::Zero)
	{
		MoveNext();
	}
	return m_pCurrent;
}

void FFmpeg::AVInputFormat::AVInputFormats::AVInputFormats::AVInputFormatEnumerator::Reset()
{
	m_pOpaque = IntPtr::Zero;
}

Object^ FFmpeg::AVInputFormat::AVInputFormats::AVInputFormatEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVInputFormat
//////////////////////////////////////////////////////
FFmpeg::AVInputFormat::AVInputFormat(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVInputFormat::ToString()
{
	if (m_pPointer)
	{
		String^ _name;
		if (String::IsNullOrEmpty(_name)) _name = long_name;
		if (String::IsNullOrEmpty(_name)) _name = name;
		if (!String::IsNullOrEmpty(_name))
		{
			return "[ AVInputFormat ] \"" +_name + "\"";
		}
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
int FFmpeg::AVInputFormat::_StructureSize::get() 
{
	return sizeof(::AVInputFormat);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVInputFormat::name::get()
{
	auto p = ((::AVInputFormat*)m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}

String^ FFmpeg::AVInputFormat::long_name::get()
{
	auto p = ((::AVInputFormat*)m_pPointer)->long_name;
	return p != nullptr ? gcnew String(p) : nullptr;
}

FFmpeg::AVfmt FFmpeg::AVInputFormat::flags::get()
{
	return (FFmpeg::AVfmt)((::AVInputFormat*)m_pPointer)->flags;
}

String^ FFmpeg::AVInputFormat::extensions::get()
{ 
	auto p = ((::AVInputFormat*)m_pPointer)->extensions;
	return p != nullptr ? gcnew String(p) : nullptr;
}

array<FFmpeg::AVCodecTag^>^ FFmpeg::AVInputFormat::codec_tag::get()
{
	List<FFmpeg::AVCodecTag^>^ _list = nullptr;
	::AVCodecTag ** p = (::AVCodecTag **)((::AVInputFormat*)m_pPointer)->codec_tag;
	if (p != nullptr && p[0]->id != 0)
	{
		_list = gcnew List<FFmpeg::AVCodecTag^>();
		::AVCodecTag * pp = p[0];
		while (pp->id != 0)
		{
			_list->Add(_CreateObject<AVCodecTag>(pp++));
		}
	}
	return _list != nullptr ? _list->ToArray() : nullptr;
}

FFmpeg::AVClass^ FFmpeg::AVInputFormat::priv_class::get()
{
	auto p = ((::AVInputFormat*)m_pPointer)->priv_class;
	return _CreateObject<FFmpeg::AVClass>((void*)p);
}

String^ FFmpeg::AVInputFormat::mime_type::get()
{
	auto p = ((::AVInputFormat*)m_pPointer)->mime_type;
	return p != nullptr ? gcnew String(p) : nullptr;
}

bool FFmpeg::AVInputFormat::can_seek::get()
{
	return (((::AVInputFormat*)m_pPointer)->read_seek != nullptr 
		|| ((::AVInputFormat*)m_pPointer)->read_seek2 != nullptr);
}

int FFmpeg::AVInputFormat::raw_codec_id::get()
{
	return ((::AVInputFormat*)m_pPointer)->raw_codec_id;
}
//////////////////////////////////////////////////////
FFmpeg::AVInputFormat^ FFmpeg::AVInputFormat::Next()
{
	return Next(this);
}
//////////////////////////////////////////////////////
FFmpeg::AVInputFormat::AVInputFormats^ FFmpeg::AVInputFormat::Formats::get()
{
	return gcnew AVInputFormats();
}
//////////////////////////////////////////////////////
FFmpeg::AVInputFormat^ FFmpeg::AVInputFormat::Next(FFmpeg::AVInputFormat^ f)
{
	FFmpeg::LibAVFormat::RegisterAll();

	const ::AVInputFormat * p = nullptr;
	LOAD_API(AVFormat,::AVInputFormat *,av_iformat_next,const ::AVInputFormat*);
	LOAD_API(AVFormat,::AVInputFormat *,av_demuxer_iterate,void **);
	if (av_iformat_next)
	{
		p = av_iformat_next((const ::AVInputFormat*)(f != nullptr ? f->m_pPointer : nullptr));
	}
	else
	{
		if (av_demuxer_iterate)
		{
			void * opaque = nullptr;
			bool bMoveNext = (f != nullptr);
			do
			{
				p = av_demuxer_iterate(&opaque);
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
	return p != nullptr ? gcnew AVInputFormat((void*)p,nullptr) : nullptr;
}

FFmpeg::AVInputFormat^ FFmpeg::AVInputFormat::FindInputFormat(String^ short_name)
{
	LibAVFormat::RegisterAll();
	char * _short_name = nullptr;
	try
	{
		_short_name = (char*)AllocString(short_name).ToPointer();
		auto p = av_find_input_format(_short_name);
		return p != nullptr ? gcnew AVInputFormat((void*)p,nullptr) : nullptr;
	}
	finally
	{
		FreeMemory((IntPtr)_short_name);
	}
}

FFmpeg::AVInputFormat^ FFmpeg::AVInputFormat::ProbeInputFormat(FFmpeg::AVProbeData^ pd, bool is_opened)
{
	LibAVFormat::RegisterAll();
	auto p = av_probe_input_format((::AVProbeData*)(pd != nullptr ? pd->_Pointer.ToPointer() : nullptr), is_opened ? 1 : 0);
	return p != nullptr ? gcnew AVInputFormat((void*)p,nullptr) : nullptr;
}

FFmpeg::AVInputFormat^ FFmpeg::AVInputFormat::ProbeInputFormat2(FFmpeg::AVProbeData^ pd, bool is_opened, int % score_max)
{
	LibAVFormat::RegisterAll();
	int _scope = score_max;
	auto p = av_probe_input_format2((::AVProbeData*)(pd != nullptr ? pd->_Pointer.ToPointer() : nullptr), is_opened ? 1 : 0,&_scope);
	score_max = _scope;
	return p != nullptr ? gcnew AVInputFormat((void*)p,nullptr) : nullptr;
}

FFmpeg::AVInputFormat^ FFmpeg::AVInputFormat::ProbeInputFormat3(FFmpeg::AVProbeData^ pd, bool is_opened, int % score_ret)
{
	LibAVFormat::RegisterAll();
	int _scope = score_ret;
	auto p = av_probe_input_format3((::AVProbeData*)(pd != nullptr ? pd->_Pointer.ToPointer() : nullptr), is_opened ? 1 : 0,&_scope);
	score_ret = _scope;
	return p != nullptr ? gcnew AVInputFormat((void*)p,nullptr) : nullptr;
}

FFmpeg::AVInputFormat^ FFmpeg::AVInputFormat::ProbeInputBuffer(FFmpeg::AVIOContext^ pb, String^ url, IntPtr logctx, UInt32 offset, UInt32 max_probe_size)
{
	LibAVFormat::RegisterAll();
	DYNAMIC_DEF_API(AVFormat,int,nullptr,av_probe_input_buffer,::AVIOContext *, const ::AVInputFormat **,
		const char *, void *,unsigned int , unsigned int );
	const ::AVInputFormat * p = nullptr;
	char * _url = nullptr;
	try
	{
		_url = (char*)AllocString(url).ToPointer();
		if (0 == av_probe_input_buffer((::AVIOContext*)(pb != nullptr ? pb->_Pointer.ToPointer() : nullptr),&p,_url,logctx.ToPointer(),offset,max_probe_size))
		{
			if (p != nullptr) return gcnew AVInputFormat((void*)p,nullptr);
		}
	}
	finally
	{
		FreeMemory((IntPtr)_url);
	}
	return nullptr;
}
//////////////////////////////////////////////////////
// AVStream
//////////////////////////////////////////////////////
FFmpeg::AVStream::AVStream(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
	, m_pContext(nullptr)
{
}
//////////////////////////////////////////////////////
int FFmpeg::AVStream::index::get()
{
	return ((::AVStream*)m_pPointer)->index;
}
int FFmpeg::AVStream::id::get()
{
	return ((::AVStream*)m_pPointer)->id;
}
void FFmpeg::AVStream::id::set(int value)
{
	((::AVStream*)m_pPointer)->id = value;
}

FFmpeg::AVCodecContext^ FFmpeg::AVStream::codec::get()
{
#if FF_API_LAVF_AVCTX
	auto p = ((::AVStream*)m_pPointer)->codec;
	return _CreateObject<FFmpeg::AVCodecContext>((void*)p);
#else
	if (m_pContext == nullptr)
	{
		LOAD_API(AVFormat,::AVCodec *,av_format_get_audio_codec,::AVFormatContext *);
		LOAD_API(AVFormat,::AVCodec *,av_format_get_video_codec,::AVFormatContext *);
		LOAD_API(AVFormat,::AVCodec *,av_format_get_subtitle_codec,::AVFormatContext *);

		::AVStream* st = (::AVStream*)m_pPointer;
		::AVCodecContext* context = nullptr;
		::AVFormatContext * fmt = (::AVFormatContext *)((AVFormatContext^)m_pParent)->_Pointer.ToPointer();
		if (!m_pContext && fmt->iformat)
		{
			const ::AVCodec * codec = nullptr;
			if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				if (av_format_get_video_codec)
				{
					codec = av_format_get_video_codec(fmt);
				}
				else
				{
					codec = fmt->video_codec;
				}
			}
			if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				if (av_format_get_audio_codec)
				{
					codec = av_format_get_audio_codec(fmt);
				}
				else
				{
					codec = fmt->audio_codec;
				}
			}
			if (st->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE)
			{
				if (av_format_get_subtitle_codec)
				{
					codec = av_format_get_subtitle_codec(fmt);
				}
				else
				{
					codec = fmt->subtitle_codec;
				}
			}
			if (!codec)
			{
				codec = avcodec_find_decoder(st->codecpar->codec_id);
			}
			context = avcodec_alloc_context3(codec);
			if (context)
			{
				context->codec_type = st->codecpar->codec_type;
				context->codec_id = st->codecpar->codec_id;
				context->bit_rate = 0;
				context->time_base = av_stream_get_codec_timebase(st);
				avcodec_parameters_to_context(context, st->codecpar);
			}
		}
		if (!m_pContext && fmt->oformat)
		{
			const ::AVCodec * codec = nullptr;
			::AVCodecID default_id = AV_CODEC_ID_NONE;
			if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				if (av_format_get_video_codec)
				{
					codec = av_format_get_video_codec(fmt);
				}
				else
				{
					codec = fmt->video_codec;
				}
				default_id = fmt->oformat->video_codec;
			}
			if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				if (av_format_get_audio_codec)
				{
					codec = av_format_get_audio_codec(fmt);
				}
				else
				{
					codec = fmt->audio_codec;
				}
				default_id = fmt->oformat->audio_codec;
			}
			if (st->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE)
			{
				if (av_format_get_subtitle_codec)
				{
					codec = av_format_get_subtitle_codec(fmt);
				}
				else
				{
					codec = fmt->subtitle_codec;
				}
				default_id = fmt->oformat->subtitle_codec;
			}
			if (!codec)
			{
				if (st->codecpar->codec_id == (int)AVCodecID::NONE)
				{
					st->codecpar->codec_id = default_id;
				}
				codec = avcodec_find_encoder(st->codecpar->codec_id);
			}
			context = avcodec_alloc_context3(codec);
			if (context)
			{
				context->codec_type = st->codecpar->codec_type;
				context->codec_id = st->codecpar->codec_id;
				context->time_base = av_stream_get_codec_timebase(st);
				avcodec_parameters_to_context(context, st->codecpar);
			}
		}
		if (context)
		{
			m_pContext = _CreateChildObject<AVCodecContext>(context, this);
			m_pContext->m_pFreep = (TFreeFNP*)avcodec_free_context;
		}
	}
	return m_pContext;
#endif
}
void FFmpeg::AVStream::codec::set(AVCodecContext^ value)
{
#if FF_API_LAVF_AVCTX
	AddObject((IntPtr)((::AVStream*)m_pPointer)->codec,value);
	((::AVStream*)m_pPointer)->codec = (value != nullptr ? (::AVCodecContext*)value->_Pointer.ToPointer() : nullptr);
#else
	AVBase^ ctx = m_pContext;
	if (value != nullptr)
	{
		AddObject(value->_Pointer, value);
		m_pContext = value;
	}
	if (ctx != nullptr)
	{
		RemoveObject(ctx->_Pointer);
		delete ctx;
	}
#endif
}

IntPtr FFmpeg::AVStream::priv_data::get()
{ 
	return (IntPtr)((::AVStream*)m_pPointer)->priv_data;
}
void FFmpeg::AVStream::priv_data::set(IntPtr value)
{ 
	((::AVStream*)m_pPointer)->priv_data = value.ToPointer();
}
FFmpeg::AVRational^ FFmpeg::AVStream::time_base::get()
{
	return _CreateObject<AVRational>((void*)&((::AVStream*)m_pPointer)->time_base);
	//return gcnew AVRational(&((::AVStream*)m_pPointer)->time_base);
}
void FFmpeg::AVStream::time_base::set(AVRational^ value)
{
	::AVRational _rational = { value != nullptr ? value->num : 0, value != nullptr ? value->den : 0 };
	((::AVStream*)m_pPointer)->time_base = _rational;
}
Int64 FFmpeg::AVStream::start_time::get()
{
	return ((::AVStream*)m_pPointer)->start_time;
}
void FFmpeg::AVStream::start_time::set(Int64 value)
{
	((::AVStream*)m_pPointer)->start_time = value;
}
Int64 FFmpeg::AVStream::duration::get()
{
	return ((::AVStream*)m_pPointer)->duration;
}
Int64 FFmpeg::AVStream::nb_frames::get()
{
	return ((::AVStream*)m_pPointer)->nb_frames;
}
void FFmpeg::AVStream::nb_frames::set(Int64 value)
{
	((::AVStream*)m_pPointer)->nb_frames = value;
}
FFmpeg::AVDisposition FFmpeg::AVStream::disposition::get()
{
	return (FFmpeg::AVDisposition)((::AVStream*)m_pPointer)->disposition;
}
void FFmpeg::AVStream::disposition::set(AVDisposition value)
{
	((::AVStream*)m_pPointer)->disposition = (int)value;
}
FFmpeg::AVDiscard FFmpeg::AVStream::discard::get()
{
	return (FFmpeg::AVDiscard)((::AVStream*)m_pPointer)->discard;
}
void FFmpeg::AVStream::discard::set(AVDiscard value)
{
	((::AVStream*)m_pPointer)->discard = (::AVDiscard)value;
}
FFmpeg::AVRational^ FFmpeg::AVStream::sample_aspect_ratio::get()
{
	return _CreateObject<AVRational>(&((::AVStream*)m_pPointer)->sample_aspect_ratio);
}
void FFmpeg::AVStream::sample_aspect_ratio::set(FFmpeg::AVRational^ value)
{
	::AVRational _rational = { value != nullptr ? value->num : 0, value != nullptr ? value->den : 0 };
	((::AVStream*)m_pPointer)->sample_aspect_ratio = _rational;
}
FFmpeg::AVDictionary^ FFmpeg::AVStream::metadata::get()
{
	auto p = ((::AVStream*)m_pPointer)->metadata;
	return _CreateObject<AVDictionary>(p);
}

void FFmpeg::AVStream::metadata::set(AVDictionary^ value)
{
	AddObject((IntPtr)((::AVStream*)m_pPointer)->metadata,value);
	((::AVStream*)m_pPointer)->metadata = (::AVDictionary*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}

FFmpeg::AVRational^ FFmpeg::AVStream::avg_frame_rate::get()
{
	return _CreateObject<AVRational>(&((::AVStream*)m_pPointer)->avg_frame_rate);
}
void FFmpeg::AVStream::avg_frame_rate::set(AVRational^ value)
{
	::AVRational _rational = { value != nullptr ? value->num : 0, value != nullptr ? value->den : 0 };
	((::AVStream*)m_pPointer)->avg_frame_rate = _rational;
}
FFmpeg::AVPacket^ FFmpeg::AVStream::attached_pic::get()
{
	return _CreateObject<AVPacket>(&((::AVStream*)m_pPointer)->attached_pic);
}
array<FFmpeg::AVPacketSideData^>^ FFmpeg::AVStream::side_data::get()
{
	List<AVPacketSideData^>^ _array = nullptr;
	int nCount = ((::AVStream*)m_pPointer)->nb_side_data;
	::AVPacketSideData * _p = ((::AVStream*)m_pPointer)->side_data;
	if (_p && nCount > 0)
	{
		_array =  gcnew List<FFmpeg::AVPacketSideData^>();
		while (nCount-- > 0)
		{
			_array->Add(_CreateObject<AVPacketSideData>(_p++));
			//_array->Add(gcnew AVPacketSideData(_p++));
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}

FFmpeg::AVStreamEventFlag FFmpeg::AVStream::event_flags::get()
{
	return (FFmpeg::AVStreamEventFlag)((::AVStream*)m_pPointer)->event_flags;
}
void FFmpeg::AVStream::event_flags::set(AVStreamEventFlag value)
{
	((::AVStream*)m_pPointer)->event_flags = (int)value;
}
//////////////////////////////////////////////////////
FFmpeg::AVRational^ FFmpeg::AVStream::r_frame_rate::get()
{
	LOAD_API(AVFormat,::AVRational,av_stream_get_r_frame_rate,::AVStream *);
	::AVRational r = {0,0};
	if (av_stream_get_r_frame_rate)
	{
		r = av_stream_get_r_frame_rate((::AVStream*)m_pPointer);
	}
	else
	{
		r = ((::AVStream*)m_pPointer)->r_frame_rate;
	}
	return gcnew AVRational(r.num,r.den);
}
void FFmpeg::AVStream::r_frame_rate::set(AVRational^ value)
{
	::AVRational _rational = { value != nullptr ? value->num : 0, value != nullptr ? value->den : 0 };
	LOAD_API(AVFormat,void,av_stream_set_r_frame_rate,::AVStream *,::AVRational);
	if (av_stream_set_r_frame_rate)
		av_stream_set_r_frame_rate((::AVStream*)m_pPointer,_rational);
	else
		((::AVStream*)m_pPointer)->r_frame_rate = _rational;
}
String^ FFmpeg::AVStream::recommended_encoder_configuration::get()
{
	char * p = nullptr;
	LOAD_API(AVFormat, char *, av_stream_get_recommended_encoder_configuration, ::AVStream *);
	if (av_stream_get_recommended_encoder_configuration)
		p = av_stream_get_recommended_encoder_configuration((::AVStream*)m_pPointer);
	return p != nullptr ? gcnew String(p) : nullptr;
}
void FFmpeg::AVStream::recommended_encoder_configuration::set(String^ value)
{
	DYNAMIC_API(AVFormat, char *, av_stream_set_recommended_encoder_configuration, ::AVStream *,char*)
	av_stream_set_recommended_encoder_configuration((::AVStream*)m_pPointer,(char*)AllocString("recommended_encoder_configuration",value).ToPointer());	
}

FFmpeg::AVCodecParameters^ FFmpeg::AVStream::codecpar::get()
{
	return _CreateObject<AVCodecParameters>(((::AVStream*)m_pPointer)->codecpar);
}

Int64 FFmpeg::AVStream::end_pts::get()
{
	return av_stream_get_end_pts((::AVStream*)m_pPointer);
}

FFmpeg::AVRational^ FFmpeg::AVStream::codec_timebase::get()
{
	auto a = av_stream_get_codec_timebase((::AVStream*)m_pPointer);
	return gcnew AVRational(a.num,a.den);
}

//////////////////////////////////////////////////////
FFmpeg::AVCodecParserContext^ FFmpeg::AVStream::GetParser()
{
	auto p = av_stream_get_parser((::AVStream*)m_pPointer);
	return _CreateObject<AVCodecParserContext>(p);
}

FFmpeg::AVRESULT FFmpeg::AVStream::AddSizeData(AVPacketSideDataType type, IntPtr data, int size)
{
	return av_stream_add_side_data((::AVStream *)m_pPointer, (::AVPacketSideDataType)type,(uint8_t *)data.ToPointer(), (size_t)size);
}

IntPtr FFmpeg::AVStream::GetSideData(AVPacketSideDataType type, Int64 % size)
{
#if LIBAVFORMAT_VERSION_MAJOR > 58
	size_t _size = 0;
#else
	int _size = 0;
#endif
	auto p = av_stream_get_side_data((const ::AVStream*)m_pPointer,(::AVPacketSideDataType)type,&_size);
	size = p != nullptr ? (Int64)_size : 0;
	return (IntPtr)p;
}

IntPtr FFmpeg::AVStream::NewSideData(AVPacketSideDataType type, int size)
{
	return (IntPtr)av_stream_new_side_data((::AVStream*)m_pPointer,(::AVPacketSideDataType)type,size);
}

int FFmpeg::AVStream::IndexSearchTimestamp(Int64 timestamp, AVSeekFlag flags)
{
	return av_index_search_timestamp((::AVStream*)m_pPointer, timestamp,(int)flags);
}

int FFmpeg::AVStream::AddIndexEntry(Int64 pos, Int64 timestamp,int size, int distance, AVIndexFlags flags)
{
	return av_add_index_entry((::AVStream*)m_pPointer,pos, timestamp,size, distance, (int)flags);
}
//////////////////////////////////////////////////////
// AVIndexEntry
//////////////////////////////////////////////////////
FFmpeg::AVIndexEntry::AVIndexEntry(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}

FFmpeg::AVIndexEntry::AVIndexEntry()
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
}
//////////////////////////////////////////////////////
int FFmpeg::AVIndexEntry::_StructureSize::get()
{
	return sizeof(::AVIndexEntry);
}
//////////////////////////////////////////////////////
Int64 FFmpeg::AVIndexEntry::pos::get()
{
	return ((::AVIndexEntry*)m_pPointer)->pos;
}
void FFmpeg::AVIndexEntry::pos::set(Int64 value)
{
	((::AVIndexEntry*)m_pPointer)->pos = value;
}
Int64 FFmpeg::AVIndexEntry::timestamp::get()
{
	return ((::AVIndexEntry*)m_pPointer)->timestamp;
}
void FFmpeg::AVIndexEntry::timestamp::set(Int64 value)
{
	((::AVIndexEntry*)m_pPointer)->timestamp = value;
}
FFmpeg::AVIndexFlags FFmpeg::AVIndexEntry::flags::get()
{
	return (FFmpeg::AVIndexFlags)((::AVIndexEntry*)m_pPointer)->flags;
}
void FFmpeg::AVIndexEntry::flags::set(AVIndexFlags value)
{
	((::AVIndexEntry*)m_pPointer)->flags = (int)value;
}
int FFmpeg::AVIndexEntry::size::get()
{
	return ((::AVIndexEntry*)m_pPointer)->size;
}
void FFmpeg::AVIndexEntry::size::set(int value)
{
	((::AVIndexEntry*)m_pPointer)->size = value;
}
int FFmpeg::AVIndexEntry::min_distance::get()
{
	return ((::AVIndexEntry*)m_pPointer)->min_distance;
}
void FFmpeg::AVIndexEntry::min_distance::set(int value)
{
	((::AVIndexEntry*)m_pPointer)->min_distance = value;
}
//////////////////////////////////////////////////////
// AVProgram
//////////////////////////////////////////////////////
FFmpeg::AVProgram::AVProgram(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
int FFmpeg::AVProgram::id::get()
{
	return ((::AVProgram*)m_pPointer)->id;
}
void FFmpeg::AVProgram::id::set(int value)
{
	((::AVProgram*)m_pPointer)->id = value;
}
FFmpeg::AVProgramFlags FFmpeg::AVProgram::flags::get()
{
	return (FFmpeg::AVProgramFlags)((::AVProgram*)m_pPointer)->flags;
}
void FFmpeg::AVProgram::flags::set(AVProgramFlags value)
{
	((::AVProgram*)m_pPointer)->flags = (int)value;
}
FFmpeg::AVDiscard FFmpeg::AVProgram::discard::get()
{
	return (FFmpeg::AVDiscard)((::AVProgram*)m_pPointer)->discard;
}
void FFmpeg::AVProgram::discard::set(FFmpeg::AVDiscard value)
{
	((::AVProgram*)m_pPointer)->discard = (::AVDiscard)value;
}
array<UInt32>^ FFmpeg::AVProgram::stream_index::get()
{
	List<UInt32>^ _array = nullptr;
	int nCount = ((::AVProgram*)m_pPointer)->nb_stream_indexes;
	unsigned int * _p = ((::AVProgram*)m_pPointer)->stream_index;
	if (_p && nCount > 0)
	{
		_array =  gcnew List<UInt32>();
		while (nCount-- > 0)
		{
			_array->Add(*_p++);
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}
FFmpeg::AVDictionary^ FFmpeg::AVProgram::metadata::get()
{
	auto p = ((::AVProgram*)m_pPointer)->metadata;
	return _CreateObject<AVDictionary>(p);
	//return p != nullptr ? gcnew FFmpeg::AVDictionary(p) : nullptr;
}
int FFmpeg::AVProgram::program_num::get()
{
	return ((::AVProgram*)m_pPointer)->program_num;
}
void FFmpeg::AVProgram::program_num::set(int value)
{
	((::AVProgram*)m_pPointer)->program_num = value;
}
int FFmpeg::AVProgram::pmt_pid::get()
{
	return ((::AVProgram*)m_pPointer)->pmt_pid;
}
void FFmpeg::AVProgram::pmt_pid::set(int value)
{ 
	((::AVProgram*)m_pPointer)->pmt_pid = value;
}
int FFmpeg::AVProgram::pcr_pid::get()
{
	return ((::AVProgram*)m_pPointer)->pcr_pid;
}
void FFmpeg::AVProgram::pcr_pid::set(int value)
{
	((::AVProgram*)m_pPointer)->pcr_pid = value;
}
int FFmpeg::AVProgram::pmt_version::get()
{
	return ((::AVProgram*)m_pPointer)->pmt_version;
}
void FFmpeg::AVProgram::pmt_version::set(int value)
{
	((::AVProgram*)m_pPointer)->pmt_version = value;
}
//////////////////////////////////////////////////////
void FFmpeg::AVProgram::AddStreamIndex(UInt32 idx)
{
	av_program_add_stream_index((::AVFormatContext*)m_pParent->_Pointer.ToPointer(), ((::AVProgram*)m_pPointer)->id, idx);
}
//////////////////////////////////////////////////////
// AVChapter
//////////////////////////////////////////////////////
FFmpeg::AVChapter::AVChapter(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
FFmpeg::AVChapter::AVChapter()
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
}
//////////////////////////////////////////////////////
int FFmpeg::AVChapter::_StructureSize::get()
{
	return sizeof(::AVChapter);
}
//////////////////////////////////////////////////////
Int64 FFmpeg::AVChapter::id::get()
{
	return ((::AVChapter*)m_pPointer)->id;
}
void FFmpeg::AVChapter::id::set(Int64 value)
{

#if (LIBAVFORMAT_VERSION_MAJOR < 59)
#	define INT_TYPE int
#else
#	define INT_TYPE int64_t
#endif
	((::AVChapter*)m_pPointer)->id = (INT_TYPE)value;
#undef INT_TYPE
}
FFmpeg::AVRational^ FFmpeg::AVChapter::time_base::get()
{
	return _CreateObject<FFmpeg::AVRational>(&((::AVChapter*)m_pPointer)->time_base);
}
void FFmpeg::AVChapter::time_base::set(AVRational^  value)
{
	::AVRational _rational = { value != nullptr ? value->num : 0, value != nullptr ? value->den : 0 };
	((::AVChapter*)m_pPointer)->time_base = _rational;
}
Int64 FFmpeg::AVChapter::start::get()
{
	return ((::AVChapter*)m_pPointer)->start;
}
void FFmpeg::AVChapter::start::set(Int64 value)
{
	((::AVChapter*)m_pPointer)->start = value;
}
Int64 FFmpeg::AVChapter::end::get()
{
	return ((::AVChapter*)m_pPointer)->end;
}
void FFmpeg::AVChapter::end::set(Int64 value)
{
	((::AVChapter*)m_pPointer)->end = value;
}
FFmpeg::AVDictionary^ FFmpeg::AVChapter::metadata::get()
{
	auto p = ((::AVChapter*)m_pPointer)->metadata;
	return _CreateObject<FFmpeg::AVDictionary>(p);
	//return p != nullptr ? gcnew FFmpeg::AVDictionary(p) : nullptr;
}

void FFmpeg::AVChapter::metadata::set(AVDictionary^ value)
{
	AddObject((IntPtr)((::AVChapter*)m_pPointer)->metadata,value);
	((::AVChapter*)m_pPointer)->metadata = (::AVDictionary*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}
//////////////////////////////////////////////////////
// AVStreamsEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVFormatContext::AVStreams::AVStreamsEnumerator::AVStreamsEnumerator(AVStreams^ streams)
	: m_pParent(streams)
	, m_nIndex(-1)
{

}
FFmpeg::AVFormatContext::AVStreams::AVStreamsEnumerator::~AVStreamsEnumerator()
{

}
//////////////////////////////////////////////////////
bool FFmpeg::AVFormatContext::AVStreams::AVStreamsEnumerator::MoveNext()
{
	if (m_nIndex + 1 >= m_pParent->Count) return false;
	m_nIndex++;
	return true;
}

FFmpeg::AVStream^ FFmpeg::AVFormatContext::AVStreams::AVStreamsEnumerator::Current::get()
{
	if (m_nIndex == -1)
	{
		if (!MoveNext()) return nullptr;
	}
	return m_pParent[m_nIndex];
}
void FFmpeg::AVFormatContext::AVStreams::AVStreamsEnumerator::Reset()
{
	m_nIndex = -1;
}
Object^ FFmpeg::AVFormatContext::AVStreams::AVStreamsEnumerator::CurrentObject::get()
{
	return Current::get(); 
}
//////////////////////////////////////////////////////
// AVFormatContext
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVFormatContext::Class::get()
{
	auto p = avformat_get_class();
	return p != nullptr ? gcnew AVClass((void*)p,nullptr) : nullptr;
}
//////////////////////////////////////////////////////
FFmpeg::AVFormatContext::AVFormatContext(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
	, m_pInterruptCB(nullptr)
{
	
}

FFmpeg::AVFormatContext::AVFormatContext()
	: AVBase(nullptr,nullptr)
{
	LibAVFormat::RegisterAll();
	m_pPointer = avformat_alloc_context();
	if (m_pPointer != nullptr) m_pFree = (TFreeFN*)avformat_free_context;
}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVFormatContext::av_class::get()
{
	auto p = ((::AVFormatContext*)m_pPointer)->av_class;
	return _CreateObject<AVClass>((void*)p);
}

FFmpeg::AVInputFormat^ FFmpeg::AVFormatContext::iformat::get()
{ 
	auto p = ((::AVFormatContext*)m_pPointer)->iformat;
	return _CreateObject<AVInputFormat>((void*)p);
}

void FFmpeg::AVFormatContext::iformat::set(AVInputFormat^ value)
{
	AddObject((IntPtr)(void*)((const ::AVFormatContext*)m_pPointer)->iformat,value);
	((::AVFormatContext*)m_pPointer)->iformat = (::AVInputFormat*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}

FFmpeg::AVOutputFormat^ FFmpeg::AVFormatContext::oformat::get()
{
	auto p = ((::AVFormatContext*)m_pPointer)->oformat;
	return _CreateObject<AVOutputFormat>((void*)p);
}

void FFmpeg::AVFormatContext::oformat::set(AVOutputFormat^ value)
{
	AddObject((IntPtr)(void*)((const ::AVFormatContext*)m_pPointer)->oformat,value);
	((::AVFormatContext*)m_pPointer)->oformat = (::AVOutputFormat*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);
}

IntPtr FFmpeg::AVFormatContext::priv_data::get()
{
	return (IntPtr)((::AVFormatContext*)m_pPointer)->priv_data;
}

FFmpeg::AVIOContext^ FFmpeg::AVFormatContext::pb::get()
{
	auto p = ((::AVFormatContext*)m_pPointer)->pb;
	return _CreateObject<AVIOContext>((void*)p);
}

void FFmpeg::AVFormatContext::pb::set(AVIOContext^ value)
{
	auto p = ((::AVFormatContext*)m_pPointer);
	AddObject((IntPtr)((::AVFormatContext*)m_pPointer)->pb,value);
	((::AVFormatContext*)m_pPointer)->pb = value != nullptr ? (::AVIOContext*)value->_Pointer.ToPointer() : nullptr;
}

FFmpeg::AVFMTCTX FFmpeg::AVFormatContext::ctx_flags::get()
{
	return (FFmpeg::AVFMTCTX)((::AVFormatContext*)m_pPointer)->ctx_flags;
}

int FFmpeg::AVFormatContext::nb_streams::get()
{ 
	return (int)((::AVFormatContext*)m_pPointer)->nb_streams;
}

FFmpeg::AVFormatContext::AVStreams^ FFmpeg::AVFormatContext::streams::get()
{
	return gcnew AVStreams(this);
}

String^ FFmpeg::AVFormatContext::filename::get()
{
#if FF_API_FORMAT_FILENAME
	return gcnew String(((::AVFormatContext*)m_pPointer)->filename);
#else
	return url::get();
#endif
}

void FFmpeg::AVFormatContext::filename::set(String^ value)
{
#if FF_API_FORMAT_FILENAME
	char * _temp = nullptr;
	try
	{
		_temp = (char*)AllocString(value).ToPointer();
		if (_temp)
		{
			strcpy_s(((::AVFormatContext*)m_pPointer)->filename,_temp);
		}
		else
		{
			memset(((::AVFormatContext*)m_pPointer)->filename,0x00,sizeof(((::AVFormatContext*)m_pPointer)->filename));
		}
	}
	finally
	{
		FreeMemory((IntPtr)_temp);
	}
#else
	url::set(value);
#endif
}

String^ FFmpeg::AVFormatContext::url::get() 
{
	return gcnew String(((::AVFormatContext*)m_pPointer)->url);
}

void FFmpeg::AVFormatContext::url::set(String^ value)
{
	char * _temp = nullptr;
	try
	{
		::AVFormatContext* p = ((::AVFormatContext*)m_pPointer);
		if (p->url)
		{
			av_freep(&p->url);
		}
		_temp = (char*)AllocString(value).ToPointer();
		if (_temp)
		{
			size_t cch = strlen(_temp) + 1;
			p->url = (char*)av_mallocz(cch);
			strcpy_s(p->url,cch,_temp);
		}
	}
	finally
	{
		FreeMemory((IntPtr)_temp);
	}
}

Int64 FFmpeg::AVFormatContext::start_time::get()
{
	return ((::AVFormatContext*)m_pPointer)->start_time;
}

Int64 FFmpeg::AVFormatContext::duration::get()
{
	return ((::AVFormatContext*)m_pPointer)->duration;
}

Int64 FFmpeg::AVFormatContext::bit_rate::get()
{
	return ((::AVFormatContext*)m_pPointer)->bit_rate;
}

void FFmpeg::AVFormatContext::bit_rate::set(Int64 value)
{
	((::AVFormatContext*)m_pPointer)->bit_rate = value;
}

UInt32 FFmpeg::AVFormatContext::packet_size::get()
{
	return ((::AVFormatContext*)m_pPointer)->packet_size;
}

void  FFmpeg::AVFormatContext::packet_size::set(UInt32 value)
{
	((::AVFormatContext*)m_pPointer)->packet_size = value;
}

int FFmpeg::AVFormatContext::max_delay::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"max_delay",0,&val))
	{
		return (int)val;
	}
	return ((::AVFormatContext*)m_pPointer)->max_delay;
}

void FFmpeg::AVFormatContext::max_delay::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"max_delay",value,0))
	{
		((::AVFormatContext*)m_pPointer)->max_delay = value;
	}
}

FFmpeg::AVfmtFlag FFmpeg::AVFormatContext::flags::get()
{
	return (FFmpeg::AVfmtFlag)((::AVFormatContext*)m_pPointer)->flags;
}

void FFmpeg::AVFormatContext::flags::set(AVfmtFlag value)
{
	((::AVFormatContext*)m_pPointer)->flags = (int)value;
}

Int64 FFmpeg::AVFormatContext::probesize::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"probesize",0,&val))
	{
		return val;
	}
	return 0;//((::AVFormatContext*)m_pPointer)->probesize2;
}

void FFmpeg::AVFormatContext::probesize::set(Int64 value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"probesize",value,0))
	{
		//((::AVFormatContext*)m_pPointer)->probesize2 = value;
	}
}

Int64 FFmpeg::AVFormatContext::max_analyze_duration::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"analyzeduration",0,&val))
	{
		return val;
	}
	return 0;//((::AVFormatContext*)m_pPointer)->max_analyze_duration2;
}

void FFmpeg::AVFormatContext::max_analyze_duration::set(Int64 value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"analyzeduration",value,0))
	{
		//((::AVFormatContext*)m_pPointer)->max_analyze_duration2 = value;
	}
}

array<byte>^ FFmpeg::AVFormatContext::key::get()
{
	List<byte>^ _array = nullptr;
	int nCount = ((::AVFormatContext*)m_pPointer)->keylen;
	uint8_t * p = (uint8_t *)((::AVFormatContext*)m_pPointer)->key;
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

void FFmpeg::AVFormatContext::key::set(array<byte>^ value)
{
	if (value != nullptr)
	{
		uint8_t * p = nullptr;
		try
		{
			const size_t _length = value->Length;
			p = (uint8_t *)av_mallocz(_length);
			Marshal::Copy(value,0,(IntPtr)p,(int)_length);
			av_opt_set_bin(m_pPointer,"cryptokey",p,(int)_length,0);
		}
		finally
		{
			if (p) av_free(p);
		}
	}
}

UInt32 FFmpeg::AVFormatContext::nb_programs::get()
{
	return ((::AVFormatContext*)m_pPointer)->nb_programs;
}

FFmpeg::AVProgram^ FFmpeg::AVFormatContext::programs::get(int idx)
{
	if (((::AVFormatContext*)m_pPointer)->nb_programs <= (unsigned int)idx || idx < 0) return nullptr;
	auto p = ((::AVFormatContext*)m_pPointer)->programs[idx];
	return _CreateObject<AVProgram>((void*)p);
}

FFmpeg::AVCodecID FFmpeg::AVFormatContext::video_codec_id::get()
{
	return (FFmpeg::AVCodecID)((::AVFormatContext*)m_pPointer)->video_codec_id;
}

void FFmpeg::AVFormatContext::video_codec_id::set(AVCodecID value)
{
	((::AVFormatContext*)m_pPointer)->video_codec_id = (::AVCodecID)value;
}

FFmpeg::AVCodecID FFmpeg::AVFormatContext::audio_codec_id::get()
{
	return (FFmpeg::AVCodecID)((::AVFormatContext*)m_pPointer)->audio_codec_id;
}
void FFmpeg::AVFormatContext::audio_codec_id::set(AVCodecID value)
{
	((::AVFormatContext*)m_pPointer)->audio_codec_id = (::AVCodecID)value;
}

FFmpeg::AVCodecID FFmpeg::AVFormatContext::subtitle_codec_id::get()
{
	return (FFmpeg::AVCodecID)((::AVFormatContext*)m_pPointer)->subtitle_codec_id;
}
void FFmpeg::AVFormatContext::subtitle_codec_id::set(AVCodecID value)
{
	((::AVFormatContext*)m_pPointer)->subtitle_codec_id = (::AVCodecID)value;
}

UInt32 FFmpeg::AVFormatContext::max_index_size::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"indexmem",0,&val))
	{
		return (UInt32)val;
	}
	return ((::AVFormatContext*)m_pPointer)->max_index_size;
}
void FFmpeg::AVFormatContext::max_index_size::set(UInt32 value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"indexmem",value,0))
	{
		((::AVFormatContext*)m_pPointer)->max_index_size = value;
	}
}

UInt32 FFmpeg::AVFormatContext::max_picture_buffer::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"rtbufsize",0,&val))
	{
		return (UInt32)val;
	}
	return ((::AVFormatContext*)m_pPointer)->max_picture_buffer;
}

void FFmpeg::AVFormatContext::max_picture_buffer::set(UInt32 value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"rtbufsize",value,0))
	{
		((::AVFormatContext*)m_pPointer)->max_picture_buffer = value;
	}
}

array<FFmpeg::AVChapter^>^ FFmpeg::AVFormatContext::chapters::get()
{
	List<FFmpeg::AVChapter^>^ _array = nullptr;
	int nCount = ((::AVFormatContext*)m_pPointer)->nb_chapters;
	::AVChapter ** p = (::AVChapter **)((::AVFormatContext*)m_pPointer)->chapters;
	if (p && nCount > 0)
	{
		_array =  gcnew List<FFmpeg::AVChapter^>();
		while (nCount-- > 0)
		{
			_array->Add(_CreateObject<AVChapter>(*p++));
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}

void FFmpeg::AVFormatContext::chapters::set(array<AVChapter^>^ value)
{
	{
		int nCount = ((::AVFormatContext*)m_pPointer)->nb_chapters;
		::AVChapter ** p = (::AVChapter **)((::AVFormatContext*)m_pPointer)->chapters;
		if (p)
		{
			while (nCount-- > 0)
			{
				RemoveObject((IntPtr)*p++);
			}
		}
		((::AVFormatContext*)m_pPointer)->nb_chapters = 0;
		((::AVFormatContext*)m_pPointer)->chapters = nullptr;
		FreeMemory("chapters");
	}
	if (value != nullptr && value->Length > 0)
	{
		::AVChapter ** p = (::AVChapter **)AllocMemory("chapters",value->Length * (sizeof(::AVChapter*))).ToPointer();
		if (p)
		{
			((::AVFormatContext*)m_pPointer)->chapters = p;
			((::AVFormatContext*)m_pPointer)->nb_chapters = value->Length;
			for (int i = 0; i < value->Length; i++)
			{
				AddObject((IntPtr)*p,value[i]);
				*p++ = (::AVChapter*)value[i]->_Pointer.ToPointer();
			}
		}
	}
}

FFmpeg::AVDictionary^ FFmpeg::AVFormatContext::metadata::get()
{ 
	auto p = ((::AVFormatContext*)m_pPointer)->metadata;
	return _CreateObject<AVDictionary>(p);
}

void FFmpeg::AVFormatContext::metadata::set(AVDictionary^ value)
{
	AddObject((IntPtr)((::AVFormatContext*)m_pPointer)->metadata,value);
	((::AVFormatContext*)m_pPointer)->metadata = (::AVDictionary*)(value != nullptr ? value->_Pointer.ToPointer() : nullptr);	
}

Int64 FFmpeg::AVFormatContext::start_time_realtime::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"start_time_realtime",0,&val))
	{
		return val;
	}
	return ((::AVFormatContext*)m_pPointer)->start_time_realtime;
}

void FFmpeg::AVFormatContext::start_time_realtime::set(Int64 value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"start_time_realtime",value,0))
	{
		((::AVFormatContext*)m_pPointer)->start_time_realtime = value;
	}
}

int FFmpeg::AVFormatContext::fps_probe_size::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"fpsprobesize",0,&val))
	{
		return (int)val;
	}
	return ((::AVFormatContext*)m_pPointer)->fps_probe_size;
}

void FFmpeg::AVFormatContext::fps_probe_size::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"fpsprobesize",value,0))
	{
		((::AVFormatContext*)m_pPointer)->fps_probe_size = value;
	}
}

int FFmpeg::AVFormatContext::error_recognition::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"err_detect",0,&val))
	{
		return (int)val;
	}
	return ((::AVFormatContext*)m_pPointer)->error_recognition;
}

void FFmpeg::AVFormatContext::error_recognition::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"err_detect",value,0))
	{
		((::AVFormatContext*)m_pPointer)->error_recognition = value;
	}
}

FFmpeg::AVIOInterruptDesc^ FFmpeg::AVFormatContext::interrupt_callback::get()
{
	return m_pInterruptCB;
}

void FFmpeg::AVFormatContext::interrupt_callback::set(AVIOInterruptDesc^ value)
{
	if (m_pInterruptCB != value)
	{
		m_pInterruptCB = value;
		IntPtr _ptr = (value != nullptr ? GCHandle::ToIntPtr(GCHandle::Alloc(value,GCHandleType::Weak)) : IntPtr::Zero);
		((::AVFormatContext*)m_pPointer)->interrupt_callback.opaque = _ptr.ToPointer();
		((::AVFormatContext*)m_pPointer)->interrupt_callback.callback = m_pInterruptCB == nullptr ? nullptr : Generic_interrupt_callback;
	}
}

FFmpeg::FDebugFlags FFmpeg::AVFormatContext::debug::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"fdebug",0,&val))
	{
		return (FFmpeg::FDebugFlags)val;
	}
	return (FFmpeg::FDebugFlags)((::AVFormatContext*)m_pPointer)->debug;
}

void FFmpeg::AVFormatContext::debug::set(FDebugFlags value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"fdebug",(int64_t)value,0))
	{
		((::AVFormatContext*)m_pPointer)->debug = (int)value;
	}
}

Int64 FFmpeg::AVFormatContext::max_interleave_delta::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"max_interleave_delta",0,&val))
	{
		return val;
	}
	return ((::AVFormatContext*)m_pPointer)->max_interleave_delta;
}

void FFmpeg::AVFormatContext::max_interleave_delta::set(Int64 value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"max_interleave_delta",value,0))
	{
		((::AVFormatContext*)m_pPointer)->max_interleave_delta = value;
	}
}

FFmpeg::FFCompliance FFmpeg::AVFormatContext::strict_std_compliance::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"strict",0,&val))
	{
		return (FFmpeg::FFCompliance)val;
	}
	return (FFmpeg::FFCompliance)((::AVFormatContext*)m_pPointer)->strict_std_compliance;
}
void FFmpeg::AVFormatContext::strict_std_compliance::set(FFCompliance value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"strict",(int64_t)value,0))
	{
		((::AVFormatContext*)m_pPointer)->strict_std_compliance = (int)value;
	}
}

FFmpeg::AVFmtEventFlags FFmpeg::AVFormatContext::event_flags::get()
{
	return (FFmpeg::AVFmtEventFlags)((::AVFormatContext*)m_pPointer)->event_flags;
}
void FFmpeg::AVFormatContext::event_flags::set(AVFmtEventFlags value)
{
	((::AVFormatContext*)m_pPointer)->event_flags = (int)value;
}

int FFmpeg::AVFormatContext::max_ts_probe::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"max_ts_probe",0,&val))
	{
		return (int)val;
	}
	return ((::AVFormatContext*)m_pPointer)->max_ts_probe;
}
void FFmpeg::AVFormatContext::max_ts_probe::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"max_ts_probe",(int64_t)value,0))
	{
		((::AVFormatContext*)m_pPointer)->max_ts_probe = (int)value;
	}
}

FFmpeg::AVFmtAvoidNegTS FFmpeg::AVFormatContext::avoid_negative_ts::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"avoid_negative_ts",0,&val))
	{
		return (FFmpeg::AVFmtAvoidNegTS)val;
	}
	return (FFmpeg::AVFmtAvoidNegTS)((::AVFormatContext*)m_pPointer)->avoid_negative_ts;
}

void FFmpeg::AVFormatContext::avoid_negative_ts::set(AVFmtAvoidNegTS value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"avoid_negative_ts",(int64_t)value,0))
	{
		((::AVFormatContext*)m_pPointer)->avoid_negative_ts = (int)value;
	}
}

int FFmpeg::AVFormatContext::ts_id::get()
{
	return ((::AVFormatContext*)m_pPointer)->ts_id;
}
void FFmpeg::AVFormatContext::ts_id::set(int value)
{
	((::AVFormatContext*)m_pPointer)->ts_id = value;
}

int FFmpeg::AVFormatContext::audio_preload::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"audio_preload",0,&val))
	{
		return (int)val;
	}
	return ((::AVFormatContext*)m_pPointer)->audio_preload;
}

void FFmpeg::AVFormatContext::audio_preload::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"audio_preload",(int64_t)value,0))
	{
		((::AVFormatContext*)m_pPointer)->audio_preload = (int)value;
	}
}

int FFmpeg::AVFormatContext::max_chunk_duration::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"chunk_duration",0,&val))
	{
		return (int)val;
	}
	return ((::AVFormatContext*)m_pPointer)->max_chunk_duration;
}

void FFmpeg::AVFormatContext::max_chunk_duration::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"chunk_duration",(int64_t)value,0))
	{
		((::AVFormatContext*)m_pPointer)->max_chunk_duration = (int)value;
	}
}

int FFmpeg::AVFormatContext::max_chunk_size::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"chunk_size",0,&val))
	{
		return (int)val;
	}
	return ((::AVFormatContext*)m_pPointer)->max_chunk_size;
}
void FFmpeg::AVFormatContext::max_chunk_size::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"chunk_size",(int64_t)value,0))
	{
		((::AVFormatContext*)m_pPointer)->max_chunk_size = (int)value;
	}
}

int FFmpeg::AVFormatContext::use_wallclock_as_timestamps::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"use_wallclock_as_timestamps",0,&val))
	{
		return (int)val;
	}
	return ((::AVFormatContext*)m_pPointer)->use_wallclock_as_timestamps;
}
void FFmpeg::AVFormatContext::use_wallclock_as_timestamps::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"use_wallclock_as_timestamps",(int64_t)value,0))
	{
		((::AVFormatContext*)m_pPointer)->use_wallclock_as_timestamps = (int)value;
	}
}

FFmpeg::AvioFlag FFmpeg::AVFormatContext::avio_flags::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"avioflags",0,&val))
	{
		return (FFmpeg::AvioFlag)val;
	}
	return (FFmpeg::AvioFlag)((::AVFormatContext*)m_pPointer)->avio_flags;
}
void FFmpeg::AVFormatContext::avio_flags::set(FFmpeg::AvioFlag value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"avioflags",(int64_t)value,0))
	{
		((::AVFormatContext*)m_pPointer)->avio_flags = (int)value;
	}
}

FFmpeg::AVDurationEstimationMethod FFmpeg::AVFormatContext::duration_estimation_method::get()
{
	return (FFmpeg::AVDurationEstimationMethod)av_fmt_ctx_get_duration_estimation_method((::AVFormatContext*)m_pPointer);
}

Int64 FFmpeg::AVFormatContext::skip_initial_bytes::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"skip_initial_bytes",0,&val))
	{
		return val;
	}
	return ((::AVFormatContext*)m_pPointer)->skip_initial_bytes;
}
void FFmpeg::AVFormatContext::skip_initial_bytes::set(Int64 value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"skip_initial_bytes",(int64_t)value,0))
	{
		((::AVFormatContext*)m_pPointer)->skip_initial_bytes = value;
	}
}

bool FFmpeg::AVFormatContext::correct_ts_overflow::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"correct_ts_overflow",0,&val))
	{
		return val != 0;
	}
	return ((::AVFormatContext*)m_pPointer)->correct_ts_overflow != 0;
}
void FFmpeg::AVFormatContext::correct_ts_overflow::set(bool value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"correct_ts_overflow",(int64_t)(value ? 1 : 0),0))
	{
		((::AVFormatContext*)m_pPointer)->correct_ts_overflow = value ? 1 : 0;
	}
}

bool FFmpeg::AVFormatContext::seek2any::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"seek2any",0,&val))
	{
		return val != 0;
	}
	return ((::AVFormatContext*)m_pPointer)->seek2any != 0;
}
void FFmpeg::AVFormatContext::seek2any::set(bool value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"seek2any",(int64_t)(value ? 1 : 0),0))
	{
		((::AVFormatContext*)m_pPointer)->seek2any = value ? 1 : 0;
	}
}

bool FFmpeg::AVFormatContext::flush_packets::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"flush_packets",0,&val))
	{
		return val != 0;
	}
	return ((::AVFormatContext*)m_pPointer)->flush_packets != 0;
}
void FFmpeg::AVFormatContext::flush_packets::set(bool value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"flush_packets",(int64_t)(value ? 1 : 0),0))
	{
		((::AVFormatContext*)m_pPointer)->flush_packets = value ? 1 : 0;
	}
}

int FFmpeg::AVFormatContext::probe_score::get()
{
	DYNAMIC_DEF_API(AVFormat,int,((::AVFormatContext*)m_pPointer)->probe_score,av_format_get_probe_score,::AVFormatContext *);
	return av_format_get_probe_score((::AVFormatContext*)m_pPointer);
}

int FFmpeg::AVFormatContext::format_probesize::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"formatprobesize",0,&val))
	{
		return (int)val;
	}
	return ((::AVFormatContext*)m_pPointer)->format_probesize;
}
void FFmpeg::AVFormatContext::format_probesize::set(int value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"formatprobesize",(int64_t)value,0))
	{
		((::AVFormatContext*)m_pPointer)->format_probesize = value;
	}
}

String^ FFmpeg::AVFormatContext::codec_whitelist::get()
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
void FFmpeg::AVFormatContext::codec_whitelist::set(String^ value)
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

String^ FFmpeg::AVFormatContext::format_whitelist::get()
{
	uint8_t * val = nullptr;
	if (0 >= av_opt_get(m_pPointer,"format_whitelist",0,&val))
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
void FFmpeg::AVFormatContext::format_whitelist::set(String^ value)
{
	char * _temp = nullptr;
	try
	{
		_temp = (char*)AllocString(value).ToPointer();
		av_opt_set(m_pPointer,"format_whitelist",_temp,0);
	}
	finally
	{
		FreeMemory((IntPtr)_temp);
	}
}

int FFmpeg::AVFormatContext::io_repositioned::get()
{
	return ((::AVFormatContext*)m_pPointer)->io_repositioned;
}

FFmpeg::AVCodec^ FFmpeg::AVFormatContext::video_codec::get()
{
	const ::AVCodec * p = ((::AVFormatContext*)m_pPointer)->video_codec;
	LOAD_API(AVFormat,const ::AVCodec *,av_format_get_video_codec,::AVFormatContext *);
	if (av_format_get_video_codec)
	{
		p = av_format_get_video_codec((::AVFormatContext*)m_pPointer);
	}
	return _CreateObject<AVCodec>((void*)p);
}

void FFmpeg::AVFormatContext::video_codec::set(AVCodec^ value)
{
	LOAD_API(AVFormat,void,av_format_set_video_codec,::AVFormatContext *,::AVCodec *);
	if (av_format_set_video_codec)
	{
		av_format_set_video_codec((::AVFormatContext*)m_pPointer, value != nullptr ? (::AVCodec*)value->_Pointer.ToPointer() : nullptr);
	}
	else
	{
		((::AVFormatContext*)m_pPointer)->video_codec = value != nullptr ? (::AVCodec*)value->_Pointer.ToPointer() : nullptr;
	}
}

FFmpeg::AVCodec^ FFmpeg::AVFormatContext::audio_codec::get()
{
	const ::AVCodec * p = ((::AVFormatContext*)m_pPointer)->audio_codec;
	LOAD_API(AVFormat,const ::AVCodec *,av_format_get_audio_codec,::AVFormatContext *);
	if (av_format_get_audio_codec)
	{
		p = av_format_get_audio_codec((::AVFormatContext*)m_pPointer);
	}
	return _CreateObject<AVCodec>((void*)p);
}

void FFmpeg::AVFormatContext::audio_codec::set(AVCodec^ value)
{
	LOAD_API(AVFormat,void,av_format_set_audio_codec,::AVFormatContext *,::AVCodec *);
	if (av_format_set_audio_codec)
	{
		av_format_set_audio_codec((::AVFormatContext*)m_pPointer, value != nullptr ? (::AVCodec*)value->_Pointer.ToPointer() : nullptr);
	}
	else
	{
		((::AVFormatContext*)m_pPointer)->audio_codec = value != nullptr ? (::AVCodec*)value->_Pointer.ToPointer() : nullptr;
	}
}

FFmpeg::AVCodec^ FFmpeg::AVFormatContext::subtitle_codec::get()
{
	const ::AVCodec * p = ((::AVFormatContext*)m_pPointer)->subtitle_codec;
	LOAD_API(AVFormat,const ::AVCodec *,av_format_get_subtitle_codec,::AVFormatContext *);
	if (av_format_get_subtitle_codec)
	{
		p = av_format_get_subtitle_codec((::AVFormatContext*)m_pPointer);
	}
	return _CreateObject<AVCodec>((void*)p);
}
void FFmpeg::AVFormatContext::subtitle_codec::set(AVCodec^ value)
{
	LOAD_API(AVFormat,void,av_format_set_subtitle_codec,::AVFormatContext *,::AVCodec *);
	if (av_format_set_subtitle_codec)
	{
		av_format_set_subtitle_codec((::AVFormatContext*)m_pPointer, value != nullptr ? (::AVCodec*)value->_Pointer.ToPointer() : nullptr);
	}
	else
	{
		((::AVFormatContext*)m_pPointer)->subtitle_codec = value != nullptr ? (::AVCodec*)value->_Pointer.ToPointer() : nullptr;
	}
}

FFmpeg::AVCodec^ FFmpeg::AVFormatContext::data_codec::get()
{
	const ::AVCodec * p = ((::AVFormatContext*)m_pPointer)->subtitle_codec;
	LOAD_API(AVFormat,const ::AVCodec *,av_format_get_data_codec,::AVFormatContext *);
	if (av_format_get_data_codec)
	{
		p = av_format_get_data_codec((::AVFormatContext*)m_pPointer);
	}
	return _CreateObject<AVCodec>((void*)p);
}
void FFmpeg::AVFormatContext::data_codec::set(AVCodec^ value)
{
	LOAD_API(AVFormat,void,av_format_set_data_codec,::AVFormatContext *,::AVCodec *);
	if (av_format_set_data_codec)
	{
		av_format_set_data_codec((::AVFormatContext*)m_pPointer, value != nullptr ? (::AVCodec*)value->_Pointer.ToPointer() : nullptr);
	}
	else
	{
		((::AVFormatContext*)m_pPointer)->data_codec = value != nullptr ? (::AVCodec*)value->_Pointer.ToPointer() : nullptr;
	}
}

int FFmpeg::AVFormatContext::metadata_header_padding::get()
{
	DYNAMIC_DEF_API(AVFormat,int,((::AVFormatContext*)m_pPointer)->metadata_header_padding,av_format_get_metadata_header_padding,::AVFormatContext *);
	return av_format_get_metadata_header_padding((::AVFormatContext*)m_pPointer);
}
void FFmpeg::AVFormatContext::metadata_header_padding::set(int value)
{
	LOAD_API(AVFormat, void, av_format_set_metadata_header_padding, ::AVFormatContext *, int);
	if (av_format_set_metadata_header_padding)
		av_format_set_metadata_header_padding((::AVFormatContext*)m_pPointer, value);
	else
		((::AVFormatContext*)m_pPointer)->metadata_header_padding = value;
}

IntPtr FFmpeg::AVFormatContext::opaque::get()
{
	DYNAMIC_DEF_API(AVFormat,void*,(IntPtr)((::AVFormatContext*)m_pPointer)->opaque,av_format_get_opaque,::AVFormatContext *);
	return (IntPtr)av_format_get_opaque((::AVFormatContext*)m_pPointer);
}

void FFmpeg::AVFormatContext::opaque::set(IntPtr value)
{
	LOAD_API(AVFormat, void, av_format_set_opaque, ::AVFormatContext *, void*);
	if (av_format_set_opaque)
		av_format_set_opaque((::AVFormatContext*)m_pPointer, value.ToPointer());
	else
		((::AVFormatContext*)m_pPointer)->opaque = value.ToPointer();
}

Int64 FFmpeg::AVFormatContext::output_ts_offset::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"output_ts_offset",0,&val))
	{
		return val;
	}
	return ((::AVFormatContext*)m_pPointer)->output_ts_offset;
}
void FFmpeg::AVFormatContext::output_ts_offset::set(Int64 value)
{
	if (AVERROR_OPTION_NOT_FOUND == av_opt_set_int(m_pPointer,"output_ts_offset",(int64_t)value,0))
	{
		((::AVFormatContext*)m_pPointer)->output_ts_offset = value;
	}
}

String^ FFmpeg::AVFormatContext::dump_separator::get()
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

void FFmpeg::AVFormatContext::dump_separator::set(String^ value)
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

FFmpeg::AVCodecID FFmpeg::AVFormatContext::data_codec_id::get()
{
	return (FFmpeg::AVCodecID)((::AVFormatContext*)m_pPointer)->data_codec_id;
}
void FFmpeg::AVFormatContext::data_codec_id::set(AVCodecID value)
{
	((::AVFormatContext*)m_pPointer)->data_codec_id = (::AVCodecID)value;
}

String^ FFmpeg::AVFormatContext::protocol_whitelist::get()
{
	uint8_t * val = nullptr;
	if (0 >= av_opt_get(m_pPointer,"protocol_whitelist",0,&val))
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

void FFmpeg::AVFormatContext::protocol_whitelist::set(String^ value)
{
	char * _temp = nullptr;
	try
	{
		_temp = (char*)AllocString(value).ToPointer();
		av_opt_set(m_pPointer,"protocol_whitelist",_temp,0);
	}
	finally
	{
		FreeMemory((IntPtr)_temp);
	}
}
String^ FFmpeg::AVFormatContext::protocol_blacklist::get()
{
	uint8_t * val = nullptr;
	if (0 >= av_opt_get(m_pPointer,"protocol_blacklist",0,&val))
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

void FFmpeg::AVFormatContext::protocol_blacklist::set(String^ value)
{
	char * _temp = nullptr;
	try
	{
		_temp = (char*)AllocString(value).ToPointer();
		av_opt_set(m_pPointer,"protocol_blacklist",_temp,0);
	}
	finally
	{
		FreeMemory((IntPtr)_temp);
	}
}

int FFmpeg::AVFormatContext::max_streams::get()
{
	return ((::AVFormatContext*)m_pPointer)->max_streams;
}

void FFmpeg::AVFormatContext::max_streams::set(int value)
{ 
	((::AVFormatContext*)m_pPointer)->max_streams = value;
}
//////////////////////////////////////////////////////
FFmpeg::AVStream^ FFmpeg::AVFormatContext::GetStream(int idx)
{
	if (((::AVFormatContext*)m_pPointer)->nb_streams <= (unsigned int)idx || idx < 0) return nullptr;
	auto p = ((::AVFormatContext*)m_pPointer)->streams[idx];
	return _CreateObject<AVStream>((void*)p);
}
//////////////////////////////////////////////////////
FFmpeg::AVStream^ FFmpeg::AVFormatContext::AddStream()
{
	return AddStream(nullptr);
}

FFmpeg::AVStream^ FFmpeg::AVFormatContext::AddStream(AVCodec^ c)
{
	auto p = avformat_new_stream((::AVFormatContext*)m_pPointer,c != nullptr ? (::AVCodec*)c->_Pointer.ToPointer() : nullptr);
	//return p != nullptr ? gcnew FFmpeg::AVStream(p) : nullptr;
	return _CreateObject<AVStream>((void*)p);
}

FFmpeg::AVProgram^ FFmpeg::AVFormatContext::AddProgram(int id)
{
	auto p = av_new_program((::AVFormatContext*)m_pPointer,id);
	return _CreateObject<AVProgram>((void*)p);
	//return p != nullptr ? gcnew FFmpeg::AVProgram(p) : nullptr;
}
//////////////////////////////////////////////////////
int FFmpeg::AVFormatContext::default_stream_index::get()
{
	return av_find_default_stream_index((::AVFormatContext*)m_pPointer);
}

void FFmpeg::AVFormatContext::DumpFormat(int index, String^ url, bool is_output)
{
	char *_url = nullptr;
	try
	{
		_url = (char*)AllocString(url).ToPointer();
		av_dump_format((::AVFormatContext*)m_pPointer, index,_url,is_output ? 1 : 0);
	}
	finally
	{
		FreeMemory((IntPtr)_url);
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVFormatContext::OpenInput()
{
	return OpenInput(nullptr,nullptr);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::OpenInput(AVDictionary^ options)
{
	return OpenInput(nullptr,nullptr,options);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::OpenInput(String^ filename)
{
	return OpenInput(nullptr,filename);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::OpenInput(AVInputFormat^ format, String^ filename)
{
	AVDictionary^ options = nullptr;
	try
	{
		options = gcnew AVDictionary();
		return OpenInput(format,filename,options);
	}
	finally
	{
		if (options) delete options;
	}
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::OpenInput(AVInputFormat^ format, String^ filename, AVDictionary^ options)
{
	int _result = -1;
	char * szFileName = nullptr;
	::AVFormatContext * p = (::AVFormatContext *)m_pPointer;
	::AVDictionary * dict = (::AVDictionary *)(options != nullptr ? options->_Pointer.ToPointer() : nullptr);
	try
	{
		szFileName = (char*)AllocString(filename).ToPointer();
		_result = avformat_open_input(&p, szFileName, (::AVInputFormat*)(format != nullptr ? format->_Pointer.ToPointer() : nullptr), &dict);
		if (0 <= _result)
		{
			if (m_pPointer != p)
			{
				if (m_pPointer != nullptr)
				{
					if (m_pDescructor)
					{
						m_pDescructor(m_pPointer);
					}
					if (m_pFree)
					{
						m_pFree(m_pPointer);
					}
					if (m_pFreep)
					{
						void* _ptr = m_pPointer;
						m_pFreep(&_ptr);
					}
				}
				m_pPointer = p;
			}
			if (options != nullptr)
			{
				options->ChangePointer(dict);
			}
			else
			{
				if (dict) av_dict_free(&dict);
			}
			m_pFreep = (TFreeFNP*)avformat_close_input;
			m_pFree = nullptr;
			m_pDescructor = nullptr;
		}
		else
		{
			m_pPointer = p;
		}
	}
	finally
	{
		FreeMemory((IntPtr)szFileName);
	}
	return _result;
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVFormatContext::OpenInput(AVFormatContext^ % context)
{
	return OpenInput(context,nullptr,nullptr);
}
FFmpeg::AVRESULT FFmpeg::AVFormatContext::OpenInput(AVFormatContext^ % context, AVDictionary^ options)
{
	return OpenInput(context,nullptr,nullptr,options);
}
FFmpeg::AVRESULT FFmpeg::AVFormatContext::OpenInput(AVFormatContext^ % context, String^ filename)
{
	return OpenInput(context,nullptr,filename);
}
FFmpeg::AVRESULT FFmpeg::AVFormatContext::OpenInput(AVFormatContext^ % context, AVInputFormat^ format, String^ filename)
{
	AVDictionary^ options = nullptr;
	try
	{
		options = gcnew AVDictionary();
		return OpenInput(context,format,filename,options);
	}
	finally
	{
		if (options) delete options;
	}
}
FFmpeg::AVRESULT FFmpeg::AVFormatContext::OpenInput(AVFormatContext^ % context, AVInputFormat^ format, String^ filename, AVDictionary^ options)
{
	LibAVFormat::RegisterAll();
	int _result = -1;
	char * szFileName = nullptr;
	::AVFormatContext * p = nullptr;
	::AVDictionary * dict = (::AVDictionary *)(options != nullptr ? options->_Pointer.ToPointer() : nullptr);
	try
	{
		szFileName = (char*)AllocString(filename).ToPointer();
		_result = avformat_open_input(&p, szFileName, (::AVInputFormat*)(format != nullptr ? format->_Pointer.ToPointer() : nullptr), &dict);
		if (0 <= _result)
		{
			context = gcnew AVFormatContext(p, nullptr);
			if (context != nullptr)
			{
				context->m_pFreep = (TFreeFNP*)avformat_close_input;
			}
			if (options != nullptr)
			{
				options->ChangePointer(dict);
			}
			else
			{
				if (dict) av_dict_free(&dict);
			}
		}
		else
		{
			context = nullptr;
		}
	}
	finally
	{
		FreeMemory((IntPtr)szFileName);
	}
	return _result;
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVFormatContext::FindStreamInfo()
{
	array<AVDictionary^>^ options = nullptr;
	try
	{
		return FindStreamInfo(options);
	}
	finally
	{
		if (options != nullptr)
		{
			for (int i = 0; i < options->Length; i++)
			{
				delete options[i];
			}
		}
	}
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::FindStreamInfo(array<AVDictionary^>^ options)
{
	int _result = -1;
	int _count = ((::AVFormatContext *)m_pPointer)->nb_streams;
	if (_count > 0)
	{
		::AVDictionary ** _options = (::AVDictionary **)av_mallocz(_count * sizeof(::AVDictionary *));
		if (options != nullptr)
		{
			for (int i = 0; i < options->Length && i < _count; i++)
			{
				_options[i] = (::AVDictionary *)options[i]->_Pointer.ToPointer();
			}
		}
		try
		{
			_result = avformat_find_stream_info((::AVFormatContext *)m_pPointer, _options);
			List<AVDictionary^>^ _list = gcnew List<AVDictionary^>();
			for (int i = 0; i < _count; i++)
			{
				_list->Add(gcnew AVDictionary(_options[i], nullptr));
			}
			options = _list->ToArray();
		}
		finally
		{
			if (_options) av_free(_options);
		}
	}
	return _result;
}

FFmpeg::AVProgram^ FFmpeg::AVFormatContext::FindProgramFromStream(FFmpeg::AVProgram^ last, int s)
{
	auto p = av_find_program_from_stream((::AVFormatContext*)m_pPointer,(::AVProgram*)(last != nullptr ? last->_Pointer.ToPointer() : nullptr),s);
	return _CreateObject<AVProgram>(p);
}

int FFmpeg::AVFormatContext::FindBestStream(AVMediaType type)
{
	return FindBestStream(type,-1,-1);
}

int FFmpeg::AVFormatContext::FindBestStream(AVMediaType type, int wanted_stream_nb, int related_stream)
{
	return av_find_best_stream((::AVFormatContext*)m_pPointer,(::AVMediaType)type,wanted_stream_nb,related_stream,nullptr,0);
}

int FFmpeg::AVFormatContext::FindBestStream(AVMediaType type, [Out] AVCodec^ % decoder_ret)
{
	return FindBestStream(type, -1, -1, decoder_ret);
}

int FFmpeg::AVFormatContext::FindBestStream(AVMediaType type, int wanted_stream_nb, int related_stream, AVCodec^ % decoder_ret)
{
	::AVCodec * _decoder = nullptr;
	try
	{
		DYNAMIC_DEF_API(AVFormat,int,AVERROR(ENOSYS),av_find_best_stream,::AVFormatContext *,::AVMediaType ,int ,int ,const ::AVCodec **,int );
		return av_find_best_stream((::AVFormatContext*)m_pPointer,(::AVMediaType)type,wanted_stream_nb,related_stream,(const ::AVCodec**)&_decoder,0);
	}
	finally
	{
		decoder_ret = _CreateObject<AVCodec>(_decoder);
	}
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::ReadFrame(AVPacket^ pkt)
{
	return av_read_frame((::AVFormatContext*)m_pPointer,(::AVPacket*)pkt->_Pointer.ToPointer());
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::SeekFrame(int stream_index, __int64 timestamp, AVSeekFlag flags)
{
	return av_seek_frame((::AVFormatContext*)m_pPointer, stream_index, timestamp, (int)flags);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::SeekFile(int stream_index, __int64 min_ts, __int64 ts, __int64 max_ts, AVSeekFlag flags)
{
	return avformat_seek_file((::AVFormatContext*)m_pPointer, stream_index, min_ts, ts, max_ts, (int)flags);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::Flush()
{
	return avformat_flush((::AVFormatContext*)m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::ReadPlay()
{
	return av_read_play((::AVFormatContext*)m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::ReadPause()
{
	return av_read_pause((::AVFormatContext*)m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::WriteHeader()
{
	return WriteHeader(nullptr);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::WriteHeader(AVDictionary^ options)
{
	::AVDictionary * _dict = options != nullptr ? (::AVDictionary *)options->_Pointer.ToPointer() : nullptr;
	int _result = avformat_write_header((::AVFormatContext*)m_pPointer,&_dict);
	if (options)
	{
		options->ChangePointer(_dict);
	}
	else
	{
		if (_dict) av_dict_free(&_dict);
	}
	return _result;
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::InitOutput(AVDictionary^ options)
{
	::AVDictionary * _dict = options != nullptr ? (::AVDictionary *)options->_Pointer.ToPointer() : nullptr;
	int _result = avformat_init_output((::AVFormatContext*)m_pPointer,&_dict);
	if (options)
	{
		options->ChangePointer(_dict);
	}
	else
	{
		if (_dict) av_dict_free(&_dict);
	}
	return _result;
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::WriteFrame(AVPacket^ pkt)
{
	return av_write_frame((::AVFormatContext*)m_pPointer,pkt != nullptr ? (::AVPacket*)pkt->_Pointer.ToPointer() : nullptr);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::InterleavedWriteFrame(AVPacket^ pkt)
{
	return av_interleaved_write_frame((::AVFormatContext*)m_pPointer,pkt != nullptr ? (::AVPacket*)pkt->_Pointer.ToPointer() : nullptr);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::WriteTrailer()
{
	return av_write_trailer((::AVFormatContext*)m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::WriteUncodedFrame(int stream_index,AVFrame^ frame)
{
	return av_write_uncoded_frame((::AVFormatContext*)m_pPointer,stream_index,(::AVFrame*)frame->_Pointer.ToPointer());
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::InterleavedWriteUncodedFrame(int stream_index,AVFrame^ frame)
{
	return av_interleaved_write_uncoded_frame((::AVFormatContext*)m_pPointer,stream_index,(::AVFrame*)frame->_Pointer.ToPointer());
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::WriteUncodedFrameQuery(int stream_index)
{
	return av_write_uncoded_frame_query((::AVFormatContext*)m_pPointer, stream_index);
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::GetOutputTimestamp(int stream,Int64 % dts,Int64 % wall)
{
	int64_t _dts,_wall;
	int _result = av_get_output_timestamp((::AVFormatContext*)m_pPointer,stream,&_dts,&_wall);
	if (_result == 0)
	{
		dts = _dts;
		wall = _wall;
	}
	return _result;
}

void FFmpeg::AVFormatContext::inject_global_side_data()
{
	av_format_inject_global_side_data((::AVFormatContext*)m_pPointer);
}

FFmpeg::AVRational^ FFmpeg::AVFormatContext::GuessSampleAspectRatio(AVStream^ stream, AVFrame^ frame)
{
	::AVRational r = av_guess_sample_aspect_ratio((::AVFormatContext*)m_pPointer
		,stream != nullptr ? (::AVStream*)stream->_Pointer.ToPointer() : nullptr
		,frame != nullptr ? (::AVFrame*)frame->_Pointer.ToPointer() : nullptr);
	return gcnew AVRational(r.num,r.den);
}
FFmpeg::AVRational^ FFmpeg::AVFormatContext::GuessFrameRate(AVStream^ stream)
{
	return GuessFrameRate(stream, nullptr);
}
FFmpeg::AVRational^ FFmpeg::AVFormatContext::GuessFrameRate(AVStream^ stream, AVFrame^ frame)
{
	::AVRational r = av_guess_frame_rate((::AVFormatContext*)m_pPointer,
		(::AVStream*)stream->_Pointer.ToPointer(),
		frame != nullptr ? (::AVFrame*)frame->_Pointer.ToPointer() : nullptr);
	return gcnew AVRational(r.num,r.den);
}
int FFmpeg::AVFormatContext::match_stream_specifier(AVStream^ st,String^ spec)
{
	char * _spec = nullptr;
	try
	{
		_spec = (char*)AllocString(spec).ToPointer();
		return avformat_match_stream_specifier((::AVFormatContext*)m_pPointer, (::AVStream*)st->_Pointer.ToPointer(),_spec);
	}
	finally
	{
		FreeMemory((IntPtr)_spec);
	}
}
FFmpeg::AVRESULT FFmpeg::AVFormatContext::queue_attached_pictures()
{
	return avformat_queue_attached_pictures((::AVFormatContext*)m_pPointer);
}

String^ FFmpeg::AVFormatContext::SDP_Create(array<AVFormatContext^>^ ac)
{
	char szTemp[1024] = { 0 };
	if (SDP_Create(ac,(IntPtr)szTemp,_countof(szTemp)))
	{
		return gcnew String(szTemp);
	}
	return nullptr;
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::SDP_Create(array<AVFormatContext^>^ ac,IntPtr buf, int size)
{
	LibAVFormat::RegisterAll();
	::AVFormatContext ** p = nullptr;
	try
	{
		p = (::AVFormatContext **)AllocMemory(sizeof(::AVFormatContext*) * ac->Length).ToPointer();
		for (int i = 0; i < ac->Length; i++)
		{
			p[i] = (::AVFormatContext *)ac[i]->_Pointer.ToPointer();
		}
		return av_sdp_create(p,ac->Length,(char*)buf.ToPointer(),size);
	}
	finally
	{
		FreeMemory((IntPtr)p);
	}
}

FFmpeg::AVRESULT FFmpeg::AVFormatContext::SDP_Create(array<AVFormatContext^>^ ac, Text::StringBuilder^ sb)
{
	size_t cch = 10 * 1024;
	char * c = (char *)av_mallocz(cch);
	try
	{
		if (SDP_Create(ac,IntPtr(c),(int)cch))
		{
			sb->Clear();
			sb->Append(gcnew String(c));
			return true;
		}
		return false;
	}
	finally
	{
		if (c) av_free(c);
	}
}

String^ FFmpeg::AVFormatContext::SDP_Create()
{
	List<AVFormatContext^>^ ac = gcnew List<AVFormatContext^>();
	try
	{
		ac->Add(this);
		return SDP_Create(ac->ToArray());
	}
	finally
	{
		if (ac) delete ac;
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVFormatContext^ FFmpeg::AVFormatContext::OpenOutput(String^ format_name, String^ filename)
{
	return OpenOutput(nullptr,format_name,filename);
}
FFmpeg::AVFormatContext^ FFmpeg::AVFormatContext::OpenOutput(AVOutputFormat^ format, String^ filename)
{
	return OpenOutput(format,nullptr,filename);
}
FFmpeg::AVFormatContext^ FFmpeg::AVFormatContext::OpenOutput(String^ filename)
{
	return OpenOutput(nullptr, nullptr, filename);
}
FFmpeg::AVFormatContext^ FFmpeg::AVFormatContext::OpenOutput(AVOutputFormat^ format)
{
	return OpenOutput(format, nullptr, nullptr);
}
void av_format_close_avio(void *ptr)
{
	::AVFormatContext * p = (::AVFormatContext *)ptr;
	if (p && p->pb && !(p->flags & AVFMT_NOFILE))
	{
		avio_closep(&p->pb);
	}
}
FFmpeg::AVFormatContext^ FFmpeg::AVFormatContext::OpenOutput(FFmpeg::AVOutputFormat^ format, String^ format_name, String^ filename)
{
	LibAVFormat::RegisterAll();

	char * szFormatName = nullptr;
	char * szFileName = nullptr;
	::AVFormatContext * p = nullptr;
	try
	{
		szFormatName = (char*)AllocString(format_name).ToPointer();
		szFileName = (char*)AllocString(filename).ToPointer();
		if (0 >= avformat_alloc_output_context2(&p, (::AVOutputFormat*)(format != nullptr ? format->_Pointer.ToPointer() : nullptr),szFormatName, szFileName))
		{
			if (p != nullptr)
			{
				FFmpeg::AVFormatContext^ ctx = gcnew FFmpeg::AVFormatContext(p,nullptr);
				ctx->m_pFree = (TFreeFN*)avformat_free_context;
				//ctx->m_pDescructor = (TFreeFN*)av_format_close_avio;
				return ctx;
			}
		}
	}
	finally
	{
		FreeMemory((IntPtr)szFormatName);
		FreeMemory((IntPtr)szFileName);
	}
	return nullptr;
}

FFmpeg::AVFormatContext^ FFmpeg::AVFormatContext::OpenInputFile(String^ filename)
{
	return OpenInputFile(nullptr,filename);
}

FFmpeg::AVFormatContext^ FFmpeg::AVFormatContext::OpenInputFile(AVInputFormat^ format, String^ filename)
{
	AVDictionary^ options = nullptr;
	try
	{
		options = gcnew AVDictionary();
		return OpenInputFile(format,filename,options);
	}
	finally
	{
		if (options) delete options;
	}
}

FFmpeg::AVFormatContext^ FFmpeg::AVFormatContext::OpenInputFile(AVInputFormat^ format, String^ filename, AVDictionary^ options)
{
	LibAVFormat::RegisterAll();
	FFmpeg::AVFormatContext^ ctx = gcnew FFmpeg::AVFormatContext();
	if (AVRESULT::OK != ctx->OpenInput(format,filename,options))
	{
		delete ctx;
		ctx = nullptr;
	}
	return ctx;
}
//////////////////////////////////////////////////////
// URLProtocol
//////////////////////////////////////////////////////
FFmpeg::URLProtocol::URLProtocol(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
String^ FFmpeg::URLProtocol::name::get()
{
	auto p = ((::URLProtocol*)m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}
FFmpeg::AVClass^ FFmpeg::URLProtocol::priv_data_class::get()
{
	auto p = ((::URLProtocol*)m_pPointer)->priv_data_class;
	return _CreateObject<AVClass>((void*)p);
}
FFmpeg::UrlProtocolFlags FFmpeg::URLProtocol::flags::get()
{
	return (UrlProtocolFlags)((::URLProtocol*)m_pPointer)->flags;
}
//////////////////////////////////////////////////////
String^ FFmpeg::URLProtocol::ToString()
{
	if (m_pPointer)
	{
		String^ _name;
		_name = name;
		if (!String::IsNullOrEmpty(_name)) return "[ URLProtocol ] \"" + _name + "\"";
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
// URLContext
//////////////////////////////////////////////////////
FFmpeg::URLContext::URLContext(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::URLContext::av_class::get()
{
	return _CreateObject<AVClass>((void*)((::URLContext*)m_pPointer)->av_class);
}

FFmpeg::URLProtocol^ FFmpeg::URLContext::prot::get()
{
	return _CreateObject<URLProtocol>((void*)((::URLContext*)m_pPointer)->prot);
}
IntPtr FFmpeg::URLContext::priv_data::get()
{
	return (IntPtr)((::URLContext*)m_pPointer)->priv_data;
}
void FFmpeg::URLContext::priv_data::set(IntPtr value)
{
	((::URLContext*)m_pPointer)->priv_data = value.ToPointer();
}
String^ FFmpeg::URLContext::filename::get()
{
	auto p = ((::URLContext*)m_pPointer)->filename;
	return p != nullptr ? gcnew String(p) : nullptr;
}
FFmpeg::AvioFlag FFmpeg::URLContext::flags::get()
{
	return (AvioFlag)((::URLContext*)m_pPointer)->flags;
}
int FFmpeg::URLContext::max_packet_size::get()
{
	return ((::URLContext*)m_pPointer)->max_packet_size;
}
void FFmpeg::URLContext::max_packet_size::set(int value)
{
	((::URLContext*)m_pPointer)->max_packet_size = value;
}
bool FFmpeg::URLContext::is_streamed::get()
{
	return ((::URLContext*)m_pPointer)->is_streamed == 1;
}
bool FFmpeg::URLContext::is_connected::get()
{
	return ((::URLContext*)m_pPointer)->is_connected == 1;
}
FFmpeg::AVIOInterruptDesc^ FFmpeg::URLContext::interrupt_callback::get()
{
	return nullptr;
}
Int64 FFmpeg::URLContext::rw_timeout::get()
{
	return ((::URLContext*)m_pPointer)->rw_timeout;
}
void FFmpeg::URLContext::rw_timeout::set(Int64 value)
{
	((::URLContext*)m_pPointer)->rw_timeout = value;
}
//////////////////////////////////////////////////////
String^ FFmpeg::URLContext::ToString()
{
	if (m_pPointer)
	{
		String^ _name;
		_name = filename;
		if (!String::IsNullOrEmpty(_name)) return "[ URLContext ] \"" + _name + "\"";
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
// AVIODirEntry
//////////////////////////////////////////////////////
FFmpeg::AVIODirEntry::AVIODirEntry()
	: AVBase(nullptr,nullptr)
{
	PTR_API(AVFormat,ff_alloc_dir_entry)
	m_pPointer = ff_alloc_dir_entry();
	m_pFreep = (TFreeFNP*)avio_free_directory_entry;
}

FFmpeg::AVIODirEntry::AVIODirEntry(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVIODirEntry::name::get()
{
	auto p = ((::AVIODirEntry*)m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}
FFmpeg::AVIODirEntryType FFmpeg::AVIODirEntry::type::get()
{
	return (AVIODirEntryType)((::AVIODirEntry*)m_pPointer)->type;
}
bool FFmpeg::AVIODirEntry::utf8::get()
{
	return ((::AVIODirEntry*)m_pPointer)->utf8 == 1;
}
Int64 FFmpeg::AVIODirEntry::size::get()
{
	return ((::AVIODirEntry*)m_pPointer)->size;
}
Int64 FFmpeg::AVIODirEntry::modification_timestamp::get()
{
	return ((::AVIODirEntry*)m_pPointer)->modification_timestamp;
}
Int64 FFmpeg::AVIODirEntry::access_timestamp::get()
{
	return ((::AVIODirEntry*)m_pPointer)->access_timestamp;
}
Int64 FFmpeg::AVIODirEntry::status_change_timestamp::get()
{
	return ((::AVIODirEntry*)m_pPointer)->status_change_timestamp;
}
Int64 FFmpeg::AVIODirEntry::user_id::get()
{
	return ((::AVIODirEntry*)m_pPointer)->user_id;
}
Int64 FFmpeg::AVIODirEntry::group_id::get()
{
	return ((::AVIODirEntry*)m_pPointer)->group_id;
}
Int64 FFmpeg::AVIODirEntry::filemode::get()
{
	return ((::AVIODirEntry*)m_pPointer)->filemode;
}
//////////////////////////////////////////////////////
int FFmpeg::AVIODirEntry::_StructureSize::get()
{
	return sizeof(::AVIODirEntry);
}
String^ FFmpeg::AVIODirEntry::ToString()
{
	if (m_pPointer)
	{
		String^ _name;
		_name = name;
		if (!String::IsNullOrEmpty(_name)) return "[ AVIODirEntry ] \"" + _name +"\"";
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
// AVIODirContext::Enumerator
//////////////////////////////////////////////////////
FFmpeg::AVIODirContext::Enumerator::Enumerator(AVIODirContext^ context)
	: m_pContext(context)
	, m_pCurrent(nullptr)
{

}
FFmpeg::AVIODirContext::Enumerator::~Enumerator()
{

}
//////////////////////////////////////////////////////
bool FFmpeg::AVIODirContext::Enumerator::MoveNext()
{
	m_pCurrent = m_pContext->Read();
	return m_pCurrent != nullptr;
}
FFmpeg::AVIODirEntry^ FFmpeg::AVIODirContext::Enumerator::Current::get ()
{ 
	return m_pCurrent;
}
void FFmpeg::AVIODirContext::Enumerator::Reset()
{

}
Object^ FFmpeg::AVIODirContext::Enumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVIODirContext
//////////////////////////////////////////////////////
FFmpeg::AVIODirContext::AVIODirContext(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
FFmpeg::URLContext^ FFmpeg::AVIODirContext::url_context::get()
{
	return _CreateObject<URLContext>(((::AVIODirContext*)m_pPointer)->url_context);
}
//////////////////////////////////////////////////////
FFmpeg::AVIODirEntry^ FFmpeg::AVIODirContext::Read()
{
	AVIODirEntry^ _result = nullptr;
	::AVIODirEntry * entry = nullptr;
	if (0 <= avio_read_dir((::AVIODirContext *)m_pPointer, &entry) && entry)
	{
		_result = _CreateObject<AVIODirEntry>(entry);
		_result->m_pFreep = (TFreeFNP*)avio_free_directory_entry;
	}
	return _result;
}
//////////////////////////////////////////////////////
FFmpeg::AVIODirContext^ FFmpeg::AVIODirContext::Open(String^ url)
{
	return Open(url,nullptr);
}
FFmpeg::AVIODirContext^ FFmpeg::AVIODirContext::Open(String^ url, AVDictionary^ options)
{
	char* szUrl = nullptr;
	::AVDictionary * _dict = options != nullptr ? (::AVDictionary *)options->_Pointer.ToPointer() : nullptr;
	try
	{
		AVIODirContext^ _result = nullptr;
		::AVIODirContext *s = nullptr;
		szUrl = (char*)AllocString(url).ToPointer();
		if (0 <= avio_open_dir(&s,szUrl,&_dict) && s)
		{
			_result = gcnew AVIODirContext(s,nullptr);
			_result->m_pFreep = (AVBase::TFreeFNP*)avio_close_dir;
		}
		if (options)
		{
			options->ChangePointer(_dict);
			_dict = nullptr;
		}
		return _result;
	}
	finally
	{
		if (_dict) av_dict_free(&_dict);
		FreeMemory((IntPtr)szUrl);
	}
}
//////////////////////////////////////////////////////