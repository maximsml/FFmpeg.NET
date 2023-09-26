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
// libavfilter
// Graph-based frame editing library.
//////////////////////////////////////////////////////
#include "stdafx.h"
#include "AVFilter.h"
//////////////////////////////////////////////////////
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::Threading;
//////////////////////////////////////////////////////
#pragma warning(disable: 4996) // marked as deprecated
//////////////////////////////////////////////////////
struct AVFilterPad {};
//////////////////////////////////////////////////////
FFmpeg::LibAVFilter::LibAVFilter()
{
}
//////////////////////////////////////////////////////
UInt32 FFmpeg::LibAVFilter::Version::get()
{
	return avfilter_version();
}

String^ FFmpeg::LibAVFilter::Configuration::get()
{
	auto p = avfilter_configuration();
	return p != nullptr ? gcnew String(p) : nullptr;
}

String^ FFmpeg::LibAVFilter::License::get()
{
	auto p = avfilter_license();
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
void FFmpeg::LibAVFilter::Register(FFmpeg::AVFilter^ _filter)
{
	AVBase::EnsureLibraryLoaded();
	if (_filter != nullptr && _filter->_Pointer != IntPtr::Zero)
	{
		s_bRegistered = true;
		VOID_API(AVFilter,avfilter_register,::AVFilter *)
		avfilter_register((::AVFilter*)_filter->_Pointer.ToPointer());
	}
}

void FFmpeg::LibAVFilter::RegisterAll()
{
	AVBase::EnsureLibraryLoaded();
	if (!s_bRegistered) 
	{
		s_bRegistered = true;
		VOID_API(AVFilter,avfilter_register_all)
		avfilter_register_all();
	}
}
//////////////////////////////////////////////////////
// AVFilterPadList
//////////////////////////////////////////////////////
FFmpeg::AVFilter::AVFilterPadList::AVFilterPadList(void * _pointer, AVBase^ _parent)
	: AVBase(_pointer, _parent)
	, m_bIsOutput(false)
	, m_bUpdated(false)
{
	m_Pads = gcnew List<AVFilterPad^>();
}
FFmpeg::AVFilter::AVFilterPadList::~AVFilterPadList()
{
	m_Pads->Clear();
	delete m_Pads;
}
//////////////////////////////////////////////////////
void FFmpeg::AVFilter::AVFilterPadList::SetOutputType(bool bOutput) {
	if (m_bIsOutput != bOutput) {
		m_bIsOutput = bOutput;
		m_bUpdated = false;
	}
}
//////////////////////////////////////////////////////
void FFmpeg::AVFilter::AVFilterPadList::Update()
{
	m_bUpdated = true;
	LOAD_API(AVFilter,int ,avfilter_filter_pad_count,const ::AVFilter *,int);
	LOAD_API(AVFilter,int ,avfilter_pad_count,const ::AVFilterPad *);

	int _count = 0;
	if (avfilter_pad_count) {
		_count = avfilter_pad_count((const ::AVFilterPad *)m_pPointer);
	}
	else {
		if (avfilter_filter_pad_count) {
			_count = avfilter_filter_pad_count(((const ::AVFilter *)m_pParent->_Pointer.ToPointer()),m_bIsOutput ? 1 : 0);
		}
	}
	if (m_Pads->Count < _count)
	{
		for (int i = m_Pads->Count; i < _count; i++)
		{
			m_Pads->Add(gcnew AVFilterPad(m_pPointer,this,i));
		}
	}
	while (m_Pads->Count > _count)
	{
		m_Pads->RemoveAt(m_Pads->Count - 1);
	}
}
//////////////////////////////////////////////////////
array<FFmpeg::AVFilterPad^>^ FFmpeg::AVFilter::AVFilterPadList::ToArray()
{
	Update();
	return m_Pads->ToArray();
}
//////////////////////////////////////////////////////
// AVFilters
//////////////////////////////////////////////////////
FFmpeg::AVFilter::AVFilters::AVFilters()
{
	LibAVFilter::RegisterAll();
}
//////////////////////////////////////////////////////
// AVFilterEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVFilter::AVFilters::AVFilterEnumerator::AVFilterEnumerator()
	: m_pOpaque(IntPtr::Zero)
{

}

FFmpeg::AVFilter::AVFilters::AVFilterEnumerator::~AVFilterEnumerator()
{
}

bool FFmpeg::AVFilter::AVFilters::AVFilterEnumerator::MoveNext()
{
	void * opaque = m_pOpaque.ToPointer();
	const ::AVFilter * p = nullptr;

	LOAD_API(AVFilter,::AVFilter *,avfilter_next,void *);
	LOAD_API(AVFilter,::AVFilter *,av_filter_iterate,void **);

	if (av_filter_iterate)
	{
		p = (const ::AVFilter *)av_filter_iterate(&opaque);
	}
	else
	{
		if (avfilter_next)
		{
			p = (const ::AVFilter *)avfilter_next(opaque);
			opaque = (void*)p;
		}
	}
	m_pCurrent = p != nullptr ? gcnew AVFilter((void*)p,nullptr) : nullptr;
	m_pOpaque = IntPtr(opaque);
	return (m_pCurrent != nullptr);
}
FFmpeg::AVFilter^ FFmpeg::AVFilter::AVFilters::AVFilterEnumerator::Current::get()
{
	if (m_pOpaque == IntPtr::Zero)
	{
		MoveNext();
	}
	return m_pCurrent;
}
void FFmpeg::AVFilter::AVFilters::AVFilterEnumerator::Reset()
{
	m_pOpaque = IntPtr::Zero;
}
Object^ FFmpeg::AVFilter::AVFilters::AVFilterEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVFilter
//////////////////////////////////////////////////////
FFmpeg::AVFilter::AVFilter(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
String^ FFmpeg::AVFilter::name::get()
{ 
	auto p = ((::AVFilter*)m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::AVFilter::description::get()
{ 
	auto p = ((::AVFilter*)m_pPointer)->description;
	return p != nullptr ? gcnew String(p) : nullptr;
}
FFmpeg::AVFilter::AVFilterPadList^ FFmpeg::AVFilter::inputs::get()
{
	const ::AVFilterPad * _pads = ((::AVFilter*)m_pPointer)->inputs;
	if (_pads)
	{
		auto list = _CreateObject<AVFilterPadList>((void*)_pads);
		list->SetOutputType(false);
		return list;
	}
	return nullptr;
}
FFmpeg::AVFilter::AVFilterPadList^ FFmpeg::AVFilter::outputs::get()
{
	const ::AVFilterPad * _pads = ((::AVFilter*)m_pPointer)->outputs;
	if (_pads)
	{
		auto list = _CreateObject<AVFilterPadList>((void*)_pads);
		list->SetOutputType(true);
		return list;
	}
	return nullptr;
}
FFmpeg::AVClass^ FFmpeg::AVFilter::priv_class::get()
{
	auto p = ((::AVFilter*)m_pPointer)->priv_class;
	return _CreateObject<AVClass>((void*)p);
}
FFmpeg::AVFilterFlags FFmpeg::AVFilter::flags::get()
{
	return (FFmpeg::AVFilterFlags)((::AVFilter*)m_pPointer)->flags;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVFilter::ToString()
{
	if (m_pPointer)
	{
		String^ _name;
		_name = name;
		if (!String::IsNullOrEmpty(_name)) return _name;
		_name = description;
		if (!String::IsNullOrEmpty(_name)) return _name;
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
FFmpeg::AVFilter^ FFmpeg::AVFilter::GetByName(String^ name)
{
	LibAVFilter::RegisterAll();
	char* szName = nullptr;
	try
	{
		szName = (char*)AllocString(name).ToPointer();
		auto p = avfilter_get_by_name(szName);
		return p == nullptr ? nullptr : gcnew AVFilter((void*)p,nullptr);
	}
	finally
	{
		FreeMemory((IntPtr)szName);
	}
}

//////////////////////////////////////////////////////
// AVFilterPad
//////////////////////////////////////////////////////
FFmpeg::AVFilterPad::AVFilterPad(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
	, m_nIndex(0)
{
}
FFmpeg::AVFilterPad::AVFilterPad(void * _pointer,AVBase^ _parent,int index)
	: AVBase(_pointer,_parent)
	, m_nIndex(index)
{
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVFilterPad::name::get()
{
	auto p = avfilter_pad_get_name((::AVFilterPad*)m_pPointer,m_nIndex);
	return p != nullptr ? gcnew String(p) : nullptr;
}
FFmpeg::AVMediaType FFmpeg::AVFilterPad::type::get()
{
	return (FFmpeg::AVMediaType)avfilter_pad_get_type((::AVFilterPad*)m_pPointer,m_nIndex);
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVFilterPad::ToString()
{
	if (m_pPointer)
	{
		String^ _name;
		_name = name;
		if (!String::IsNullOrEmpty(_name)) return _name;
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
// AVFilterLinkEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVFilterContext::AVFilterLinkArray::AVFilterLinkEnumerator::AVFilterLinkEnumerator(AVFilterLinkArray^ _array)
	: m_nIndex(-1)
	, m_pLinkArray(_array)
{
}
FFmpeg::AVFilterContext::AVFilterLinkArray::AVFilterLinkEnumerator::~AVFilterLinkEnumerator()
{
}
bool FFmpeg::AVFilterContext::AVFilterLinkArray::AVFilterLinkEnumerator::MoveNext()
{
	if (m_nIndex < m_pLinkArray->Count)
	{
		m_nIndex++;
		return true;
	}
	return false;
}
FFmpeg::AVFilterLink^ FFmpeg::AVFilterContext::AVFilterLinkArray::AVFilterLinkEnumerator::Current::get()
{
	if (m_nIndex < 0)
	{
		if (!MoveNext()) return nullptr;
	}
	return m_pLinkArray[m_nIndex];
}
void FFmpeg::AVFilterContext::AVFilterLinkArray::AVFilterLinkEnumerator::Reset()
{
	m_nIndex = -1;
}
Object^ FFmpeg::AVFilterContext::AVFilterLinkArray::AVFilterLinkEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVFilterLinkArray
//////////////////////////////////////////////////////
FFmpeg::AVFilterContext::AVFilterLinkArray::AVFilterLinkArray(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
	, m_pnCount(nullptr)
{

}
FFmpeg::AVFilterLink^ FFmpeg::AVFilterContext::AVFilterLinkArray::default::get(int index)
{
	if (index < 0 || index >= Count) throw gcnew ArgumentOutOfRangeException();
	::AVFilterLink ** p = (::AVFilterLink **)m_pPointer;
	return _CreateObject<AVFilterLink>(p[index]);
}
int FFmpeg::AVFilterContext::AVFilterLinkArray::Count::get()
{
	return (m_pnCount == nullptr) ? 0 : (int)*m_pnCount;
}
array<FFmpeg::AVFilterLink^>^ FFmpeg::AVFilterContext::AVFilterLinkArray::ToArray()
{
	List<AVFilterLink^>^ _list = gcnew List<AVFilterLink^>();
	try
	{
		for (int i = 0; i < Count; i++)
		{
			_list->Add(this[i]);
		}
		return _list->Count > 0 ? _list->ToArray() : nullptr;
	}
	finally
	{
		delete _list;
	}
}
System::Collections::IEnumerator^ FFmpeg::AVFilterContext::AVFilterLinkArray::GetEnumerator()
{
	return gcnew AVFilterLinkEnumerator(this);
}
System::Collections::Generic::IEnumerator<FFmpeg::AVFilterLink^>^ FFmpeg::AVFilterContext::AVFilterLinkArray::GetEnumeratorGeneric()
{
	return gcnew AVFilterLinkEnumerator(this);
}
//////////////////////////////////////////////////////
// AVFilterPadEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVFilterContext::AVFilterPadArray::AVFilterPadEnumerator::AVFilterPadEnumerator(AVFilterPadArray^ _array)
	: m_nIndex(-1)
	, m_pArray(_array)
{
}
FFmpeg::AVFilterContext::AVFilterPadArray::AVFilterPadEnumerator::~AVFilterPadEnumerator()
{
}
bool FFmpeg::AVFilterContext::AVFilterPadArray::AVFilterPadEnumerator::MoveNext()
{
	if (m_nIndex < m_pArray->Count)
	{
		m_nIndex++;
		return true;
	}
	return false;
}
FFmpeg::AVFilterPad^ FFmpeg::AVFilterContext::AVFilterPadArray::AVFilterPadEnumerator::Current::get()
{
	if (m_nIndex < 0)
	{
		if (!MoveNext()) return nullptr;
	}
	return m_pArray[m_nIndex];
}
void FFmpeg::AVFilterContext::AVFilterPadArray::AVFilterPadEnumerator::Reset()
{
	m_nIndex = -1;
}
Object^ FFmpeg::AVFilterContext::AVFilterPadArray::AVFilterPadEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVFilterPadArray
//////////////////////////////////////////////////////
FFmpeg::AVFilterContext::AVFilterPadArray::AVFilterPadArray(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
	, m_pnCount(nullptr)
{

}
FFmpeg::AVFilterPad^ FFmpeg::AVFilterContext::AVFilterPadArray::default::get(int index)
{
	if (index < 0 || index >= Count) throw gcnew ArgumentOutOfRangeException();
	::AVFilterPad ** p = (::AVFilterPad**)m_pPointer;
	return _CreateObject<AVFilterPad>(p[index]);
}
int FFmpeg::AVFilterContext::AVFilterPadArray::Count::get()
{
	return (m_pnCount == nullptr) ? 0 : (int)*m_pnCount;
}
array<FFmpeg::AVFilterPad^>^ FFmpeg::AVFilterContext::AVFilterPadArray::ToArray()
{
	List<AVFilterPad^>^ _list = gcnew List<AVFilterPad^>();
	try
	{
		for (int i = 0; i < Count; i++)
		{
			_list->Add(this[i]);
		}
		return _list->Count > 0 ? _list->ToArray() : nullptr;
	}
	finally
	{
		delete _list;
	}
}
System::Collections::IEnumerator^ FFmpeg::AVFilterContext::AVFilterPadArray::GetEnumerator()
{
	return gcnew AVFilterPadEnumerator(this);
}
System::Collections::Generic::IEnumerator<FFmpeg::AVFilterPad^>^ FFmpeg::AVFilterContext::AVFilterPadArray::GetEnumeratorGeneric()
{
	return gcnew AVFilterPadEnumerator(this);
}
//////////////////////////////////////////////////////
// AVFilterContext
//////////////////////////////////////////////////////
FFmpeg::AVFilterContext::AVFilterContext(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVFilterContext::av_class::get()
{
	return _CreateObject<AVClass>((void*)((::AVFilterContext*)m_pPointer)->av_class);
}
FFmpeg::AVFilter^ FFmpeg::AVFilterContext::filter::get()
{
	return _CreateObject<AVFilter>((void*)((::AVFilterContext*)m_pPointer)->filter);
}
String^ FFmpeg::AVFilterContext::name::get()
{
	auto p = ((::AVFilterContext*)m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}
FFmpeg::AVFilterContext::AVFilterPadArray^ FFmpeg::AVFilterContext::input_pads::get()
{
	AVFilterPadArray^ _array = _CreateObject<AVFilterPadArray>((void*)((::AVFilterContext*)m_pPointer)->input_pads);
	if (_array)
	{
		_array->m_pnCount = &((::AVFilterContext*)m_pPointer)->nb_inputs;
	}
	return _array;
}
FFmpeg::AVFilterContext::AVFilterLinkArray^ FFmpeg::AVFilterContext::inputs::get()
{
	AVFilterLinkArray^ _array = _CreateObject<AVFilterLinkArray>((void*)((::AVFilterContext*)m_pPointer)->inputs);
	if (_array)
	{
		_array->m_pnCount = &((::AVFilterContext*)m_pPointer)->nb_inputs;
	}
	return _array;
}
FFmpeg::AVFilterContext::AVFilterPadArray^ FFmpeg::AVFilterContext::output_pads::get()
{
	AVFilterPadArray^ _array = _CreateObject<AVFilterPadArray>((void*)((::AVFilterContext*)m_pPointer)->output_pads);
	if (_array)
	{
		_array->m_pnCount = &((::AVFilterContext*)m_pPointer)->nb_outputs;
	}
	return _array;
}
FFmpeg::AVFilterContext::AVFilterLinkArray^ FFmpeg::AVFilterContext::outputs::get()
{
	AVFilterLinkArray^ _array = _CreateObject<AVFilterLinkArray>((void*)((::AVFilterContext*)m_pPointer)->outputs);
	if (_array)
	{
		_array->m_pnCount = &((::AVFilterContext*)m_pPointer)->nb_outputs;
	}
	return _array;
}
FFmpeg::AVFilterGraph^ FFmpeg::AVFilterContext::graph::get()
{
	return _CreateObject<AVFilterGraph>((void*)((::AVFilterContext*)m_pPointer)->graph);
}
FFmpeg::AVFilterThreadType FFmpeg::AVFilterContext::thread_type::get()
{
	return (AVFilterThreadType)((::AVFilterContext*)m_pPointer)->thread_type;
}
void FFmpeg::AVFilterContext::thread_type::set(AVFilterThreadType value)
{
	((::AVFilterContext*)m_pPointer)->thread_type = (int)value;
}

String^ FFmpeg::AVFilterContext::enable_str::get()
{
	auto p = ((::AVFilterContext*)m_pPointer)->enable_str;
	return p != nullptr ? gcnew String(p) : nullptr;
}

void FFmpeg::AVFilterContext::enable_str::set(String^ value)
{
	char* szValue = nullptr;
	try
	{
		szValue = (char*)AllocString(value).ToPointer();
		avfilter_process_command((::AVFilterContext*)m_pPointer,"enable",szValue,nullptr,0,0);
	}
	finally
	{
		FreeMemory((IntPtr)szValue);
	}
}

bool FFmpeg::AVFilterContext::is_disabled::get()
{
	return (((::AVFilterContext*)m_pPointer)->is_disabled != 0);
}

void FFmpeg::AVFilterContext::is_disabled::set(bool value)
{
	((::AVFilterContext*)m_pPointer)->is_disabled = value ? 1 : 0;
}
FFmpeg::AVBufferRef^ FFmpeg::AVFilterContext::hw_device_ctx::get()
{
	return _CreateObject<AVBufferRef>(((::AVFilterContext*)m_pPointer)->hw_device_ctx);
}
int FFmpeg::AVFilterContext::nb_threads::get()
{
	return ((::AVFilterContext*)m_pPointer)->nb_threads;
}
void FFmpeg::AVFilterContext::nb_threads::set(int value)
{
	((::AVFilterContext*)m_pPointer)->nb_threads = value;
}
bool FFmpeg::AVFilterContext::ready::get()
{ 
	return (((::AVFilterContext*)m_pPointer)->ready != 0);
}
int FFmpeg::AVFilterContext::extra_hw_frames::get()
{
	return ((::AVFilterContext*)m_pPointer)->extra_hw_frames;
}
void FFmpeg::AVFilterContext::extra_hw_frames::set(int value)
{
	((::AVFilterContext*)m_pPointer)->extra_hw_frames = value;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVFilterContext::ToString()
{
	if (m_pPointer)
	{
		String^ _name;
		_name = name;
		if (!String::IsNullOrEmpty(_name)) return _name;
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVFilterContext::ConfigLinks()
{
	return avfilter_config_links((::AVFilterContext*)m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::AVFilterContext::ProcessCommand(String^ cmd,String^ arg, IntPtr res, int res_len, AVFilterCmdFlags flags)
{
	char* szCmd = nullptr;
	char* szArgs = nullptr;
	try
	{
		szCmd = (char*)AllocString(cmd).ToPointer();
		szArgs = (char*)AllocString(arg).ToPointer();
		return avfilter_process_command((::AVFilterContext*)m_pPointer,szCmd,szArgs,(char*)res.ToPointer(),res_len,(int)flags);
	}
	finally
	{
		FreeMemory((IntPtr)szCmd);
		FreeMemory((IntPtr)szArgs);
	}
}

FFmpeg::AVRESULT FFmpeg::AVFilterContext::Init(String^ args)
{
	char* szArgs = nullptr;
	try
	{
		szArgs = (char*)AllocString(args).ToPointer();
		return avfilter_init_str((::AVFilterContext*)m_pPointer,szArgs);
	}
	finally
	{
		FreeMemory((IntPtr)szArgs);
	}
}

FFmpeg::AVRESULT FFmpeg::AVFilterContext::Init()
{
	return avfilter_init_str((::AVFilterContext*)m_pPointer,nullptr);
}

FFmpeg::AVRESULT FFmpeg::AVFilterContext::Init(AVDictionary^ options)
{
	int _result = 0;
	::AVDictionary * _dict = options != nullptr ? (::AVDictionary *)options->_Pointer.ToPointer() : nullptr;
	try
	{
		_result = avfilter_init_dict((::AVFilterContext*)m_pPointer,&_dict);
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

FFmpeg::AVRESULT FFmpeg::AVFilterContext::Link(AVFilterContext^ dst,int srcpad, int dstpad)
{
	return avfilter_link((::AVFilterContext*)m_pPointer,srcpad,(::AVFilterContext*)dst->_Pointer.ToPointer(),dstpad);
}
FFmpeg::AVRESULT FFmpeg::AVFilterContext::Link(AVFilterContext^ dst)
{
	return avfilter_link((::AVFilterContext*)m_pPointer,0,(::AVFilterContext*)dst->_Pointer.ToPointer(),0);
}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVFilterContext::GetClass()
{
	auto p = avfilter_get_class();
	return p != nullptr ? gcnew AVClass((void*)p, nullptr) : nullptr;
}
//////////////////////////////////////////////////////
// AVFilterLink
//////////////////////////////////////////////////////
FFmpeg::AVFilterLink::AVFilterLink(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
FFmpeg::AVFilterContext^ FFmpeg::AVFilterLink::src::get()
{
	return _CreateObject<AVFilterContext>(((::AVFilterLink*)m_pPointer)->src);
}
FFmpeg::AVFilterPad^ FFmpeg::AVFilterLink::srcpad::get()
{
	return _CreateObject<AVFilterPad>(((::AVFilterLink*)m_pPointer)->srcpad);
}
FFmpeg::AVFilterContext^ FFmpeg::AVFilterLink::dst::get()
{
	return _CreateObject<AVFilterContext>(((::AVFilterLink*)m_pPointer)->dst);
}
FFmpeg::AVFilterPad^ FFmpeg::AVFilterLink::dstpad::get()
{
	return _CreateObject<AVFilterPad>(((::AVFilterLink*)m_pPointer)->dstpad);
}
FFmpeg::AVMediaType FFmpeg::AVFilterLink::type::get()
{
	return (AVMediaType)((::AVFilterLink*)m_pPointer)->type;
}
int FFmpeg::AVFilterLink::w::get()
{
	return ((::AVFilterLink*)m_pPointer)->w;
}
int FFmpeg::AVFilterLink::h::get()
{
	return ((::AVFilterLink*)m_pPointer)->h;
}
FFmpeg::AVRational^ FFmpeg::AVFilterLink::sample_aspect_ratio::get()
{
	return _CreateObject<AVRational>(&((::AVFilterLink*)m_pPointer)->sample_aspect_ratio);
}
FFmpeg::AVChannelLayout FFmpeg::AVFilterLink::channel_layout::get()
{
	return (AVChannelLayout)((::AVFilterLink*)m_pPointer)->channel_layout;
}
int FFmpeg::AVFilterLink::sample_rate::get()
{
	return ((::AVFilterLink*)m_pPointer)->sample_rate;
}
int FFmpeg::AVFilterLink::format::get()
{
	return ((::AVFilterLink*)m_pPointer)->format;
}
FFmpeg::AVRational^ FFmpeg::AVFilterLink::time_base::get()
{
	return _CreateObject<AVRational>(&((::AVFilterLink*)m_pPointer)->time_base);
}

int FFmpeg::AVFilterLink::channels::get()
{
	int channels = 0;
#if FF_API_OLD_CHANNEL_LAYOUT
	channels = av_get_channel_layout_nb_channels(((::AVFilterLink*)m_pPointer)->channel_layout);
#else
#if (LIBAVFILTER_VERSION_MAJOR * 100 + LIBAVFILTER_VERSION_MINOR > 801)
	channels = ((::AVFilterLink*)m_pPointer)->ch_layout.nb_channels;
#	else 
	channels = ((::AVFilterLink*)m_pPointer)->channels;
#	endif
#endif
	DYNAMIC_DEF_API(AVFilter,int,channels,avfilter_link_get_channels,::AVFilterLink * )
	return avfilter_link_get_channels((::AVFilterLink*)m_pPointer);
}

void FFmpeg::AVFilterLink::closed::set(int value)
{
	VOID_API(AVFilter,avfilter_link_set_closed,::AVFilterLink *, int )
	avfilter_link_set_closed((::AVFilterLink*)m_pPointer, value);
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVFilterLink::InsertFilter(AVFilterContext^ filt,int filt_srcpad_idx, int filt_dstpad_idx)
{
	return avfilter_insert_filter((::AVFilterLink*)m_pPointer,(::AVFilterContext*)filt->_Pointer.ToPointer(),filt_srcpad_idx, filt_dstpad_idx);
}
//////////////////////////////////////////////////////
// AVFilterInOut
//////////////////////////////////////////////////////
FFmpeg::AVFilterInOut::AVFilterInOut(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
FFmpeg::AVFilterInOut::AVFilterInOut()
	: AVBase(nullptr,nullptr)
{
	LibAVFilter::RegisterAll();
	m_pPointer = avfilter_inout_alloc();
	m_pFreep = (TFreeFNP*)avfilter_inout_free;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVFilterInOut::name::get()
{
	auto p = ((::AVFilterInOut*)m_pPointer)->name;
	return p != nullptr ? gcnew String(p) : nullptr;
}
void FFmpeg::AVFilterInOut::name::set(String^ s)
{
	char* szName = nullptr;
	try
	{
		szName = (char*)AllocString(s).ToPointer();
		av_freep(&((::AVFilterInOut*)m_pPointer)->name);
		((::AVFilterInOut*)m_pPointer)->name = av_strdup(szName);
	}
	finally
	{
		FreeMemory((IntPtr)szName);
	}
}

FFmpeg::AVFilterContext^ FFmpeg::AVFilterInOut::filter_ctx::get()
{
	return _CreateObject<AVFilterContext>(((::AVFilterInOut*)m_pPointer)->filter_ctx);
}
void FFmpeg::AVFilterInOut::filter_ctx::set(AVFilterContext^ ctx)
{
	AddObject(ctx);
	((::AVFilterInOut*)m_pPointer)->filter_ctx = ctx != nullptr ? (::AVFilterContext*)ctx->_Pointer.ToPointer() : nullptr;
}
int FFmpeg::AVFilterInOut::pad_idx::get()
{
	return ((::AVFilterInOut*)m_pPointer)->pad_idx;
}
void FFmpeg::AVFilterInOut::pad_idx::set(int idx)
{
	((::AVFilterInOut*)m_pPointer)->pad_idx = idx;
}

FFmpeg::AVFilterInOut^ FFmpeg::AVFilterInOut::next::get()
{
	return _CreateObject<AVFilterInOut>(((::AVFilterInOut*)m_pPointer)->next);
}
void FFmpeg::AVFilterInOut::next::set(AVFilterInOut^ p)
{
	AddObject(p);
	((::AVFilterInOut*)m_pPointer)->next = p != nullptr ? (::AVFilterInOut*)p->_Pointer.ToPointer() : nullptr;
}
//////////////////////////////////////////////////////
// AVFiltersEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVFilterGraph::AVFiltersArray::AVFiltersEnumerator::AVFiltersEnumerator(AVFiltersArray^ _array)
	: m_nIndex(-1)
	, m_pArray(_array)
{
}
FFmpeg::AVFilterGraph::AVFiltersArray::AVFiltersEnumerator::~AVFiltersEnumerator()
{
}
bool FFmpeg::AVFilterGraph::AVFiltersArray::AVFiltersEnumerator::MoveNext()
{
	if (m_nIndex < m_pArray->Count - 1)
	{
		m_nIndex++;
		return true;
	}
	return false;
}
FFmpeg::AVFilterContext^ FFmpeg::AVFilterGraph::AVFiltersArray::AVFiltersEnumerator::Current::get()
{
	if (m_nIndex < 0)
	{
		if (!MoveNext()) return nullptr;
	}
	return m_pArray[m_nIndex];
}
void FFmpeg::AVFilterGraph::AVFiltersArray::AVFiltersEnumerator::Reset()
{
	m_nIndex = -1;
}
Object^ FFmpeg::AVFilterGraph::AVFiltersArray::AVFiltersEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVFiltersArray
//////////////////////////////////////////////////////
FFmpeg::AVFilterGraph::AVFiltersArray::AVFiltersArray(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
	, m_pnCount(nullptr)
{

}
FFmpeg::AVFilterContext^ FFmpeg::AVFilterGraph::AVFiltersArray::default::get(int index)
{
	if (index < 0 || index > Count) throw gcnew ArgumentOutOfRangeException();
	::AVFilterContext ** p = (::AVFilterContext **)m_pPointer;
	return _CreateObject<AVFilterContext>(p[index]);
}
int FFmpeg::AVFilterGraph::AVFiltersArray::Count::get()
{
	return (m_pnCount == nullptr) ? 0 : (int)*m_pnCount;
}
array<FFmpeg::AVFilterContext^>^ FFmpeg::AVFilterGraph::AVFiltersArray::ToArray()
{
	List<AVFilterContext^>^ _list = gcnew List<AVFilterContext^>();
	try
	{
		for (int i = 0; i < Count; i++)
		{
			_list->Add(this[i]);
		}
		return _list->Count > 0 ? _list->ToArray() : nullptr;
	}
	finally
	{
		delete _list;
	}
}
System::Collections::IEnumerator^ FFmpeg::AVFilterGraph::AVFiltersArray::GetEnumerator()
{
	return gcnew AVFiltersEnumerator(this);
}
System::Collections::Generic::IEnumerator<FFmpeg::AVFilterContext^>^ FFmpeg::AVFilterGraph::AVFiltersArray::GetEnumeratorGeneric()
{
	return gcnew AVFiltersEnumerator(this);
}
//////////////////////////////////////////////////////
// AVFilterGraph
//////////////////////////////////////////////////////
ref class AVDelegate
{
internal:
	::avfilter_action_func * m_pFunc;
internal:
	AVDelegate(avfilter_action_func * func) {m_pFunc = func;}
};
//////////////////////////////////////////////////////
int avfilter_action_handler(FFmpeg::AVFilterContext^ ctx, Object^ target, IntPtr arg, int jobnr, int nb_jobs)
{
	if (target)
	{
		return ((AVDelegate^)target)->m_pFunc((::AVFilterContext*)ctx->_Pointer.ToPointer(),arg.ToPointer(),jobnr, nb_jobs);
	}
	return AVERROR_INVALIDDATA;
}
//////////////////////////////////////////////////////
int avfiltergraph_handler_execute(::AVFilterContext *ctx, avfilter_action_func *func, void *arg, int *ret, int nb_jobs)
{
	if (ctx && ctx->graph && ctx->graph->opaque)
	{
		GCHandle _handle = GCHandle::FromIntPtr((IntPtr)ctx->graph->opaque);
		if (_handle.IsAllocated)
		{
			FFmpeg::AVFilterGraph^ graph = ((FFmpeg::AVFilterGraph^)_handle.Target);
			FFmpeg::AVFilterContext^ c = FFmpeg::AVBase::_CreateChildObject<FFmpeg::AVFilterContext>(ctx,graph);
			int r = ret ? *ret : 0;
			try
			{
				return graph->execute(c, graph->m_pAction, gcnew AVDelegate(func), (IntPtr)arg, r, nb_jobs);
			}
			finally
			{
				if (ret) *ret = r;
			}
		}
	}
	return 0;
}
//////////////////////////////////////////////////////
FFmpeg::AVFilterGraph::AVFilterGraph(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
	, m_pExecuteNative(IntPtr::Zero)
	, m_pOpaque(IntPtr::Zero)
	, m_pExecute(nullptr)
{
	if (m_pPointer)
	{
		m_pExecuteNative = (IntPtr)((::AVFilterGraph*)m_pPointer)->execute;
	}
}

FFmpeg::AVFilterGraph::AVFilterGraph()
	: AVBase(nullptr,nullptr)
	, m_pExecuteNative(IntPtr::Zero)
	, m_pOpaque(IntPtr::Zero)
	, m_pExecute(nullptr)
{
	LibAVFilter::RegisterAll();
	m_pPointer = avfilter_graph_alloc();
	m_pFreep = (TFreeFNP*)avfilter_graph_free;
	if (m_pPointer)
	{
		m_pExecuteNative = (IntPtr)((::AVFilterGraph*)m_pPointer)->execute;
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVFilterGraph::av_class::get()
{
	return _CreateObject<AVClass>((void*)((::AVFilterGraph*)m_pPointer)->av_class);
}
FFmpeg::AVFilterGraph::AVFiltersArray^ FFmpeg::AVFilterGraph::filters::get()
{
	AVFiltersArray^ _array = _CreateObject<AVFiltersArray>((void*)((::AVFilterGraph*)m_pPointer)->filters);
	if (_array)
	{
		_array->m_pnCount = &((::AVFilterGraph*)m_pPointer)->nb_filters;
	}
	return _array;
}
String^ FFmpeg::AVFilterGraph::scale_sws_opts::get()
{
	auto p = ((::AVFilterGraph*)m_pPointer)->scale_sws_opts;
	return p != nullptr ? gcnew String(p) : nullptr;
}

void FFmpeg::AVFilterGraph::scale_sws_opts::set(String^ value)
{
	auto p = _CreateObject<AVOptions>(m_pPointer);
	p->set("scale_sws_opts",value,AVOptSearch::None);
}
String^ FFmpeg::AVFilterGraph::resample_lavr_opts::get()
{
	String^ opt;
	auto p = _CreateObject<AVOptions>(m_pPointer);
	p->get("aresample_swr_opts",AVOptSearch::None,opt);
	return opt;
}
void FFmpeg::AVFilterGraph::resample_lavr_opts::set(String^ value)
{
	auto p = _CreateObject<AVOptions>(m_pPointer);
	p->set("aresample_swr_opts",value,AVOptSearch::None);
}
FFmpeg::AVFilterThreadType FFmpeg::AVFilterGraph::thread_type::get()
{
	return (AVFilterThreadType)((::AVFilterGraph*)m_pPointer)->thread_type;
}
void FFmpeg::AVFilterGraph::thread_type::set(AVFilterThreadType value)
{
	((::AVFilterGraph*)m_pPointer)->thread_type = (int)value;
}
int FFmpeg::AVFilterGraph::nb_threads::get()
{
	return ((::AVFilterGraph*)m_pPointer)->nb_threads;
}
void FFmpeg::AVFilterGraph::nb_threads::set(int value)
{
	((::AVFilterGraph*)m_pPointer)->nb_threads = value;
}
IntPtr FFmpeg::AVFilterGraph::opaque::get()
{
	Threading::Monitor::Enter(this);
	try
	{
		if (m_pExecute)
		{
			return m_pOpaque;
		}
		else
		{
			return (IntPtr)((::AVFilterGraph*)m_pPointer)->opaque;
		}
	}
	finally
	{
		Threading::Monitor::Exit(this);
	}
}
void FFmpeg::AVFilterGraph::opaque::set(IntPtr value)
{
	Threading::Monitor::Enter(this);
	try
	{
		if (m_pExecute)
		{
			m_pOpaque = value;
		}
		else
		{
			((::AVFilterGraph*)m_pPointer)->opaque = value.ToPointer();
		}
	}
	finally
	{
		Threading::Monitor::Exit(this);
	}
}
FFmpeg::AVFilterExecuteDelegate^ FFmpeg::AVFilterGraph::execute::get()
{
	Threading::Monitor::Enter(this);
	try
	{
		return m_pExecute;
	}
	finally
	{
		Threading::Monitor::Exit(this);
	}
}
void FFmpeg::AVFilterGraph::execute::set(AVFilterExecuteDelegate^ value)
{
	if (m_pExecute != value)
	{
		Threading::Monitor::Enter(this);
		try
		{
			if (value == nullptr)
			{
				((::AVFilterGraph *)m_pPointer)->opaque = m_pOpaque.ToPointer();
				((::AVFilterGraph *)m_pPointer)->execute = (avfilter_execute_func*)m_pExecuteNative.ToPointer();
			}
			else
			{
				if (m_pAction == nullptr)
				{
					m_pAction = gcnew AVFilterActionDelegate(avfilter_action_handler);
				}
				if (m_pExecute == nullptr)
				{
					m_pExecuteNative = (IntPtr)((::AVFilterGraph *)m_pPointer)->execute;
					m_pOpaque = (IntPtr)((::AVFilterGraph *)m_pPointer)->opaque;
					if (!m_ThisPtr.IsAllocated)
					{
						m_ThisPtr = GCHandle::Alloc(this, GCHandleType::Weak);
					}
					((::AVFilterGraph *)m_pPointer)->opaque = (void*)GCHandle::ToIntPtr(m_ThisPtr);
				}
				((::AVFilterGraph *)m_pPointer)->execute = avfiltergraph_handler_execute;
			}
			m_pExecute = value;
		}
		finally
		{
			Threading::Monitor::Exit(this);
		}
	}
}
String^ FFmpeg::AVFilterGraph::aresample_swr_opts::get()
{
	auto p = ((::AVFilterGraph*)m_pPointer)->aresample_swr_opts;
	return p != nullptr ? gcnew String(p) : nullptr;
}

void FFmpeg::AVFilterGraph::aresample_swr_opts::set(String^ value)
{
	auto p = _CreateObject<AVOptions>(m_pPointer);
	p->set("aresample_swr_opts",value,AVOptSearch::None);
}

FFmpeg::AVFilterAutoConvert FFmpeg::AVFilterGraph::flags::get()
{
	return (AVFilterAutoConvert)((::AVFilterGraph*)m_pPointer)->disable_auto_convert;
}

void FFmpeg::AVFilterGraph::flags::set(AVFilterAutoConvert value)
{
	avfilter_graph_set_auto_convert((::AVFilterGraph*)m_pPointer,(unsigned int)value);
}
//////////////////////////////////////////////////////
FFmpeg::AVFilterContext^ FFmpeg::AVFilterGraph::AllocFilter(AVFilter^ filter, String^ name)
{
	char* szName = nullptr;
	try
	{
		szName = (char*)AllocString(name).ToPointer();
		auto p = avfilter_graph_alloc_filter(((::AVFilterGraph*)m_pPointer), (const ::AVFilter*)filter->_Pointer.ToPointer(), szName);
		AVFilterContext^ context = _CreateObject<AVFilterContext>(p);
		if (context != nullptr)
		{
			context->m_pFree = (TFreeFN*)avfilter_free;
		}
		return context;
	}
	finally
	{
		FreeMemory((IntPtr)szName);
	}
}

FFmpeg::AVFilterContext^ FFmpeg::AVFilterGraph::CreateFilter(AVFilter^ filter, String^ name)
{
	AVFilterContext^ context = nullptr;
	CreateFilter(context,filter,name,nullptr,IntPtr::Zero);
	return context;
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::CreateFilter(AVFilterContext^ % context, AVFilter^ filter, String^ name, String^ args)
{
	return CreateFilter(context,filter,name,args,IntPtr::Zero);
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::CreateFilter(AVFilterContext^ % context,AVFilter^ filter,String^ name)
{
	return CreateFilter(context,filter,name,nullptr,IntPtr::Zero);
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::CreateFilter(AVFilterContext^ % context, AVFilter^ filter, String^ name, String^ args, IntPtr opaque)
{
	int result = -1;
	char* szName = nullptr;
	char* szArgs = nullptr;
	try
	{
		::AVFilterContext * p = nullptr;
		szName = (char*)AllocString(name).ToPointer();
		szArgs = (char*)AllocString(args).ToPointer();
		result = avfilter_graph_create_filter(&p,(const ::AVFilter*)filter->_Pointer.ToPointer(),szName,szArgs,opaque.ToPointer(),((::AVFilterGraph*)m_pPointer));
		context = _CreateObject<AVFilterContext>(p);
	}
	finally
	{
		FreeMemory((IntPtr)szArgs);
		FreeMemory((IntPtr)szName);
	}
	if (context != nullptr)
	{
		context->m_pFree = (TFreeFN*)avfilter_free;
	}
	return result;
}

FFmpeg::AVFilterContext^ FFmpeg::AVFilterGraph::GetFilter(String^ name)
{
	AVFilterContext^ _context = nullptr;
	char* szName = nullptr;
	try
	{
		szName = (char*)AllocString(name).ToPointer();
		auto p = avfilter_graph_get_filter(((::AVFilterGraph*)m_pPointer),szName);
		_context = _CreateObject<AVFilterContext>(p);
	}
	finally
	{
		FreeMemory((IntPtr)szName);
	}
	return _context;
}

FFmpeg::AVFilterContext^ FFmpeg::AVFilterGraph::CreateFilter(AVFilter^ filter,String^ name,String^ args, IntPtr opaque)
{
	AVFilterContext^ _context = nullptr;
	char* szName = nullptr;
	char* szArgs = nullptr;
	try
	{
		szName = (char*)AllocString(name).ToPointer();
		szArgs = (char*)AllocString(args).ToPointer();
		::AVFilterContext * p = nullptr;
		if (0 <= avfilter_graph_create_filter(&p,(const ::AVFilter*)filter->_Pointer.ToPointer(),szName,szArgs,opaque.ToPointer(),((::AVFilterGraph*)m_pPointer)))
		{
			_context = _CreateObject<AVFilterContext>(p);
		}
		else
		{
			if (p) avfilter_free(p);
		}
	}
	finally
	{
		FreeMemory((IntPtr)szName);
		FreeMemory((IntPtr)szArgs);
	}
	if (_context != nullptr)
	{
		_context->m_pFree = (TFreeFN*)avfilter_free;
	}
	return _context;
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::Config()
{
	return Config(IntPtr::Zero);
}
FFmpeg::AVRESULT FFmpeg::AVFilterGraph::Config(IntPtr log_ctx)
{
	return avfilter_graph_config((::AVFilterGraph*)m_pPointer,log_ctx.ToPointer());
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::Parse(String^ filters, AVFilterInOut^ inputs, AVFilterInOut^ outputs)
{
	return 	Parse(filters, inputs, outputs,IntPtr::Zero);
}
FFmpeg::AVRESULT FFmpeg::AVFilterGraph::Parse(String^ filters,AVFilterInOut^ inputs,AVFilterInOut^ outputs,IntPtr log_ctx)
{
	char* szFilters = nullptr;
	try
	{
		szFilters = (char*)AllocString(filters).ToPointer();
		return  avfilter_graph_parse(((::AVFilterGraph*)m_pPointer),szFilters,inputs != nullptr ? (::AVFilterInOut*)inputs->_Pointer.ToPointer() : nullptr,outputs != nullptr ? (::AVFilterInOut*)outputs->_Pointer.ToPointer() : nullptr,log_ctx.ToPointer());
	}
	finally
	{
		FreeMemory((IntPtr)szFilters);
	}
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::ParsePtr(String^ filters,AVFilterInOut^ inputs,AVFilterInOut^ outputs)
{
	return ParsePtr(filters,inputs,outputs,IntPtr::Zero);
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::ParsePtr(String^ filters,AVFilterInOut^ inputs,AVFilterInOut^ outputs,IntPtr log_ctx)
{
	int _result = 0;
	char* szFilters = nullptr;
	::AVFilterInOut * _inputs = inputs != nullptr ? (::AVFilterInOut *)inputs->_Pointer.ToPointer() : nullptr;
	::AVFilterInOut * _outputs = outputs != nullptr ? (::AVFilterInOut *)outputs->_Pointer.ToPointer() : nullptr;
	try
	{
		szFilters = (char*)AllocString(filters).ToPointer();
		_result = avfilter_graph_parse_ptr(((::AVFilterGraph*)m_pPointer),szFilters,&_inputs,&_outputs,log_ctx.ToPointer());
		if (_result >= 0)
		{
			if (inputs != nullptr) 
			{
				inputs->ChangePointer(_inputs);
				inputs->m_pFreep = (TFreeFNP*)avfilter_inout_free;
				_inputs = nullptr;
			}
			if (outputs != nullptr) 
			{
				outputs->ChangePointer(_outputs);
				outputs->m_pFreep = (TFreeFNP*)avfilter_inout_free;
				_outputs = nullptr;
			}
		}
	}
	finally
	{
		FreeMemory((IntPtr)szFilters);
		if (_inputs && inputs == nullptr) avfilter_inout_free(&_inputs);
		if (_outputs && outputs == nullptr) avfilter_inout_free(&_outputs);
	}
	return _result;
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::ParsePtr2(String^ filters, [Out] AVFilterInOut^ % inputs, [Out] AVFilterInOut^ % outputs)
{
	return ParsePtr2(filters, inputs, outputs,IntPtr::Zero);
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::ParsePtr2(String^ filters, [Out] AVFilterInOut^ % inputs, [Out] AVFilterInOut^ % outputs, IntPtr log_ctx)
{
	int _result = 0;
	char* szFilters = nullptr;
	::AVFilterInOut * _inputs = nullptr;
	::AVFilterInOut * _outputs = nullptr;
	try
	{
		szFilters = (char*)AllocString(filters).ToPointer();
		_result = avfilter_graph_parse_ptr(((::AVFilterGraph*)m_pPointer),szFilters,&_inputs,&_outputs,log_ctx.ToPointer());
		if (_result == 0)
		{
			if (_inputs) 
			{
				inputs = _CreateObject<AVFilterInOut>(_inputs);
				inputs->m_pFreep = (TFreeFNP*)avfilter_inout_free;
				_inputs = nullptr;
			}
			if (_outputs) 
			{
				outputs = _CreateObject<AVFilterInOut>(_outputs);
				outputs->m_pFreep = (TFreeFNP*)avfilter_inout_free;
				_outputs = nullptr;
			}
		}
	}
	finally
	{
		FreeMemory((IntPtr)szFilters);
		if (_inputs) avfilter_inout_free(&_inputs);
		if (_outputs) avfilter_inout_free(&_outputs);
	}
	return _result;
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::Parse2(String^ filters,[Out] AVFilterInOut^ % inputs,[Out] AVFilterInOut^ % outputs)
{
	int _result = 0;
	char* szFilters = nullptr;
	::AVFilterInOut * _inputs = nullptr;
	::AVFilterInOut * _outputs = nullptr;
	try
	{
		szFilters = (char*)AllocString(filters).ToPointer();
		_result = avfilter_graph_parse2(((::AVFilterGraph*)m_pPointer),szFilters,&_inputs,&_outputs);
		if (_result == 0)
		{
			if (_inputs) 
			{
				inputs = _CreateObject<AVFilterInOut>(_inputs);
				inputs->m_pFreep = (TFreeFNP*)avfilter_inout_free;
				_inputs = nullptr;
			}
			if (_outputs) 
			{
				outputs = _CreateObject<AVFilterInOut>(_outputs);
				outputs->m_pFreep = (TFreeFNP*)avfilter_inout_free;
				_outputs = nullptr;
			}
		}
	}
	finally
	{
		FreeMemory((IntPtr)szFilters);
		if (_inputs) avfilter_inout_free(&_inputs);
		if (_outputs) avfilter_inout_free(&_outputs);
	}
	return _result;
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::SendCommand(String^ target,String^ cmd,String^ arg, IntPtr res, int res_len, AVFilterCmdFlags flags)
{
	char* szCmd = nullptr;
	char* szArgs = nullptr;
	char* szTarget = nullptr;
	try
	{
		szTarget = (char*)AllocString(target).ToPointer();
		szCmd = (char*)AllocString(cmd).ToPointer();
		szArgs = (char*)AllocString(arg).ToPointer();
		return avfilter_graph_send_command((::AVFilterGraph*)m_pPointer,szTarget,szCmd,szArgs,(char*)res.ToPointer(),res_len,(int)flags);
	}
	finally
	{
		FreeMemory((IntPtr)szCmd);
		FreeMemory((IntPtr)szTarget);
		FreeMemory((IntPtr)szArgs);
	}
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::QueueCommand(String^ target,String^ cmd,String^ arg, AVFilterCmdFlags flags, double ts)
{
	char* szCmd = nullptr;
	char* szArgs = nullptr;
	char* szTarget = nullptr;
	try
	{
		szTarget = (char*)AllocString(target).ToPointer();
		szCmd = (char*)AllocString(cmd).ToPointer();
		szArgs = (char*)AllocString(arg).ToPointer();
		return avfilter_graph_queue_command((::AVFilterGraph*)m_pPointer,szTarget,szCmd,szArgs,(int)flags,ts);
	}
	finally
	{
		FreeMemory((IntPtr)szCmd);
		FreeMemory((IntPtr)szTarget);
		FreeMemory((IntPtr)szArgs);
	}
}

String^ FFmpeg::AVFilterGraph::Dump()
{
	return Dump(nullptr);
}

String^ FFmpeg::AVFilterGraph::Dump(String^ options)
{
	char* szOptions = nullptr;
	char* p = nullptr;
	try
	{
		szOptions = (char*)AllocString(options).ToPointer();
		p = avfilter_graph_dump((::AVFilterGraph*)m_pPointer,szOptions);
		return p != nullptr ? gcnew String(p) : nullptr;
	}
	finally
	{
		if (p) av_free(p);
		FreeMemory((IntPtr)szOptions);
	}
}

FFmpeg::AVRESULT FFmpeg::AVFilterGraph::RequestOldest()
{
	return avfilter_graph_request_oldest((::AVFilterGraph*)m_pPointer);
}
//////////////////////////////////////////////////////
// AVPixelFormatsEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVBufferSinkParams::AVPixelFormats::AVPixelFormatsEnumerator::AVPixelFormatsEnumerator(AVPixelFormats^ _array)
	: m_pFormats(_array)
{
	Reset();
}
FFmpeg::AVBufferSinkParams::AVPixelFormats::AVPixelFormatsEnumerator::~AVPixelFormatsEnumerator()
{
}
bool FFmpeg::AVBufferSinkParams::AVPixelFormats::AVPixelFormatsEnumerator::MoveNext()
{
	int idx = m_nIndex;
	Monitor::Enter(m_pFormats);
	if (m_nVersion == m_pFormats->m_nVersion)
	{
		const ::AVPixelFormat * p = (const ::AVPixelFormat *)m_pFormats->_Pointer.ToPointer();
		if (m_nIndex == -1)
		{
			if (p) m_nIndex++;
		}
		else
		{
			if (p[m_nIndex] != AV_PIX_FMT_NONE)
			{
				m_nIndex++;
			}
		}
	}
	Monitor::Exit(m_pFormats);
	return (idx != m_nIndex);
}
FFmpeg::AVPixelFormat FFmpeg::AVBufferSinkParams::AVPixelFormats::AVPixelFormatsEnumerator::Current::get()
{
	AVPixelFormat fmt = AVPixelFormat::NONE;
	if (m_nIndex == -1)
	{
		if (!MoveNext()) return fmt;
	}
	{
		Monitor::Enter(m_pFormats);
		const ::AVPixelFormat * p = (const ::AVPixelFormat *)m_pFormats->_Pointer.ToPointer();
		if (m_nVersion != m_pFormats->m_nVersion)
		{
			int idx = 0;
			while (idx <= m_nIndex)
			{
				if (p[idx++] == AV_PIX_FMT_NONE)
				{
					break;
				}
			}
			if (idx > m_nIndex)
			{
				fmt = (AVPixelFormat)p[m_nIndex];
			}
		}
		else
		{
			fmt = (AVPixelFormat)p[m_nIndex];
		}
		Monitor::Exit(m_pFormats);
	}
	return fmt;
}
void FFmpeg::AVBufferSinkParams::AVPixelFormats::AVPixelFormatsEnumerator::Reset()
{
	Monitor::Enter(m_pFormats);
	m_nVersion = m_pFormats->m_nVersion;
	Monitor::Exit(m_pFormats);
	m_nIndex = -1;
}
Object^ FFmpeg::AVBufferSinkParams::AVPixelFormats::AVPixelFormatsEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVPixelFormats
//////////////////////////////////////////////////////
FFmpeg::AVBufferSinkParams::AVPixelFormats::AVPixelFormats(void * _pointer, AVBase^ _parent)
	: AVBase(_pointer, _parent)
	, m_bReadOnly(true)
	, m_nVersion(0)
	, m_nAllocated(0)
{
}
//////////////////////////////////////////////////////
FFmpeg::AVPixelFormat FFmpeg::AVBufferSinkParams::AVPixelFormats::default::get(int index)
{
	Monitor::Enter(this);
	try
	{
		auto p = ((::AVPixelFormat *)m_pPointer);
		if (p)
		{
			do
			{
				if (index-- == 0)
				{
					return (AVPixelFormat)*p;
				}
			}
			while (*p++ != AV_PIX_FMT_NONE);
		}
		return AVPixelFormat::NONE;
	}
	finally
	{
		Monitor::Exit(this);
	}
}

int FFmpeg::AVBufferSinkParams::AVPixelFormats::Count::get()
{
	Monitor::Enter(this);
	try
	{
		int count = 0;
		auto p = ((::AVPixelFormat *)m_pPointer);
		if (p)
		{
			while (*p++ != AV_PIX_FMT_NONE) count++;
		}
		return count;
	}
	finally
	{
		Monitor::Exit(this);
	}
}
//////////////////////////////////////////////////////
bool FFmpeg::AVBufferSinkParams::AVPixelFormats::EnsureAllocatedPointer(int count)
{
	Monitor::Enter(this);
	try
	{
		bool bAllocated = true;
		if (m_nAllocated < count)
		{
			int size = 0;
			::AVPixelFormat * old = (::AVPixelFormat *)m_pPointer;
			if (old) { while (old[size] != AV_PIX_FMT_NONE) size++; }

			bool bFree = (m_nAllocated > 0);
			while (m_nAllocated < count) m_nAllocated += 20;
			::AVPixelFormat * p = (::AVPixelFormat *)AllocMemory(m_nAllocated * sizeof(::AVPixelFormat)).ToPointer();
			if (p)
			{
				if (size) memcpy(p,old,size * sizeof(::AVPixelFormat));
				p[size] = AV_PIX_FMT_NONE;
				((::AVBufferSinkParams*)m_pParent->_Pointer.ToPointer())->pixel_fmts = p;
			}
			bAllocated = (p != nullptr);
			if (bFree)
			{
				FreeMemory(IntPtr(old));
			}
		}
		return bAllocated;
	}
	finally
	{
		Monitor::Exit(this);
	}
}
//////////////////////////////////////////////////////
bool FFmpeg::AVBufferSinkParams::AVPixelFormats::Add(array<AVPixelFormat>^ formats)
{
	int added = 0;
	if (formats != nullptr)
	{
		for each(auto format in  formats)
		{
			if (Add(format)) added++;
		}
	}
	return (added > 0);
}
bool FFmpeg::AVBufferSinkParams::AVPixelFormats::Remove(array<AVPixelFormat>^ formats)
{
	int removed = 0;
	if (formats != nullptr)
	{
		for each(auto format in  formats)
		{
			if (Remove(format)) removed++;
		}
	}
	return (removed > 0);
}
bool FFmpeg::AVBufferSinkParams::AVPixelFormats::Add(AVPixelFormat format)
{
	Monitor::Enter(this);
	try
	{
		int count = 0; 
		{
			auto p = ((::AVPixelFormat *)m_pPointer);
			if (p)
			{
				while (*p != AV_PIX_FMT_NONE)
				{
					if (*p++ == (::AVPixelFormat)format)
					{
						return true;
					}
					count++;
				}
			}
		}
		if (EnsureAllocatedPointer(count + 1))
		{
			((::AVPixelFormat *)m_pPointer)[count++] = (::AVPixelFormat)format;
			((::AVPixelFormat *)m_pPointer)[count] = AV_PIX_FMT_NONE;
			m_nVersion++;
			return true;
		}
		return false;
	}
	finally
	{
		Monitor::Exit(this);
	}
}
bool FFmpeg::AVBufferSinkParams::AVPixelFormats::Remove(AVPixelFormat format)
{
	int idx = -1;
	Monitor::Enter(this);
	try
	{
		bool found = false;
		auto p = ((::AVPixelFormat *)m_pPointer);
		if (p)
		{
			while (p[++idx] != AV_PIX_FMT_NONE && !found)
			{
				found = (p[idx] == (::AVPixelFormat)format);
			}
		}
		if (!found)return false;
	}
	finally
	{
		Monitor::Exit(this);
	}
	return RemoveAt(idx);
}
bool FFmpeg::AVBufferSinkParams::AVPixelFormats::Contains(AVPixelFormat format)
{
	Monitor::Enter(this);
	try
	{
		auto p = ((::AVPixelFormat *)m_pPointer);
		if (p)
		{
			while (*p != AV_PIX_FMT_NONE)
			{
				if (*p++ == (::AVPixelFormat)format)
				{
					return true;
				}
			}
		}
		return false;
	}
	finally
	{
		Monitor::Exit(this);
	}
}
bool FFmpeg::AVBufferSinkParams::AVPixelFormats::RemoveAt(int index)
{
	Monitor::Enter(this);
	try
	{
		int count = 0;
		{
			auto p = ((::AVPixelFormat *)m_pPointer);
			if (p)
			{
				while (p[count] != AV_PIX_FMT_NONE) count++;
				if (index >= count) return false;
			}
		}
		if (EnsureAllocatedPointer(m_nAllocated))
		{
			auto p = ((::AVPixelFormat *)m_pPointer);
			for (int i = index; i < count; i++)
			{
				p[i] = p[i+1];
			}
			m_nVersion++;
			return true;
		}
		return false;
	}
	finally
	{
		Monitor::Exit(this);
	}
}
bool FFmpeg::AVBufferSinkParams::AVPixelFormats::RemoveAll()
{
	Monitor::Enter(this);
	try
	{
		{
			auto p = ((::AVPixelFormat *)m_pPointer);
			if (p && *p == AV_PIX_FMT_NONE)
			{
				return true;
			}
		}
		if (EnsureAllocatedPointer(1))
		{
			((::AVPixelFormat *)m_pPointer)[0] = AV_PIX_FMT_NONE;
			m_nVersion++;
			return true;
		}
		return false;
	}
	finally
	{
		Monitor::Exit(this);
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVBufferSinkParams::AVPixelFormats^ FFmpeg::AVBufferSinkParams::AVPixelFormats::operator += (AVPixelFormat format)
{
	Add(format);
	return this;
}
FFmpeg::AVBufferSinkParams::AVPixelFormats^ FFmpeg::AVBufferSinkParams::AVPixelFormats::operator -= (AVPixelFormat format)
{
	Remove(format);
	return this;
}
FFmpeg::AVBufferSinkParams::AVPixelFormats^ FFmpeg::AVBufferSinkParams::AVPixelFormats::operator += (array<AVPixelFormat>^ formats)
{
	Add(formats);
	return this;
}
FFmpeg::AVBufferSinkParams::AVPixelFormats^ FFmpeg::AVBufferSinkParams::AVPixelFormats::operator -= (array<AVPixelFormat>^ formats)
{
	Remove(formats);
	return this;
}
FFmpeg::AVBufferSinkParams::AVPixelFormats^ FFmpeg::AVBufferSinkParams::AVPixelFormats::operator + (AVPixelFormats^ target,AVPixelFormat format)
{
	target += format;
	return target;
}
FFmpeg::AVBufferSinkParams::AVPixelFormats^ FFmpeg::AVBufferSinkParams::AVPixelFormats::operator - (AVPixelFormats^ target,AVPixelFormat format)
{
	target -= format;
	return target;
}
FFmpeg::AVBufferSinkParams::AVPixelFormats^ FFmpeg::AVBufferSinkParams::AVPixelFormats::operator + (AVPixelFormats^ target,array<AVPixelFormat>^ formats)
{
	target += formats;
	return target;
}
FFmpeg::AVBufferSinkParams::AVPixelFormats^ FFmpeg::AVBufferSinkParams::AVPixelFormats::operator - (AVPixelFormats^ target,array<AVPixelFormat>^ formats)
{
	target -= formats;
	return target;
}
//////////////////////////////////////////////////////
array<FFmpeg::AVPixelFormat>^ FFmpeg::AVBufferSinkParams::AVPixelFormats::ToArray()
{
	Monitor::Enter(this);
	try
	{
		List<AVPixelFormat>^ _list = gcnew List<AVPixelFormat>();
		try
		{
			for (int i = 0; i < Count; i++)
			{
				_list->Add(this[i]);
			}
			return _list->ToArray();
		}
		finally
		{
			delete _list;
		}
	}
	finally
	{
		Monitor::Exit(this);
	}
}
//////////////////////////////////////////////////////
System::Collections::IEnumerator^ FFmpeg::AVBufferSinkParams::AVPixelFormats::GetEnumerator()
{
	return gcnew AVPixelFormatsEnumerator(this);
}
System::Collections::Generic::IEnumerator<FFmpeg::AVPixelFormat>^ FFmpeg::AVBufferSinkParams::AVPixelFormats::GetEnumeratorGeneric()
{
	return gcnew AVPixelFormatsEnumerator(this);
}
//////////////////////////////////////////////////////
// AVBufferSinkParams
//////////////////////////////////////////////////////
FFmpeg::AVBufferSinkParams::AVBufferSinkParams(void * _pointer, AVBase^ _parent)
	: AVBase(_pointer, _parent)
{
	if (_pointer)
	{
		m_Formats = _CreateObject<AVPixelFormats>((void*)((::AVBufferSinkParams*)_pointer)->pixel_fmts);
	}
}
FFmpeg::AVBufferSinkParams::AVBufferSinkParams()
	: AVBase(nullptr, nullptr)
{
	PTR_API(AVFilter,av_buffersink_params_alloc)
	m_pPointer = av_buffersink_params_alloc();
	if (m_pPointer)
	{
		m_pFree = ::av_free;
		m_Formats = _CreateObject<AVPixelFormats>((void*)((::AVBufferSinkParams*)m_pPointer)->pixel_fmts);
	}
}
//////////////////////////////////////////////////////
// AVABufferSinkParams
//////////////////////////////////////////////////////
FFmpeg::AVABufferSinkParams::AVABufferSinkParams(void * _pointer, AVBase^ _parent)
	: AVBase(_pointer, _parent)
{

}

FFmpeg::AVABufferSinkParams::AVABufferSinkParams()
	: AVBase(nullptr, nullptr)
{
	PTR_API(AVFilter,av_abuffersink_params_alloc)
	m_pPointer = av_abuffersink_params_alloc();
	if (m_pPointer)
	{
		m_pFree = ::av_free;
	}
}
//////////////////////////////////////////////////////
array<FFmpeg::AVSampleFormat>^ FFmpeg::AVABufferSinkParams::sample_fmts::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVABufferSinkParams::sample_fmts::set(array<AVSampleFormat>^)
{
	//->TODO:
	throw gcnew NotImplementedException();
}
array<FFmpeg::AVChannelLayout>^ FFmpeg::AVABufferSinkParams::channel_layouts::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVABufferSinkParams::channel_layouts::set(array<AVChannelLayout>^)
{
	//->TODO:
	throw gcnew NotImplementedException();
}
array<int>^ FFmpeg::AVABufferSinkParams::channel_counts::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVABufferSinkParams::channel_counts::set(array<int>^)
{
	//->TODO:
	throw gcnew NotImplementedException();
}
int FFmpeg::AVABufferSinkParams::all_channel_counts::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVABufferSinkParams::all_channel_counts::set(int )
{
	//->TODO:
	throw gcnew NotImplementedException();
}
array<int>^ FFmpeg::AVABufferSinkParams::sample_rates::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVABufferSinkParams::sample_rates::set(array<int>^)
{
	//->TODO:
	throw gcnew NotImplementedException();
}
//////////////////////////////////////////////////////
// AVBufferSink
//////////////////////////////////////////////////////
FFmpeg::AVBufferSink::AVBufferSink(AVFilterContext^ ctx)
	: m_pContext(ctx)
{

}
//////////////////////////////////////////////////////
FFmpeg::AVMediaType FFmpeg::AVBufferSink::type::get()
{
	DYNAMIC_DEF_API(AVFilter,::AVMediaType,m_pContext->inputs[0]->type,av_buffersink_get_type,::AVFilterContext *)
	return (AVMediaType)av_buffersink_get_type((::AVFilterContext *)m_pContext->_Pointer.ToPointer());
}
FFmpeg::AVRational^ FFmpeg::AVBufferSink::time_base::get()
{
	DYNAMIC_DEF_API(AVFilter,::AVRational,m_pContext->inputs[0]->time_base,av_buffersink_get_time_base,::AVFilterContext *)
	return gcnew AVRational(av_buffersink_get_time_base((::AVFilterContext *)m_pContext->_Pointer.ToPointer()));
}
int FFmpeg::AVBufferSink::format::get()
{
	DYNAMIC_DEF_API(AVFilter,int,m_pContext->inputs[0]->format,av_buffersink_get_format,::AVFilterContext *);
	return av_buffersink_get_format((::AVFilterContext *)m_pContext->_Pointer.ToPointer());
}
FFmpeg::AVRational^ FFmpeg::AVBufferSink::frame_rate::get()
{
	::AVRational r = ((::AVFilterLink*)m_pContext->inputs[0]->_Pointer.ToPointer())->frame_rate;
	DYNAMIC_API(AVFilter,::AVRational,av_buffersink_get_frame_rate,::AVFilterContext *)
	r = av_buffersink_get_frame_rate((::AVFilterContext *)m_pContext->_Pointer.ToPointer());
	return gcnew AVRational(r.num,r.den);
}
int FFmpeg::AVBufferSink::w::get()
{
	DYNAMIC_DEF_API(AVFilter,int,m_pContext->inputs[0]->w,av_buffersink_get_w,::AVFilterContext *);
	return av_buffersink_get_w((::AVFilterContext *)m_pContext->_Pointer.ToPointer());
}
int FFmpeg::AVBufferSink::h::get()
{
	DYNAMIC_DEF_API(AVFilter,int,m_pContext->inputs[0]->h,av_buffersink_get_h,::AVFilterContext *);
	return av_buffersink_get_h((::AVFilterContext *)m_pContext->_Pointer.ToPointer());
}
FFmpeg::AVRational^ FFmpeg::AVBufferSink::sample_aspect_ratio::get()
{
	::AVRational r = ((::AVFilterLink*)m_pContext->inputs[0]->_Pointer.ToPointer())->sample_aspect_ratio;
	DYNAMIC_API(AVFilter,::AVRational,av_buffersink_get_sample_aspect_ratio,::AVFilterContext *)
	r = av_buffersink_get_sample_aspect_ratio((::AVFilterContext *)m_pContext->_Pointer.ToPointer());
	return gcnew AVRational(r.num,r.den);
}
int FFmpeg::AVBufferSink::channels::get()
{
	DYNAMIC_DEF_API(AVFilter,int,m_pContext->inputs[0]->channels,av_buffersink_get_channels,::AVFilterContext *);
	return av_buffersink_get_channels((::AVFilterContext *)m_pContext->_Pointer.ToPointer());
}
FFmpeg::AVChannelLayout FFmpeg::AVBufferSink::channel_layout::get()
{
	DYNAMIC_DEF_API(AVFilter,uint64_t,m_pContext->inputs[0]->channel_layout,av_buffersink_get_channel_layout,::AVFilterContext *);
	return (AVChannelLayout)av_buffersink_get_channel_layout((::AVFilterContext *)m_pContext->_Pointer.ToPointer());
}
int FFmpeg::AVBufferSink::sample_rate::get()
{
	DYNAMIC_DEF_API(AVFilter,int,m_pContext->inputs[0]->sample_rate,av_buffersink_get_sample_rate,::AVFilterContext *);
	return av_buffersink_get_sample_rate((::AVFilterContext *)m_pContext->_Pointer.ToPointer());
}
FFmpeg::AVBufferRef^ FFmpeg::AVBufferSink::hw_frames_ctx::get()
{
	::AVBufferRef* p = ((::AVFilterLink*)m_pContext->inputs[0]->_Pointer.ToPointer())->hw_frames_ctx;
	DYNAMIC_API(AVFilter,::AVBufferRef*,av_buffersink_get_hw_frames_ctx,::AVFilterContext *)
	p = av_buffersink_get_hw_frames_ctx((::AVFilterContext *)m_pContext->_Pointer.ToPointer());
	return AVBase::_CreateChildObject<AVBufferRef>(p,m_pContext);
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVBufferSink::get_frame(AVFrame^ frame, AVBufferSinkFlags flags)
{
	DYNAMIC_DEF_API(AVFilter,int,AVERROR(ENOSYS),av_buffersink_get_frame_flags,::AVFilterContext *,::AVFrame*,int);
	return av_buffersink_get_frame_flags(
		((::AVFilterContext *)m_pContext->_Pointer.ToPointer()), 
		((::AVFrame*)frame->_Pointer.ToPointer()),
		(int)flags);
}
FFmpeg::AVRESULT FFmpeg::AVBufferSink::get_frame(AVFrame^ frame)
{
	DYNAMIC_DEF_API(AVFilter,int,AVERROR(ENOSYS),av_buffersink_get_frame,::AVFilterContext *,::AVFrame*);
	return av_buffersink_get_frame(
		((::AVFilterContext *)m_pContext->_Pointer.ToPointer()), 
		((::AVFrame*)frame->_Pointer.ToPointer()));
}
void FFmpeg::AVBufferSink::set_frame_size(unsigned frame_size)
{
	VOID_API(AVFilter,av_buffersink_set_frame_size,::AVFilterContext *,unsigned)
	av_buffersink_set_frame_size(((::AVFilterContext *)m_pContext->_Pointer.ToPointer()),frame_size);
}
FFmpeg::AVRESULT FFmpeg::AVBufferSink::get_samples(AVFrame^ frame, int nb_samples)
{
	DYNAMIC_DEF_API(AVFilter,int,AVERROR(ENOSYS),av_buffersink_get_samples,::AVFilterContext *,::AVFrame*,int);
	return av_buffersink_get_samples(
		((::AVFilterContext *)m_pContext->_Pointer.ToPointer()), 
		((::AVFrame*)frame->_Pointer.ToPointer()),nb_samples);
}
//////////////////////////////////////////////////////
// AVBufferSrcParameters
//////////////////////////////////////////////////////
FFmpeg::AVBufferSrcParameters::AVBufferSrcParameters(void * _pointer, AVBase^ _parent)
	: AVBase(_pointer, _parent)
{
}

FFmpeg::AVBufferSrcParameters::AVBufferSrcParameters()
	: AVBase(nullptr, nullptr)
{
	PTR_API(AVFilter,av_buffersrc_parameters_alloc)
	m_pPointer = av_buffersrc_parameters_alloc();
	if (m_pPointer)
	{
		m_pFree = ::av_free;
	}
}
//////////////////////////////////////////////////////
int FFmpeg::AVBufferSrcParameters::format::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVBufferSrcParameters::format::set(int)
{
	//->TODO:
}
FFmpeg::AVRational^ FFmpeg::AVBufferSrcParameters::time_base::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVBufferSrcParameters::time_base::set(AVRational^)
{
	//->TODO:
}
int FFmpeg::AVBufferSrcParameters::width::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVBufferSrcParameters::width::set(int)
{
	//->TODO:
}
int FFmpeg::AVBufferSrcParameters::height::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVBufferSrcParameters::height::set(int)
{
	//->TODO:
}
FFmpeg::AVRational^ FFmpeg::AVBufferSrcParameters::sample_aspect_ratio::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVBufferSrcParameters::sample_aspect_ratio::set(AVRational^)
{
	//->TODO:
}
FFmpeg::AVRational^ FFmpeg::AVBufferSrcParameters::frame_rate::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVBufferSrcParameters::frame_rate::set(AVRational^)
{
	//->TODO:
}
FFmpeg::AVBufferRef^ FFmpeg::AVBufferSrcParameters::hw_frames_ctx::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVBufferSrcParameters::hw_frames_ctx::set(AVBufferRef^)
{
	//->TODO:
}
int FFmpeg::AVBufferSrcParameters::sample_rate::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVBufferSrcParameters::sample_rate::set(int )
{
	//->TODO:
}
FFmpeg::AVChannelLayout FFmpeg::AVBufferSrcParameters::channel_layout::get()
{
	//->TODO:
	throw gcnew NotImplementedException();
}
void FFmpeg::AVBufferSrcParameters::channel_layout::set(AVChannelLayout )
{
	//->TODO:
}
//////////////////////////////////////////////////////
// AVBufferSrc
//////////////////////////////////////////////////////
FFmpeg::AVBufferSrc::AVBufferSrc(AVFilterContext^ ctx)
	: m_pContext(ctx)
{
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVBufferSrc::parameters_set(AVBufferSrcParameters^ param)
{
	DYNAMIC_DEF_API(AVFilter,int,AVERROR(ENOSYS),av_buffersrc_parameters_set,::AVFilterContext *,::AVBufferSrcParameters*);
	return av_buffersrc_parameters_set((::AVFilterContext *)m_pContext->_Pointer.ToPointer(),
		(::AVBufferSrcParameters*)param->_Pointer.ToPointer());
}
FFmpeg::AVRESULT FFmpeg::AVBufferSrc::write_frame(AVFrame^ frame)
{
	DYNAMIC_DEF_API(AVFilter,int,AVERROR(ENOSYS),av_buffersrc_write_frame,::AVFilterContext *,::AVFrame*);
	return av_buffersrc_write_frame((::AVFilterContext *)m_pContext->_Pointer.ToPointer(),
		frame != nullptr ? (::AVFrame*)frame->_Pointer.ToPointer() : nullptr);
}
FFmpeg::AVRESULT FFmpeg::AVBufferSrc::add_frame(AVFrame^ frame)
{
	DYNAMIC_DEF_API(AVFilter,int,AVERROR(ENOSYS),av_buffersrc_add_frame,::AVFilterContext *,::AVFrame*);
	return av_buffersrc_add_frame((::AVFilterContext *)m_pContext->_Pointer.ToPointer(),
		frame != nullptr ? (::AVFrame*)frame->_Pointer.ToPointer() : nullptr);
}
FFmpeg::AVRESULT FFmpeg::AVBufferSrc::add_frame(AVFrame^ frame,AVBufferSrcFlags flags)
{
	DYNAMIC_DEF_API(AVFilter,int,AVERROR(ENOSYS),av_buffersrc_add_frame_flags,::AVFilterContext *,::AVFrame*,int);
	return av_buffersrc_add_frame_flags((::AVFilterContext *)m_pContext->_Pointer.ToPointer(),
		frame != nullptr ? (::AVFrame*)frame->_Pointer.ToPointer() : nullptr,(int)flags);
}
FFmpeg::AVRESULT FFmpeg::AVBufferSrc::close(Int64 pts,AVBufferSrcFlags flags)
{
	DYNAMIC_DEF_API(AVFilter,int,AVERROR(ENOSYS),av_buffersrc_close,::AVFilterContext *,int64_t,int);
	return av_buffersrc_close((::AVFilterContext *)m_pContext->_Pointer.ToPointer(),
		(int64_t)pts,(int)flags);
}
unsigned FFmpeg::AVBufferSrc::get_nb_failed_requests()
{
	DYNAMIC_DEF_API(AVFilter,unsigned,0,av_buffersrc_get_nb_failed_requests,::AVFilterContext *);
	return av_buffersrc_get_nb_failed_requests((::AVFilterContext *)m_pContext->_Pointer.ToPointer());
}
//////////////////////////////////////////////////////