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
#pragma once
//////////////////////////////////////////////////////
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;  
//////////////////////////////////////////////////////
#include "AVFormat.h"
//////////////////////////////////////////////////////
namespace FFmpeg {
//////////////////////////////////////////////////////
#pragma region Enums
//////////////////////////////////////////////////////
/// Message types used by avdevice_app_to_dev_control_message().
public enum class AVAppToDevMessageType : UInt32
{   
	/// Dummy message.
	NONE = MKBETAG('N','O','N','E'),
    
	/// Window size change message.
	///
	/// Message is sent to the device every time the application changes the size
	/// of the window device renders to.
	/// Message should also be sent right after window is created.
	///
	/// data: AVDeviceRect: new window size.
	WINDOW_SIZE = MKBETAG('G','E','O','M'),
    
	/// Repaint request message.
	///
	/// Message is sent to the device when window has to be repainted.
	///
	/// data: AVDeviceRect: area required to be repainted.
	///       NULL: whole area is required to be repainted.
	WINDOW_REPAINT = MKBETAG('R','E','P','A'),
    
	/// Request pause/play.
	///
	/// Application requests pause/unpause playback.
	/// Mostly usable with devices that have internal buffer.
	/// By default devices are not paused.
	///
	/// data: NULL
	PAUSE        = MKBETAG('P', 'A', 'U', ' '),
	PLAY         = MKBETAG('P', 'L', 'A', 'Y'),
	TOGGLE_PAUSE = MKBETAG('P', 'A', 'U', 'T'),
    
	/// Volume control message.
	///
	/// Set volume level. It may be device-dependent if volume
	/// is changed per stream or system wide. Per stream volume
	/// change is expected when possible.
	///
	/// data: double: new volume with range of 0.0 - 1.0.    
	SET_VOLUME = MKBETAG('S', 'V', 'O', 'L'),
    
	/// Mute control messages.
	///
	/// Change mute state. It may be device-dependent if mute status
	/// is changed per stream or system wide. Per stream mute status
	/// change is expected when possible.
	///
	/// data: NULL.    
	MUTE        = MKBETAG(' ', 'M', 'U', 'T'),
	UNMUTE      = MKBETAG('U', 'M', 'U', 'T'),
	TOGGLE_MUTE = MKBETAG('T', 'M', 'U', 'T'),

	/// Get volume/mute messages.
	///
	/// Force the device to send AV_DEV_TO_APP_VOLUME_LEVEL_CHANGED or
	/// AV_DEV_TO_APP_MUTE_STATE_CHANGED command respectively.
	///
	/// data: NULL.
    GET_VOLUME = MKBETAG('G', 'V', 'O', 'L'),
    GET_MUTE   = MKBETAG('G', 'M', 'U', 'T'),
};
//////////////////////////////////////////////////////
/// Message types used by avdevice_dev_to_app_control_message().
public enum class AVDevToAppMessageType : UInt32
{
	/// Dummy message.
	NONE = MKBETAG('N','O','N','E'),

	/// Create window buffer message.
	///
	/// Device requests to create a window buffer. Exact meaning is device-
	/// and application-dependent. Message is sent before rendering first
	/// frame and all one-shot initializations should be done here.
	/// Application is allowed to ignore preferred window buffer size.
	///
	/// @note: Application is obligated to inform about window buffer size
	///        with AV_APP_TO_DEV_WINDOW_SIZE message.
	///
	/// data: AVDeviceRect: preferred size of the window buffer.
	///       NULL: no preferred size of the window buffer.
	CREATE_WINDOW_BUFFER = MKBETAG('B','C','R','E'),

	/// Prepare window buffer message.
	///
	/// Device requests to prepare a window buffer for rendering.
	/// Exact meaning is device- and application-dependent.
	/// Message is sent before rendering of each frame.
	///
	/// data: NULL.
	PREPARE_WINDOW_BUFFER = MKBETAG('B','P','R','E'),

