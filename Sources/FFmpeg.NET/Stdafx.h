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
#	ifndef __STDC_FORMAT_MACROS
#		define __STDC_FORMAT_MACROS
#   endif
#	pragma warning ( push )
#	pragma warning (disable:4244 )
#	include <inttypes.h>
#	include <libavutil/version.h>
#	include <libavcodec/version.h>
#	include <libavdevice/version.h>
#	include <libavfilter/version.h>
#	include <libavformat/version.h>
#	include <libpostproc/version.h>
#	include <libswresample/version.h>
#	include <libswscale/version.h>
#	include <libavutil/imgutils.h>
#	include <libavutil/samplefmt.h>
#	include <libavutil/opt.h>
#	include <libavutil/dict.h>
#	include <libavutil/crc.h>
#	include <libavutil/time.h>
#	include <libavutil/fifo.h>
#	include <libavutil/random_seed.h>
#	include <libavutil/audio_fifo.h>
#	include <libavutil/timestamp.h>
#	include <libavutil/base64.h>
#	include <libavcodec/avcodec.h>
#	include <libavcodec/avfft.h>
#	include <libavformat/avformat.h>
#	include <libavformat/avio.h>
#	include <libswscale/swscale.h>
#	include <libswresample/swresample.h>
#	include <libavfilter/avfilter.h>
#	include <libavfilter/buffersink.h>
#	include <libavfilter/buffersrc.h>
#	include <libavdevice/avdevice.h>
#	include <libpostproc/postprocess.h>
#if (LIBAVUTIL_VERSION_MAJOR > 52)
#	include <libavutil/motion_vector.h>
#	include <libavutil/stereo3d.h>
#	include <libavutil/hash.h>
#endif
#if (LIBAVCODEC_VERSION_MAJOR > 58)
#	include <libavcodec/bsf.h>
#endif
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