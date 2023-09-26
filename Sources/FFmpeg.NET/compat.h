//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper
// Copyright (C) 2018 Maxim Kartavenkov (mkartavenkov@gmail.com)
//
// The FFmpeg.NET Wrapper of ffmpeg libraries is provided "as is" 
// without warranty of any kind; without even the implied warranty 
// of merchantability or fitness for a particular purpose. 
//
// Usage terms described in attached license text file
//
// Compatibility of the different ffmpeg versions 
//////////////////////////////////////////////////////
#pragma once 
//////////////////////////////////////////////////////
#ifndef FF_API_LAVF_AVCTX
#	define FF_API_LAVF_AVCTX                (LIBAVFORMAT_VERSION_MAJOR < 59)
#endif
#ifndef FF_API_AVPICTURE
#	define FF_API_AVPICTURE				    (LIBAVCODEC_VERSION_MAJOR < 59)
#endif
#ifndef FF_API_OLD_BSF
#	define FF_API_OLD_BSF					(LIBAVCODEC_VERSION_MAJOR < 59)
#endif
#ifndef FF_API_FIFO_OLD_API
#	define FF_API_FIFO_OLD_API              (LIBAVUTIL_VERSION_MAJOR < 58)
#endif
#ifndef FF_API_OLD_CHANNEL_LAYOUT
#	define FF_API_OLD_CHANNEL_LAYOUT		(LIBAVUTIL_VERSION_MAJOR < 58)
#endif
#ifndef FF_API_AVIOCONTEXT_WRITTEN
#	define FF_API_AVIOCONTEXT_WRITTEN       (LIBAVFORMAT_VERSION_MAJOR < 60)
#endif
#ifndef FF_API_BUFFERSINK_ALLOC
#	define FF_API_BUFFERSINK_ALLOC          (LIBAVFILTER_VERSION_MAJOR < 9)
#endif
#ifndef FF_API_SUB_TEXT_FORMAT
#	define FF_API_SUB_TEXT_FORMAT			(LIBAVCODEC_VERSION_MAJOR < 60)
#endif
#ifndef FF_API_DEBUG_MV
#	define FF_API_DEBUG_MV					(LIBAVCODEC_VERSION_MAJOR < 60)
#endif
#ifndef FF_API_THREAD_SAFE_CALLBACKS
#	define FF_API_THREAD_SAFE_CALLBACKS		(LIBAVCODEC_VERSION_MAJOR < 60)
#endif
#ifndef FF_API_DEVICE_CAPABILITIES
#	define FF_API_DEVICE_CAPABILITIES		(LIBAVDEVICE_VERSION_MAJOR < 60)
#endif
//////////////////////////////////////////////////////
#if (LIBAVFILTER_VERSION_MAJOR >= 8) 
//////////////////////////////////////////////////////
#ifndef FF_API_OLD_CHANNEL_LAYOUT
#	define FF_API_OLD_CHANNEL_LAYOUT 1
#endif
//////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////
#if !FF_API_BUFFERSINK_ALLOC
//////////////////////////////////////////////////////
typedef struct AVBufferSinkParams {
	const enum AVPixelFormat *pixel_fmts;
} AVBufferSinkParams;
//////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////
#if !FF_API_DEVICE_CAPABILITIES
//////////////////////////////////////////////////////
typedef struct AVDeviceCapabilitiesQuery {
	const AVClass *av_class;
	AVFormatContext *device_context;
	enum AVCodecID codec;
	enum AVSampleFormat sample_format;
	enum AVPixelFormat pixel_format;
	int sample_rate;
	int channels;
	int64_t channel_layout;
	int window_width;
	int window_height;
	int frame_width;
	int frame_height;
	AVRational fps;
} AVDeviceCapabilitiesQuery;
//////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////
#if !FF_API_AVPICTURE
//////////////////////////////////////////////////////
typedef struct AVPicture {
	uint8_t *data[AV_NUM_DATA_POINTERS];
	int linesize[AV_NUM_DATA_POINTERS];
}AVPicture;
//////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////
#if !FF_API_OLD_BSF
//////////////////////////////////////////////////////
typedef struct AVBitStreamFilterContext {
	void *priv_data;
	const struct AVBitStreamFilter *filter;
	AVCodecParserContext *parser;
	struct AVBitStreamFilterContext *next;
	char *args;
} AVBitStreamFilterContext;
//////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////
#if !FF_API_FIFO_OLD_API
//////////////////////////////////////////////////////
typedef struct AVFifoBuffer {
	int unused;
}AVFifoBuffer;
//////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////
typedef struct AVFifo {
	int unused;
}AVFifo;
//////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////
#ifndef AVFORMAT_URL_H
//////////////////////////////////////////////////////
typedef struct URLProtocol {
	const char *name;
	void * dummy[14];
	int priv_data_size;
	const AVClass *priv_data_class;
	int flags;
} URLProtocol;
//////////////////////////////////////////////////////
typedef struct URLContext {
	const AVClass *av_class;
	const struct URLProtocol *prot;
	void *priv_data;
	char *filename;
	int flags;
	int max_packet_size;
	int is_streamed;
	int is_connected;
	AVIOInterruptCB interrupt_callback;
	int64_t rw_timeout;
} URLContext;
//////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////