	/// Display window buffer message.
	///
	/// Device requests to display a window buffer.
	/// Message is sent when new frame is ready to be displayed.
	/// Usually buffers need to be swapped in handler of this message.
	///
	/// data: NULL.
	DISPLAY_WINDOW_BUFFER = MKBETAG('B','D','I','S'),

	/// Destroy window buffer message.
	///
	/// Device requests to destroy a window buffer.
	/// Message is sent when device is about to be destroyed and window
	/// buffer is not required anymore.
	///
	/// data: NULL.
	DESTROY_WINDOW_BUFFER = MKBETAG('B','D','E','S'),

	/// Buffer fullness status messages.
	///
	/// Device signals buffer overflow/underflow.
	///
	/// data: NULL.
	BUFFER_OVERFLOW = MKBETAG('B','O','F','L'),
	BUFFER_UNDERFLOW = MKBETAG('B','U','F','L'),

	/// Buffer readable/writable.
	///
	/// Device informs that buffer is readable/writable.
	/// When possible, device informs how many bytes can be read/write.
	///
	/// @warning Device may not inform when number of bytes than can be read/write changes.
	///
	/// data: int64_t: amount of bytes available to read/write.
	///       NULL: amount of bytes available to read/write is not known.
	BUFFER_READABLE = MKBETAG('B','R','D',' '),
	BUFFER_WRITABLE = MKBETAG('B','W','R',' '),

	/// Mute state change message.
	///
	/// Device informs that mute state has changed.
	///
	/// data: int: 0 for not muted state, non-zero for muted state.
	MUTE_STATE_CHANGED = MKBETAG('C','M','U','T'),

