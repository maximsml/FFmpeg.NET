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
#if (LIBAVCODEC_VERSION_MAJOR < 56)
//////////////////////////////////////////////////////
#ifndef FF_API_AVPICTURE
#	define FF_API_AVPICTURE 1
#endif
#ifndef FF_API_OLD_BSF
#	define FF_API_OLD_BSF 1
#endif
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