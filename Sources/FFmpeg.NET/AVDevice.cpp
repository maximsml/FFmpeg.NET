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
// libavdevice
// Special devices muxing/demuxing library.
//////////////////////////////////////////////////////
#include "stdafx.h"
#include "AVDevice.h"
//////////////////////////////////////////////////////
#pragma warning(disable: 4996) // marked as deprecated
//////////////////////////////////////////////////////
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
//////////////////////////////////////////////////////
FFmpeg::LibAVDevice::LibAVDevice()
{
}
//////////////////////////////////////////////////////
UInt32 FFmpeg::LibAVDevice::Version::get()
{
	return avdevice_version();
}

String^ FFmpeg::LibAVDevice::Configuration::get()
{
	auto p = avdevice_configuration();
	return p != nullptr ? gcnew String(p) : nullptr;
}

String^ FFmpeg::LibAVDevice::License::get()
{
	auto p = avdevice_license();
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
void FFmpeg::LibAVDevice::RegisterAll()
{
	AVBase::EnsureLibraryLoaded();
	if (!s_bRegistered) 
	{
		s_bRegistered = true;
		LibAVFormat::RegisterAll();
		avdevice_register_all();
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVDeviceRect::AVDeviceRect()
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
}
FFmpeg::AVDeviceRect::AVDeviceRect(int _x, int _y, int _width, int _height)
	: AVBase(nullptr,nullptr)
{
	AllocPointer(_StructureSize);
	x = _x;
	y = _y;
	width = _width;
	height = _height;
}
//////////////////////////////////////////////////////
int FFmpeg::AVDeviceRect::_StructureSize::get() 
{
	return sizeof(::AVDeviceRect);
}
//////////////////////////////////////////////////////
int FFmpeg::AVDeviceRect::x::get()
{
	return ((::AVDeviceRect*)m_pPointer)->x;
}
void FFmpeg::AVDeviceRect::x::set(int value)
{
	((::AVDeviceRect*)m_pPointer)->x = value;
}
int FFmpeg::AVDeviceRect::y::get()
{
	return ((::AVDeviceRect*)m_pPointer)->y;
}
void FFmpeg::AVDeviceRect::y::set(int value)
{
	((::AVDeviceRect*)m_pPointer)->y = value;
}
int FFmpeg::AVDeviceRect::width::get()
{
	return ((::AVDeviceRect*)m_pPointer)->width;
}
void FFmpeg::AVDeviceRect::width::set(int value)
{
	((::AVDeviceRect*)m_pPointer)->width = value;
}
int FFmpeg::AVDeviceRect::height::get()
{
	return ((::AVDeviceRect*)m_pPointer)->height;
}
void FFmpeg::AVDeviceRect::height::set(int value)
{
	((::AVDeviceRect*)m_pPointer)->height = value;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVDeviceRect::ToString()
{
	if (m_pPointer)
	{
		return String::Format("Rect X: {0} Y: {1} width: {2} height: {3}",x,y,width,height);
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
// AVDeviceCapabilitiesQuery
//////////////////////////////////////////////////////
FFmpeg::AVDeviceCapabilitiesQuery::AVDeviceCapabilitiesQuery(void * _pointer,AVBase^ _parent)
	: m_pFormatContext(nullptr)
	, AVBase(_pointer,_parent)
{

}

FFmpeg::AVDeviceCapabilitiesQuery::~AVDeviceCapabilitiesQuery()
{
	if (m_pPointer && m_pFormatContext)
	{
		::AVDeviceCapabilitiesQuery * _caps = (::AVDeviceCapabilitiesQuery *)m_pPointer;
		avdevice_capabilities_free(&_caps, (::AVFormatContext*)m_pFormatContext->_Pointer.ToPointer());
		m_pPointer = nullptr;
		m_pFormatContext = nullptr;
	}
}
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::AVDeviceCapabilitiesQuery::av_class::get()
{
	return _CreateObject<AVClass>((void*)((::AVDeviceCapabilitiesQuery*)m_pPointer)->av_class);
}
FFmpeg::AVFormatContext^ FFmpeg::AVDeviceCapabilitiesQuery::device_context::get()
{
	return _CreateObject<AVFormatContext>((void*)((::AVDeviceCapabilitiesQuery*)m_pPointer)->device_context);
}
FFmpeg::AVCodecID FFmpeg::AVDeviceCapabilitiesQuery::codec::get()
{
	return (AVCodecID)((::AVDeviceCapabilitiesQuery*)m_pPointer)->codec;
}
void FFmpeg::AVDeviceCapabilitiesQuery::codec::set(AVCodecID value)
{
	((::AVDeviceCapabilitiesQuery*)m_pPointer)->codec = (::AVCodecID)value;
}
FFmpeg::AVSampleFormat FFmpeg::AVDeviceCapabilitiesQuery::sample_format::get()
{
	return (AVSampleFormat)((::AVDeviceCapabilitiesQuery*)m_pPointer)->sample_format;
}
void FFmpeg::AVDeviceCapabilitiesQuery::sample_format::set(AVSampleFormat value)
{
	((::AVDeviceCapabilitiesQuery*)m_pPointer)->sample_format = (::AVSampleFormat)value;
}
FFmpeg::AVPixelFormat FFmpeg::AVDeviceCapabilitiesQuery::pixel_format::get()
{
	return (AVPixelFormat)((::AVDeviceCapabilitiesQuery*)m_pPointer)->pixel_format;
}
void FFmpeg::AVDeviceCapabilitiesQuery::pixel_format::set(AVPixelFormat value)
{
	((::AVDeviceCapabilitiesQuery*)m_pPointer)->pixel_format = (::AVPixelFormat)value;
}
int FFmpeg::AVDeviceCapabilitiesQuery::sample_rate::get()
{
	return ((::AVDeviceCapabilitiesQuery*)m_pPointer)->sample_rate;
}
void FFmpeg::AVDeviceCapabilitiesQuery::sample_rate::set(int value)
{
	((::AVDeviceCapabilitiesQuery*)m_pPointer)->sample_rate = value;
}
int FFmpeg::AVDeviceCapabilitiesQuery::channels::get()
{
	return ((::AVDeviceCapabilitiesQuery*)m_pPointer)->channels;
}
void FFmpeg::AVDeviceCapabilitiesQuery::channels::set(int value)
{
	((::AVDeviceCapabilitiesQuery*)m_pPointer)->channels = value;
}
FFmpeg::AVChannelLayout FFmpeg::AVDeviceCapabilitiesQuery::channel_layout::get()
{
	return (AVChannelLayout)((::AVDeviceCapabilitiesQuery*)m_pPointer)->channel_layout;
}
void FFmpeg::AVDeviceCapabilitiesQuery::channel_layout::set(AVChannelLayout value)
{
	((::AVDeviceCapabilitiesQuery*)m_pPointer)->channel_layout = (int64_t)value;
}
int FFmpeg::AVDeviceCapabilitiesQuery::window_width::get()
{
	return ((::AVDeviceCapabilitiesQuery*)m_pPointer)->window_width;
}
void FFmpeg::AVDeviceCapabilitiesQuery::window_width::set(int value)
{
	((::AVDeviceCapabilitiesQuery*)m_pPointer)->window_width = value;
}
int FFmpeg::AVDeviceCapabilitiesQuery::window_height::get()
{
	return ((::AVDeviceCapabilitiesQuery*)m_pPointer)->window_height;
}
void FFmpeg::AVDeviceCapabilitiesQuery::window_height::set(int value)
{
	((::AVDeviceCapabilitiesQuery*)m_pPointer)->window_height = value;
}
int FFmpeg::AVDeviceCapabilitiesQuery::frame_width::get()
{
	return ((::AVDeviceCapabilitiesQuery*)m_pPointer)->frame_width;
}
void FFmpeg::AVDeviceCapabilitiesQuery::frame_width::set(int value)
{
	((::AVDeviceCapabilitiesQuery*)m_pPointer)->frame_width = value;
}
int FFmpeg::AVDeviceCapabilitiesQuery::frame_height::get()
{
	return ((::AVDeviceCapabilitiesQuery*)m_pPointer)->frame_height;
}
void FFmpeg::AVDeviceCapabilitiesQuery::frame_height::set(int value)
{
	((::AVDeviceCapabilitiesQuery*)m_pPointer)->frame_height = value;
}
FFmpeg::AVRational^ FFmpeg::AVDeviceCapabilitiesQuery::fps::get()
{
	return _CreateObject<AVRational>(&((::AVDeviceCapabilitiesQuery*)m_pPointer)->fps);
}
void FFmpeg::AVDeviceCapabilitiesQuery::fps::set(AVRational^ value)
{
	::AVRational _fps = {value->num,value->den};
	((::AVDeviceCapabilitiesQuery*)m_pPointer)->fps = _fps;
}
//////////////////////////////////////////////////////
FFmpeg::AVDeviceCapabilitiesQuery^ FFmpeg::AVDeviceCapabilitiesQuery::Create(AVFormatContext^ s,AVDictionary^ device_options)
{
	::AVDeviceCapabilitiesQuery *caps = nullptr;
	::AVDictionary * _dict = device_options != nullptr ? (::AVDictionary *)device_options->_Pointer.ToPointer() : nullptr;
	try
	{
		int _result = avdevice_capabilities_create(&caps, (::AVFormatContext*)s->_Pointer.ToPointer(),&_dict);
		if (device_options)
		{
			device_options->ChangePointer(_dict);
			_dict = nullptr;
		}
		if (_result >= 0)
		{
			auto _caps = _CreateChildObject<AVDeviceCapabilitiesQuery>(caps,s);
			if (_caps != nullptr)
			{
				_caps->m_pFormatContext = s;
			}
			return _caps;
		}
	}
	finally
	{
		if (_dict) av_dict_free(&_dict);
	}
	return nullptr;
}
//////////////////////////////////////////////////////
// AVDeviceInfo
//////////////////////////////////////////////////////
FFmpeg::AVDeviceInfo::AVDeviceInfo(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{

}
//////////////////////////////////////////////////////
String^ FFmpeg::AVDeviceInfo::device_name::get()
{
	auto p = ((::AVDeviceInfo*)m_pPointer)->device_name;
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::AVDeviceInfo::device_description::get()
{
	auto p = ((::AVDeviceInfo*)m_pPointer)->device_description;
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
String^ FFmpeg::AVDeviceInfo::ToString()
{
	if (m_pPointer)
	{
		String^ _name;
		_name = device_name;
		if (!String::IsNullOrEmpty(_name)) return _name;
		_name = device_description;
		if (!String::IsNullOrEmpty(_name)) return _name;
	}
	return Object::ToString();
}
//////////////////////////////////////////////////////
// AVDevicesEnumerator
//////////////////////////////////////////////////////
FFmpeg::AVDeviceInfoList::AVDevicesEnumerator::AVDevicesEnumerator(AVDeviceInfoList^ _array)
	: m_nIndex(-1)
	, m_pArray(_array)
{
}
FFmpeg::AVDeviceInfoList::AVDevicesEnumerator::~AVDevicesEnumerator()
{
}
bool FFmpeg::AVDeviceInfoList::AVDevicesEnumerator::MoveNext()
{
	if (m_nIndex < m_pArray->Count)
	{
		m_nIndex++;
		return true;
	}
	return false;
}
FFmpeg::AVDeviceInfo^ FFmpeg::AVDeviceInfoList::AVDevicesEnumerator::Current::get()
{
	if (m_nIndex < 0)
	{
		if (!MoveNext()) return nullptr;
	}
	return m_pArray[m_nIndex];
}
void FFmpeg::AVDeviceInfoList::AVDevicesEnumerator::Reset()
{
	m_nIndex = -1;
}
Object^ FFmpeg::AVDeviceInfoList::AVDevicesEnumerator::CurrentObject::get()
{
	return Current::get();
}
//////////////////////////////////////////////////////
// AVDeviceInfoList
//////////////////////////////////////////////////////
FFmpeg::AVDeviceInfoList::AVDeviceInfoList(void * _pointer,AVBase^ _parent)
	: AVBase(_pointer,_parent)
{
}
//////////////////////////////////////////////////////
FFmpeg::AVDeviceInfo^ FFmpeg::AVDeviceInfoList::default::get(int index)
{
	if (index < 0 || index > Count) throw gcnew ArgumentOutOfRangeException();
	::AVDeviceInfo ** p = (::AVDeviceInfo **)((::AVDeviceInfoList*)m_pPointer)->devices;
	return _CreateObject<AVDeviceInfo>(p[index]);
}
int FFmpeg::AVDeviceInfoList::Count::get()
{
	return ((::AVDeviceInfoList*)m_pPointer)->nb_devices;
}
int FFmpeg::AVDeviceInfoList::nb_devices::get()
{
	return ((::AVDeviceInfoList*)m_pPointer)->nb_devices;
}
int FFmpeg::AVDeviceInfoList::default_device::get()
{
	return ((::AVDeviceInfoList*)m_pPointer)->default_device;
}
//////////////////////////////////////////////////////
array<FFmpeg::AVDeviceInfo^>^ FFmpeg::AVDeviceInfoList::ToArray()
{
	List<AVDeviceInfo^>^ _list = gcnew List<AVDeviceInfo^>();
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
//////////////////////////////////////////////////////
System::Collections::IEnumerator^ FFmpeg::AVDeviceInfoList::GetEnumerator()
{
	return gcnew AVDevicesEnumerator(this);
}
System::Collections::Generic::IEnumerator<FFmpeg::AVDeviceInfo^>^ FFmpeg::AVDeviceInfoList::GetEnumeratorGeneric()
{
	return gcnew AVDevicesEnumerator(this);
}
//////////////////////////////////////////////////////
// AVDevices
//////////////////////////////////////////////////////
FFmpeg::AVInputFormat^ FFmpeg::AVDevices::input_audio_device_next(AVInputFormat^ d)
{
	LibAVDevice::RegisterAll();
	auto p = av_input_audio_device_next(d != nullptr ? (::AVInputFormat*)d->_Pointer.ToPointer() : nullptr);
	return p != nullptr ? gcnew AVInputFormat((void*)p,nullptr) : nullptr;
}
FFmpeg::AVInputFormat^ FFmpeg::AVDevices::input_video_device_next(AVInputFormat^ d)
{
	LibAVDevice::RegisterAll();
	auto p = av_input_video_device_next(d != nullptr ? (::AVInputFormat*)d->_Pointer.ToPointer() : nullptr);
	return p != nullptr ? gcnew AVInputFormat((void*)p,nullptr) : nullptr;
}
FFmpeg::AVOutputFormat^ FFmpeg::AVDevices::output_audio_device_next(AVOutputFormat^ d)
{
	LibAVDevice::RegisterAll();
	auto p = av_output_audio_device_next(d != nullptr ? (::AVOutputFormat*)d->_Pointer.ToPointer() : nullptr);
	return p != nullptr ? gcnew AVOutputFormat((void*)p,nullptr) : nullptr;
}
FFmpeg::AVOutputFormat^ FFmpeg::AVDevices::output_video_device_next(AVOutputFormat^ d)
{
	LibAVDevice::RegisterAll();
	auto p = av_output_video_device_next(d != nullptr ? (::AVOutputFormat*)d->_Pointer.ToPointer() : nullptr);
	return p != nullptr ? gcnew AVOutputFormat((void*)p,nullptr) : nullptr;
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVDevices::app_to_dev_control_message(AVFormatContext^ s, AVAppToDevMessageType type, IntPtr data, int data_size)
{
	return avdevice_app_to_dev_control_message(s != nullptr ? (::AVFormatContext*)s->_Pointer.ToPointer() : nullptr,(::AVAppToDevMessageType)type,data.ToPointer(),data_size);
}
FFmpeg::AVRESULT FFmpeg::AVDevices::app_to_dev_control_message(AVFormatContext^ s, AVAppToDevMessageType type, AVBase^ structure)
{
	return app_to_dev_control_message(s,type,structure != nullptr ? structure->_Pointer : IntPtr::Zero,structure != nullptr ? structure->_StructureSize : 0);
}
FFmpeg::AVRESULT FFmpeg::AVDevices::app_to_dev_control_message(AVFormatContext^ s, AVAppToDevMessageType type, double value)
{
	return app_to_dev_control_message(s,type,(IntPtr)&value,sizeof(double));
}
FFmpeg::AVRESULT FFmpeg::AVDevices::app_to_dev_control_message(AVFormatContext^ s, AVAppToDevMessageType type)
{
	return app_to_dev_control_message(s,type,IntPtr::Zero,0);
}
//////////////////////////////////////////////////////
FFmpeg::AVRESULT FFmpeg::AVDevices::dev_to_app_control_message(AVFormatContext^ s, AVDevToAppMessageType type, IntPtr data, int data_size)
{
	return avdevice_dev_to_app_control_message(s != nullptr ? (::AVFormatContext*)s->_Pointer.ToPointer() : nullptr,(::AVDevToAppMessageType)type,data.ToPointer(),data_size);
}
FFmpeg::AVRESULT FFmpeg::AVDevices::dev_to_app_control_message(AVFormatContext^ s, AVDevToAppMessageType type, AVBase^ structure)
{
	return dev_to_app_control_message(s,type,structure != nullptr ? structure->_Pointer : IntPtr::Zero,structure != nullptr ? structure->_StructureSize : 0);
}
FFmpeg::AVRESULT FFmpeg::AVDevices::dev_to_app_control_message(AVFormatContext^ s, AVDevToAppMessageType type, double value)
{
	return dev_to_app_control_message(s,type,(IntPtr)&value,sizeof(double));
}
FFmpeg::AVRESULT FFmpeg::AVDevices::dev_to_app_control_message(AVFormatContext^ s, AVDevToAppMessageType type, int value)
{
	return dev_to_app_control_message(s,type,(IntPtr)&value,sizeof(int));
}
FFmpeg::AVRESULT FFmpeg::AVDevices::dev_to_app_control_message(AVFormatContext^ s, AVDevToAppMessageType type, Int64 value)
{
	return dev_to_app_control_message(s,type,(IntPtr)&value,sizeof(Int64));
}
FFmpeg::AVRESULT FFmpeg::AVDevices::dev_to_app_control_message(AVFormatContext^ s, AVDevToAppMessageType type)
{
	return dev_to_app_control_message(s,type,IntPtr::Zero,0);
}
//////////////////////////////////////////////////////
FFmpeg::AVDeviceInfoList^ FFmpeg::AVDevices::list_devices(AVFormatContext^ s)
{
	LibAVDevice::RegisterAll();
	::AVDeviceInfoList * device_list = nullptr;
	if (0 < avdevice_list_devices(s != nullptr ? (::AVFormatContext *)s->_Pointer.ToPointer() : nullptr, &device_list) && device_list)
	{
		AVDeviceInfoList^ _list = gcnew AVDeviceInfoList(device_list,nullptr);
		_list->m_pFreep = (AVBase::TFreeFNP*)avdevice_free_list_devices;
		return _list;
	}
	return nullptr;
}
//////////////////////////////////////////////////////
FFmpeg::AVDeviceInfoList^ FFmpeg::AVDevices::list_input_sources(AVInputFormat^ device, String^ device_name,AVDictionary^ device_options)
{
	LibAVDevice::RegisterAll();
	::AVDictionary * _dict = device_options != nullptr ? (::AVDictionary *)device_options->_Pointer.ToPointer() : nullptr;
	char* szName = nullptr;
	try
	{
		szName = (char*)AVBase::AllocString(device_name).ToPointer();
		::AVDeviceInfoList * device_list = nullptr;
		if (0 < avdevice_list_input_sources(device != nullptr ? (::AVInputFormat *)device->_Pointer.ToPointer() : nullptr,szName,_dict,&device_list) && device_list)
		{
			AVDeviceInfoList^ _list = gcnew AVDeviceInfoList(device_list,nullptr);
			_list->m_pFreep = (AVBase::TFreeFNP*)avdevice_free_list_devices;
			if (device_options)
			{
				device_options->ChangePointer(_dict);
				_dict = nullptr;
			}
			return _list;
		}
		else
		{
			if (device_options) _dict = nullptr;
		}
	}
	finally
	{
		if (_dict) av_dict_free(&_dict);
		AVBase::FreeMemory((IntPtr)szName);
	}
	return nullptr;
}

FFmpeg::AVDeviceInfoList^ FFmpeg::AVDevices::list_output_sinks(AVOutputFormat^ device, String^ device_name,AVDictionary^ device_options)
{
	LibAVDevice::RegisterAll();
	::AVDictionary * _dict = device_options != nullptr ? (::AVDictionary *)device_options->_Pointer.ToPointer() : nullptr;
	char* szName = nullptr;
	try
	{
		szName = (char*)AVBase::AllocString(device_name).ToPointer();
		::AVDeviceInfoList * device_list = nullptr;
		if (0 < avdevice_list_output_sinks(device != nullptr ? (::AVOutputFormat *)device->_Pointer.ToPointer() : nullptr,szName,_dict,&device_list) && device_list)
		{
			AVDeviceInfoList^ _list = gcnew AVDeviceInfoList(device_list,nullptr);
			_list->m_pFreep = (AVBase::TFreeFNP*)avdevice_free_list_devices;
			if (device_options)
			{
				device_options->ChangePointer(_dict);
				_dict = nullptr;
			}
			return _list;
		}
		else
		{
			if (device_options) _dict = nullptr;
		}
	}
	finally
	{
		if (_dict) av_dict_free(&_dict);
		AVBase::FreeMemory((IntPtr)szName);
	}
	return nullptr;
}
//////////////////////////////////////////////////////