	/// Volume level change message.
	///
	/// Device informs that volume level has changed.
	///
	/// data: double: new volume with range of 0.0 - 1.0.
    VOLUME_LEVEL_CHANGED = MKBETAG('C','V','O','L'),
};
//////////////////////////////////////////////////////
#pragma endregion
//////////////////////////////////////////////////////
#pragma region Classes
//////////////////////////////////////////////////////
// LibAVDevice
public ref class LibAVDevice
{
private:
	static bool s_bRegistered = false;
private:
	LibAVDevice();
public:
	// Return the LIBAVDEVICE_VERSION_INT constant.
	static property UInt32 Version { UInt32 get(); }
	// Return the libavdevice build-time configuration.
	static property String^ Configuration { String^ get(); }
	// Return the libavdevice license.
	static property String^ License { String^ get(); }
public:
	// Initialize libavdevice and register all the input and output devices.
	static void RegisterAll();
};
//////////////////////////////////////////////////////
// AVDeviceRect
public ref class AVDeviceRect : public AVBase
{
public:
	AVDeviceRect();
	AVDeviceRect(int _x, int _y, int _width, int _height);
public:
	property int _StructureSize { virtual int get() override; }
public:
	///< x coordinate of top left corner
	property int x { int get(); void set(int);  }
	///< y coordinate of top left corner
    property int y { int get(); void set(int);  }
	///< width
    property int width { int get(); void set(int);  }
	///< height
    property int height { int get(); void set(int);  }
public:
	virtual String^ ToString() override;
};
//////////////////////////////////////////////////////
// AVDeviceCapabilitiesQuery
/// Structure describes device capabilities.
///
/// It is used by devices in conjunction with av_device_capabilities AVOption table
/// to implement capabilities probing API based on AVOption API. Should not be used directly.
public ref class AVDeviceCapabilitiesQuery : public AVBase
{
private:
	AVFormatContext^ m_pFormatContext;
internal:
	AVDeviceCapabilitiesQuery(void * _pointer,AVBase^ _parent);
	~AVDeviceCapabilitiesQuery();
public:
	property AVClass^ av_class { AVClass^ get(); }
    property AVFormatContext^ device_context { AVFormatContext^ get(); }
	property AVCodecID codec { AVCodecID get(); void set(AVCodecID); }
    property AVSampleFormat sample_format { AVSampleFormat get(); void set(AVSampleFormat); }
    property AVPixelFormat pixel_format { AVPixelFormat get(); void set(AVPixelFormat); }
    property int sample_rate { int get(); void set(int); }
    property int channels { int get(); void set(int); }
	property AVChannelLayout channel_layout { AVChannelLayout get(); void set(AVChannelLayout); }
    property int window_width { int get(); void set(int); }
    property int window_height { int get(); void set(int); }
    property int frame_width { int get(); void set(int); }
    property int frame_height { int get(); void set(int); }
    property AVRational^ fps { AVRational^ get(); void set(AVRational^);}
public:
	/// Initialize capabilities probing API based on AVOption API.
	///
	/// avdevice_capabilities_free() must be called when query capabilities API is
	/// not used anymore.
	///
	/// @param[out] caps      Device capabilities data. Pointer to a NULL pointer must be passed.
	/// @param s              Context of the device.
	/// @param device_options An AVDictionary filled with device-private options.
	///                       On return this parameter will be destroyed and replaced with a dict
	///                       containing options that were not found. May be NULL.
	///                       The same options must be passed later to avformat_write_header() for output
	///                       devices or avformat_open_input() for input devices, or at any other place
	///                       that affects device-private options.
	///
	/// @return >= 0 on success, negative otherwise.
	static AVDeviceCapabilitiesQuery^ Create(AVFormatContext^ s, AVDictionary^ device_options);
};
//////////////////////////////////////////////////////
/// Structure describes basic parameters of the device.
public ref class AVDeviceInfo : public AVBase
{
internal:
	AVDeviceInfo(void * _pointer,AVBase^ _parent);
public:
	///< device name, format depends on device
    property String^ device_name { String^ get(); }
	///< human friendly name
    property String^ device_description { String^ get(); }
public:
	virtual String^ ToString() override;
};
//////////////////////////////////////////////////////
/// List of devices.
public ref class AVDeviceInfoList : public AVBase
								  , public System::Collections::IEnumerable
								  , public System::Collections::Generic::IEnumerable<AVDeviceInfo^>
{
private:
	ref class AVDevicesEnumerator : public System::Collections::IEnumerator
							 	  , public System::Collections::Generic::IEnumerator<AVDeviceInfo^>
	{
	protected:
		int					m_nIndex;
		AVDeviceInfoList^	m_pArray;
	public:
		AVDevicesEnumerator(AVDeviceInfoList^ _array);
		~AVDevicesEnumerator();
	public:
		// IEnumerator
		virtual bool MoveNext();
		virtual property AVDeviceInfo^ Current { AVDeviceInfo^ get (); }
		virtual void Reset();
		virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
	};
internal:
	AVDeviceInfoList(void * _pointer,AVBase^ _parent);
public:
	///< list of autodetected devices
	property AVDeviceInfo^ default[int] { AVDeviceInfo^ get(int index); }
	///< number of autodetected devices
	property int Count { int get(); }
	property int nb_devices { int get(); }
	///< index of default device or -1 if no default
    property int default_device { int get(); }
public:
	array<AVDeviceInfo^>^ ToArray();
public:
	// IEnumerable
	virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator;
public:
	// IEnumerable<AVDeviceInfo>
	virtual System::Collections::Generic::IEnumerator<AVDeviceInfo^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVDeviceInfo^>::GetEnumerator;
};
//////////////////////////////////////////////////////
// AVDevices
public ref class AVDevices
{
private:
	AVDevices() {}
public:
	/// Audio input devices iterator.
	///
	/// If d is NULL, returns the first registered input audio/video device,
	/// if d is non-NULL, returns the next registered input audio/video device after d
	/// or NULL if d is the last one.
	static AVInputFormat^ input_audio_device_next(AVInputFormat^ d);

	/// Video input devices iterator.
	///
	/// If d is NULL, returns the first registered input audio/video device,
	/// if d is non-NULL, returns the next registered input audio/video device after d
	/// or NULL if d is the last one.
	static AVInputFormat^ input_video_device_next(AVInputFormat^ d);

	/// Audio output devices iterator.
	///
	/// If d is NULL, returns the first registered output audio/video device,
	/// if d is non-NULL, returns the next registered output audio/video device after d
	/// or NULL if d is the last one.
	static AVOutputFormat^ output_audio_device_next(AVOutputFormat^ d);

	/// Video output devices iterator.
	///
	/// If d is NULL, returns the first registered output audio/video device,
	/// if d is non-NULL, returns the next registered output audio/video device after d
	/// or NULL if d is the last one.
	static AVOutputFormat^ output_video_device_next(AVOutputFormat^ d);
public:
	/// Send control message from application to device.
	///
	/// @param s         device context.
	/// @param type      message type.
	/// @param data      message data. Exact type depends on message type.
	/// @param data_size size of message data.
	/// @return >= 0 on success, negative on error.
	///         AVERROR(ENOSYS) when device doesn't implement handler of the message.
	static AVRESULT app_to_dev_control_message(AVFormatContext^ s, AVAppToDevMessageType type, IntPtr data, int data_size);
	static AVRESULT app_to_dev_control_message(AVFormatContext^ s, AVAppToDevMessageType type, AVBase^ structure);
	static AVRESULT app_to_dev_control_message(AVFormatContext^ s, AVAppToDevMessageType type, double value);
	static AVRESULT app_to_dev_control_message(AVFormatContext^ s, AVAppToDevMessageType type);

	/// Send control message from device to application.
	///
	/// @param s         device context.
	/// @param type      message type.
	/// @param data      message data. Can be NULL.
	/// @param data_size size of message data.
	/// @return >= 0 on success, negative on error.
	///         AVERROR(ENOSYS) when application doesn't implement handler of the message.
	static AVRESULT dev_to_app_control_message(AVFormatContext^ s, AVDevToAppMessageType type, IntPtr data, int data_size);
	static AVRESULT dev_to_app_control_message(AVFormatContext^ s, AVDevToAppMessageType type, AVBase^ structure);
	static AVRESULT dev_to_app_control_message(AVFormatContext^ s, AVDevToAppMessageType type, double value);
	static AVRESULT dev_to_app_control_message(AVFormatContext^ s, AVDevToAppMessageType type, int value);
	static AVRESULT dev_to_app_control_message(AVFormatContext^ s, AVDevToAppMessageType type, Int64 value);
	static AVRESULT dev_to_app_control_message(AVFormatContext^ s, AVDevToAppMessageType type);
public:
	/// List devices.
	///
	/// Returns available device names and their parameters.
	///
	/// @note: Some devices may accept system-dependent device names that cannot be
	///        autodetected. The list returned by this function cannot be assumed to
	///        be always completed.
	///
	/// @param s                device context.
	/// @param[out] device_list list of autodetected devices.
	/// @return count of autodetected devices, negative on error.
	static AVDeviceInfoList^ list_devices(AVFormatContext^ s);
	/// List devices.
	///
	/// Returns available device names and their parameters.
	/// These are convinient wrappers for avdevice_list_devices().
	/// Device context is allocated and deallocated internally.
	///
	/// @param device           device format. May be NULL if device name is set.
	/// @param device_name      device name. May be NULL if device format is set.
	/// @param device_options   An AVDictionary filled with device-private options. May be NULL.
	///                         The same options must be passed later to avformat_write_header() for output
	///                         devices or avformat_open_input() for input devices, or at any other place
	///                         that affects device-private options.
	/// @param[out] device_list list of autodetected devices
	/// @return count of autodetected devices, negative on error.
	/// @note device argument takes precedence over device_name when both are set.
	static AVDeviceInfoList^ list_input_sources(AVInputFormat^ device, String^ device_name,AVDictionary^ device_options);
	static AVDeviceInfoList^ list_output_sinks(AVOutputFormat^ device, String^ device_name,AVDictionary^ device_options);
};
//////////////////////////////////////////////////////
#pragma endregion
//////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////