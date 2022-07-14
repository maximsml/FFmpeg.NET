// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently
//////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////
#include <atlbase.h>
//////////////////////////////////////////////////////
extern "C" {
//////////////////////////////////////////////////////
#ifndef AVFORMAT_AVFORMAT_H
#	ifndef _XKEYCHECK_H
#		define _XKEYCHECK_H
#	endif
#	ifndef __STDC_LIMIT_MACROS
#		define __STDC_LIMIT_MACROS
#	endif
#	ifndef __STDC_CONSTANT_MACROS
#		define __STDC_CONSTANT_MACROS
#	endif
#	pragma warning ( push )
#	pragma warning (disable:4244 )
#	include <libavutil/imgutils.h>
#	include <libavutil/samplefmt.h>
#	include <libavutil/opt.h>
#	include <libavutil/dict.h>
#	include <libavutil/crc.h>
#	include <libavutil/time.h>
#	include <libavutil/random_seed.h>
#	include <libavutil/audio_fifo.h>
#	include <libavutil/motion_vector.h>
#	include <libavutil/stereo3d.h>
#	include <libavutil/timestamp.h>
#	include <libavcodec/avcodec.h>
#	include <libavformat/avformat.h>
#	include <libavformat/avio.h>
#	include <libswscale/swscale.h>
#	include <libswresample/swresample.h>
#	include <libavfilter/avfilter.h>
#	include <libavfilter/buffersink.h>
#	include <libavfilter/buffersrc.h>
#	include <libavdevice/avdevice.h>
#	include <libpostproc/postprocess.h>
#ifdef HAVE_FFMPEG_SOURCES
#	include <libavformat/url.h>
#endif
#	pragma warning ( pop )
#endif
//////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////
#include <vcclr.h>
//////////////////////////////////////////////////////
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"swresample.lib")
//////////////////////////////////////////////////////
#include "compat.h"
//////////////////////////////////////////////////////