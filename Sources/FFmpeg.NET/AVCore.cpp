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
// Core classes and utils wrapper.
//////////////////////////////////////////////////////
// The software is provided "as is"
//////////////////////////////////////////////////////
#include "stdafx.h"
#include "AVCore.h"
#include "AVCodec.h"
#include "AVFormat.h"
//////////////////////////////////////////////////////
#using <mscorlib.dll>
//////////////////////////////////////////////////////
#pragma warning(disable: 4996) // marked as deprecated
//////////////////////////////////////////////////////
using namespace System;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;
//////////////////////////////////////////////////////
// AVColorSpace
//////////////////////////////////////////////////////
FFmpeg::AVColorSpace::AVColorSpace(int value)
{
	m_nValue = value;
}

FFmpeg::AVColorSpace::AVColorSpace(::AVColorSpace value)
{
	m_nValue = (int)value;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVColorSpace::ToString()
{
	LOAD_API(AVUtil,const char *,av_get_colorspace_name,const ::AVColorSpace);
	LOAD_API(AVUtil,const char *,av_color_space_name,const ::AVColorSpace);

	const char * p = nullptr;
	if (av_color_space_name) {
		p = av_color_space_name((::AVColorSpace)m_nValue);
	} else {
		if (av_get_colorspace_name) {
			p = av_get_colorspace_name((::AVColorSpace)m_nValue);
		}
	}
	return p != nullptr ? gcnew String(p) : __super::ToString();
}

FFmpeg::AVColorSpace FFmpeg::AVColorSpace::get_colorspace(String^ name)
{
	char * szName = nullptr;
	szName = (char*)AVBase::AllocString(name).ToPointer();
	try
	{
		return (AVColorSpace)av_color_space_from_name(szName);
	}
	finally
	{
		AVBase::FreeMemory((IntPtr)szName);
	}
}
//////////////////////////////////////////////////////
// AVPixelFormat
//////////////////////////////////////////////////////
FFmpeg::AVPixelFormat::AVPixelFormat::AVPixelFormat(int value)
{
	m_nValue = value;
}
FFmpeg::AVPixelFormat::AVPixelFormat::AVPixelFormat(unsigned int value)
{
	m_nValue = (int)value;
}
FFmpeg::AVPixelFormat::AVPixelFormat(__int64 value)
{
	m_nValue = (int)value;
}
FFmpeg::AVPixelFormat::AVPixelFormat(::AVPixelFormat value)
{
	m_nValue = (int)value;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVPixelFormat::name::get()
{
	auto p = av_get_pix_fmt_name((::AVPixelFormat)m_nValue);
	return p != nullptr ? gcnew String(p) : ToString();
}

System::Drawing::Size^ FFmpeg::AVPixelFormat::chroma_sub_sample::get()
{
	int h_shift = 0;
	int v_shift = 0;
	av_pix_fmt_get_chroma_sub_sample((::AVPixelFormat)m_nValue,&h_shift,&v_shift);
	return gcnew System::Drawing::Size(h_shift,v_shift);
}

int FFmpeg::AVPixelFormat::planes::get()
{
	return av_pix_fmt_count_planes((::AVPixelFormat)m_nValue);
}

UInt32 FFmpeg::AVPixelFormat::codec_tag::get()
{
	return avcodec_pix_fmt_to_codec_tag((::AVPixelFormat)m_nValue);
}

FFmpeg::AVPixFmtDescriptor^ FFmpeg::AVPixelFormat::format::get()
{
	auto p = av_pix_fmt_desc_get((::AVPixelFormat)m_nValue);
	return p != nullptr ? gcnew AVPixFmtDescriptor((void*)p,nullptr) : nullptr;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVPixelFormat::ToString()
{
	if (m_nValue <= AV_PIX_FMT_NONE) return "NONE";
	char buf[250] = {0};
	auto p = av_get_pix_fmt_string(buf,_countof(buf),(::AVPixelFormat)m_nValue);
	return p != nullptr && strlen(p) ? gcnew String(p) : __super::ToString();
}
FFmpeg::AVPixelFormat FFmpeg::AVPixelFormat::swap_endianness()
{
	return (AVPixelFormat)av_pix_fmt_swap_endianness((::AVPixelFormat)m_nValue);
}
FFmpeg::FFLoss FFmpeg::AVPixelFormat::get_pix_fmt_loss(AVPixelFormat dst_pix_fmt,AVPixelFormat src_pix_fmt,bool has_alpha)
{
	return (FFLoss)av_get_pix_fmt_loss((::AVPixelFormat)dst_pix_fmt,(::AVPixelFormat)src_pix_fmt,has_alpha ? 1 : 0);
}

FFmpeg::AVPixelFormat FFmpeg::AVPixelFormat::find_best_pix_fmt_of_list(array<AVPixelFormat>^ pix_fmt_list,AVPixelFormat src_pix_fmt,bool has_alpha, FFLoss % loss_ptr)
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

FFmpeg::AVPixelFormat FFmpeg::AVPixelFormat::find_best_pix_fmt_of_2(AVPixelFormat dst_pix_fmt1,AVPixelFormat dst_pix_fmt2,AVPixelFormat src_pix_fmt,bool has_alpha,FFLoss % loss_ptr)
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

FFmpeg::AVPixelFormat FFmpeg::AVPixelFormat::get_pix_fmt(String^ name)
{
	char * szName = nullptr;
	szName = (char*)AVBase::AllocString(name->ToLower()).ToPointer();
	try
	{
		return (AVPixelFormat)av_get_pix_fmt(szName);
	}
	finally
	{
		AVBase::FreeMemory((IntPtr)szName);
	}
}
//////////////////////////////////////////////////////
// AVSampleFormat
//////////////////////////////////////////////////////
FFmpeg::AVSampleFormat::AVSampleFormat(int value)
{
	m_nValue = value; 
}

FFmpeg::AVSampleFormat::AVSampleFormat(unsigned int value)
{ 
	m_nValue = (int)value; 
}
//////////////////////////////////////////////////////
int FFmpeg::AVSampleFormat::bytes_per_sample::get()
{
	return av_get_bytes_per_sample((::AVSampleFormat)m_nValue);
}

bool FFmpeg::AVSampleFormat::is_planar::get()
{
	return (1 == av_sample_fmt_is_planar((::AVSampleFormat)m_nValue));
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVSampleFormat::ToString()
{
	auto p = av_get_sample_fmt_name((::AVSampleFormat)m_nValue);
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
FFmpeg::AVSampleFormat FFmpeg::AVSampleFormat::get_alt_sample_fmt(bool planar)
{
	return (AVSampleFormat)av_get_alt_sample_fmt((::AVSampleFormat)m_nValue,planar ? 1 : 0);
}

FFmpeg::AVSampleFormat FFmpeg::AVSampleFormat::get_packed_sample_fmt()
{
	return (AVSampleFormat)av_get_packed_sample_fmt((::AVSampleFormat)m_nValue);
}

FFmpeg::AVSampleFormat FFmpeg::AVSampleFormat::get_planar_sample_fmt()
{
	return (AVSampleFormat)av_get_planar_sample_fmt((::AVSampleFormat)m_nValue);
}
//////////////////////////////////////////////////////
FFmpeg::AVSampleFormat FFmpeg::AVSampleFormat::get_sample_fmt(String^ name)
{
	char * szName = nullptr;
	szName = (char*)AVBase::AllocString(name).ToPointer();
	try
	{
		return (AVSampleFormat)av_get_sample_fmt(szName);
	}
	finally
	{
		AVBase::FreeMemory((IntPtr)szName);
	}
}
//////////////////////////////////////////////////////
// AVChannelLayout
//////////////////////////////////////////////////////
FFmpeg::AVChannelLayout::AVChannelLayout(unsigned __int64 value)
{
	m_nValue = value;
}

FFmpeg::AVChannelLayout::AVChannelLayout(__int64 value)
{
	m_nValue = (unsigned __int64)value;
}
//////////////////////////////////////////////////////
int FFmpeg::AVChannelLayout::channels::get()
{
	return av_get_channel_layout_nb_channels((uint64_t)m_nValue);
}
String^ FFmpeg::AVChannelLayout::name::get()
{
	auto p = av_get_channel_name((uint64_t)m_nValue);
	return p != nullptr ? gcnew String(p) : description::get();
}
String^ FFmpeg::AVChannelLayout::description::get()
{
	auto p = av_get_channel_description((uint64_t)m_nValue);
	return p != nullptr ? gcnew String(p) : ToString();
}
FFmpeg::AVChannelLayout FFmpeg::AVChannelLayout::default::get(int channel)
{
	return extract_channel(channel);
}
int FFmpeg::AVChannelLayout::default::get(AVChannelLayout channel)
{
	return get_channel_index(channel);
}
//////////////////////////////////////////////////////
FFmpeg::AVChannelLayout FFmpeg::AVChannelLayout::get_channel_layout(String^ name)
{
	char * szName = nullptr;
	if (!String::IsNullOrEmpty(name))
	{
		szName = (char*) AVBase::AllocString(name).ToPointer();
		try
		{
			return (AVChannelLayout)av_get_channel_layout(szName);
		}
		finally
		{
			AVBase::FreeMemory((IntPtr)szName);
		}
	}
	return AVChannelLayout::UNKNOWN;
}

FFmpeg::AVChannelLayout FFmpeg::AVChannelLayout::get_default_channel_layout(int nb_channels)
{
	return (AVChannelLayout)av_get_default_channel_layout(nb_channels);
}

int FFmpeg::AVChannelLayout::get_channel_index(AVChannelLayout channel)
{
	return av_get_channel_layout_channel_index(m_nValue,(uint64_t)channel);
}

FFmpeg::AVChannelLayout FFmpeg::AVChannelLayout::extract_channel(int index)
{
	return (AVChannelLayout)av_channel_layout_extract_channel(m_nValue,index);
}

String^ FFmpeg::AVChannelLayout ::ToString()
{
	char buf[300] = {0};
	av_get_channel_layout_string(buf,_countof(buf),-1,m_nValue);
	return gcnew String(buf);
}

bool FFmpeg::AVChannelLayout::get_standard_channel_layout(UInt32 index, [Out] AVChannelLayout % layout,String^ % name)
{
	uint64_t _layout = 0;
	const char * _name = nullptr;
	try
	{
		return 0 == av_get_standard_channel_layout(index, &_layout,&_name);
	}
	finally
	{
		layout = (AVChannelLayout)_layout;
		name = (_name != nullptr ? gcnew String(_name) : nullptr);
	}
}
//////////////////////////////////////////////////////
// AVRESULT
//////////////////////////////////////////////////////
FFmpeg::AVRESULT::AVRESULT(int error)
	: m_nValue(error)
{
	
}
FFmpeg::AVRESULT::AVRESULT(unsigned int error)
	: m_nValue((int)error)
{
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVRESULT::Text::get()
{
	char buf[256] = {0};
	char sText[256] = {0};
	if (0 == av_strerror(m_nValue,buf,_countof(buf)))
	{
		sprintf_s(sText,"RESULT: 0x%08X (%d) \"%s\"",m_nValue,m_nValue,buf);
	}
	else
	{
		if (m_nValue == 0) strcpy_s(buf,"SUCCEEDED");
		if (m_nValue  < 0) strcpy_s(buf,"GENERIC ERROR");
		if (m_nValue  > 0) strcpy_s(buf,"");
		sprintf_s(sText, "RESULT: 0x%08X (%d) %s", m_nValue, m_nValue,buf);
	}
	return gcnew String(sText);
}
//////////////////////////////////////////////////////
int FFmpeg::AVRESULT::CompareTo(System::Object ^obj)
{ 
	AVRESULT^ r = dynamic_cast<AVRESULT^>(obj); 
	if (r != nullptr)
	{
		return m_nValue.CompareTo(r->m_nValue);
	}
	return m_nValue.CompareTo(obj);
}

System::Object ^ FFmpeg::AVRESULT::Clone()
{
	return (Object ^)AVRESULT(m_nValue);
}
//////////////////////////////////////////////////////
// AVBase
//////////////////////////////////////////////////////
FFmpeg::AVMemory::AVMemory()
{
	s_MemoryReference++;
}

FFmpeg::AVMemory::~AVMemory()
{

}

FFmpeg::AVMemory::!AVMemory()
{
	if (--s_MemoryReference == 0)
	{
		Monitor::Enter(m_AllocatedMemory);
		try
		{
			while (m_AllocatedMemory->Count > 0)
			{
				auto p = m_AllocatedMemory[0];
				if (p != IntPtr::Zero) av_free(p.ToPointer());
				m_AllocatedMemory->RemoveAt(0);
			}
		}
		finally
		{
			Monitor::Exit(m_AllocatedMemory);
		}
	}
}
//////////////////////////////////////////////////////
// AVBase
//////////////////////////////////////////////////////
FFmpeg::AVBase::AVBase(void * _pointer,AVBase^ _parent)
	 : m_pPointer(_pointer)
	 , m_bAllocated(false)
	 , m_pFree(nullptr)
	 , m_pFreep(nullptr)
	 , m_pDescructor(nullptr)
	 , m_pParent(_parent)
	 , m_lRefCount(0)
	 , m_lInternalRefCount(0)
	 , m_bDisposed(false)
{
	EnsureLibraryLoaded();
	InternalAddRef();
	m_ObjectMemory = gcnew Generic::SortedList<String^,IntPtr>();
	m_Objects = gcnew Generic::List<AVBase^>();
	if (_parent != nullptr)
	{
		if (_parent->AddObject(this))
		{
			InternalRelease();
		}
	}
	else
	{
		AddRef();
	}
}

FFmpeg::AVBase::AVBase()
	: AVBase(nullptr,nullptr)
{
}

FFmpeg::AVBase::~AVBase()
{
	if (!m_bDisposed)
	{
		Release();
		m_bDisposed = true;
	}
}

FFmpeg::AVBase::!AVBase()
{
	if (m_lInternalRefCount > 0)
	{
		//Debug::WriteLine(String::Format("Object Not Disposed: {0}",ToString()));
		_Free();
	}
}

void FFmpeg::AVBase::EnsureLibraryLoaded()
{
	if (!s_bDllLoaded)
	{
		s_bDllLoaded = true;
#define LoadLib(_name,_lib) { \
			int _index = 100; while (--_index >= -1 && !AVBase::##_lib) { \
			swprintf_s(szFileName,_countof(szFileName),(_index >= 0 ? L"%s-%d.dll" : L"%s.dll"),_name,_index); \
			swprintf_s(szPath,_countof(szPath),L"%s%s%s",szDrive,szDir,szFileName);AVBase::##_lib = LoadLibraryW(szPath); } \
		}
		WCHAR szDrive[MAX_PATH], szDir[MAX_PATH], szFileName[MAX_PATH], szExt[MAX_PATH], szPath[MAX_PATH];
		GetModuleFileNameW(NULL, szPath, MAX_PATH);
		_wsplitpath_s(szPath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);
		
		LoadLib(L"avutil",m_hLibAVUtil);
		LoadLib(L"swresample",m_hLibSwresample);
		LoadLib(L"swscale",m_hLibSwscale);
		LoadLib(L"postproc",m_hLibPostproc);
		LoadLib(L"avcodec",m_hLibAVCodec);
		LoadLib(L"avformat",m_hLibAVFormat);
		LoadLib(L"avfilter",m_hLibAVFilter);
		LoadLib(L"avdevice",m_hLibAVDevice);

#undef LoadLib
	}
}
//////////////////////////////////////////////////////
long FFmpeg::AVBase::AddRef()
{
	long lRef = 0;
	if (!m_bDisposed)
	{
		lRef = m_lRefCount++;
	}
	return lRef;
}

long FFmpeg::AVBase::Release()
{
	long lRef = 0;
	if (!m_bDisposed && m_lRefCount > 0)
	{
		lRef = --m_lRefCount;
		if (lRef == 0)
		{
			InternalRelease();
		}
	}
	return lRef;
}
//////////////////////////////////////////////////////
long FFmpeg::AVBase::InternalAddRef()
{
	return ++m_lInternalRefCount;
}

long FFmpeg::AVBase::InternalRelease()
{
	long lRef = --m_lInternalRefCount;
	if (lRef == 0)
	{		
		_Free();
		if (!m_bDisposed)
		{
			delete this;
		}
	}
	return lRef;
}
//////////////////////////////////////////////////////
void FFmpeg::AVBase::ValidateObject()
{
	if (m_bDisposed) throw gcnew System::ObjectDisposedException(this->ToString());
}
//////////////////////////////////////////////////////
void FFmpeg::AVBase::_FreeObjects()
{
	if (m_Objects)
	{
		auto _obj = m_Objects;
		Monitor::Enter(_obj);
		try
		{
			m_Objects = nullptr;
			while (_obj->Count > 0)
			{
				_obj[0]->InternalRelease();
				_obj->RemoveAt(0);
			}
		}
		finally
		{
			Monitor::Exit(_obj);
		}
		delete _obj;
	}
}

void FFmpeg::AVBase::_Free()
{
	_FreeObjects();
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
	m_pFree  = nullptr;
	m_pFreep = nullptr;
	m_pDescructor = nullptr;
	m_pParent = nullptr;
	if (m_bAllocated)
	{
		m_bAllocated = false;
	}
	if (m_ObjectMemory)
	{
		auto _mem = m_ObjectMemory;
		Monitor::Enter(_mem);
		try
		{
			m_ObjectMemory = nullptr;
			while (_mem->Count > 0)
			{
				FreeMemory(_mem->Values[0]);
				_mem->RemoveAt(0);
			}
		}
		finally
		{
			Monitor::Exit(_mem);
		}
		delete _mem;
	}
}
//////////////////////////////////////////////////////
bool FFmpeg::AVBase::_EnsurePointer()
{
	return _EnsurePointer(true);
}

bool FFmpeg::AVBase::_EnsurePointer(bool bThrow)
{
	if (m_pPointer == nullptr)
	{
		ValidateObject();
		if (bThrow) throw gcnew System::NullReferenceException();
		return false;
	}
	return true;
}
//////////////////////////////////////////////////////
FFmpeg::AVBase^ FFmpeg::AVBase::GetObject(IntPtr p)
{
	if (p != IntPtr::Zero)
	{
		Monitor::Enter(m_Objects);
		try
		{
			for (int i = 0; i < m_Objects->Count; i++)
			{
				if (m_Objects[i]->_Pointer == p) return m_Objects[i];
			}
		}
		finally
		{
			Monitor::Exit(m_Objects);
		}
	}
	return nullptr;
}

bool FFmpeg::AVBase::AddObject(AVBase^ _object)
{
	bool bResult = false;
	ValidateObject();
	Monitor::Enter(m_Objects);
	try
	{
		if (_object != nullptr && !m_Objects->Contains(_object))
		{
			_object->InternalAddRef();
			m_Objects->Add(_object);
			bResult = true;
		}
	}
	finally
	{
		Monitor::Exit(m_Objects);
	}
	return bResult;
}

bool FFmpeg::AVBase::AddObject(IntPtr p,AVBase^ _object)
{
	if (_object != nullptr && _object->_Pointer == p)
	{
		return false;
	}
	AVBase^ tmp = GetObject(p);
	if (tmp) tmp->InternalAddRef();
	try
	{
		RemoveObject(p);
		return AddObject(_object);
	}
	finally
	{
		if (tmp) tmp->InternalRelease();
	}
}

void FFmpeg::AVBase::RemoveObject(IntPtr p)
{
	if (p != IntPtr::Zero)
	{
		Monitor::Enter(m_Objects);
		try
		{
			for (int i = 0; i < m_Objects->Count; i++)
			{
				if (m_Objects[i]->_Pointer == p) 
				{
					m_Objects[i]->InternalRelease();
					m_Objects->RemoveAt(i);
					break;
				}
			}
		}
		finally
		{
			Monitor::Exit(m_Objects);
		}
	}
}
//////////////////////////////////////////////////////
bool FFmpeg::AVBase::Equals(Object^ obj)
{
	if (Object::ReferenceEquals(this,obj)) return true;
	AVBase^ _object = dynamic_cast<AVBase^>(obj);
	if (_object != nullptr)
	{
		if (_object->m_pPointer == m_pPointer) return true;
	}
	return Object::Equals(obj);
}
//////////////////////////////////////////////////////
IntPtr FFmpeg::AVBase::_Pointer::get()
{
	return IntPtr(m_pPointer);
}

void FFmpeg::AVBase::_Pointer::set(IntPtr _ptr)
{
	if (m_pPointer != _ptr.ToPointer())
	{
		_Free();
		m_pPointer = _ptr.ToPointer();
	}
}

bool FFmpeg::AVBase::_IsValid::get()
{
	return (m_pPointer != nullptr);
}

bool FFmpeg::AVBase::_IsAllocated::get()
{
	return m_bAllocated;
}

int FFmpeg::AVBase::_StructureSize::get()
{
	return 0;
}
//////////////////////////////////////////////////////
FFmpeg::AVBase^ FFmpeg::AVBase::_Parent::get()
{
	return m_pParent;
}
//////////////////////////////////////////////////////
void FFmpeg::AVBase::AllocPointer(int _size)
{
	Debug::Assert(m_pPointer == nullptr);
	m_pPointer = av_mallocz(_size);
	m_pFree = ::av_free;
	m_pFreep = nullptr;
	m_bAllocated = (m_pPointer != nullptr);
}
//////////////////////////////////////////////////////
IntPtr FFmpeg::AVBase::GetMemory(String^ _key)
{
	Monitor::Enter(m_ObjectMemory);
	try
	{
		if (m_ObjectMemory->ContainsKey(_key))
		{
			return m_ObjectMemory[_key];
		}
	}
	finally
	{
		Monitor::Exit(m_ObjectMemory);
	}
	return IntPtr::Zero;
}

void FFmpeg::AVBase::SetMemory(String^ _key,IntPtr _pointer)
{
	if (String::IsNullOrEmpty(_key)) throw gcnew ArgumentNullException();
	FreeMemory(_key);
	Monitor::Enter(m_ObjectMemory);
	try
	{
		m_ObjectMemory->Add(_key,_pointer);
	}
	finally
	{
		Monitor::Exit(m_ObjectMemory);
	}
}

IntPtr FFmpeg::AVBase::AllocMemory(String^ _key,int _size)
{
	IntPtr _memory = AllocMemory(_size);
	if (_memory != IntPtr::Zero)
	{
		if (!String::IsNullOrEmpty(_key))
		{
			SetMemory(_key,_memory);
		}
	}
	return _memory;
}

IntPtr FFmpeg::AVBase::AllocMemory(int _size)
{
	if (_size <= 0) throw gcnew ArgumentOutOfRangeException();
	IntPtr _memory = (IntPtr)av_mallocz(_size);
	if (_memory != IntPtr::Zero)
	{
		Monitor::Enter(m_AllocatedMemory);
		try
		{
			m_AllocatedMemory->Add(_memory);
		}
		finally
		{
			Monitor::Exit(m_AllocatedMemory);
		}
	}
	return _memory;
}

IntPtr FFmpeg::AVBase::AllocString(String^ _value)
{
	return AllocString(_value,false);
}

IntPtr FFmpeg::AVBase::AllocString(String^ _value,bool bUnicode)
{
	IntPtr _memory = IntPtr::Zero;
	if (!String::IsNullOrEmpty(_value))
	{
		IntPtr _str = bUnicode ? Marshal::StringToCoTaskMemUni(_value) : Marshal::StringToCoTaskMemAnsi(_value);
		if (_str != IntPtr::Zero)
		{
			try
			{
				if (!bUnicode)
				{
					_memory = (IntPtr)av_strdup((const char*)_str.ToPointer());
				}
				else
				{
					size_t cch = (wcslen((wchar_t*)_str.ToPointer()) + 1) * sizeof(wchar_t);
					void * p = av_mallocz(cch);
					if (p) memcpy(p,_str.ToPointer(),cch);
					_memory = (IntPtr)p;
				}
				if (_memory != IntPtr::Zero)
				{
					Monitor::Enter(m_AllocatedMemory);
					try
					{
						m_AllocatedMemory->Add(_memory);
					}
					finally
					{
						Monitor::Exit(m_AllocatedMemory);
					}
				}
			}
			finally
			{
				Marshal::FreeCoTaskMem(_str);
			}
		}
	}
	return _memory;
}

IntPtr FFmpeg::AVBase::AllocString(String^ _key,String^ _value)
{
	return AllocString(_key,_value,false);
}

IntPtr FFmpeg::AVBase::AllocString(String^ _key,String^ _value,bool bUnicode)
{
	IntPtr _memory = AllocString(_value,bUnicode);
	if (_memory != IntPtr::Zero)
	{
		if (!String::IsNullOrEmpty(_key))
		{
			SetMemory(_key,_memory);
		}
	}
	return _memory;
}

void FFmpeg::AVBase::FreeMemory(String^ _key)
{
	Monitor::Enter(m_ObjectMemory);
	try
	{
		if (m_ObjectMemory->ContainsKey(_key))
		{
			FreeMemory(m_ObjectMemory[_key]);
			m_ObjectMemory->Remove(_key);
		}
	}
	finally
	{
		Monitor::Exit(m_ObjectMemory);
	}
}

void FFmpeg::AVBase::FreeMemory(IntPtr _memory)
{
	if (_memory != IntPtr::Zero) 
	{
		Monitor::Enter(m_AllocatedMemory);
		try
		{
			if (m_AllocatedMemory->Contains(_memory))
			{
				m_AllocatedMemory->Remove(_memory);
			}
		}
		finally
		{
			Monitor::Exit(m_AllocatedMemory);
		}
		av_free(_memory.ToPointer());
	}
}

bool FFmpeg::AVBase::IsAllocated(String^ _key)
{
	Monitor::Enter(m_ObjectMemory);
	try
	{
		return m_ObjectMemory->ContainsKey(_key);
	}
	finally
	{
		Monitor::Exit(m_ObjectMemory);
	}
}
//////////////////////////////////////////////////////
bool FFmpeg::AVBase::operator == (FFmpeg::AVBase^ _source,FFmpeg::AVBase^ _dest)
{
	if (((Object^)(_source)) != nullptr && ((Object^)(_dest)) != nullptr)
	{
		 return (_source->_Pointer == _dest->_Pointer);
	}
	else
	{
		return ((Object^)(_dest) == ((Object^)(_source)));
	}
}

bool FFmpeg::AVBase::operator != (FFmpeg::AVBase^ _source,FFmpeg::AVBase^ _dest)
{
	return !(_source == _dest);
}

bool FFmpeg::AVBase::operator == (AVBase^ _source, IntPtr p)
{
	if (((Object^)(_source)) == nullptr)
	{
		return p == IntPtr::Zero;
	}
	return (_source->_Pointer == p);
}

bool FFmpeg::AVBase::operator != (AVBase^_source, IntPtr p)
{
	return !(_source == p);
}
//////////////////////////////////////////////////////
bool FFmpeg::AVBase::TraceValues(FFmpeg::AVBase^ _object)
{
    if (_object == nullptr) return true;
    int _pass = 0;
    int _skipped = 0;
#ifdef _DEBUG
	using namespace System::Reflection;
    List<PropertyInfo^>^ fProperties = gcnew List<PropertyInfo^>();
    String^ _separator = "---------------------------------------";
    String^ _separator2 = "//////////////////////////////////////";
    Console::WriteLine();
    Console::WriteLine(_separator);
    Console::WriteLine(String::Format("Tracing object: \"{0}\"", _object->ToString()));
    Console::WriteLine(_separator);

    array<PropertyInfo^>^ _properties = _object->GetType()->GetProperties(BindingFlags::Instance | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::DeclaredOnly);
    Console::WriteLine(String::Format("Properties Count: {0}", _properties != nullptr ? _properties->Length : 0));
    for (int i = 0; i < _properties->Length; i++)
    {
		auto _property = _properties[i];
        Console::WriteLine(_separator2);
        Console::WriteLine(String::Format("Property: \"{0}\"", _property->Name));
        Console::WriteLine(String::Format("Type: \"{0}\"", _property->PropertyType));

        try
        {
            Console::Write("Value: ");
            Object^ _value = _property->GetValue(_object, nullptr);
            Console::WriteLine(_value);
            _pass++;
        }
        catch (Exception^ _exception)
        {
            Console::WriteLine(_exception->ToString());
            if (fProperties->IndexOf(_property) == -1) fProperties->Add(_property);
        }
        Console::WriteLine();
    }
    Console::WriteLine(_separator);
    Console::WriteLine();
    Console::WriteLine("Result : passed {0}, skipped : {1}, failed {2} ", _pass, _skipped, fProperties->Count);
    Console::WriteLine(_separator);
    if (fProperties->Count > 0)
    {
        for (int i = 0; i < fProperties->Count; i++)
        {
            Console::WriteLine(String::Format("Failed Property: \"{0}\"", fProperties[i]->Name));
        }
        Console::WriteLine(_separator);
    }
    return fProperties->Count == 0;
#else
	return false;
#endif
}
//////////////////////////////////////////////////////
// AVColor
//////////////////////////////////////////////////////
// BT 601
static double s_Matrices[2][3][3] = {
	{	// RGB To YUV
		{ { 0.256788 }, { 0.504129 }, { 0.097906 } },	// Y
		{ {-0.148223 }, {-0.290993 }, { 0.439216 } },	// U
		{ { 0.439216 }, {-0.367788 }, {-0.071427 } },	// V
	},
	{	// YUV To RGB
		{ { 1.164383 }, { 0.000000 }, { 1.596027 } },	// R
		{ { 1.164383 }, {-0.391762 }, {-0.812968 } },	// G
		{ { 1.164383 }, { 2.017232 }, { 0.000000 } },	// B
	},
};
//////////////////////////////////////////////////////
#define TABLE_SIZE 256
///////////////////////////////////////////////////////
uint8_t		g_bColorValue[1024] = { 0 };
uint8_t  *	g_pColorValue = &g_bColorValue[320];
///////////////////////////////////////////////////////
int g_RY[TABLE_SIZE], g_RU[TABLE_SIZE], g_RV[TABLE_SIZE];
int g_GY[TABLE_SIZE], g_GU[TABLE_SIZE], g_GV[TABLE_SIZE];
int g_BY[TABLE_SIZE], g_BU[TABLE_SIZE], g_BV[TABLE_SIZE];
///////////////////////////////////////////////////////
int g_YR[TABLE_SIZE], g_YG[TABLE_SIZE], g_YB[TABLE_SIZE];
int g_UR[TABLE_SIZE], g_UG[TABLE_SIZE], g_UB[TABLE_SIZE];
int g_VR[TABLE_SIZE], g_VG[TABLE_SIZE], g_VB[TABLE_SIZE];
///////////////////////////////////////////////////////
const int c_nShift =  16;
///////////////////////////////////////////////////////
void FFmpeg::AVColor::InitializeTables()
{
	if (!s_bTableInitialized)
	{
		s_bTableInitialized = true;

		const __int64 fShift = 1 << c_nShift;

		for (int j = 0; j < 256; j++)
		{
			g_RY[j]	= (int)((s_Matrices[1][0][0] * (j - 16))  * fShift);
			g_RU[j]	= (int)((s_Matrices[1][0][1] * (j - 128)) * fShift);
			g_RV[j] = (int)((s_Matrices[1][0][2] * (j - 128)) * fShift);
			
			g_GY[j]	= (int)((s_Matrices[1][1][0] * (j - 16))  * fShift);
			g_GU[j]	= (int)((s_Matrices[1][1][1] * (j - 128)) * fShift);
			g_GV[j]	= (int)((s_Matrices[1][1][2] * (j - 128)) * fShift);
			
			g_BY[j]	= (int)((s_Matrices[1][2][0] * (j - 16))  * fShift);
			g_BU[j]	= (int)((s_Matrices[1][2][1] * (j - 128)) * fShift);
			g_BV[j]	= (int)((s_Matrices[1][2][2] * (j - 128)) * fShift);

			g_YR[j] = (int)((s_Matrices[0][0][0] * j) * fShift); //16
			g_YG[j] = (int)((s_Matrices[0][0][1] * j) * fShift);
			g_YB[j] = (int)((s_Matrices[0][0][2] * j) * fShift);

			g_UR[j] = (int)((s_Matrices[0][1][0] * j) * fShift); //128
			g_UG[j] = (int)((s_Matrices[0][1][1] * j) * fShift);
			g_UB[j] = (int)((s_Matrices[0][1][2] * j) * fShift);

			g_VR[j] = (int)((s_Matrices[0][2][0] * j) * fShift); //128
			g_VG[j] = (int)((s_Matrices[0][2][1] * j) * fShift);
			g_VB[j] = (int)((s_Matrices[0][2][2] * j) * fShift);
		}

		for (int i = 0; i < sizeof(g_bColorValue); i++)
		{
			g_bColorValue[i] = (uint8_t)(i < 320 ? 0x00 : (i - 320 > 255 ? 255 : i - 320));
		}
	}
}
///////////////////////////////////////////////////////
void FFmpeg::AVColor::SetColorspaceMatrices(array<double, 2>^ rgb, array<double, 2>^ yuv)
{
	if (rgb != nullptr)
	{
		for (int i = 0; i < rgb->GetLength(0) && i < 3;i++)
		{
			for (int j = 0; j < rgb->GetLength(1) && j < 3;j++)
			{
				s_Matrices[0][i][j] = rgb[i,j];
			}
		}
	}
	if (yuv != nullptr)
	{
		for (int i = 0; i < yuv->GetLength(0) && i < 3;i++)
		{
			for (int j = 0; j < yuv->GetLength(1) && j < 3;j++)
			{
				s_Matrices[1][i][j] = yuv[i,j];
			}
		}
	}
	s_bTableInitialized = false;
	InitializeTables();
}
///////////////////////////////////////////////////////
#define RGB_TO_Y(r,g,b) ((int) ((g_YB[b] + g_YG[g] + g_YR[r]) >> c_nShift) + 16)
#define RGB_TO_U(r,g,b) ((int) ((g_UB[b] + g_UG[g] + g_UR[r]) >> c_nShift) + 128)
#define RGB_TO_V(r,g,b) ((int) ((g_VB[b] + g_VG[g] + g_VR[r]) >> c_nShift) + 128)
///////////////////////////////////////////////////////
#define YUV_TO_R(y,u,v) (g_pColorValue[((g_RY[y] + g_RU[u] + g_RV[v]) >> c_nShift)])
#define YUV_TO_G(y,u,v) (g_pColorValue[((g_GY[y] + g_GU[u] + g_GV[v]) >> c_nShift)])
#define YUV_TO_B(y,u,v) (g_pColorValue[((g_BY[y] + g_BU[u] + g_BV[v]) >> c_nShift)])
//////////////////////////////////////////////////////
void FFmpeg::AVColor::RGBToYUV(int r, int g, int b, int % y, int % u, int % v)
{
	InitializeTables();
	y = RGB_TO_Y(r,g,b);
	u = RGB_TO_U(r,g,b);
	v = RGB_TO_V(r,g,b);
}

void FFmpeg::AVColor::YUVToRGB(int y, int u, int v, int % r, int % g, int % b)
{
	InitializeTables();
	r = YUV_TO_R(y,u,v);
	g = YUV_TO_G(y,u,v);
	b = YUV_TO_B(y,u,v);
}
//////////////////////////////////////////////////////
FFmpeg::AVColor::AVColor(void * p, uint32_t fcc, int size) 
{
	m_pPointer = (uint8_t*)p; 
	m_nSize = size;
	m_nFourCC = fcc;
	InitializeTables();
}
//////////////////////////////////////////////////////
int FFmpeg::AVColor::Size::get()
{
	return m_nSize;
}
byte FFmpeg::AVColor::default::get(int idx)
{
	if (idx < 0 || (idx >= m_nSize && m_nSize != 0))
	{
		throw gcnew ArgumentOutOfRangeException("Index Out Of Range");
	}
	return m_pPointer[idx];
}
void FFmpeg::AVColor::default::set(int idx, byte value)
{
	if (idx < 0 || (idx >= m_nSize && m_nSize != 0))
	{
		throw gcnew ArgumentOutOfRangeException("Index Out Of Range");
	}
	m_pPointer[idx] = value;
}
//////////////////////////////////////////////////////
System::Drawing::Color FFmpeg::AVColor::Color::get()
{
	if (m_pPointer == nullptr) return System::Drawing::Color::Empty;
	int color = this->operator int(this);
	return System::Drawing::Color::FromArgb(color);
}
void FFmpeg::AVColor::Color::set(System::Drawing::Color c)
{
	this->operator=(c.ToArgb());
}
//////////////////////////////////////////////////////
FFmpeg::AVColor::operator IntPtr(FFmpeg::AVColor^ c)
{
	if (c == nullptr) return IntPtr::Zero;
	return IntPtr(c->m_pPointer);
}
//////////////////////////////////////////////////////
FFmpeg::AVColor::operator byte(FFmpeg::AVColor^ c)
{
	if (((Object^)c) == nullptr || c->m_pPointer == nullptr) return 0;
	return c->m_pPointer[0];
}
FFmpeg::AVColor::operator short(FFmpeg::AVColor^ c)
{
	int color = c->operator int (c);
	return (short)color;
}
FFmpeg::AVColor::operator int(FFmpeg::AVColor^ c)
{
	if (((Object^)c) == nullptr || c->m_pPointer == nullptr) return 0;
	int color = 0;
	for (int i = c->m_nSize - 1; i >= 0; i--)
	{
		color = (color << 8) | (int)c->m_pPointer[i];
	}
	return color;
}
FFmpeg::AVColor::operator unsigned short(FFmpeg::AVColor^ c)
{
	int color = c->operator int (c);
	return (unsigned short)color;
}
FFmpeg::AVColor::operator unsigned int(FFmpeg::AVColor^ c)
{
	int color = c->operator int (c);
	return (unsigned int)color;
}
//////////////////////////////////////////////////////
bool FFmpeg::AVColor::operator == (AVColor^ a, AVColor^ b)
{
	if (((Object^)a) == nullptr || ((Object^)b) == nullptr)
	{
		return (((Object^)a) == nullptr && ((Object^)b) == nullptr);
	}
	else
	{
		if (a->m_pPointer == b->m_pPointer)
		{
			return true;
		}
		if (a->m_pPointer != nullptr && b->m_pPointer != nullptr)
		{	
			if (a->m_nFourCC == b->m_nFourCC)
			{
				return (memcmp(a->m_pPointer,b->m_pPointer,a->m_nSize) == 0);
			}
			else
			{
				return (a->Color == b->Color);
			}
		}
		else
		{
			return false;
		}
	}
}
bool FFmpeg::AVColor::operator != (AVColor^ a, AVColor^ b)
{
	return !(a == b);
}
//////////////////////////////////////////////////////
void FFmpeg::AVColor::operator = (int n)
{
	if (m_pPointer != nullptr)
	{
		memcpy(m_pPointer, &n, m_nSize);
	}
}
void FFmpeg::AVColor::operator = (AVColor^ c)
{
	if (((Object^)c) != nullptr && c->m_pPointer != nullptr && m_pPointer != nullptr)
	{
		if (c->m_pPointer != m_pPointer)
		{
			if (c->m_nFourCC == m_nFourCC)
			{
				memcpy(m_pPointer, c->m_pPointer, m_nSize);
			}
			else
			{
				Color::set(c->Color);
			}
		}
	}
	else
	{
		this->Color = System::Drawing::Color::Empty;
	}
}
void FFmpeg::AVColor::operator = (System::Drawing::Color c)
{
	Color::set(c);
}
//////////////////////////////////////////////////////
array<byte>^ FFmpeg::AVColor::ToArray()
{
	array<byte>^ a = gcnew array<byte>(3);
	for (int i = 0; i < m_nSize; i++)
	{
		a[i] = m_pPointer[i];
	}
	return a;
}
String^ FFmpeg::AVColor::ToString()
{
	String^ fcc = "";
	String^ color = "0x";

	auto _data = BitConverter::GetBytes(m_nFourCC);
	fcc = System::Text::Encoding::Default->GetString(_data);
	for (int i = 0; i < _data->Length; i++)
	{
		if (_data[i] <= 30 || _data[i] > 128)
		{
			fcc = "";
			break;
		}
	}
	auto c = this->Color;
	for (int i = 0; i < m_nSize; i++)
	{
		color += String::Format("{0:X2}",(byte)m_pPointer[i]);
	}
	if (fcc != "")
	{
		color = "[ " + fcc + " ] " + color;
	}
	if (c.IsKnownColor)
	{
		color += " (" + c.ToKnownColor().ToString() + ")";
	}
	return color;
}
//////////////////////////////////////////////////////
System::Drawing::Color FFmpeg::RGB::Color::get()
{
	int color = this->operator int (this);
	return System::Drawing::Color::FromArgb((color | (0xff << 24)));
}
void FFmpeg::RGB::Color::set(System::Drawing::Color c) 
{
	AVColor::operator=(c.ToArgb());
}
//////////////////////////////////////////////////////
System::Drawing::Color FFmpeg::AYUV::Color::get()
{
	return System::Drawing::Color::FromArgb(A,
		YUV_TO_R(Y,U,V),
		YUV_TO_G(Y,U,V),
		YUV_TO_B(Y,U,V)
	);
}
void FFmpeg::AYUV::Color::set(System::Drawing::Color c) 
{
	Y = RGB_TO_Y(c.R,c.G,c.B);
	U = RGB_TO_U(c.R,c.G,c.B);
	V = RGB_TO_V(c.R,c.G,c.B);
	A = c.A;
}
//////////////////////////////////////////////////////
System::Drawing::Color FFmpeg::YUY2::Color::get()
{
	return System::Drawing::Color::FromArgb(0xff,
		YUV_TO_R(Y0,U,V),
		YUV_TO_G(Y0,U,V),
		YUV_TO_B(Y0,U,V)
	);
}
void FFmpeg::YUY2::Color::set(System::Drawing::Color c) 
{
	Y0 = RGB_TO_Y(c.R,c.G,c.B);
	Y1 = Y0;
	U = RGB_TO_U(c.R,c.G,c.B);
	V = RGB_TO_V(c.R,c.G,c.B);
}
//////////////////////////////////////////////////////
System::Drawing::Color FFmpeg::UYVY::Color::get()
{
	return System::Drawing::Color::FromArgb(0xff,
		YUV_TO_R(Y0,U,V),
		YUV_TO_G(Y0,U,V),
		YUV_TO_B(Y0,U,V)
	);
}
void FFmpeg::UYVY::Color::set(System::Drawing::Color c) 
{
	Y0 = RGB_TO_Y(c.R,c.G,c.B);
	Y1 = Y0;
	U = RGB_TO_U(c.R,c.G,c.B);
	V = RGB_TO_V(c.R,c.G,c.B);
}
//////////////////////////////////////////////////////
// AVMemPtr
//////////////////////////////////////////////////////
FFmpeg::AVMemPtr::AVMemPtr()
	: m_pRoot(nullptr)
	, m_bAllocated(false)
	, m_bDisposed(false)
	, m_nRef(1)
	, m_pGetSize(nullptr)
	, m_pOpaque(nullptr)
	, m_nPosition(0)
	, m_nSize(0)
	, m_pPointer(nullptr)
{
}
FFmpeg::AVMemPtr::AVMemPtr(AVMemPtr^ root, int position)
	: m_pRoot(nullptr)
	, m_bAllocated(false)
	, m_bDisposed(false)
	, m_nRef(1)
	, m_pGetSize(nullptr)
	, m_pOpaque(nullptr)
{
	m_nPosition = position;
	m_nSize = root->m_nSize;
	m_pPointer = root->m_pPointer;
	if (root->m_bAllocated)
	{
		m_pRoot = root;
		m_pRoot->AddRef();
	}
}
FFmpeg::AVMemPtr::AVMemPtr(IntPtr p, TGetSizeFN * pGetSize, void * opaque)
	: m_pRoot(nullptr)
	, m_bAllocated(false)
	, m_bDisposed(false)
	, m_nRef(1)
	, m_nPosition(0)
	, m_nSize(0)
	, m_pGetSize(pGetSize)
	, m_pOpaque(opaque)
{
	m_pPointer = p.ToPointer();
}
FFmpeg::AVMemPtr::AVMemPtr(int size, int align)
	: m_pRoot(nullptr)
	, m_bAllocated(true)
	, m_bDisposed(false)
	, m_nRef(1)
	, m_nPosition(0)
	, m_nSize(size)
	, m_pGetSize(nullptr)
	, m_pOpaque(nullptr)
{
	m_pPointer = av_malloc(size);
	if (m_pPointer)
	{
		Monitor::Enter(m_AllocatedMemory);
		m_AllocatedMemory->Add((IntPtr)m_pPointer);
		Monitor::Exit(m_AllocatedMemory);
	}
}
FFmpeg::AVMemPtr::AVMemPtr(int size)
	: m_pRoot(nullptr)
	, m_bAllocated(true)
	, m_bDisposed(false)
	, m_nRef(1)
	, m_nPosition(0)
	, m_nSize(size)
	, m_pGetSize(nullptr)
	, m_pOpaque(nullptr)
{
	m_pPointer = av_malloc(size);
	if (m_pPointer)
	{
		Monitor::Enter(m_AllocatedMemory);
		m_AllocatedMemory->Add((IntPtr)m_pPointer);
		Monitor::Exit(m_AllocatedMemory);
	}
}
FFmpeg::AVMemPtr::AVMemPtr(IntPtr p)
	: m_pRoot(nullptr)
	, m_bAllocated(false)
	, m_bDisposed(false)
	, m_nRef(1)
	, m_nPosition(0)
	, m_nSize(-1)
	, m_pGetSize(nullptr)
	, m_pOpaque(nullptr)
{
	m_pPointer = p.ToPointer();
}
FFmpeg::AVMemPtr::AVMemPtr(IntPtr p, int size)
	: m_pRoot(nullptr)
	, m_bAllocated(false)
	, m_bDisposed(false)
	, m_nRef(1)
	, m_nPosition(0)
	, m_nSize(size)
	, m_pGetSize(nullptr)
	, m_pOpaque(nullptr)
{
	m_pPointer = p.ToPointer();
}
FFmpeg::AVMemPtr::~AVMemPtr()
{
	if (!m_bDisposed)
	{
		Release();
		m_bDisposed = true;
	}
}
FFmpeg::AVMemPtr::!AVMemPtr()
{
	if (!m_bDisposed)
	{
		m_bDisposed = true;
		Release();
	}
}
//////////////////////////////////////////////////////
long FFmpeg::AVMemPtr::AddRef()
{
	long lRef = 0;
	if (!m_bDisposed)
	{
		lRef = m_nRef++;
	}
	return lRef;
}
long FFmpeg::AVMemPtr::Release()
{
	long lRef = 0;
	if (m_nRef > 0)
	{
		lRef = --m_nRef;
		if (lRef == 0)
		{
			Free();
		}
	}
	return lRef;
}
//////////////////////////////////////////////////////
void FFmpeg::AVMemPtr::Free()
{
	AVMemPtr^ ptr = m_pRoot;
	m_pRoot = nullptr;
	if (ptr)
	{
		ptr->Release();
	}
	if (m_bAllocated)
	{
		Monitor::Enter(m_AllocatedMemory);
		int idx = m_AllocatedMemory->IndexOf((IntPtr)m_pPointer);
		if (idx >= 0) m_AllocatedMemory->RemoveAt(idx);
		Monitor::Exit(m_AllocatedMemory);
		m_bAllocated = false;
		av_free(m_pPointer);
	}
	m_pPointer = nullptr;
	m_nSize = 0;
	m_nPosition = 0;
}
//////////////////////////////////////////////////////
bool FFmpeg::AVMemPtr::allocated::get()
{ 
	return m_pRoot != nullptr ? m_pRoot->allocated : m_bAllocated; 
}
int FFmpeg::AVMemPtr::size::get()
{
	int _size;
	if (m_pRoot != nullptr)
	{
		_size = m_pRoot->size;
	}
	else
	{
		if (m_pGetSize)
		{
			_size = m_pGetSize(m_pPointer,m_pOpaque);
		}
		else
		{
			_size = m_nSize;
		}
	}
	return _size > 0 ? (_size - m_nPosition) : 0;
}
byte FFmpeg::AVMemPtr::default::get(int idx)
{
	return ((uint8_t*)m_pPointer)[idx + m_nPosition];
}
void FFmpeg::AVMemPtr::default::set(int idx, byte value)
{
	((uint8_t*)m_pPointer)[idx + m_nPosition] = value;
}
//////////////////////////////////////////////////////
IntPtr FFmpeg::AVMemPtr::Detach()
{
	IntPtr p = (IntPtr)m_pPointer;
	m_nSize = 0;
	m_nPosition = 0;
	m_bAllocated = false;
	m_pPointer = nullptr;
	return p;
}

void FFmpeg::AVMemPtr::Attach(IntPtr p)
{
	if (p.ToPointer() != m_pPointer)
	{
		Free();
		Monitor::Enter(m_AllocatedMemory);
		int idx = m_AllocatedMemory->IndexOf(p);
		if (idx < 0)
		{
			m_AllocatedMemory->Add(p);
			m_bAllocated = true;
		}
		Monitor::Exit(m_AllocatedMemory);
		m_pPointer = p.ToPointer();
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVMemPtr::operator IntPtr(AVMemPtr^ p)
{
	return (Object^)p != nullptr ? (IntPtr)((uint8_t*)p->m_pPointer + p->m_nPosition): IntPtr::Zero;
}
FFmpeg::AVMemPtr::operator AVMemPtr^(IntPtr p)
{
	return ((p == IntPtr::Zero) ? nullptr : gcnew AVMemPtr(p));
}
//////////////////////////////////////////////////////
FFmpeg::AVMemPtr::operator bool(AVMemPtr^ p)
{
	return ((Object^)p != nullptr && p->m_pPointer != nullptr);
}
bool FFmpeg::AVMemPtr::operator !(AVMemPtr^ p)
{
	return ((Object^)p == nullptr || p->m_pPointer == nullptr);
}
//////////////////////////////////////////////////////
FFmpeg::AVMemPtr::operator byte(AVMemPtr^ p)
{
	uint8_t * value = ((uint8_t *)p->m_pPointer) + p->m_nPosition;
	return *value;
}
FFmpeg::AVMemPtr::operator short(AVMemPtr^ p)
{
	int16_t * value = (int16_t *)(((uint8_t *)p->m_pPointer) + p->m_nPosition);
	return *value;
}
FFmpeg::AVMemPtr::operator int(AVMemPtr^ p)
{
	int32_t * value = (int32_t *)(((uint8_t *)p->m_pPointer) + p->m_nPosition);
	return *value;
}
FFmpeg::AVMemPtr::operator __int64(AVMemPtr^ p)
{
	int64_t * value = (int64_t *)(((uint8_t *)p->m_pPointer) + p->m_nPosition);
	return *value;
}
FFmpeg::AVMemPtr::operator float(AVMemPtr^ p)
{
	float * value = (float *)(((uint8_t *)p->m_pPointer) + p->m_nPosition);
	return *value;
}
FFmpeg::AVMemPtr::operator double(AVMemPtr^ p)
{
	double * value = (double *)(((uint8_t *)p->m_pPointer) + p->m_nPosition);
	return *value;
}
FFmpeg::AVMemPtr::operator unsigned short(AVMemPtr^ p)
{
	unsigned short * value = (unsigned short *)(((uint8_t *)p->m_pPointer) + p->m_nPosition);
	return *value;
}
FFmpeg::AVMemPtr::operator unsigned int(AVMemPtr^ p)
{
	unsigned int * value = (unsigned int *)(((uint8_t *)p->m_pPointer) + p->m_nPosition);
	return *value;
}
//////////////////////////////////////////////////////
bool FFmpeg::AVMemPtr::operator == (AVMemPtr^ a,AVMemPtr^ b)
{
	if ((Object^)a != nullptr && (Object^)b != nullptr)
	{
		return (a->m_pPointer == b->m_pPointer) && (a->m_nPosition == b->m_nPosition);
	}
	return ((Object^)a) == ((Object^)b);
}
bool FFmpeg::AVMemPtr::operator != (AVMemPtr^ a,AVMemPtr^ b)
{
	return !(a == b);
}
bool FFmpeg::AVMemPtr::operator == (AVMemPtr^ a,IntPtr p)
{
	if ((Object^)a != nullptr)
	{
		return ((void*)((uint8_t*)a->m_pPointer + a->m_nPosition) == p.ToPointer());
	}
	return p == IntPtr::Zero;
}
bool FFmpeg::AVMemPtr::operator != (AVMemPtr^a,IntPtr p )
{
	return !(a == p);
}
bool FFmpeg::AVMemPtr::operator == (AVMemPtr^ a,int i)
{
	return (a == (IntPtr)i);
}
bool FFmpeg::AVMemPtr::operator != (AVMemPtr^ a ,int i)
{
	return !(a == i);
}
bool FFmpeg::AVMemPtr::operator == (AVMemPtr^ a,Int64 ll)
{
	return (a == (IntPtr)ll);
}
bool FFmpeg::AVMemPtr::operator != (AVMemPtr^ a,Int64 ll)
{
	return !(a == ll);
}
//////////////////////////////////////////////////////
FFmpeg::AVMemPtr^ FFmpeg::AVMemPtr::operator + (AVMemPtr^ a,int n)
{
	return gcnew AVMemPtr(a,a->m_nPosition + n);
}
FFmpeg::AVMemPtr^ FFmpeg::AVMemPtr::operator - (AVMemPtr^ a,int n)
{
	return gcnew AVMemPtr(a,a->m_nPosition - n);
}
FFmpeg::AVMemPtr^ FFmpeg::AVMemPtr::operator + (AVMemPtr^ a,Int64 ll)
{
	return gcnew AVMemPtr(a,a->m_nPosition + (int)ll);
}
FFmpeg::AVMemPtr^ FFmpeg::AVMemPtr::operator - (AVMemPtr^ a,Int64 ll)
{
	return gcnew AVMemPtr(a,a->m_nPosition - (int)ll);
}
//////////////////////////////////////////////////////
FFmpeg::AVMemPtr^ FFmpeg::AVMemPtr::operator += (int n)
{
	m_nPosition += n;
	return this;
}
FFmpeg::AVMemPtr^ FFmpeg::AVMemPtr::operator -= (int n)
{
	m_nPosition -= n;
	return this;
}
FFmpeg::AVMemPtr^ FFmpeg::AVMemPtr::operator += (Int64 ll)
{
	m_nPosition += (int)ll;
	return this;
}
FFmpeg::AVMemPtr^ FFmpeg::AVMemPtr::operator -= (Int64 ll)
{
	m_nPosition -= (int)ll;
	return this;
}
FFmpeg::AVMemPtr^ FFmpeg::AVMemPtr::operator ++ ()
{
	m_nPosition++;
	return this;
}
FFmpeg::AVMemPtr^ FFmpeg::AVMemPtr::operator ++ (int n)
{
	m_nPosition++;
	return this;
}
FFmpeg::AVMemPtr^ FFmpeg::AVMemPtr::operator -- ()
{
	m_nPosition--;
	return this;
}
FFmpeg::AVMemPtr^ FFmpeg::AVMemPtr::operator -- (int n)
{
	m_nPosition--;
	return this;
}
//////////////////////////////////////////////////////
FFmpeg::AVArray<byte>^  FFmpeg::AVMemPtr::bytes::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<byte>((IntPtr)p,count, false);
}
FFmpeg::AVArray<short>^ FFmpeg::AVMemPtr::shorts::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<short>((IntPtr)p,count / sizeof(short), false);
}
FFmpeg::AVArray<int>^ FFmpeg::AVMemPtr::integers::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<int>((IntPtr)p,count / sizeof(int), false);
}
FFmpeg::AVArray<float>^ FFmpeg::AVMemPtr::floats::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<float>((IntPtr)p,count / sizeof(float), false);
}
FFmpeg::AVArray<double>^ FFmpeg::AVMemPtr::doubles::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<double>((IntPtr)p,count / sizeof(double), false);
}
FFmpeg::AVArray<IntPtr>^ FFmpeg::AVMemPtr::pointers::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<IntPtr>((IntPtr)p,count / sizeof(void*));
}
FFmpeg::AVArray<unsigned int>^	FFmpeg::AVMemPtr::uints::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<unsigned int>((IntPtr)p,count / sizeof(unsigned int), false);
}
FFmpeg::AVArray<unsigned short>^ FFmpeg::AVMemPtr::ushorts::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<unsigned short>((IntPtr)p,count / sizeof(unsigned short), false);
}
FFmpeg::AVArray<FFmpeg::RGB^>^   FFmpeg::AVMemPtr::rgb::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<RGB^>((IntPtr)p,count / 3, false);
}
FFmpeg::AVArray<FFmpeg::RGBA^>^  FFmpeg::AVMemPtr::rgba::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<RGBA^>((IntPtr)p,count / 4, false);
}
FFmpeg::AVArray<FFmpeg::AYUV^>^  FFmpeg::AVMemPtr::ayuv::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<AYUV^>((IntPtr)p,count / 4, false);
}
FFmpeg::AVArray<FFmpeg::YUY2^>^  FFmpeg::AVMemPtr::yuy2::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<YUY2^>((IntPtr)p,count / 4, false);
}
FFmpeg::AVArray<FFmpeg::UYVY^>^  FFmpeg::AVMemPtr::uyvy::get()
{
	if (m_pPointer == nullptr) return nullptr;
	uint8_t * p = ((uint8_t *)m_pPointer) + m_nPosition;
	int count = size;
	return gcnew AVArray<UYVY^>((IntPtr)p,count > 4 ? count / 4 : 1, false);
}
//////////////////////////////////////////////////////
array<byte>^ FFmpeg::AVMemPtr::ToArray()
{
	array<byte>^ a = nullptr;
	int _size = size;
	IntPtr p = this;
	if (_size > 0 && p != IntPtr::Zero)
	{
		a = gcnew array<byte>(_size);
		Marshal::Copy(p,a,0,_size);
	}
	return a;
}
bool FFmpeg::AVMemPtr::Dump(String^ sFileName)
{
	return Dump(sFileName, false);
}
bool FFmpeg::AVMemPtr::Dump(String^ sFileName,bool bAppend)
{
	bool _result = false;
	int _size = size;
	IntPtr p = this;
	if (_size > 0 && p != IntPtr::Zero)
	{
		wchar_t * path = (wchar_t *)AVBase::AllocString(sFileName, true).ToPointer();
		try
		{
			HANDLE hFile = CreateFileW(path, FILE_GENERIC_WRITE, FILE_SHARE_READ, NULL, bAppend ? OPEN_ALWAYS : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				SetFilePointer(hFile, 0, NULL, FILE_END);
				DWORD dwResult;
				_result = (WriteFile(hFile, p.ToPointer(), (DWORD)_size, &dwResult, NULL) == TRUE);
				CloseHandle(hFile);
			}
		}
		finally
		{
			AVBase::FreeMemory((IntPtr)path);
		}
	}
	return _result;
}
bool FFmpeg::AVMemPtr::Dump(Stream^ _stream)
{
	array<byte>^ _array = ToArray();
	if (_array != nullptr)
	{
		_stream->Write(_array,0,_array->Length);
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVMemPtr::ToString()
{
	return ((IntPtr)((uint8_t*)m_pPointer + m_nPosition)).ToString();
}
//////////////////////////////////////////////////////
// LibAVUtil
//////////////////////////////////////////////////////
FFmpeg::LibAVUtil::LibAVUtil()
{
}
//////////////////////////////////////////////////////
UInt32 FFmpeg::LibAVUtil::Version::get()
{
	return avutil_version();
}
String^ FFmpeg::LibAVUtil::Configuration::get()
{
	auto p = avutil_configuration();
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::LibAVUtil::License::get()
{
	auto p = avutil_license();
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
// AVSampleFmt
//////////////////////////////////////////////////////
String^ FFmpeg::AVSampleFmt::get_sample_fmt_name(AVSampleFormat sample_fmt)
{
	auto p = av_get_sample_fmt_name((::AVSampleFormat)sample_fmt);
	return p != nullptr ? gcnew String(p) : nullptr;
}

FFmpeg::AVSampleFormat FFmpeg::AVSampleFmt::get_sample_fmt(String^ name)
{
	char * szName = nullptr;
	szName = (char*)AVBase::AllocString(name).ToPointer();
	try
	{
		return (AVSampleFormat)av_get_sample_fmt(szName);
	}
	finally
	{
		AVBase::FreeMemory((IntPtr)szName);
	}
}

FFmpeg::AVSampleFormat FFmpeg::AVSampleFmt::get_alt_sample_fmt(AVSampleFormat sample_fmt, bool planar)
{
	return (AVSampleFormat)av_get_alt_sample_fmt((::AVSampleFormat)sample_fmt,planar ? 1 : 0);
}

FFmpeg::AVSampleFormat FFmpeg::AVSampleFmt::get_packed_sample_fmt(AVSampleFormat sample_fmt)
{
	return (AVSampleFormat)av_get_packed_sample_fmt((::AVSampleFormat)sample_fmt);
}

FFmpeg::AVSampleFormat FFmpeg::AVSampleFmt::get_planar_sample_fmt(AVSampleFormat sample_fmt)
{
	return (AVSampleFormat)av_get_planar_sample_fmt((::AVSampleFormat)sample_fmt);
}

int FFmpeg::AVSampleFmt::get_bytes_per_sample(AVSampleFormat sample_fmt)
{
	return av_get_bytes_per_sample((::AVSampleFormat)sample_fmt);
}

bool FFmpeg::AVSampleFmt::sample_fmt_is_planar(AVSampleFormat sample_fmt)
{
	return (1 == av_sample_fmt_is_planar((::AVSampleFormat)sample_fmt));
}
//////////////////////////////////////////////////////
// AVChannels
//////////////////////////////////////////////////////
FFmpeg::AVChannelLayout FFmpeg::AVChannels::get_channel_layout(String^ name)
{
	char * szName = nullptr;
	if (!String::IsNullOrEmpty(name))
	{
		szName = (char*) AVBase::AllocString(name).ToPointer();
		try
		{
			return (AVChannelLayout)av_get_channel_layout(szName);
		}
		finally
		{
			AVBase::FreeMemory((IntPtr)szName);
		}
	}
	return AVChannelLayout::UNKNOWN;
}

int FFmpeg::AVChannels::get_nb_channels(AVChannelLayout channel_layout)
{
	return av_get_channel_layout_nb_channels((uint64_t)channel_layout);
}

FFmpeg::AVChannelLayout FFmpeg::AVChannels::get_default_channel_layout(int nb_channels)
{
	return (AVChannelLayout)av_get_default_channel_layout(nb_channels);
}

int FFmpeg::AVChannels::get_channel_index(AVChannelLayout channel_layout,AVChannelLayout channel)
{
	return av_get_channel_layout_channel_index((uint64_t)channel_layout,(uint64_t)channel);
}

FFmpeg::AVChannelLayout FFmpeg::AVChannels::extract_channel(AVChannelLayout channel_layout, int index)
{
	return (AVChannelLayout)av_channel_layout_extract_channel((uint64_t)channel_layout,index);
}

String^ FFmpeg::AVChannels::get_channel_name(AVChannelLayout channel)
{
	auto p = av_get_channel_name((uint64_t)channel);
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::AVChannels::get_channel_layout_string(AVChannelLayout layout)
{
	return get_channel_layout_string(-1,layout);
}

String^ FFmpeg::AVChannels::get_channel_layout_string(int nb_channels, AVChannelLayout layout)
{
	char buf[300] = {0};
	av_get_channel_layout_string(buf,_countof(buf),nb_channels,(uint64_t)layout);
	return gcnew String(buf);
}

String^ FFmpeg::AVChannels::get_channel_description(AVChannelLayout channel)
{
	auto p = av_get_channel_name((uint64_t)channel);
	return p != nullptr ? gcnew String(p) : nullptr;
}

bool FFmpeg::AVChannels::get_standard_channel_layout(UInt32 index, AVChannelLayout % layout,String^ % name)
{
	uint64_t _layout = 0;
	const char * _name = nullptr;
	try
	{
		return 0 == av_get_standard_channel_layout(index, &_layout,&_name);
	}
	finally
	{
		layout = (AVChannelLayout)_layout;
		name = (_name != nullptr ? gcnew String(_name) : nullptr);
	}
}
//////////////////////////////////////////////////////
// AVSamples
//////////////////////////////////////////////////////
int FFmpeg::AVSamples::get_buffer_size(int nb_channels, int nb_samples, AVSampleFormat sample_fmt)
{
	return get_buffer_size(nb_channels, nb_samples, sample_fmt,false);
}
int FFmpeg::AVSamples::get_buffer_size([Out] int % linesize, int nb_channels, int nb_samples, AVSampleFormat sample_fmt)
{
	return get_buffer_size(linesize,nb_channels, nb_samples, sample_fmt,false);
}
int FFmpeg::AVSamples::get_buffer_size(int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align)
{
	int linesize;
	return get_buffer_size(linesize,nb_channels,nb_samples,sample_fmt,align);
}

int FFmpeg::AVSamples::get_buffer_size(int % linesize, int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align)
{
	int _linesize = 0;
	try
	{
		return av_samples_get_buffer_size(&_linesize,nb_channels, nb_samples,(::AVSampleFormat)sample_fmt,align ? 1 : 0);
	}
	finally
	{
		linesize = _linesize;
	}
}
//////////////////////////////////////////////////////
int FFmpeg::AVSamples::fill_arrays(array<IntPtr>^ % audio_data, IntPtr buf,
	int nb_channels, int nb_samples, AVSampleFormat sample_fmt)
{
	return fill_arrays(audio_data, buf,nb_channels, nb_samples, sample_fmt, false);
}

int FFmpeg::AVSamples::fill_arrays(AVArray<IntPtr>^ audio_data, IntPtr buf,
	int nb_channels, int nb_samples, AVSampleFormat sample_fmt)
{
	return fill_arrays(audio_data, buf,nb_channels, nb_samples, sample_fmt, false);
}

int FFmpeg::AVSamples::fill_arrays(array<IntPtr>^ % audio_data, [Out] int % linesize, IntPtr buf,
	int nb_channels, int nb_samples, AVSampleFormat sample_fmt)
{
	return fill_arrays(audio_data, linesize, buf,nb_channels, nb_samples, sample_fmt, false);
}

int FFmpeg::AVSamples::fill_arrays(AVArray<IntPtr>^ audio_data, [Out] int % linesize, IntPtr buf,
	int nb_channels, int nb_samples, AVSampleFormat sample_fmt)
{
	return fill_arrays(audio_data, linesize, buf,nb_channels, nb_samples, sample_fmt, false);
}

int FFmpeg::AVSamples::fill_arrays(array<IntPtr>^ % audio_data, IntPtr buf,
	int nb_channels, int nb_samples, AVSampleFormat sample_fmt, bool align)
{
	int linesize;
	return fill_arrays(audio_data, linesize, buf,nb_channels, nb_samples, sample_fmt, align);
}

int FFmpeg::AVSamples::fill_arrays(AVArray<IntPtr>^ audio_data, IntPtr buf,
	int nb_channels, int nb_samples, AVSampleFormat sample_fmt, bool align)
{
	int linesize;
	return fill_arrays(audio_data, linesize, buf,nb_channels, nb_samples, sample_fmt, align);
}

int FFmpeg::AVSamples::fill_arrays(array<IntPtr>^ % audio_data, int % linesize,IntPtr buf,
                           int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align)
{
	int _linesize = 0;
	uint8_t *_data[AV_NUM_DATA_POINTERS] = { 0 };
	try
	{
		return av_samples_fill_arrays(_data,(int*)&_linesize,(uint8_t *)buf.ToPointer(),nb_channels, nb_samples,(::AVSampleFormat)sample_fmt,align ? 1 : 0);
	}
	finally
	{
		linesize = _linesize;
		for (int i = 0; i < audio_data->Length && i < AV_NUM_DATA_POINTERS; i++) { audio_data[i] = (IntPtr)_data[i]; } 
	}
}

int FFmpeg::AVSamples::fill_arrays(AVArray<IntPtr>^ audio_data, int % linesize,IntPtr buf,
                           int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align)
{
	int _linesize = 0;
	uint8_t *_data[AV_NUM_DATA_POINTERS] = { 0 };
	try
	{
		return av_samples_fill_arrays(_data,&_linesize,(uint8_t *)buf.ToPointer(),nb_channels, nb_samples,(::AVSampleFormat)sample_fmt,align ? 1 : 0);
	}
	finally
	{
		linesize = _linesize;
		for (int i = 0; i < audio_data->Count && i < AV_NUM_DATA_POINTERS; i++) { audio_data[i] = (IntPtr)_data[i]; } 
	}
}

int FFmpeg::AVSamples::fill_arrays(AVPicture^ _frame, IntPtr buf, int nb_channels, int nb_samples, AVSampleFormat sample_fmt)
{
	return fill_arrays(_frame, buf, nb_channels, nb_samples, sample_fmt, false);
}
int FFmpeg::AVSamples::fill_arrays(AVFrame^ _frame, IntPtr buf, int nb_samples)
{
	return fill_arrays(_frame, buf, nb_samples, false);
}

int FFmpeg::AVSamples::fill_arrays(AVPicture^ _frame,IntPtr buf,
                           int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align)
{
	_frame->Free();
	int _linesize = 0;
	uint8_t *_data[AV_NUM_DATA_POINTERS] = { 0 };
	try
	{
		return av_samples_fill_arrays(_data,(int*)&_linesize,(uint8_t *)buf.ToPointer(),nb_channels, nb_samples,(::AVSampleFormat)sample_fmt,align ? 1 : 0);
	}
	finally
	{
		for (int i = 0; i < AV_NUM_DATA_POINTERS; i++) { _frame->linesize[i] = _linesize; _frame->data[i] = (IntPtr)_data[i]; } 
	}
}

int FFmpeg::AVSamples::fill_arrays(AVFrame^ _frame,IntPtr buf,int nb_samples,bool align)
{
	return fill_arrays(_frame, buf, _frame->channels, nb_samples, (AVSampleFormat)_frame->format, align);
}

int FFmpeg::AVSamples::copy(array<IntPtr>^ dst, array<IntPtr>^ src, int dst_offset,
                    int src_offset, int nb_samples, int nb_channels,
                    AVSampleFormat sample_fmt)
{
	uint8_t *_dst[AV_NUM_DATA_POINTERS] = { 0 };
	uint8_t *_src[AV_NUM_DATA_POINTERS] = { 0 };
	for (int i = 0; i < dst->Length && i < AV_NUM_DATA_POINTERS; i++) { _dst[i] = (uint8_t *)dst[i].ToPointer(); _src[i] = (uint8_t *)src[i].ToPointer(); } 
	return av_samples_copy(_dst,_src,dst_offset,src_offset, nb_samples,nb_channels,(::AVSampleFormat)sample_fmt);
}

int FFmpeg::AVSamples::copy(AVArray<IntPtr>^ dst, AVArray<IntPtr>^ src, int dst_offset,
                    int src_offset, int nb_samples, int nb_channels,
                    AVSampleFormat sample_fmt)
{
	return copy(dst->ToArray(), src->ToArray(), dst_offset, src_offset, nb_samples,nb_channels,sample_fmt);
}

int FFmpeg::AVSamples::copy(AVPicture^ dst, AVPicture^ src, int dst_offset,
                    int src_offset, int nb_samples, int nb_channels,
                    AVSampleFormat sample_fmt)
{
	uint8_t ** _dst = (uint8_t **)dst->data->_Pointer.ToPointer();
	uint8_t **_src = (uint8_t **)src->data->_Pointer.ToPointer();
	return av_samples_copy(_dst,_src,dst_offset,src_offset,nb_samples,nb_channels,(::AVSampleFormat)sample_fmt);
}

int FFmpeg::AVSamples::copy(AVPicture^ dst, AVFrame^ src, int dst_offset,
                    int src_offset, int nb_samples)
{
	uint8_t ** _dst = (uint8_t **)dst->data->_Pointer.ToPointer();
	uint8_t **_src = (uint8_t **)src->data->_Pointer.ToPointer();
	return av_samples_copy(_dst,_src,dst_offset,src_offset,nb_samples,src->channels,(::AVSampleFormat)src->format);
}
int FFmpeg::AVSamples::copy(array<IntPtr>^ dst, AVFrame^ src, int dst_offset,
	int src_offset, int nb_samples)
{
	uint8_t **_src = (uint8_t **)src->data->_Pointer.ToPointer();
	uint8_t *_dst[AV_NUM_DATA_POINTERS] = { 0 };
	for (int i = 0; i < dst->Length && i < AV_NUM_DATA_POINTERS; i++) { _dst[i] = (uint8_t *)dst[i].ToPointer(); }
	return av_samples_copy(_dst,_src,dst_offset,src_offset,nb_samples,src->channels,(::AVSampleFormat)src->format);
}
int FFmpeg::AVSamples::copy(AVPicture^ dst, array<IntPtr>^ src, int dst_offset,
	int src_offset, int nb_samples, int nb_channels,
	AVSampleFormat sample_fmt)
{
	uint8_t ** _dst = (uint8_t **)dst->data->_Pointer.ToPointer();
	uint8_t *_src[AV_NUM_DATA_POINTERS] = { 0 };
	for (int i = 0; i < src->Length && i < AV_NUM_DATA_POINTERS; i++) { _src[i] = (uint8_t *)src[i].ToPointer(); }
	return av_samples_copy(_dst,_src,dst_offset,src_offset, nb_samples,nb_channels,sample_fmt);
}

int FFmpeg::AVSamples::alloc(array<IntPtr>^ % audio_data, int % linesize,
                           int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align)
{
	audio_data = gcnew array<IntPtr>(8);
	int _linesize = { 0 };
	uint8_t *_data[AV_NUM_DATA_POINTERS] = { 0 };
	try
	{
		return av_samples_alloc(_data,(int*)&_linesize,nb_channels, nb_samples,(::AVSampleFormat)sample_fmt,align ? 1 : 0);
	}
	finally
	{
		linesize = _linesize;
		for (int i = 0; i < AV_NUM_DATA_POINTERS; i++) { audio_data[i] = (IntPtr)_data[i]; } 
	}
}

int FFmpeg::AVSamples::alloc(AVArray<IntPtr>^ audio_data, int % linesize,
                           int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align)
{
	int _linesize = 0;
	uint8_t *_data[AV_NUM_DATA_POINTERS] = { 0 };
	try
	{
		return av_samples_alloc(_data,(int*)&_linesize,nb_channels, nb_samples,(::AVSampleFormat)sample_fmt,align ? 1 : 0);
	}
	finally
	{
		linesize = _linesize;
		for (int i = 0; i < audio_data->Count && i < AV_NUM_DATA_POINTERS; i++) { audio_data[i] = (IntPtr)_data[i]; } 
	}
}

int FFmpeg::AVSamples::alloc(AVPicture^ frame,int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align)
{
	frame->Free();
	int _linesize = { 0 };
	uint8_t *_data[AV_NUM_DATA_POINTERS] = { 0 };
	try
	{
		return av_samples_alloc(_data,&_linesize,nb_channels, nb_samples,(::AVSampleFormat)sample_fmt,align ? 1 : 0);
	}
	finally
	{
		for (int i = 0; i < AV_NUM_DATA_POINTERS; i++) { frame->linesize[i] = _linesize; frame->data[i] = (IntPtr)_data[i]; } 
	}
}

int FFmpeg::AVSamples::free(array<IntPtr>^ audio_data)
{
	if (audio_data != nullptr && audio_data->Length > 0 && audio_data[0] != IntPtr::Zero)
	{
		av_free(audio_data[0].ToPointer());
	}
	return 0;
}

int FFmpeg::AVSamples::free(AVArray<IntPtr>^ audio_data)
{
	if (audio_data == nullptr) return 0;
	return free(audio_data->ToArray());
}

int FFmpeg::AVSamples::free(AVPicture^ frame)
{
	if (frame == nullptr) return 0;
	return free(frame->data->ToPtrArray());
}

int FFmpeg::AVSamples::set_silence(AVArray<IntPtr>^ audio_data, int offset, int nb_samples,
                           int nb_channels, AVSampleFormat sample_fmt)
{
	return set_silence(audio_data->ToArray(),offset, nb_samples,nb_channels,sample_fmt);
}

int FFmpeg::AVSamples::set_silence(array<IntPtr>^ audio_data, int offset, int nb_samples,
                           int nb_channels, AVSampleFormat sample_fmt)
{
	uint8_t *_data[AV_NUM_DATA_POINTERS] = { 0 };
	for (int i = 0; i < audio_data->Length && i < AV_NUM_DATA_POINTERS; i++) { _data[i] = (uint8_t *)audio_data[i].ToPointer(); } 
	return av_samples_set_silence(_data,offset,nb_samples,nb_channels,(::AVSampleFormat)sample_fmt);
}

int FFmpeg::AVSamples::set_silence(AVPicture^ dst, int offset, int nb_samples,
                           int nb_channels, AVSampleFormat sample_fmt)
{
	uint8_t **_data = (uint8_t **)dst->data->_Pointer.ToPointer();
	return av_samples_set_silence(_data,offset,nb_samples,nb_channels,(::AVSampleFormat)sample_fmt);
}
//////////////////////////////////////////////////////
// AVMath
//////////////////////////////////////////////////////
FFmpeg::AVRational^ FFmpeg::AVMath::TIME_BASE_Q::get()
{
	return gcnew AVRational(1, AV_TIME_BASE);
}
//////////////////////////////////////////////////////
Int64 FFmpeg::AVMath::gcd(Int64 a, Int64 b)
{
	return av_gcd(a,b);
}

Int64 FFmpeg::AVMath::rescale(Int64 a, Int64 b, Int64 c)
{
	return av_rescale(a,b,c);
}

Int64 FFmpeg::AVMath::rescale_rnd(Int64 a, Int64 b, Int64 c, AVRounding r)
{
	return av_rescale_rnd(a,b,c,(::AVRounding)r);
}

Int64 FFmpeg::AVMath::rescale_q(Int64 a, AVRational^ bq, AVRational^ cq)
{
	return av_rescale_q(a,*((::AVRational*)bq->_Pointer.ToPointer()),*((::AVRational*)cq->_Pointer.ToPointer()));
}

Int64 FFmpeg::AVMath::rescale_q_rnd(Int64 a, AVRational^ bq, AVRational^ cq,AVRounding r)
{
	return av_rescale_q_rnd(a,*((::AVRational*)bq->_Pointer.ToPointer()),*((::AVRational*)cq->_Pointer.ToPointer()),(::AVRounding)r);
}

int FFmpeg::AVMath::compare_ts(Int64 ts_a, AVRational^ tb_a, Int64 ts_b, AVRational^ tb_b)
{
	return av_compare_ts(ts_a,*((::AVRational*)tb_a->_Pointer.ToPointer()),ts_b,*((::AVRational*)tb_b->_Pointer.ToPointer()));
}

Int64 FFmpeg::AVMath::compare_mod(UInt64 a, UInt64 b, UInt64 mod)
{
	return av_compare_mod(a,b,mod);
}

Int64 FFmpeg::AVMath::rescale_delta(AVRational^ in_tb, Int64 in_ts,  AVRational^ fs_tb, int duration, Int64 % last, AVRational^ out_tb)
{
	int64_t _last = last;
	try
	{	
		return av_rescale_delta(*((::AVRational*)in_tb->_Pointer.ToPointer()),in_ts,*((::AVRational*)fs_tb->_Pointer.ToPointer()),duration, &_last,*((::AVRational*)out_tb->_Pointer.ToPointer()));
	}
	finally
	{
		last = _last;
	}
}

Int64 FFmpeg::AVMath::add_stable(AVRational^ ts_tb, Int64 ts, AVRational^ inc_tb, Int64 inc)
{
	return av_add_stable(*((::AVRational*)ts_tb->_Pointer.ToPointer()),ts,*((::AVRational*)inc_tb->_Pointer.ToPointer()),inc);
}
//////////////////////////////////////////////////////
// AVUtil
//////////////////////////////////////////////////////
char FFmpeg::AVUtil::get_picture_type_char(AVPictureType pict_type)
{
	return av_get_picture_type_char((::AVPictureType)pict_type);
}

String^ FFmpeg::AVUtil::get_media_type_string(AVMediaType media_type)
{
	auto p = av_get_media_type_string((::AVMediaType)media_type);
	return p != nullptr ? gcnew String(p) : nullptr;
}

FFmpeg::AV_CPU_FLAG FFmpeg::AVUtil::get_cpu_flags()
{
	AVBase::EnsureLibraryLoaded();
	DYNAMIC_DEF_API(AVUtil,int,(AV_CPU_FLAG)0,av_get_cpu_flags);
	return (AV_CPU_FLAG)av_get_cpu_flags();
}
void FFmpeg::AVUtil::force_cpu_flags(AV_CPU_FLAG flags)
{
	AVBase::EnsureLibraryLoaded();
	VOID_API(AVUtil,av_force_cpu_flags,int)
	av_force_cpu_flags((int)flags);
}
int FFmpeg::AVUtil::cpu_count()
{
	AVBase::EnsureLibraryLoaded();
	DYNAMIC_DEF_API(AVUtil,int,1,av_cpu_count);
	return av_cpu_count();
}

String^ FFmpeg::AVUtil::strerror(int errnum)
{
	char szTemp[512] = {0};
	av_strerror(errnum,szTemp,_countof(szTemp));
	return gcnew String(szTemp);
}
Int64 FFmpeg::AVUtil::gettime()
{
	return av_gettime();
}

Int64 FFmpeg::AVUtil::gettime_relative()
{
	return av_gettime_relative();
}

bool FFmpeg::AVUtil::gettime_relative_is_monotonic()
{
	return 1 == av_gettime_relative_is_monotonic();
}

int FFmpeg::AVUtil::usleep(UInt32 usec)
{
	return av_usleep(usec);
}

UInt32 FFmpeg::AVUtil::get_random_seed()
{
	return av_get_random_seed();
}

String^ FFmpeg::AVUtil::ts2timestr(Int64 ts, AVRational^ tb)
{
	char _temp[AV_TS_MAX_STRING_SIZE];
	memset(_temp,0,sizeof(_temp));
	return gcnew String(av_ts_make_time_string(_temp,ts,tb != nullptr? (::AVRational*)tb->_Pointer.ToPointer() : nullptr));
}

String^ FFmpeg::AVUtil::ts2timestr(Int64 ts)
{
	char _temp[AV_TS_MAX_STRING_SIZE];
	memset(_temp,0,sizeof(_temp));
	return gcnew String(av_ts_make_string(_temp,ts));
}

//////////////////////////////////////////////////////
// AVOption
//////////////////////////////////////////////////////
FFmpeg::AVOption::AVOption(void * _pointer,AVBase^ parent)
	: AVBase(_pointer,parent)
{

}
//////////////////////////////////////////////////////
int FFmpeg::AVOption::_StructureSize::get()
{
	return sizeof(::AVOption);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVOption::name::get()
{
	auto p = ((::AVOption*) m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::AVOption::help::get()
{ 
	auto p = ((::AVOption*) m_pPointer)->help;
	return p != nullptr ? gcnew String(p) : nullptr;
}
int FFmpeg::AVOption::offset::get()
{ 
	return ((::AVOption*)m_pPointer)->offset;
}
FFmpeg::AVOptionType FFmpeg::AVOption::type::get()
{ 
	return (FFmpeg::AVOptionType)((::AVOption*)m_pPointer)->type;
}
IntPtr FFmpeg::AVOption::default_val::get()
{
	return (IntPtr)((void*)((::AVOption*)m_pPointer)->default_val.str);
}
double FFmpeg::AVOption::min::get()
{
	return ((::AVOption*)m_pPointer)->min;
}
double FFmpeg::AVOption::max::get()
{
	return ((::AVOption*)m_pPointer)->max;
}
FFmpeg::AVOptFlag FFmpeg::AVOption::flags::get()
{
	return (FFmpeg::AVOptFlag)((::AVOption*)m_pPointer)->flags;
}
String^ FFmpeg::AVOption::unit::get()
{
	auto p = ((::AVOption*) m_pPointer)->unit;
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVOption::ToString()
{
	if (m_pPointer)
	{
		String^ _temp = name::get();
		if (!String::IsNullOrEmpty(_temp))
		{
			_temp = "[ AVOption ] \"" + _temp + "\"";
			String^ _help = help::get();
			if (!String::IsNullOrEmpty(_help))
			{
				_temp += " ('" + _help + "')";
			}
			return _temp;
		}
	}
	return __super::ToString();
}
//////////////////////////////////////////////////////
// AVOptions::Enumerator
//////////////////////////////////////////////////////
FFmpeg::AVOptions::Enumerator::Enumerator(AVOptions^ parent)
	: m_pParent(parent)
{

}
FFmpeg::AVOptions::Enumerator::~Enumerator()
{

}
//////////////////////////////////////////////////////
// IEnumerator
bool FFmpeg::AVOptions::Enumerator::MoveNext()
{
	const ::AVOption * p = (const ::AVOption *)m_pOption.ToPointer();
	m_pOption = (IntPtr)(void*)av_opt_next(m_pParent->_Pointer.ToPointer(), p);
	return m_pOption != IntPtr::Zero;
}
FFmpeg::AVOption^ FFmpeg::AVOptions::Enumerator::Current::get()
{
	if (m_pOption == IntPtr::Zero)
	{
		if (!MoveNext()) return nullptr;
	}
	return _CreateChildObject<AVOption>(m_pOption.ToPointer(),m_pParent);
}
void FFmpeg::AVOptions::Enumerator::Reset()
{
	m_pOption = IntPtr::Zero;
}
Object^ FFmpeg::AVOptions::Enumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVOptions
//////////////////////////////////////////////////////
FFmpeg::AVOptions::AVOptions(void * _pointer,AVBase^ parent)
	: AVBase(_pointer,parent)
{
}

FFmpeg::AVOptions::AVOptions(IntPtr obj)
	: AVBase(obj.ToPointer(),nullptr)
{

}

FFmpeg::AVOptions::AVOptions(AVBase^ obj)
	: AVBase(obj->_Pointer.ToPointer(),obj)
{

}
//////////////////////////////////////////////////////
int FFmpeg::AVOptions::Count::get()
{
	int count = 0;
	const ::AVOption * p = nullptr;;
	while ((p = av_opt_next(m_pPointer, p)) != nullptr) { count++; }
	return count;
}

FFmpeg::AVOption^ FFmpeg::AVOptions::default::get(int index)
{
	const ::AVOption * p = nullptr;
	while ((p = av_opt_next(m_pPointer, p)) != nullptr) 
	{ 
		if (index-- == 0)
		{
			return _CreateObject<AVOption>((void*)p);
		}
	}
	return nullptr;
}

FFmpeg::AVOption^ FFmpeg::AVOptions::default::get(String^ name)
{
	return Find(name,nullptr,AVOptFlag::All,AVOptSearch::None);
}

bool FFmpeg::AVOptions::Contains(String^ name)
{
	return IndexOf(name) >= 0;
}

int FFmpeg::AVOptions::IndexOf(String^ name)
{
	bool found = false;
	int index = -1;
	if (name != nullptr && name->Length > 0)
	{
		char * _name = (char *)AllocString(name).ToPointer();
		try
		{
			const ::AVOption * p = nullptr;;
			while ((p = av_opt_next(m_pPointer, p)) != nullptr && !found)
			{
				index++;
				found = (_stricmp(p->name,_name) == 0);
			}
		}
		finally
		{
			FreeMemory((IntPtr)_name);
		}
	}
	return found ? index : -1;
}

int FFmpeg::AVOptions::IndexOf(AVOption^ value)
{
	bool found = false;
	int index = -1;
	if (value && value->_Pointer != IntPtr::Zero)
	{
		const ::AVOption * p = nullptr;;
		while ((p = av_opt_next(m_pPointer, p)) != nullptr && !found)
		{
			index++;
			found = (p == value->_Pointer.ToPointer());
		}
	}
	return found ? index : -1;
}
//////////////////////////////////////////////////////
void FFmpeg::AVOptions::set_defaults()
{
	av_opt_set_defaults(m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::AVOptions::set_options_string(String^ opts,String^ key_val_sep, String^ pairs_sep)
{
	char * _opts = nullptr;
	char * _key_val_sep = nullptr;
	char * _pairs_sep = nullptr;
	try
	{
		_opts = (char*)AllocString(opts).ToPointer();
		_key_val_sep = (char*)AllocString(key_val_sep).ToPointer();
		_pairs_sep = (char*)AllocString(pairs_sep).ToPointer();
		return av_set_options_string(m_pPointer, _opts,_key_val_sep, _pairs_sep);
	}
	finally
	{
		FreeMemory((IntPtr)_opts);
		FreeMemory((IntPtr)_key_val_sep);
		FreeMemory((IntPtr)_pairs_sep);
	}
}

void FFmpeg::AVOptions::opt_free()
{
	av_opt_free(m_pPointer);
}

bool FFmpeg::AVOptions::opt_flag_is_set( String^ field_name, String^ flag_name)
{
	char * _field_name = nullptr;
	char * _flag_name = nullptr;
	try
	{
		_field_name = (char*)AllocString(field_name).ToPointer();
		_flag_name = (char*)AllocString(flag_name).ToPointer();
		return (0 != av_opt_flag_is_set(m_pPointer, _field_name,_flag_name));
	}
	finally
	{
		FreeMemory((IntPtr)_field_name);
		FreeMemory((IntPtr)_flag_name);
	}
}

FFmpeg::AVRESULT FFmpeg::AVOptions::set_dict( AVDictionary^ options)
{
	int _result = 0;
	::AVDictionary * _dict = options != nullptr ? (::AVDictionary *)options->_Pointer.ToPointer() : nullptr;
	try
	{
		_result = av_opt_set_dict(m_pPointer,&_dict);
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

FFmpeg::AVRESULT FFmpeg::AVOptions::set_dict( AVDictionary^ options,AVOptSearch search_flags)
{
	int _result = 0;
	::AVDictionary * _dict = options != nullptr ? (::AVDictionary *)options->_Pointer.ToPointer() : nullptr;
	try
	{
		_result = av_opt_set_dict2(m_pPointer,&_dict,(int)search_flags);
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

FFmpeg::AVRESULT FFmpeg::AVOptions::eval_flags( AVOption^ o, String^ val, [Out] int % flags_out)
{
	char * _val = nullptr;
	int _flags_out = 0;
	try
	{
		_val = (char*)AllocString(val).ToPointer();
		return av_opt_eval_flags(m_pPointer,(::AVOption*)o->_Pointer.ToPointer(),_val,&_flags_out);
	}
	finally
	{
		flags_out = _flags_out;
		FreeMemory((IntPtr)_val);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::eval_int( AVOption^ o, String^ val, [Out] int % int_out)
{
	char * _val = nullptr;
	int _int_out = 0;
	try
	{
		_val = (char*)AllocString(val).ToPointer();
		return av_opt_eval_int(m_pPointer,(::AVOption*)o->_Pointer.ToPointer(),_val,&_int_out);
	}
	finally
	{
		int_out = _int_out;
		FreeMemory((IntPtr)_val);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::eval_int64( AVOption^ o, String^ val, [Out] Int64 % int64_out)
{
	char * _val = nullptr;
	int64_t _int64_out = 0;
	try
	{
		_val = (char*)AllocString(val).ToPointer();
		return av_opt_eval_int64(m_pPointer,(::AVOption*)o->_Pointer.ToPointer(),_val,&_int64_out);
	}
	finally
	{
		int64_out = _int64_out;
		FreeMemory((IntPtr)_val);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::eval_float( AVOption^ o, String^ val, [Out] float % float_out)
{
	char * _val = nullptr;
	float _float_out = 0;
	try
	{
		_val = (char*)AllocString(val).ToPointer();
		return av_opt_eval_float(m_pPointer,(::AVOption*)o->_Pointer.ToPointer(),_val,&_float_out);
	}
	finally
	{
		float_out = _float_out;
		FreeMemory((IntPtr)_val);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::eval_double( AVOption^ o, String^ val, [Out] double % double_out)
{
	char * _val = nullptr;
	double _double_out = 0;
	try
	{
		_val = (char*)AllocString(val).ToPointer();
		return av_opt_eval_double(m_pPointer,(::AVOption*)o->_Pointer.ToPointer(),_val,&_double_out);
	}
	finally
	{
		double_out = _double_out;
		FreeMemory((IntPtr)_val);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::eval_q( AVOption^ o, String^ val, [Out] AVRational^ % q_out)
{
	char * _val = nullptr;
	::AVRational _rational = { 0, 0 };
	try
	{
		_val = (char*)AllocString(val).ToPointer();
		return av_opt_eval_q(m_pPointer,(::AVOption*)o->_Pointer.ToPointer(),_val,&_rational);
	}
	finally
	{
		q_out = gcnew AVRational(_rational.num,_rational.den);
		FreeMemory((IntPtr)_val);
	}
}

FFmpeg::AVOption^ FFmpeg::AVOptions::Find(String^ name,String^ unit,AVOptFlag opt_flags, AVOptSearch search_flags)
{
	char * _name = nullptr;
	char * _unit = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		_unit = (char*)AllocString(unit).ToPointer();
		const ::AVOption * p = av_opt_find(m_pPointer,_name,_unit,(int)opt_flags,(int)search_flags);
		return p != nullptr ? _CreateChildObject<AVOption>((void*)p,this) : nullptr;
	}
	finally
	{
		FreeMemory((IntPtr)_name);
		FreeMemory((IntPtr)_unit);
	}
}

FFmpeg::AVOption^ FFmpeg::AVOptions::Find(String^ name,String^ unit,AVOptFlag opt_flags, AVOptSearch search_flags,AVBase^ % target_obj)
{
	char * _name = nullptr;
	char * _unit = nullptr;
	void * _object = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		_unit = (char*)AllocString(unit).ToPointer();
		const ::AVOption * p = av_opt_find2(m_pPointer,_name,_unit,(int)opt_flags,(int)search_flags,&_object);
		return (p != nullptr) ? _CreateChildObject<AVOption>((void*)p,this) : nullptr;
	}
	finally
	{
		FreeMemory((IntPtr)_name);
		FreeMemory((IntPtr)_unit);
		target_obj = _object != nullptr ? _CreateChildObject<AVBase>(_object,this) : nullptr;
	}
}

FFmpeg::AVOption^ FFmpeg::AVOptions::Next(AVOption^ prev)
{
	auto p = av_opt_next(m_pPointer,(const ::AVOption*)(prev != nullptr ? prev->_Pointer.ToPointer() : nullptr));
	return (p != nullptr) ? _CreateChildObject<AVOption>((void*)p,this) : nullptr;
}

FFmpeg::AVBase^ FFmpeg::AVOptions::child_next(AVBase^ prev)
{
	auto p = av_opt_child_next(m_pPointer,prev != nullptr ? prev->_Pointer.ToPointer() : nullptr);
	return (p != nullptr) ? _CreateChildObject<AVOption>((void*)p,this) : nullptr;
}

FFmpeg::AVRESULT FFmpeg::AVOptions::set(String^ name, String^ val, AVOptSearch search_flags)
{
	char * _name = nullptr;
	char * _val = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		_val = (char*)AllocString(val).ToPointer();
		return av_opt_set(m_pPointer,_name,_val,(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
		FreeMemory((IntPtr)_val);
	}
}

FFmpeg::AVRESULT FFmpeg::AVOptions::set_int( String^ name, Int64 val, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_set_int(m_pPointer,_name,val,(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::set_double( String^ name, double val, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_set_double(m_pPointer,_name,val,(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::set_q( String^ name, AVRational^ val, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_set_q(m_pPointer,_name,*((::AVRational*)(val->_Pointer.ToPointer())),(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::set_bin( String^ name, IntPtr val, int size, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_set_bin(m_pPointer,_name,(const uint8_t*)val.ToPointer(),size,(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::set_image_size( String^ name, int w, int h, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_set_image_size(m_pPointer,_name,w,h,(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::set_pixel_fmt( String^ name, AVPixelFormat fmt, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_set_pixel_fmt(m_pPointer,_name,(::AVPixelFormat)fmt,(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::set_sample_fmt( String^ name, AVSampleFormat fmt, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_set_sample_fmt(m_pPointer,_name,(::AVSampleFormat)fmt,(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::set_video_rate( String^ name, AVRational^ val, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_set_video_rate(m_pPointer,_name,*((::AVRational*)(val->_Pointer.ToPointer())),(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::set_channel_layout( String^ name, AVChannelLayout ch_layout, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_set_channel_layout(m_pPointer,_name,(int64_t)ch_layout,(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::set_int_list(String^ name, array<int>^ val, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		if (val != nullptr && val->Length > 0)
		{
			int size = val != nullptr ? val->Length * sizeof(int) : 0;
			pin_ptr<int> _ptr = &val[0];
			uint8_t * data = ((uint8_t *)_ptr);
			return av_opt_set_bin(m_pPointer,_name,data,size,(int)search_flags);
		}
		return av_opt_set_bin(m_pPointer,_name,nullptr,0,(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}

FFmpeg::AVRESULT FFmpeg::AVOptions::set_int_list(String^ name, array<Int64>^ val, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		if (val != nullptr && val->Length > 0)
		{
			int size = val != nullptr ? val->Length * sizeof(Int64) : 0;
			pin_ptr<__int64> _ptr = &val[0];
			uint8_t * data = ((uint8_t *)_ptr);
			return av_opt_set_bin(m_pPointer,_name,data,size,(int)search_flags);
		}
		return av_opt_set_bin(m_pPointer,_name,nullptr,0,(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}

FFmpeg::AVRESULT FFmpeg::AVOptions::set_dict_val( String^ name, AVDictionary^ val, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_set_dict_val(m_pPointer,_name,(const ::AVDictionary*)val->_Pointer.ToPointer(),(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}

FFmpeg::AVRESULT FFmpeg::AVOptions::get( String^ name, AVOptSearch search_flags, [Out] String^  % out_val)
{
	char * _name = nullptr;
	uint8_t * buf = nullptr;
	out_val = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		int _result = av_opt_get(m_pPointer,_name,(int)search_flags,&buf);
		if (_result >= 0 && buf)
		{
			out_val = gcnew String((const char *)buf);
		}
		return _result;
	}
	finally
	{
		if (buf) av_free(buf);
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::get_int( String^ name, AVOptSearch search_flags, [Out] Int64 % out_val)
{
	char * _name = nullptr;
	int64_t val = 0;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_get_int(m_pPointer,_name,(int)search_flags,&val);
	}
	finally
	{
		out_val = val;
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::get_double( String^ name, AVOptSearch search_flags, [Out] double % out_val)
{
	char * _name = nullptr;
	double val = 0;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_get_double(m_pPointer,_name,(int)search_flags,&val);
	}
	finally
	{
		out_val = val;
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::get_q( String^ name, AVOptSearch search_flags, [Out] AVRational^ % out_val)
{
	char * _name = nullptr;
	::AVRational val = { 0, 0};
	out_val = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		int _result = av_opt_get_q(m_pPointer,_name,(int)search_flags,&val);
		if (_result >= 0)
		{
			out_val = gcnew AVRational(val.num,val.den);
		}
		return _result;
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}

FFmpeg::AVRESULT FFmpeg::AVOptions::get_image_size( String^ name, AVOptSearch search_flags, [Out] int % w_out, [Out] int % h_out)
{
	char * _name = nullptr;
	int w = 0;
	int h = 0;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_get_image_size(m_pPointer,_name,(int)search_flags,&w,&h);
	}
	finally
	{
		w_out = w;
		h_out = h;
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::get_pixel_fmt( String^ name, AVOptSearch search_flags, [Out] AVPixelFormat % out_fmt)
{
	char * _name = nullptr;
	::AVPixelFormat fmt = AV_PIX_FMT_NONE;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_get_pixel_fmt(m_pPointer,_name,(int)search_flags,&fmt);
	}
	finally
	{
		out_fmt = (AVPixelFormat)fmt;
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::get_sample_fmt( String^ name, AVOptSearch search_flags, [Out] AVSampleFormat % out_fmt)
{
	char * _name = nullptr;
	::AVSampleFormat fmt = AV_SAMPLE_FMT_NONE;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_get_sample_fmt(m_pPointer,_name,(int)search_flags,&fmt);
	}
	finally
	{
		out_fmt = (AVSampleFormat)fmt;
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::get_video_rate( String^ name, AVOptSearch search_flags, [Out] AVRational^ % out_val)
{
	char * _name = nullptr;
	::AVRational val = { 0, 0};
	out_val = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		int _result = av_opt_get_video_rate(m_pPointer,_name,(int)search_flags,&val);
		if (_result >= 0)
		{
			out_val = gcnew AVRational(val.num,val.den);
		}
		return _result;
		
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}
FFmpeg::AVRESULT FFmpeg::AVOptions::get_channel_layout( String^ name, AVOptSearch search_flags, [Out] AVChannelLayout % ch_layout)
{
	char * _name = nullptr;
	int64_t layout = 0;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_get_channel_layout(m_pPointer,_name,(int)search_flags,&layout);
	}
	finally
	{
		ch_layout = (AVChannelLayout)layout;
		FreeMemory((IntPtr)_name);
	}
}

FFmpeg::AVRESULT FFmpeg::AVOptions::get_dict_val( String^ name, AVOptSearch search_flags, [Out] AVDictionary^ % out_val)
{
	char * _name = nullptr;
	::AVDictionary * dict = nullptr;
	out_val = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		int _result = av_opt_get_dict_val(m_pPointer,_name,(int)search_flags,&dict);
		if (_result >= 0 && dict)
		{
			out_val = _CreateChildObject<AVDictionary>(dict,this);
			dict = nullptr;
		}
		return _result;
	}
	finally
	{
		if (dict) av_dict_free(&dict);
		FreeMemory((IntPtr)_name);
	}
}

IntPtr FFmpeg::AVOptions::ptr(AVClass^ avclass, String^ name)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return (IntPtr)av_opt_ptr((const ::AVClass *)avclass->_Pointer.ToPointer(),m_pPointer,_name);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}

int FFmpeg::AVOptions::query_ranges([Out] AVOptionRanges^ % ranges, String^ key, AVOptFlag flags)
{
	char * _key = nullptr;
	::AVOptionRanges * _ranges = nullptr;
	ranges = nullptr;
	try
	{
		_key = (char*)AllocString(key).ToPointer();
		int _result = av_opt_query_ranges(&_ranges,m_pPointer,_key,(int)flags);
		if (_result >= 0 && _ranges)
		{
			ranges = _CreateChildObject<AVOptionRanges>(_ranges,this);
			ranges->m_pFreep = (TFreeFNP*)av_opt_freep_ranges;
			_ranges = nullptr;
		}
		return _result;
	}
	finally
	{
		if (_ranges) av_opt_freep_ranges(&_ranges);
		FreeMemory((IntPtr)_key);
	}
}

FFmpeg::AVRESULT FFmpeg::AVOptions::CopyTo(AVBase^ dest)
{
	return av_opt_copy(dest->_Pointer.ToPointer(),m_pPointer);
}

int FFmpeg::AVOptions::query_ranges_default([Out] AVOptionRanges^ % ranges, String^ key, AVOptFlag flags)
{
	char * _key = nullptr;
	::AVOptionRanges * _ranges = nullptr;
	ranges = nullptr;
	try
	{
		_key = (char*)AllocString(key).ToPointer();
		int _result = av_opt_query_ranges_default(&_ranges,m_pPointer,_key,(int)flags);
		if (_result >= 0 && _ranges)
		{
			ranges = _CreateChildObject<AVOptionRanges>(_ranges,this);
			ranges->m_pFreep = (TFreeFNP*)av_opt_freep_ranges;
			_ranges = nullptr;
		}
		return _result;
	}
	finally
	{
		if (_ranges) av_opt_freep_ranges(&_ranges);
		FreeMemory((IntPtr)_key);
	}
}

int FFmpeg::AVOptions::is_set_to_default( AVOption^ o)
{
	return av_opt_is_set_to_default(m_pPointer,(const ::AVOption*)o->_Pointer.ToPointer());
}

int FFmpeg::AVOptions::is_set_to_default( String^ name, AVOptSearch search_flags)
{
	char * _name = nullptr;
	try
	{
		_name = (char*)AllocString(name).ToPointer();
		return av_opt_is_set_to_default_by_name(m_pPointer,_name,(int)search_flags);
	}
	finally
	{
		FreeMemory((IntPtr)_name);
	}
}

FFmpeg::AVRESULT FFmpeg::AVOptions::Serialize( AVOptFlag opt_flags, AVOptSerializeFlags flags, [Out] String^ % buffer, Char key_val_sep, Char pairs_sep)
{
	char *_buffer = nullptr;
	buffer = nullptr;
	try
	{
		AVRESULT _result = av_opt_serialize(m_pPointer,(int)opt_flags,(int)flags, &_buffer,(const char)key_val_sep,(const char)pairs_sep);
		if (_result >= 0 && _buffer)
		{
			buffer = gcnew String(_buffer);
		}
		return _result;
	}
	finally
	{
		if (_buffer) av_free(_buffer);
	}
}
//////////////////////////////////////////////////////
// AVOptionRange
//////////////////////////////////////////////////////
FFmpeg::AVOptionRange::AVOptionRange(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
	
}
//////////////////////////////////////////////////////
int FFmpeg::AVOptionRange::_StructureSize::get()
{
	return sizeof(::AVOptionRange);
}
//////////////////////////////////////////////////////
IntPtr FFmpeg::AVOptionRange::str::get()
{
	return (IntPtr)((void*)((::AVOptionRange*)m_pPointer)->str);
}
double FFmpeg::AVOptionRange::value_min::get()
{
	return ((::AVOptionRange*)m_pPointer)->value_min;
}
double FFmpeg::AVOptionRange::value_max::get()
{
	return ((::AVOptionRange*)m_pPointer)->value_max;
}
double FFmpeg::AVOptionRange::component_min::get()
{
	return ((::AVOptionRange*)m_pPointer)->component_min;
}
double FFmpeg::AVOptionRange::component_max::get()
{
	return ((::AVOptionRange*)m_pPointer)->component_max;
}
bool FFmpeg::AVOptionRange::is_range::get()
{
	return (((::AVOptionRange*)m_pPointer)->is_range == 1);
}
//////////////////////////////////////////////////////
// AVOptionRangesEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVOptionRanges::AVOptionRangesEnumerator::AVOptionRangesEnumerator(AVOptionRanges^ _range)
	: m_nIndex(-1)
	, m_pRanges(_range)
{

}

FFmpeg::AVOptionRanges::AVOptionRangesEnumerator::~AVOptionRangesEnumerator()
{
}
//////////////////////////////////////////////////////
// IEnumerator
bool FFmpeg::AVOptionRanges::AVOptionRangesEnumerator::MoveNext()
{
	::AVOptionRanges* pRanges = (::AVOptionRanges*)m_pRanges->m_pPointer;
	if (m_nIndex + 1 >= pRanges->nb_components * pRanges->nb_ranges)
	{
		return false;
	}
	m_nIndex++;
	return true;
}
FFmpeg::AVOptionRange^ FFmpeg::AVOptionRanges::AVOptionRangesEnumerator::Current::get()
{
	::AVOptionRanges* pRanges = (::AVOptionRanges*)m_pRanges->m_pPointer;
	if (m_nIndex == -1)
	{
		if (!MoveNext()) return nullptr;
	}
	return _CreateChildObject<FFmpeg::AVOptionRange>((void*)pRanges->range[m_nIndex],m_pRanges);
	//return gcnew AVOptionRange(pRanges->range[m_nIndex]);
}
void FFmpeg::AVOptionRanges::AVOptionRangesEnumerator::Reset()
{
	m_nIndex = -1;
}
Object^ FFmpeg::AVOptionRanges::AVOptionRangesEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVOptionRanges
//////////////////////////////////////////////////////
FFmpeg::AVOptionRanges::AVOptionRanges(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
int FFmpeg::AVOptionRanges::_StructureSize::get()
{
	return sizeof(::AVOptionRanges);
}
//////////////////////////////////////////////////////
array<FFmpeg::AVOptionRange^>^ FFmpeg::AVOptionRanges::range::get()
{
	List<FFmpeg::AVOptionRange^>^ _array = nullptr;
	::AVOptionRange ** pRanges = ((::AVOptionRanges*)m_pPointer)->range;
	int nCount = ((::AVOptionRanges*)m_pPointer)->nb_ranges * ((::AVOptionRanges*)m_pPointer)->nb_components;
	if (pRanges && nCount > 0)
	{
		_array =  gcnew List<FFmpeg::AVOptionRange^>();
		while (nCount-- > 0)
		{
			_array->Add(_CreateObject<FFmpeg::AVOptionRange>((void*)pRanges++));
			//_array->Add(gcnew AVOptionRange((void*)pRanges++));
		}
	}
	return _array != nullptr ? _array->ToArray() : nullptr;
}
int FFmpeg::AVOptionRanges::nb_ranges::get()
{
	return ((::AVOptionRanges*)m_pPointer)->nb_ranges;
}
int FFmpeg::AVOptionRanges::nb_components::get()
{
	return ((::AVOptionRanges*)m_pPointer)->nb_components;
}
//////////////////////////////////////////////////////
FFmpeg::AVOptionRange^ FFmpeg::AVOptionRanges::default::get(int _component,int _range)
{
	::AVOptionRanges* pRanges = (::AVOptionRanges*)m_pPointer;
	if (_component < 0 || _component > pRanges->nb_components || _range < 0 || _range > pRanges->nb_ranges) 
	{
		throw gcnew ArgumentOutOfRangeException();
	}
	return _CreateObject<FFmpeg::AVOptionRange>((void*)pRanges->range[ _component * pRanges->nb_ranges + _range]);
	//return gcnew AVOptionRange((void*)pRanges->range[ _component * pRanges->nb_ranges + _range]);
}
//////////////////////////////////////////////////////
// IEnumerable
System::Collections::IEnumerator^ FFmpeg::AVOptionRanges::GetEnumerator()
{
	return gcnew AVOptionRangesEnumerator(this);
}
//////////////////////////////////////////////////////
// IEnumerable<AVOptionRange>
System::Collections::Generic::IEnumerator<FFmpeg::AVOptionRange^>^ FFmpeg::AVOptionRanges::GetEnumeratorGeneric()
{
	return gcnew AVOptionRangesEnumerator(this);
}
//////////////////////////////////////////////////////
// AVClass::Enumerator
//////////////////////////////////////////////////////
FFmpeg::AVClass::Enumerator::Enumerator(AVClass^ parent)
	: m_pParent(parent)
	, m_pOpaque(IntPtr::Zero)
	, m_pCurrent(nullptr)
{

}
FFmpeg::AVClass::Enumerator::~Enumerator()
{

}
//////////////////////////////////////////////////////
// IEnumerator
bool FFmpeg::AVClass::Enumerator::MoveNext()
{
	void * opaque = m_pOpaque.ToPointer();
	::AVClass * p = nullptr;

	LOAD_API(AVUtil,::AVClass *,av_opt_child_class_next,const ::AVClass *,void *);
	LOAD_API(AVUtil,::AVClass *,av_opt_child_class_iterate,const ::AVClass *,void **);

	if (av_opt_child_class_iterate)
	{
		p = av_opt_child_class_iterate((const ::AVClass*)m_pParent->_Pointer.ToPointer(),&opaque);
	}
	else
	{
		if (av_opt_child_class_next)
		{
			p = av_opt_child_class_next((const ::AVClass*)m_pParent->_Pointer.ToPointer(),opaque);
			opaque = (void*)p;
		}
	}
	m_pCurrent = _CreateChildObject<AVClass>(p,m_pParent);
	m_pOpaque = IntPtr(opaque);
	return (m_pCurrent != nullptr);
}
FFmpeg::AVClass^ FFmpeg::AVClass::Enumerator::Current::get()
{
	if (m_pOpaque == IntPtr::Zero)
	{
		if (!MoveNext()) return nullptr;
	}
	return m_pCurrent;
}
void FFmpeg::AVClass::Enumerator::Reset()
{
	m_pOpaque = IntPtr::Zero;
}
Object^ FFmpeg::AVClass::Enumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVClass
//////////////////////////////////////////////////////
FFmpeg::AVClass::AVClass(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVClass::class_name::get()
{
	auto p = ((::AVClass*) m_pPointer)->class_name;
	return p != nullptr ? gcnew String(p) : nullptr;
}

array<FFmpeg::AVOption^>^ FFmpeg::AVClass::option::get()
{ 
	List<AVOption^>^ list = gcnew List<AVOption^>();
	const ::AVOption * p = (const ::AVOption *)((::AVClass*) m_pPointer)->option;
	if (p)
	{
		while (p->name)
		{
			list->Add(_CreateObject<AVOption>((void*)(p)));
			p++;
		}
	}
	return list->ToArray();
}

int FFmpeg::AVClass::version::get()
{ 
	return ((::AVClass*) m_pPointer)->version;
}

int FFmpeg::AVClass::log_level_offset_offset::get()
{
	return ((::AVClass*) m_pPointer)->log_level_offset_offset;
}

int FFmpeg::AVClass::parent_log_context_offset::get()
{ 
	return ((::AVClass*) m_pPointer)->parent_log_context_offset;
}

FFmpeg::AVClassCategory FFmpeg::AVClass::category::get()
{
	return (FFmpeg::AVClassCategory)(((::AVClass*) m_pPointer)->category);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVClass::ToString()
{
	if (m_pPointer)
	{
		String^ _name = class_name;
		if (!String::IsNullOrEmpty(_name)) return _name;
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
int FFmpeg::AVClass::_StructureSize::get()
{
	return sizeof(::AVClass);
}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVClass::GetCodecClass()
{
	const ::AVClass * _ptr = ::avcodec_get_class();
	if (_ptr) return gcnew AVClass((void*)_ptr,nullptr);
	return nullptr;
}

FFmpeg::AVClass^ FFmpeg::AVClass::GetFrameClass()
{
	void * _ptr = nullptr;
	PTR_API(AVCodec,avcodec_get_frame_class)
	_ptr = avcodec_get_frame_class();
	if (_ptr) return gcnew AVClass((void*)_ptr,nullptr);
	return nullptr;
}

FFmpeg::AVClass^ FFmpeg::AVClass::GetSubtitleRectClass()
{
	const ::AVClass * _ptr = ::avcodec_get_subtitle_rect_class();
	if (_ptr) return gcnew AVClass((void*)_ptr,nullptr);
	return nullptr;
}

FFmpeg::AVClass^ FFmpeg::AVClass::GetFormatClass()
{
	const ::AVClass * _ptr = ::avformat_get_class();
	if (_ptr) return gcnew AVClass((void*)_ptr,nullptr);
	return nullptr;
}

FFmpeg::AVClass^ FFmpeg::AVClass::GetSwsClass()
{
	const ::AVClass * _ptr = ::sws_get_class();
	if (_ptr) return gcnew AVClass((void*)_ptr,nullptr);
	return nullptr;
}

FFmpeg::AVClass^ FFmpeg::AVClass::GetSwrClass()
{
	const ::AVClass * _ptr = ::swr_get_class();
	if (_ptr) return gcnew AVClass((void*)_ptr,nullptr);
	return nullptr;
}
//////////////////////////////////////////////////////
// AVMimeTypes
//////////////////////////////////////////////////////
int FFmpeg::AVCodecDescriptor::AVMimeTypes::Count::get()
{ 
	int count = 0;
	while (true)
	{
		if (nullptr == m_Parent->GetMimeType(count))
		{
			break;
		}
		count++;
	}
	return count;
}
array<String^>^ FFmpeg::AVCodecDescriptor::AVMimeTypes::ToArray()
{
	List<String^>^ list = gcnew List<String^>();
	try
	{
		int idx = 0;
		while (true)
		{
			String^ s = m_Parent->GetMimeType(idx++);
			if (nullptr == s)
			{
				break;
			}
			list->Add(s);
		}
		return list->ToArray();
	}
	finally
	{
		delete list;
	}
}
System::Collections::IEnumerator^ FFmpeg::AVCodecDescriptor::AVMimeTypes::GetEnumerator()
{
	return gcnew AVMimeTypesEnumerator(this->m_Parent);
}
System::Collections::Generic::IEnumerator<String^>^ FFmpeg::AVCodecDescriptor::AVMimeTypes::GetEnumeratorGeneric()
{
	return gcnew AVMimeTypesEnumerator(this->m_Parent);
}
//////////////////////////////////////////////////////
// AVMimeTypesEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVCodecDescriptor::AVMimeTypes::AVMimeTypesEnumerator::AVMimeTypesEnumerator(AVCodecDescriptor^ _descriptor)
	: m_pDescriptor(_descriptor)
	, m_nIndex(-1)
{
	
}
FFmpeg::AVCodecDescriptor::AVMimeTypes::AVMimeTypesEnumerator::~AVMimeTypesEnumerator()
{
}
//////////////////////////////////////////////////////
bool FFmpeg::AVCodecDescriptor::AVMimeTypes::AVMimeTypesEnumerator::MoveNext()
{
	if (nullptr != m_pDescriptor->GetMimeType(m_nIndex + 1))
	{
		m_nIndex += 1;
		return true;
	}
	return false;
}
String^ FFmpeg::AVCodecDescriptor::AVMimeTypes::AVMimeTypesEnumerator::Current::get()
{
	if (m_nIndex == -1)
	{
		if (!MoveNext()) return nullptr;
	}
	return m_pDescriptor->GetMimeType(m_nIndex);
}
void FFmpeg::AVCodecDescriptor::AVMimeTypes::AVMimeTypesEnumerator::Reset()
{
	m_nIndex = -1;
}
Object^ FFmpeg::AVCodecDescriptor::AVMimeTypes::AVMimeTypesEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVProfiles
//////////////////////////////////////////////////////
int FFmpeg::AVCodecDescriptor::AVProfiles::Count::get()
{ 
	int count = 0;
	while (true)
	{
		if (nullptr == m_Parent->GetProfile(count))
		{
			break;
		}
		count++;
	}
	return count;
}
array<FFmpeg::AVProfile^>^ FFmpeg::AVCodecDescriptor::AVProfiles::ToArray()
{
	List<AVProfile^>^ list = gcnew List<AVProfile^>();
	int idx = 0;
	while (true)
	{
		AVProfile^ s = m_Parent->GetProfile(idx++);
		if (nullptr == s)
		{
			break;
		}
		list->Add(s);
	}
	return list->ToArray();
}
System::Collections::IEnumerator^ FFmpeg::AVCodecDescriptor::AVProfiles::GetEnumerator()
{
	return gcnew AVProfilesEnumerator(this->m_Parent);
}
System::Collections::Generic::IEnumerator<FFmpeg::AVProfile^>^ FFmpeg::AVCodecDescriptor::AVProfiles::GetEnumeratorGeneric()
{
	return gcnew AVProfilesEnumerator(this->m_Parent);
}
//////////////////////////////////////////////////////
// AVProfilesEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVCodecDescriptor::AVProfiles::AVProfilesEnumerator::AVProfilesEnumerator(AVCodecDescriptor^ _descriptor)
	: m_pDescriptor(_descriptor)
	, m_nIndex(-1)
{

}
FFmpeg::AVCodecDescriptor::AVProfiles::AVProfilesEnumerator::~AVProfilesEnumerator()
{
}
//////////////////////////////////////////////////////
bool FFmpeg::AVCodecDescriptor::AVProfiles::AVProfilesEnumerator::MoveNext()
{
	if (nullptr != m_pDescriptor->GetProfile(m_nIndex + 1))
	{
		m_nIndex += 1;
		return true;
	}
	return false;
}
FFmpeg::AVProfile^ FFmpeg::AVCodecDescriptor::AVProfiles::AVProfilesEnumerator::Current::get()
{
	if (m_nIndex == -1)
	{
		if (!MoveNext()) return nullptr;
	}
	return m_pDescriptor->GetProfile(m_nIndex);
}
void FFmpeg::AVCodecDescriptor::AVProfiles::AVProfilesEnumerator::Reset()
{
	m_nIndex = -1;
}
Object^ FFmpeg::AVCodecDescriptor::AVProfiles::AVProfilesEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVCodecDescriptor::AVCodecDescriptors::Enumerator
//////////////////////////////////////////////////////
bool FFmpeg::AVCodecDescriptor::AVCodecDescriptors::Enumerator::MoveNext()
{
	m_pCurrent = AVCodecDescriptor::Next(m_pCurrent);
	return (m_pCurrent != nullptr);
}
FFmpeg::AVCodecDescriptor^ FFmpeg::AVCodecDescriptor::AVCodecDescriptors::Enumerator::Current::get()
{
	if (m_pCurrent == nullptr) MoveNext();
	return m_pCurrent;
}
void FFmpeg::AVCodecDescriptor::AVCodecDescriptors::Enumerator::Reset()
{
	m_pCurrent = nullptr;
}
Object^ FFmpeg::AVCodecDescriptor::AVCodecDescriptors::Enumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVCodecDescriptor
//////////////////////////////////////////////////////
FFmpeg::AVCodecDescriptor::AVCodecDescriptor(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
FFmpeg::AVCodecID FFmpeg::AVCodecDescriptor::id::get()
{
	return (FFmpeg::AVCodecID)(((::AVCodecDescriptor*) m_pPointer)->id);
}
FFmpeg::AVMediaType FFmpeg::AVCodecDescriptor::type::get()
{
	return (FFmpeg::AVMediaType)(((::AVCodecDescriptor*) m_pPointer)->type);
}
String^ FFmpeg::AVCodecDescriptor::name::get()
{
	auto p = ((::AVCodecDescriptor*) m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::AVCodecDescriptor::long_name::get()
{
	auto p = ((::AVCodecDescriptor*) m_pPointer)->long_name;
	return p != nullptr ? gcnew String(p) : nullptr;
}
FFmpeg::AVCodecProp FFmpeg::AVCodecDescriptor::props::get()
{
	return (FFmpeg::AVCodecProp)(((::AVCodecDescriptor*) m_pPointer)->props);
}
FFmpeg::AVCodecDescriptor::AVMimeTypes^ FFmpeg::AVCodecDescriptor::mime_types::get()
{
	return gcnew FFmpeg::AVCodecDescriptor::AVMimeTypes(this);
}
FFmpeg::AVCodecDescriptor::AVProfiles^ FFmpeg::AVCodecDescriptor::profiles::get()
{
	return gcnew FFmpeg::AVCodecDescriptor::AVProfiles(this);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVCodecDescriptor::GetMimeType(int n)
{
	if (((::AVCodecDescriptor*) m_pPointer)->mime_types)
	{
		int i = 0;
		while (((::AVCodecDescriptor*) m_pPointer)->mime_types[i] != nullptr)
		{
			if (i++ == n)
			{
				return gcnew String(((::AVCodecDescriptor*) m_pPointer)->mime_types[n]);
			}
		}
	}
	return nullptr;
}
FFmpeg::AVProfile^ FFmpeg::AVCodecDescriptor::GetProfile(int n)
{
	const ::AVProfile * p = ((::AVCodecDescriptor*) m_pPointer)->profiles;
	if (p)
	{
		int i = 0;
		while (p[i].profile != FF_PROFILE_UNKNOWN)
		{
			if (i++ == n)
			{
				return _CreateObject<AVProfile>((void*)&p[i]);
			}
		}
	}
	return nullptr;
}
//////////////////////////////////////////////////////
FFmpeg::AVCodecDescriptor^ FFmpeg::AVCodecDescriptor::Next()
{
	return Next(this);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVCodecDescriptor::ToString()
{
	if (m_pPointer)
	{
		String^ _name;
		_name = long_name;
		if (String::IsNullOrEmpty(_name))
		{
			_name = name;
		}
		if (String::IsNullOrEmpty(_name))
		{
			_name = Enum::GetName(AVCodecID::typeid, id);
		}
		if (!String::IsNullOrEmpty(_name))
		{
			return "[ AVCodecDescriptor ] \"" + _name + "\"";
		}
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
int FFmpeg::AVCodecDescriptor::_StructureSize::get()
{
	return sizeof(::AVCodecDescriptor);
}
//////////////////////////////////////////////////////
FFmpeg::AVCodecDescriptor^ FFmpeg::AVCodecDescriptor::Next(FFmpeg::AVCodecDescriptor^ _prev)
{
	LibAVCodec::RegisterAll();
	const ::AVCodecDescriptor * _ptr = ::avcodec_descriptor_next((const ::AVCodecDescriptor *)_prev->m_pPointer);
	if (_ptr) return gcnew AVCodecDescriptor((void*)_ptr,nullptr);
	return nullptr;
}

FFmpeg::AVCodecDescriptor^ FFmpeg::AVCodecDescriptor::Find(String^ _name)
{
	LibAVCodec::RegisterAll();
	char* szName = nullptr;
	try
	{
		szName = (char*)AllocString(_name).ToPointer();
		const ::AVCodecDescriptor * _ptr = ::avcodec_descriptor_get_by_name(szName);
		if (_ptr) return gcnew AVCodecDescriptor((void*)_ptr,nullptr);
	}
	finally
	{
		FreeMemory((IntPtr)szName);
	}
	return nullptr;
}

FFmpeg::AVCodecDescriptor^ FFmpeg::AVCodecDescriptor::Find(FFmpeg::AVCodecID _id)
{
	LibAVCodec::RegisterAll();
	const ::AVCodecDescriptor * _ptr = ::avcodec_descriptor_get((::AVCodecID)(int)_id);
	if (_ptr) return gcnew AVCodecDescriptor((void*)_ptr,nullptr);
	return nullptr;
}
//////////////////////////////////////////////////////
// AVProfile
//////////////////////////////////////////////////////
FFmpeg::AVProfile::AVProfile(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
	
}
//////////////////////////////////////////////////////
FFmpeg::Profile FFmpeg::AVProfile::profile::get()
{
	return (FFmpeg::Profile)((::AVProfile*)m_pPointer)->profile;
}

String^ FFmpeg::AVProfile::name::get()
{
	auto p = ((::AVProfile*) m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVProfile::ToString()
{
	if (m_pPointer)
	{
		String^ _name = name;
		if (!String::IsNullOrEmpty(_name))
		{
			return "[ AVProfile ] \"" + _name + "\"";
		}
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
int FFmpeg::AVProfile::_StructureSize::get()
{
	return sizeof(::AVProfile);
}
//////////////////////////////////////////////////////
// AVRational
//////////////////////////////////////////////////////
FFmpeg::AVRational::AVRational(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}

FFmpeg::AVRational::AVRational(const ::AVRational & r)
	: AVRational(r.num,r.den)
{
}

FFmpeg::AVRational::AVRational(Int64 q)
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
	this->num = (int)(q >> 32); 
	this->den = (int)(q & 0xffffffff); 
}

FFmpeg::AVRational::AVRational(int _num, int _den)
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
	this->num = _num;
	this->den = _den;
}
//////////////////////////////////////////////////////
int FFmpeg::AVRational::num::get()
{
	return ((::AVRational*)m_pPointer)->num;
}

void FFmpeg::AVRational::num::set(int _value)
{
	((::AVRational*)m_pPointer)->num = _value;
}

int FFmpeg::AVRational::den::get()
{
	return ((::AVRational*)m_pPointer)->den;
}

void FFmpeg::AVRational::den::set(int _value)
{
	((::AVRational*)m_pPointer)->den = _value;;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVRational::ToString()
{
	return String::Format("Rational : ({0},{1})", num, den);
}

int FFmpeg::AVRational::GetHashCode()
{
	return ToLong().GetHashCode();
}

bool FFmpeg::AVRational::Equals(Object^ obj)
{
	if (Object::ReferenceEquals(this,obj)) return true;
	AVRational^ _object = dynamic_cast<AVRational^>(obj);
	if (_object != nullptr)
	{
		if (_object->m_pPointer == m_pPointer) return true;
		return (_object->num == this->num && _object->den == this->den);
	}
	else
	{
		{
			Int64^ _value = dynamic_cast<Int64^>(obj);
			if (_value != nullptr)
			{
				return _value == ToLong();
			}
		}
		{
			double^ _value = dynamic_cast<double^>(obj);
			if (_value != nullptr)
			{
				return _value == ToDouble();
			}
		}
		return m_pPointer == nullptr;
	}
	return false;
}
//////////////////////////////////////////////////////
int FFmpeg::AVRational::_StructureSize::get()
{
	return sizeof(::AVRational);
}
//////////////////////////////////////////////////////
double FFmpeg::AVRational::ToDouble()
{
	return (double)num / (double)den;
}

Int64 FFmpeg::AVRational::ToLong()
{
	return ((Int64)num << 32) + (Int64)den;
}

int FFmpeg::AVRational::ToIntFloat()
{
	::AVRational _a = {this->num,this->den };
	return av_q2intfloat(_a);
}
//////////////////////////////////////////////////////
FFmpeg::AVRational^ FFmpeg::AVRational::inv_q()
{
	return gcnew AVRational(den,num);
}
//////////////////////////////////////////////////////
int FFmpeg::AVRational::find_nearest_q_idx(array<FFmpeg::AVRational^>^ _list)
{
	::AVRational * q_list = (::AVRational *)av_mallocz(sizeof(::AVRational) * (_list->Length + 1));
	try
	{
		for (int i = 0; i < _list->Length; i++)
		{
			q_list[i].num = _list[i]->num;
			q_list[i].den = _list[i]->den;
		}
		::AVRational _a = {this->num,this->den };
		return av_find_nearest_q_idx(_a,q_list);
	}
	finally
	{
		if (q_list) av_free(q_list);
	}
}

int FFmpeg::AVRational::nearer_q(FFmpeg::AVRational^ q1, FFmpeg::AVRational^ q2)
{
	::AVRational _a = {this->num,this->den };
	::AVRational _q1 = {q1->num,q1->den };
	::AVRational _q2 = {q2->num,q2->den };
	return av_nearer_q(_a,_q1,_q2);
}

bool FFmpeg::AVRational::reduce(Int64 _num,Int64 _den,Int64 _max)
{
	::AVRational _a = {this->num,this->den };
	bool _result = (1 == av_reduce(&_a.num, &_a.den, _num, _den, _max));
	if (_result)
	{
		num = _a.num;
		den = _a.den;
	}
	return _result;
}
//////////////////////////////////////////////////////
FFmpeg::AVRational::operator double(FFmpeg::AVRational^ a)
{
	return a->ToDouble();
}

FFmpeg::AVRational::operator Int64(FFmpeg::AVRational^ a)
{
	return a->ToLong();
}
//////////////////////////////////////////////////////
FFmpeg::AVRational::operator ::AVRational(AVRational^ a)
{
	::AVRational r = {a->num,a->den};
	return r;
}
FFmpeg::AVRational::operator ::AVRational*(AVRational^ a)
{
	return (::AVRational*)a->m_pPointer;
}
//////////////////////////////////////////////////////
FFmpeg::AVRational^ FFmpeg::AVRational::operator * (FFmpeg::AVRational^ a, FFmpeg::AVRational^ b)
{
	::AVRational _a = {a->num,a->den };
	::AVRational _b = {b->num,b->den };
	::AVRational _result = av_mul_q(_a,_b);
	return gcnew AVRational(_result.num,_result.den);
}

FFmpeg::AVRational^ FFmpeg::AVRational::operator / (FFmpeg::AVRational^ a, FFmpeg::AVRational^ b)
{
	::AVRational _a = {a->num,a->den };
	::AVRational _b = {b->num,b->den };
	::AVRational _result = av_div_q(_a,_b);
	return gcnew AVRational(_result.num,_result.den);
}

FFmpeg::AVRational^ FFmpeg::AVRational::operator - (FFmpeg::AVRational^ a, FFmpeg::AVRational^ b)
{
	::AVRational _a = {a->num,a->den };
	::AVRational _b = {b->num,b->den };
	::AVRational _result = av_sub_q(_a,_b);
	return gcnew AVRational(_result.num,_result.den);
}

FFmpeg::AVRational^ FFmpeg::AVRational::operator + (FFmpeg::AVRational^ a, FFmpeg::AVRational^ b)
{
	::AVRational _a = {a->num,a->den };
	::AVRational _b = {b->num,b->den };
	::AVRational _result = av_add_q(_a,_b);
	return gcnew AVRational(_result.num,_result.den);
}
//////////////////////////////////////////////////////
FFmpeg::AVRational^ FFmpeg::AVRational::operator += (FFmpeg::AVRational^ b)
{
	::AVRational _a = {this->num,this->den };
	::AVRational _b = {b->num,b->den };
	::AVRational _result = av_add_q(_a,_b);
	this->num = _result.num;
	this->den = _result.den;
	return this;
}
FFmpeg::AVRational^ FFmpeg::AVRational::operator -= (FFmpeg::AVRational^ b)
{
	::AVRational _a = {this->num,this->den };
	::AVRational _b = {b->num,b->den };
	::AVRational _result = av_sub_q(_a,_b);
	this->num = _result.num;
	this->den = _result.den;
	return this;
}
FFmpeg::AVRational^ FFmpeg::AVRational::operator *= (FFmpeg::AVRational^ b)
{
	::AVRational _a = {this->num,this->den };
	::AVRational _b = {b->num,b->den };
	::AVRational _result = av_mul_q(_a,_b);
	this->num = _result.num;
	this->den = _result.den;
	return this;
}
FFmpeg::AVRational^ FFmpeg::AVRational::operator /= (FFmpeg::AVRational^ b)
{
	::AVRational _a = {this->num,this->den };
	::AVRational _b = {b->num,b->den };
	::AVRational _result = av_div_q(_a,_b);
	this->num = _result.num;
	this->den = _result.den;
	return this;
}
//////////////////////////////////////////////////////
bool FFmpeg::AVRational::operator == (FFmpeg::AVRational^ a,FFmpeg::AVRational^ b)
{
	if ((Object^)a != nullptr && (Object^)b != nullptr)
	{
		return (a->num == b->num && a->den == b->den);
	}
	else
	{
		return ((Object^)a == nullptr && (Object^)b == nullptr);
	}
}
bool FFmpeg::AVRational::operator != (FFmpeg::AVRational^ a,FFmpeg::AVRational^ b)
{
	return !(a == b);
}
//////////////////////////////////////////////////////
FFmpeg::AVRational^ FFmpeg::AVRational::AVTimeBaseQ::get()
{
	return gcnew AVRational(1, AV_TIME_BASE);
}
//////////////////////////////////////////////////////
FFmpeg::AVRational^ FFmpeg::AVRational::get_time_base_q()
{
	::AVRational a = av_get_time_base_q();
	return gcnew AVRational(a.num,a.den);
}

FFmpeg::AVRational^ FFmpeg::AVRational::d2q(double d, int max)
{
	::AVRational a = av_d2q(d,max);
	return gcnew AVRational(a.num,a.den);
}

int FFmpeg::AVRational::cmp_q(AVRational^ a, AVRational^ b)
{
	if ((Object^)a != nullptr && (Object^)b != nullptr)
	{
		return av_cmp_q(*(::AVRational*)a->m_pPointer, *(::AVRational*)b->m_pPointer);
	}
	return ((Object^)a == nullptr && (Object^)b == nullptr) ? 0 : INT_MIN;
}
//////////////////////////////////////////////////////
// AVDictionaryEntry
//////////////////////////////////////////////////////
FFmpeg::AVDictionaryEntry::AVDictionaryEntry(void * _pointer)
	: AVBase(_pointer,nullptr)
{
}

FFmpeg::AVDictionaryEntry::AVDictionaryEntry(void * _pointer, AVBase^ parent)
	: AVBase(_pointer,parent)
{

}

String^ FFmpeg::AVDictionaryEntry::key::get()
{
	auto p = ((::AVDictionaryEntry*)m_pPointer)->key;
	return p != nullptr ? gcnew String(p) : nullptr;
}

String^ FFmpeg::AVDictionaryEntry::value::get()
{
	auto p = ((::AVDictionaryEntry*)m_pPointer)->value;
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVDictionaryEntry::ToString()
{
	String^ _key = key;
	String^ _value = value;
	if (_key != nullptr || _value != nullptr)
	{
		return String::Format("[ AVDictionaryEntry ] {0}={1}",_key != nullptr ? _key : "",_value != nullptr ? _value : "");
	}
	return __super::ToString();
}
//////////////////////////////////////////////////////
// AVDictionaryEnumerator
//////////////////////////////////////////////////////
private struct AVDictionary {
	int count;
	AVDictionaryEntry *elems;
};
//////////////////////////////////////////////////////
FFmpeg::AVDictionary::AVDictionaryEnumerator::AVDictionaryEnumerator(FFmpeg::AVDictionary^ _dictionary)
	: m_pEntry(nullptr)
	, m_pDictionary(_dictionary)
{

}
FFmpeg::AVDictionary::AVDictionaryEnumerator::~AVDictionaryEnumerator()
{
}
//////////////////////////////////////////////////////
// IEnumerator
bool FFmpeg::AVDictionary::AVDictionaryEnumerator::MoveNext()
{
	void * p = m_pDictionary->m_pPointer;
	m_pEntry = av_dict_get((::AVDictionary*)p, "",(::AVDictionaryEntry*)m_pEntry, AV_DICT_IGNORE_SUFFIX);
	return m_pEntry != nullptr;
}

FFmpeg::AVDictionaryEntry^ FFmpeg::AVDictionary::AVDictionaryEnumerator::Current::get()
{
	if (m_pEntry == nullptr)
	{
		if (!MoveNext()) return nullptr;
	}
	return _CreateChildObject<AVDictionaryEntry>(m_pEntry,m_pDictionary);
}

void FFmpeg::AVDictionary::AVDictionaryEnumerator::Reset()
{
	m_pEntry = nullptr;
}

Object^ FFmpeg::AVDictionary::AVDictionaryEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVDictionary
//////////////////////////////////////////////////////
FFmpeg::AVDictionary::AVDictionary(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}

FFmpeg::AVDictionary::AVDictionary()
	: AVBase(nullptr,nullptr)
{
	m_pFreep = (TFreeFNP*)av_dict_free;
}

FFmpeg::AVDictionary::AVDictionary(FFmpeg::AVDictionary^ _dictionary,FFmpeg::AVDictFlags _flags)
	: AVBase(nullptr,nullptr)
{
	m_pFreep = (TFreeFNP*)av_dict_free;
	::AVDictionary* pd = nullptr;
	void * p = _dictionary->m_pPointer;
	av_dict_copy(&pd,(::AVDictionary*)p,(int)_flags);
	m_pPointer = pd;
}
//////////////////////////////////////////////////////
int FFmpeg::AVDictionary::Count::get()
{
	return GetCount();
}
String^ FFmpeg::AVDictionary::default::get(int _index)
{
	return GetValue(_index);
}
void FFmpeg::AVDictionary::default::set(int _index,String^ _value)
{
	SetValue(GetKey(_index),_value);
}
String^ FFmpeg::AVDictionary::default::get(String^ _key)
{
	return GetValue(_key);
}
void FFmpeg::AVDictionary::default::set(String^ _key,String^ _value)
{
	SetValue(_key,_value);
}
FFmpeg::AVDictionary::AVKeys^ FFmpeg::AVDictionary::Keys::get()
{
	return gcnew FFmpeg::AVDictionary::AVKeys(this);
}

FFmpeg::AVDictionary::AVValues^ FFmpeg::AVDictionary::Values::get()
{
	return gcnew FFmpeg::AVDictionary::AVValues(this);
}
//////////////////////////////////////////////////////
int FFmpeg::AVDictionary::GetCount()
{
	void * pd = m_pPointer;
	return av_dict_count((::AVDictionary*)pd);
}
String^ FFmpeg::AVDictionary::GetKey(int _index)
{
	System::Collections::IEnumerator^ _enumerator = GetEnumerator();
	while (_enumerator->MoveNext())
	{
		if (_index-- == 0)
		{
			return ((AVDictionaryEntry^)_enumerator->Current)->key;
		}
	}
	return nullptr;
}
String^ FFmpeg::AVDictionary::GetValue(int _index)
{
	return GetValue(GetKey(_index));
}
String^ FFmpeg::AVDictionary::GetValue(String^ _key)
{
	if (!String::IsNullOrEmpty(_key))
	{
		System::Collections::IEnumerator^ _enumerator = GetEnumerator();
		while (_enumerator->MoveNext())
		{
			AVDictionaryEntry^ _entry = (AVDictionaryEntry^)_enumerator->Current;
			if (_entry->key == _key) return _entry->value;
		}
	}
	return nullptr;
	
}
void FFmpeg::AVDictionary::SetValue(String^ _key,String^ _value)
{
	SetValue(_key,_value,AVDictFlags::NONE);
}
void FFmpeg::AVDictionary::SetValue(String^ _key,String^ _value, FFmpeg::AVDictFlags _flags)
{
	if (!String::IsNullOrEmpty(_key))
	{
		void * pd = m_pPointer;
		char* szKey = nullptr;
		char* szValue = nullptr;
		try
		{
			szKey = (char*)AllocString(_key).ToPointer();
			szValue = (char*)AllocString(_value).ToPointer();
			av_dict_set((::AVDictionary**)&pd,szKey,szValue,(int)_flags);
		}
		finally
		{
			FreeMemory((IntPtr)szKey);
			FreeMemory((IntPtr)szValue);
			m_pPointer = pd;
		}
	}
}
int FFmpeg::AVDictionary::IndexOf(String^ _key)
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (GetKey(i) == _key) return i;
	}
	return -1;
}
bool FFmpeg::AVDictionary::IsExists(String^ _key)
{
	return IndexOf(_key) >= 0; 
}

FFmpeg::AVDictionaryEntry^ FFmpeg::AVDictionary::Find(String^ _key)
{
	return Find(_key,AVDictFlags::NONE);
}

FFmpeg::AVDictionaryEntry^ FFmpeg::AVDictionary::Find(String^ _key, AVDictFlags _flags)
{
	char * szKey = nullptr;
	try
	{
		szKey = (char*)AllocString(_key).ToPointer();
		void * p = av_dict_get((::AVDictionary*)m_pPointer, szKey,NULL, (int)_flags);
		return _CreateObject<AVDictionaryEntry>(p);
	}
	finally
	{
		FreeMemory((IntPtr)szKey);
	}
}
//////////////////////////////////////////////////////
// IEnumerable
System::Collections::IEnumerator^ FFmpeg::AVDictionary::GetEnumerator()
{
	return gcnew AVDictionaryEnumerator(this);
}

System::Collections::Generic::IEnumerator<FFmpeg::AVDictionaryEntry^>^ FFmpeg::AVDictionary::GetEnumeratorGeneric()
{
	return gcnew AVDictionaryEnumerator(this);
}
//////////////////////////////////////////////////////
// ICloneable
Object^ FFmpeg::AVDictionary::Clone()
{
	return gcnew AVDictionary(this,AVDictFlags::NONE);
}
//////////////////////////////////////////////////////
// AVLog
//////////////////////////////////////////////////////
void FFmpeg::AVLog::log(AVLogLevel level, String^ text)
{
	return log(IntPtr::Zero,level,text);
}

void FFmpeg::AVLog::log(AVBase^ avcl,AVLogLevel level, String^ text)
{
	return log(avcl != nullptr ? avcl->_Pointer : IntPtr::Zero,level,text);
}

void FFmpeg::AVLog::log(IntPtr avcl, AVLogLevel level, String^ text)
{
	if (!String::IsNullOrEmpty(text))
	{
		char* szText = nullptr;
		try
		{
			szText = (char*)AVBase::AllocString(text).ToPointer();
			av_log(avcl.ToPointer(),(int)level,szText);
		}
		finally
		{
			AVBase::FreeMemory((IntPtr)szText);
		}
	}
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVLog::default_item_name(IntPtr avcl)
{
	return gcnew String(av_default_item_name(avcl.ToPointer()));
}
String^ FFmpeg::AVLog::default_item_name(FFmpeg::AVBase^ avcl)
{
	return default_item_name(avcl != nullptr ? avcl->_Pointer : IntPtr::Zero);
}
FFmpeg::AVClassCategory FFmpeg::AVLog::default_get_category(IntPtr ptr)
{
	return (AVClassCategory)av_default_get_category(ptr.ToPointer());
}

FFmpeg::AVClassCategory FFmpeg::AVLog::default_get_category(AVBase^ ptr)
{
	return default_get_category(ptr != nullptr ? ptr->_Pointer : IntPtr::Zero);
}
//////////////////////////////////////////////////////
FFmpeg::AVLogLevel FFmpeg::AVLog::Level::get()
{
	return (AVLogLevel)av_log_get_level();
}
void FFmpeg::AVLog::Level::set(AVLogLevel _value)
{
	av_log_set_level((int)_value);
}
FFmpeg::AVLogFlags FFmpeg::AVLog::Flags::get()
{
	return (AVLogFlags)av_log_get_flags();
}
void FFmpeg::AVLog::Flags::set(FFmpeg::AVLogFlags _flags)
{
	av_log_set_flags((int)_flags);
}
//////////////////////////////////////////////////////
void Generic_log_callback(void *avcl, int level, const char *fmt, va_list vl)
{
	FFmpeg::AVLogCallback^ _callback = FFmpeg::AVLog::Callback;
	if (_callback != nullptr)
	{
		_callback(avcl != nullptr ? gcnew FFmpeg::AVBase(avcl,nullptr) : nullptr,level,gcnew String(fmt),(IntPtr)vl);
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVLogCallback^ FFmpeg::AVLog::Callback::get()
{
	Monitor::Enter(s_csCallbackLock);
	try
	{
		return s_pCallback;
	}
	finally
	{
		Monitor::Exit(s_csCallbackLock);
	}
}

void FFmpeg::AVLog::Callback::set(AVLogCallback^ callback)
{
	Monitor::Enter(s_csCallbackLock);
	try
	{
		if (s_pCallback != callback)
		{
			s_pCallback = callback;
			if (s_pCallback == nullptr)
			{
				av_log_set_callback(av_log_default_callback);
			}
			else
			{
				av_log_set_callback(Generic_log_callback);
			}
		}
	}
	finally
	{
		Monitor::Exit(s_csCallbackLock);
	}
}
//////////////////////////////////////////////////////
void FFmpeg::AVLog::hex_dump_log(AVBase^ avcl, int level, IntPtr buf, int size)
{
	av_hex_dump_log(avcl != nullptr ? avcl->_Pointer.ToPointer() : nullptr,level, (const uint8_t *)buf.ToPointer(),size);
}

void FFmpeg::AVLog::pkt_dump_log(AVBase^ avcl, int level, AVPacket^ pkt, int dump_payload,AVStream^ st)
{
	av_pkt_dump_log2(avcl != nullptr ? avcl->_Pointer.ToPointer() : nullptr,level, pkt != nullptr ?(const ::AVPacket *)pkt->_Pointer.ToPointer() : nullptr,dump_payload,st != nullptr ? (const ::AVStream*)st->_Pointer.ToPointer() : nullptr);
}
//////////////////////////////////////////////////////
// AVMotionVector
//////////////////////////////////////////////////////
FFmpeg::AVMotionVector::AVMotionVector(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}

FFmpeg::AVMotionVector::AVMotionVector()
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
	//m_pFreep = (TFreeFNP*)av_freep;
}
//////////////////////////////////////////////////////
int FFmpeg::AVMotionVector::_StructureSize::get()
{
	return sizeof(::AVMotionVector);
}
//////////////////////////////////////////////////////
Int32 FFmpeg::AVMotionVector::source::get()
{
	return ((::AVMotionVector*)m_pPointer)->source;
}
void FFmpeg::AVMotionVector::source::set(Int32 value)
{
	((::AVMotionVector*)m_pPointer)->source = value;
}
Byte FFmpeg::AVMotionVector::w::get()
{
	return ((::AVMotionVector*)m_pPointer)->w;
}
void FFmpeg::AVMotionVector::w::set(Byte value)
{
	((::AVMotionVector*)m_pPointer)->w = value;
}
Byte FFmpeg::AVMotionVector::h::get()
{
	return ((::AVMotionVector*)m_pPointer)->h;
}
void FFmpeg::AVMotionVector::h::set(Byte value)
{
	((::AVMotionVector*)m_pPointer)->h = value;
}
Int16 FFmpeg::AVMotionVector::src_x::get()
{
	return ((::AVMotionVector*)m_pPointer)->src_x;
}
void FFmpeg::AVMotionVector::src_x::set(Int16 value)
{
	((::AVMotionVector*)m_pPointer)->src_x = value;
}
Int16 FFmpeg::AVMotionVector::src_y::get()
{
	return ((::AVMotionVector*)m_pPointer)->src_y;
}
void FFmpeg::AVMotionVector::src_y::set(Int16 value)
{
	((::AVMotionVector*)m_pPointer)->src_y = value;
}
Int16 FFmpeg::AVMotionVector::dst_x::get()
{
	return ((::AVMotionVector*)m_pPointer)->dst_x;
}
void FFmpeg::AVMotionVector::dst_x::set(Int16 value)
{
	((::AVMotionVector*)m_pPointer)->dst_x = value;
}
Int16 FFmpeg::AVMotionVector::dst_y::get() 
{
	return ((::AVMotionVector*)m_pPointer)->dst_y;
}
void FFmpeg::AVMotionVector::dst_y::set(Int16 value)
{
	((::AVMotionVector*)m_pPointer)->dst_y = value;
}
UInt64 FFmpeg::AVMotionVector::flags::get()
{
	return ((::AVMotionVector*)m_pPointer)->flags;
}
void FFmpeg::AVMotionVector::flags::set(UInt64 value)
{
	((::AVMotionVector*)m_pPointer)->flags = value;
}
//////////////////////////////////////////////////////
// AVCRC
//////////////////////////////////////////////////////
FFmpeg::AVCRC::AVCRC(AVCRCId id)
	: AVBase(nullptr,nullptr)
{
	m_pPointer = (void*)av_crc_get_table((::AVCRCId)id);
}

FFmpeg::AVCRC::AVCRC(IntPtr ctx, bool le, int bits, UInt32 poly, int ctx_size)
	: AVBase(nullptr,nullptr)
{
	m_pPointer = ctx.ToPointer();
	av_crc_init((::AVCRC *)m_pPointer, le ? 1 : 0,bits, poly, ctx_size);
}
//////////////////////////////////////////////////////
UInt32 FFmpeg::AVCRC::CRC(UInt32 crc,IntPtr buffer, int length)
{
	return av_crc((::AVCRC *)m_pPointer, crc,(const uint8_t *)buffer.ToPointer(),(size_t)length);
}
//////////////////////////////////////////////////////
UInt32 FFmpeg::AVCRC::CRC(IntPtr buffer, int length)
{
	return CRC(0, buffer, length);
}
UInt32 FFmpeg::AVCRC::CRC(UInt32 crc,array<byte>^ buffer)
{
	return CRC(crc,buffer,0,buffer->Length);
}
UInt32 FFmpeg::AVCRC::CRC(UInt32 crc,array<byte>^ buffer,int count)
{
	return CRC(crc,buffer,0,count);
}
UInt32 FFmpeg::AVCRC::CRC(UInt32 crc,array<byte>^ buffer,int index, int count)
{
	IntPtr ptr = AllocMemory(count * sizeof(byte));
	try
	{
		Marshal::Copy(buffer, index, ptr, count);
		return CRC(crc,ptr,count);
	}
	finally
	{
		FreeMemory(ptr);
	}
}
UInt32 FFmpeg::AVCRC::CRC(array<byte>^ buffer)
{
	return CRC(buffer,0,buffer->Length);
}
UInt32 FFmpeg::AVCRC::CRC(array<byte>^ buffer,int count)
{
	return CRC(buffer,0,count);
}
UInt32 FFmpeg::AVCRC::CRC(array<byte>^ buffer,int index, int count)
{
	return CRC(0, buffer, index, count);
}
//////////////////////////////////////////////////////
UInt32 FFmpeg::AVCRC::CRC(AVCRCId crc_id, UInt32 crc,IntPtr buffer, int length)
{
	AVCRC^ _crc = gcnew AVCRC(crc_id);
	try
	{
		return _crc->CRC(crc, buffer, length);
	}
	finally
	{
		delete _crc;
	}
}
UInt32 FFmpeg::AVCRC::CRC(AVCRCId crc_id, UInt32 crc,array<byte>^ buffer, int index, int count)
{
	AVCRC^ _crc = gcnew AVCRC(crc_id);
	try
	{
		return _crc->CRC(crc, buffer, index, count);
	}
	finally
	{
		delete _crc;
	}
}
//////////////////////////////////////////////////////
// AVAudioFifo
//////////////////////////////////////////////////////
struct AVAudioFifo {};
//////////////////////////////////////////////////////
FFmpeg::AVAudioFifo::AVAudioFifo(AVSampleFormat sample_fmt, int channels,int nb_samples)
	: AVBase(nullptr,nullptr)
{
	m_pPointer = av_audio_fifo_alloc((::AVSampleFormat)sample_fmt, channels, nb_samples);
	m_pFree = (TFreeFN*)av_audio_fifo_free;
}
//////////////////////////////////////////////////////
int FFmpeg::AVAudioFifo::Size::get()
{
	return av_audio_fifo_size((::AVAudioFifo*)m_pPointer);
}

int FFmpeg::AVAudioFifo::Space::get()
{
	return av_audio_fifo_space((::AVAudioFifo*)m_pPointer);
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVAudioFifo::Realloc(int nb_samples)
{
	return av_audio_fifo_realloc((::AVAudioFifo*)m_pPointer,nb_samples);
}

void FFmpeg::AVAudioFifo::Reset()
{
	av_audio_fifo_reset((::AVAudioFifo*)m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::AVAudioFifo::Drain(int nb_samples)
{
	return av_audio_fifo_drain((::AVAudioFifo*)m_pPointer,nb_samples);
}
//////////////////////////////////////////////////////
int FFmpeg::AVAudioFifo::Write(array<IntPtr>^ data, int nb_samples)
{
	void * _data[AV_NUM_DATA_POINTERS] = { 0 };
	for (int i = 0; i < data->Length && i < AV_NUM_DATA_POINTERS; i++) { _data[i] = data[i].ToPointer(); }
	return av_audio_fifo_write((::AVAudioFifo*)m_pPointer,_data,nb_samples);
}
int FFmpeg::AVAudioFifo::Write(IntPtr data, int nb_samples)
{
	void * _data[AV_NUM_DATA_POINTERS] = { data.ToPointer(),0,0,0,0,0,0,0 };
	return av_audio_fifo_write((::AVAudioFifo*)m_pPointer,_data,nb_samples);
}
int FFmpeg::AVAudioFifo::Write(AVArray<IntPtr>^ data, int nb_samples)
{
	void ** _data = (void**)data->_Pointer.ToPointer();
	return av_audio_fifo_write((::AVAudioFifo*)m_pPointer,_data,nb_samples);
}
int FFmpeg::AVAudioFifo::Write(array<AVMemPtr^>^ data, int nb_samples)
{
	void * _data[AV_NUM_DATA_POINTERS] = { 0 };
	for (int i = 0; i < data->Length && i < AV_NUM_DATA_POINTERS; i++) { _data[i] = ((IntPtr)data[i]).ToPointer(); }
	return av_audio_fifo_write((::AVAudioFifo*)m_pPointer,_data,nb_samples);
}
int FFmpeg::AVAudioFifo::Write(AVArray<AVMemPtr^>^ data, int nb_samples)
{
	return Write(data->ToPtrArray(),nb_samples);
}
int FFmpeg::AVAudioFifo::Write(AVFrame^ frame, int nb_samples)
{
	return Write(frame->data->ToPtrArray(),nb_samples);
}
int FFmpeg::AVAudioFifo::Write(AVFrame^ frame)
{
	return Write(frame->data->ToPtrArray(),frame->nb_samples);
}
//////////////////////////////////////////////////////
int FFmpeg::AVAudioFifo::Read(array<IntPtr>^ data, int nb_samples)
{
	void * _data[AV_NUM_DATA_POINTERS] = { 0 };
	for (int i = 0; i < data->Length && i < AV_NUM_DATA_POINTERS; i++) { _data[i] = data[i].ToPointer(); }
	return av_audio_fifo_read((::AVAudioFifo*)m_pPointer,_data,nb_samples);
}
int FFmpeg::AVAudioFifo::Read(IntPtr data, int nb_samples)
{
	void * _data[AV_NUM_DATA_POINTERS] = { data.ToPointer(),0,0,0,0,0,0,0 };
	return av_audio_fifo_read((::AVAudioFifo*)m_pPointer,_data,nb_samples);
}
int FFmpeg::AVAudioFifo::Read(AVArray<IntPtr>^ data, int nb_samples)
{
	void ** _data = (void**)data->_Pointer.ToPointer();
	return av_audio_fifo_read((::AVAudioFifo*)m_pPointer,_data,nb_samples);
}
int FFmpeg::AVAudioFifo::Read(AVArray<AVMemPtr^>^ data, int nb_samples)
{
	void * _data[AV_NUM_DATA_POINTERS] = { 0 };
	for (int i = 0; i < data->Count && i < AV_NUM_DATA_POINTERS; i++) { _data[i] = ((IntPtr)data[i]).ToPointer(); }
	return av_audio_fifo_read((::AVAudioFifo*)m_pPointer,_data,nb_samples);
}
int FFmpeg::AVAudioFifo::Read(array<AVMemPtr^>^ data, int nb_samples)
{
	void * _data[AV_NUM_DATA_POINTERS] = { 0 };
	for (int i = 0; i < data->Length && i < AV_NUM_DATA_POINTERS; i++) { _data[i] = ((IntPtr)data[i]).ToPointer(); }
	return av_audio_fifo_read((::AVAudioFifo*)m_pPointer,_data,nb_samples);
}
int FFmpeg::AVAudioFifo::Read(AVFrame^ frame, int nb_samples)
{
	return Read(frame->data,nb_samples);
}
int FFmpeg::AVAudioFifo::Read(AVFrame^ frame) 
{
	return Read(frame->data,frame->nb_samples);
}
//////////////////////////////////////////////////////
// AVFifoBuffer
//////////////////////////////////////////////////////
FFmpeg::AVFifoBuffer::AVFifoBuffer(int _size)
	: AVBase(nullptr,nullptr)
{
	m_pPointer = av_fifo_alloc(_size);
	m_pFreep = (TFreeFNP*)av_fifo_freep;
}
FFmpeg::AVFifoBuffer::AVFifoBuffer(int nmemb, int size)
	: AVBase(nullptr,nullptr)
{
	m_pPointer = av_fifo_alloc_array(nmemb, size);
	m_pFreep = (TFreeFNP*)av_fifo_freep;
}
//////////////////////////////////////////////////////
int FFmpeg::AVFifoBuffer::Size::get()
{
	return av_fifo_size((::AVFifoBuffer*)m_pPointer);
}
int FFmpeg::AVFifoBuffer::Space::get()
{
	return av_fifo_space((::AVFifoBuffer*)m_pPointer);
}
//////////////////////////////////////////////////////
void FFmpeg::AVFifoBuffer::Reset()
{
	return av_fifo_reset((::AVFifoBuffer*)m_pPointer);
}
int FFmpeg::AVFifoBuffer::Realloc(int size)
{
	return av_fifo_realloc2((::AVFifoBuffer*)m_pPointer,size);
}
int FFmpeg::AVFifoBuffer::Grow(int additional_space)
{
	return av_fifo_grow((::AVFifoBuffer*)m_pPointer,additional_space);
}
void FFmpeg::AVFifoBuffer::Drain(int size)
{
	av_fifo_drain((::AVFifoBuffer*)m_pPointer,size);
}
IntPtr FFmpeg::AVFifoBuffer::Peek(int offs)
{
	return (IntPtr)av_fifo_peek2((::AVFifoBuffer*)m_pPointer,offs);
}
//////////////////////////////////////////////////////
int FFmpeg::AVFifoBuffer::Read(IntPtr dest, int buf_size, AVFifoReadDelegate^ func)
{
	IntPtr _ptr = Marshal::GetFunctionPointerForDelegate(func);
	typedef void(*PFN)(void*, void*, int);
	return av_fifo_generic_read((::AVFifoBuffer*)m_pPointer,dest.ToPointer(), buf_size, (PFN)_ptr.ToPointer());
}

int FFmpeg::AVFifoBuffer::Write(IntPtr src, int size, AVFifoWriteDelegate^ func)
{
	IntPtr _ptr = Marshal::GetFunctionPointerForDelegate(func);
	typedef int(*PFN)(void*, void*, int);
	return av_fifo_generic_write((::AVFifoBuffer*)m_pPointer,src.ToPointer(), size, (PFN)_ptr.ToPointer());
}
//////////////////////////////////////////////////////
// AVStereo3D
//////////////////////////////////////////////////////
FFmpeg::AVStereo3D::AVStereo3D(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}

FFmpeg::AVStereo3D::AVStereo3D()
	: AVBase(nullptr,nullptr)
{
	m_pPointer = av_stereo3d_alloc();
	m_pFreep = (TFreeFNP*)av_freep;
}
//////////////////////////////////////////////////////
int FFmpeg::AVStereo3D::_StructureSize::get()
{
	return sizeof(::AVStereo3D);
}
//////////////////////////////////////////////////////
FFmpeg::AVStereo3DType FFmpeg::AVStereo3D::type::get()
{
	return (AVStereo3DType)((::AVStereo3D*)m_pPointer)->type;
}
void FFmpeg::AVStereo3D::type::set(AVStereo3DType value)
{
	((::AVStereo3D*)m_pPointer)->type = (::AVStereo3DType)value;
}
FFmpeg::AVStereo3DFlags FFmpeg::AVStereo3D::flags::get()
{
	return (AVStereo3DFlags)((::AVStereo3D*)m_pPointer)->flags;
}
void FFmpeg::AVStereo3D::flags::set(AVStereo3DFlags value)
{
	((::AVStereo3D*)m_pPointer)->flags = (int)value;
}
//////////////////////////////////////////////////////
FFmpeg::AVStereo3D^ FFmpeg::AVStereo3D::CreateSideData(AVFrame^ frame)
{
	::AVStereo3D * p = av_stereo3d_create_side_data((::AVFrame*)frame->_Pointer.ToPointer());
	if (p != nullptr)
	{
		return _CreateChildObject<AVStereo3D>(p,frame);
	}
	return nullptr;
}
//////////////////////////////////////////////////////
// AVMD5
//////////////////////////////////////////////////////
struct AVMD5 {};
//////////////////////////////////////////////////////
FFmpeg::AVMD5::AVMD5()
	: AVBase(nullptr,nullptr)
{
	PTR_API(AVUtil,av_md5_alloc)
	m_pPointer = av_md5_alloc();
	if (m_pPointer)
	{
		m_pFreep = (TFreeFNP *)::av_freep;
		Reset();
	}
}
//////////////////////////////////////////////////////
void FFmpeg::AVMD5::Reset()
{
	VOID_API(AVUtil,av_md5_init,::AVMD5 *ctx)
	av_md5_init((::AVMD5*)m_pPointer);
}
//////////////////////////////////////////////////////
void FFmpeg::AVMD5::Update(IntPtr src, int length)
{
	VOID_API(AVUtil,av_md5_update,::AVMD5 *ctx,const uint8_t *src, int len)
	av_md5_update((::AVMD5*)m_pPointer, (const uint8_t *)src.ToPointer(),length);
}
void FFmpeg::AVMD5::Update(array<byte>^ src)
{
	Update(src,0,src->Length);
}
void FFmpeg::AVMD5::Update(array<byte>^ src, int length)
{
	Update(src,0,length);
}
void FFmpeg::AVMD5::Update(array<byte>^ src, int start, int length)
{
	pin_ptr<byte> _ptr = &src[0];
	uint8_t * data = ((uint8_t *)_ptr) + start;
	Update((IntPtr)data,length);
}
void FFmpeg::AVMD5::Final(IntPtr dst)
{
	VOID_API(AVUtil,av_md5_final,::AVMD5 *ctx,const uint8_t *dst)
	av_md5_final((::AVMD5*)m_pPointer, (uint8_t *)dst.ToPointer());
}
void FFmpeg::AVMD5::Final(array<byte>^ % dst)
{
	dst = gcnew array<byte>(16);
	pin_ptr<byte> _ptr = &dst[0];
	uint8_t * data = ((uint8_t *)_ptr);
	Final((IntPtr)data);
}
//////////////////////////////////////////////////////
void FFmpeg::AVMD5::sum(IntPtr dst, IntPtr src, int len)
{
	VOID_API(AVUtil,av_md5_sum,uint8_t *dst, const uint8_t *src, const int len)
	av_md5_sum((uint8_t *)dst.ToPointer(),(const uint8_t *)src.ToPointer(),len);
}
void FFmpeg::AVMD5::sum(IntPtr dst, array<byte>^ src, int start, int len)
{
	pin_ptr<byte> _ptr = &src[0];
	uint8_t * data = ((uint8_t *)_ptr) + start;
	sum(dst,(IntPtr)data,len);
}
void FFmpeg::AVMD5::sum(IntPtr dst, array<byte>^ src, int len)
{
	sum(dst, src, 0, len);
}
void FFmpeg::AVMD5::sum(IntPtr dst, array<byte>^ src)
{
	sum(dst, src, 0, src->Length);
}
void FFmpeg::AVMD5::sum(array<byte>^ % dst, array<byte>^ src, int start, int len)
{
	dst = gcnew array<byte>(16);
	pin_ptr<byte> _dst = &dst[0];
	uint8_t * dst_p = ((uint8_t *)_dst);
	pin_ptr<byte> _src = &src[0];
	uint8_t * src_p = ((uint8_t *)_src) + start;
	sum((IntPtr)dst_p,(IntPtr)src_p,len);
}
void FFmpeg::AVMD5::sum(array<byte>^ % dst, array<byte>^ src, int len)
{
	sum(dst, src, 0, len);
}
void FFmpeg::AVMD5::sum(array<byte>^ % dst, array<byte>^ src)
{
	sum(dst, src, 0, src->Length);
}
//////////////////////////////////////////////////////
// AVHashContext
//////////////////////////////////////////////////////
struct AVHashContext {};
//////////////////////////////////////////////////////
FFmpeg::AVHashContext::AVHashContext(String^ name)
	: AVBase(nullptr,nullptr)
{
	::AVHashContext * p = nullptr;
	char * szName = nullptr;
	szName = (char*)AVBase::AllocString(name).ToPointer();
	try
	{
		if (0 == av_hash_alloc(&p,szName))
		{
			m_pPointer = p;
			if (m_pPointer)
			{
				m_pFreep = (TFreeFNP *)::av_hash_freep;
				Reset();
			}
		}
	}
	finally
	{
		AVBase::FreeMemory((IntPtr)szName);
	}
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVHashContext::name::get()
{
	if (!m_pPointer) return "";
	return gcnew String(av_hash_get_name((const ::AVHashContext *)m_pPointer));
}
int FFmpeg::AVHashContext::size::get()
{
	if (!m_pPointer) return 0;
	return av_hash_get_size((const ::AVHashContext *)m_pPointer);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVHashContext::ToString()
{
	return "[ AVHashContext ] \"" + name + "\"";
}
//////////////////////////////////////////////////////
void FFmpeg::AVHashContext::Reset()
{
	av_hash_init((::AVHashContext *)m_pPointer);
}
void FFmpeg::AVHashContext::Update(IntPtr src, int length)
{
	av_hash_update((::AVHashContext*)m_pPointer, (const uint8_t *)src.ToPointer(),length);
}
void FFmpeg::AVHashContext::Update(array<byte>^ src)
{
	Update(src,0,src->Length);
}
void FFmpeg::AVHashContext::Update(array<byte>^ src, int length)
{
	Update(src,0,length);
}
void FFmpeg::AVHashContext::Update(array<byte>^ src, int start, int length)
{
	pin_ptr<byte> _ptr = &src[0];
	uint8_t * data = ((uint8_t *)_ptr) + start;
	Update((IntPtr)data,length);
}
void FFmpeg::AVHashContext::Update(AVMemPtr^ ptr, int length)
{
	Update((IntPtr)ptr,length);
}
void FFmpeg::AVHashContext::Update(AVMemPtr^ ptr)
{
	Update(ptr,ptr->size);
}
void FFmpeg::AVHashContext::Final(IntPtr dst)
{
	av_hash_final((::AVHashContext*)m_pPointer,(uint8_t *)dst.ToPointer());
	Reset();
}
void FFmpeg::AVHashContext::Final([Out] array<byte>^ % dst)
{
	dst = gcnew array<byte>(size);
	pin_ptr<byte> _ptr = &dst[0];
	uint8_t * data = ((uint8_t *)_ptr);
	Final((IntPtr)data);
}
void FFmpeg::AVHashContext::Final([Out] AVMemPtr^ % dst)
{
	dst = gcnew AVMemPtr(size);
	Final((IntPtr)dst);
}
//////////////////////////////////////////////////////
void FFmpeg::AVHashContext::FinalBin(IntPtr dst, int size)
{
	av_hash_final_bin((::AVHashContext*)m_pPointer,(uint8_t *)dst.ToPointer(),size);
	Reset();
}
void FFmpeg::AVHashContext::FinalHex([Out] String^ % dst)
{
	char temp[(AV_HASH_MAX_SIZE << 1) + 1] = {0};
	av_hash_final_hex((::AVHashContext*)m_pPointer,(uint8_t*)temp,_countof(temp));
	dst = gcnew String(temp);
	Reset();
}
void FFmpeg::AVHashContext::FinalB64([Out] String^ % dst)
{
	char temp[AV_BASE64_SIZE(AV_HASH_MAX_SIZE) + 1] = {0};
	av_hash_final_b64((::AVHashContext*)m_pPointer,(uint8_t*)temp,_countof(temp));
	dst = gcnew String(temp);
	Reset();
}
//////////////////////////////////////////////////////
array<String^>^ FFmpeg::AVHashContext::Names::get()
{
	List<String^>^ list = gcnew List<String^>();
	int idx = 0;
	while (true)
	{
		const char * name = av_hash_names(idx++);
		if (!name) break;
		list->Add(gcnew String(name));
	}
	return list->ToArray();
}
//////////////////////////////////////////////////////
// RDFTContext
//////////////////////////////////////////////////////
struct RDFTContext {};
//////////////////////////////////////////////////////
FFmpeg::RDFTContext::RDFTContext(int nbits, RDFTransformType trans)
	: AVBase(nullptr,nullptr)
{
	LOAD_API(AVCodec,::RDFTContext *,av_rdft_init,int ,::RDFTransformType);
	LOAD_API(AVCodec,void ,av_rdft_end,::RDFTContext *);
	if (av_rdft_init)
	{
		m_pPointer = av_rdft_init(nbits, (::RDFTransformType)trans);
	}
	if (m_pPointer)
	{
		if (av_rdft_end)
		{
			m_pFree = (TFreeFN*)av_rdft_end;
		}
		else
		{
			m_pFree = (TFreeFN*)av_free;
		}
	}
}
//////////////////////////////////////////////////////
void FFmpeg::RDFTContext::calc(array<FFTSample>^ data)
{
	return calc(data,0,data->Length);
}

void FFmpeg::RDFTContext::calc(array<FFTSample>^ data, int start_idx, int length)
{
	pin_ptr<FFTSample> pinp = &data[start_idx];
	calc((IntPtr)((float*)pinp),length);
}

void FFmpeg::RDFTContext::calc(IntPtr data)
{
	float * f = (float *)data.ToPointer();
	VOID_API(AVCodec,av_rdft_calc,::RDFTContext *,::FFTSample *)
	av_rdft_calc((::RDFTContext*)m_pPointer, f);
}

void FFmpeg::RDFTContext::calc(IntPtr data, int size)
{
	return calc(data, 0,size);
}

void FFmpeg::RDFTContext::calc(IntPtr data,int start_idx,int length)
{
	float * f = (float *)data.ToPointer();
	return calc((IntPtr)(&f[start_idx]));
}
//////////////////////////////////////////////////////
// DCTContext
//////////////////////////////////////////////////////
struct DCTContext {};
//////////////////////////////////////////////////////
FFmpeg::DCTContext::DCTContext(int nbits, DCTTransformType trans)
	: AVBase(nullptr,nullptr)
{
	LOAD_API(AVCodec,::RDFTContext *,av_dct_init,int ,::DCTTransformType);
	LOAD_API(AVCodec,void ,av_dct_end,::RDFTContext *);
	if (av_dct_init)
	{
		m_pPointer = av_dct_init(nbits, (::DCTTransformType)trans);
	}
	if (m_pPointer)
	{
		if (av_dct_end)
		{
			m_pFree = (TFreeFN*)av_dct_end;
		}
		else
		{
			m_pFree = (TFreeFN*)av_free;
		}
	}
}
//////////////////////////////////////////////////////
void FFmpeg::DCTContext::calc(array<FFTSample>^ data)
{
	return calc(data,0,data->Length);
}

void FFmpeg::DCTContext::calc(array<FFTSample>^ data, int start_idx, int length)
{
	pin_ptr<FFTSample> pinp = &data[start_idx];
	calc((IntPtr)((float*)pinp),length);
}

void FFmpeg::DCTContext::calc(IntPtr data)
{
	float * f = (float *)data.ToPointer();
	VOID_API(AVCodec,av_dct_calc,::DCTContext *,::FFTSample *)
	av_dct_calc((::DCTContext*)m_pPointer, f);
}

void FFmpeg::DCTContext::calc(IntPtr data, int size)
{
	return calc(data, 0,size);
}

void FFmpeg::DCTContext::calc(IntPtr data,int start_idx,int length)
{
	float * f = (float *)data.ToPointer();
	return calc((IntPtr)(&f[start_idx]));
}
//////////////////////////////////////////////////////