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
// libswresample
// Audio resampling, sample format conversion and mixing library.
//////////////////////////////////////////////////////
#include "stdafx.h"
#include "SWResample.h"
//////////////////////////////////////////////////////
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
//////////////////////////////////////////////////////
#pragma warning(disable: 4996) // marked as deprecated
//////////////////////////////////////////////////////
private struct SwrContext {};
//////////////////////////////////////////////////////
// LibSWResample
//////////////////////////////////////////////////////
FFmpeg::LibSWResample::LibSWResample()
{
}
//////////////////////////////////////////////////////
UInt32 FFmpeg::LibSWResample::Version::get()
{
	return swresample_version();
}
String^ FFmpeg::LibSWResample::Configuration::get()
{
	auto p = swresample_configuration();
	return p != nullptr ? gcnew String(p) : nullptr;
}
String^ FFmpeg::LibSWResample::License::get()
{
	auto p = swresample_license();
	return p != nullptr ? gcnew String(p) : nullptr;
}
//////////////////////////////////////////////////////
// SwrContext
//////////////////////////////////////////////////////
FFmpeg::AVClass^ FFmpeg::SwrContext::Class::get()
{
	auto p = swr_get_class();
	return p != nullptr ? gcnew AVClass((void*)p,nullptr) : nullptr;
}
//////////////////////////////////////////////////////
FFmpeg::SwrContext::SwrContext()
	: AVBase(nullptr,nullptr)
{
	m_pPointer = swr_alloc();
	m_pFreep = (TFreeFNP *)swr_free;
}
FFmpeg::SwrContext::SwrContext(AVChannelLayout out_ch_layout, AVSampleFormat out_sample_fmt, int out_sample_rate,
		   AVChannelLayout in_ch_layout, AVSampleFormat  in_sample_fmt, int  in_sample_rate,
		   int log_offset, IntPtr log_ctx)
	: AVBase(nullptr,nullptr)
{
	Init(out_ch_layout,out_sample_fmt, out_sample_rate,
		in_ch_layout,in_sample_fmt,in_sample_rate,
		log_offset,log_ctx);
}
FFmpeg::SwrContext::SwrContext(AVChannelLayout out_ch_layout, AVSampleFormat out_sample_fmt, int out_sample_rate,
		   AVChannelLayout in_ch_layout, AVSampleFormat  in_sample_fmt, int  in_sample_rate)
	: AVBase(nullptr,nullptr)
{
	Init(out_ch_layout,out_sample_fmt, out_sample_rate,
		in_ch_layout,in_sample_fmt,in_sample_rate,
		0,IntPtr::Zero);
}
//////////////////////////////////////////////////////
void FFmpeg::SwrContext::Init(AVChannelLayout out_ch_layout, AVSampleFormat out_sample_fmt, int out_sample_rate,
		  AVChannelLayout in_ch_layout, AVSampleFormat  in_sample_fmt, int  in_sample_rate,
		  int log_offset, IntPtr log_ctx)
{
	m_pPointer = swr_alloc_set_opts(nullptr,
		(int64_t)out_ch_layout,(::AVSampleFormat)out_sample_fmt,out_sample_rate,
		(int64_t)in_ch_layout,(::AVSampleFormat)in_sample_fmt,in_sample_rate,
		log_offset,log_ctx.ToPointer()
		);
	m_pFreep = (TFreeFNP *)swr_free;
}
//////////////////////////////////////////////////////
FFmpeg::AVChannelLayout FFmpeg::SwrContext::OutChannelLayout::get()
{
	int64_t val;
	if (0 >= av_opt_get_channel_layout(m_pPointer,"out_channel_layout",0,&val))
	{
		return (AVChannelLayout)val;
	}
	return (AVChannelLayout)0;
}
void FFmpeg::SwrContext::OutChannelLayout::set(AVChannelLayout value)
{
	av_opt_set_channel_layout(m_pPointer,"out_channel_layout",(int64_t)value,0);
}
FFmpeg::AVSampleFormat FFmpeg::SwrContext::OutSampleFormat::get()
{
	::AVSampleFormat val;
	if (0 >= av_opt_get_sample_fmt(m_pPointer,"out_sample_fmt",0,&val))
	{
		return (AVSampleFormat)val;
	}
	return (AVSampleFormat)0;
}
void FFmpeg::SwrContext::OutSampleFormat::set(AVSampleFormat value)
{
	av_opt_set_sample_fmt(m_pPointer,"out_sample_fmt",(::AVSampleFormat)value,0);
}
int FFmpeg::SwrContext::OutSampleRate::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"out_sample_rate",0,&val))
	{
		return (int)val;
	}
	return 0;
}
void FFmpeg::SwrContext::OutSampleRate::set(int value)
{
	av_opt_set_int(m_pPointer,"out_sample_rate",(int64_t)value,0);
}
FFmpeg::AVChannelLayout FFmpeg::SwrContext::InChannelLayout::get()
{
	int64_t val;
	if (0 >= av_opt_get_channel_layout(m_pPointer,"in_channel_layout",0,&val))
	{
		return (AVChannelLayout)val;
	}
	return (AVChannelLayout)0;
}
void FFmpeg::SwrContext::InChannelLayout::set(AVChannelLayout value)
{
	av_opt_set_channel_layout(m_pPointer,"in_channel_layout",(int64_t)value,0);
}
FFmpeg::AVSampleFormat FFmpeg::SwrContext::InSampleFormat::get()
{
	::AVSampleFormat val;
	if (0 >= av_opt_get_sample_fmt(m_pPointer,"in_sample_fmt",0,&val))
	{
		return (AVSampleFormat)val;
	}
	return (AVSampleFormat)0;
}
void FFmpeg::SwrContext::InSampleFormat::set(AVSampleFormat value)
{
	av_opt_set_sample_fmt(m_pPointer,"in_sample_fmt",(::AVSampleFormat)value,0);
}
int FFmpeg::SwrContext::InSampleRate::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"in_sample_rate",0,&val))
	{
		return (int)val;
	}
	return 0;
}
void FFmpeg::SwrContext::InSampleRate::set(int value)
{
	av_opt_set_int(m_pPointer,"in_sample_rate",(int64_t)value,0);
}
FFmpeg::SwrFlags FFmpeg::SwrContext::Flags::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"flags",0,&val))
	{
		return (FFmpeg::SwrFlags)val;
	}
	return (FFmpeg::SwrFlags)0;
}
void FFmpeg::SwrContext::Flags::set(SwrFlags value)
{
	av_opt_set_int(m_pPointer,"flags",(int64_t)value,0);
}
float FFmpeg::SwrContext::DitherScale::get()
{
	double val;
	if (0 >= av_opt_get_double(m_pPointer,"dither_scale",0,&val))
	{
		return (float)val;
	}
	return (float)0;
}
void FFmpeg::SwrContext::DitherScale::set(float value)
{
	av_opt_set_double(m_pPointer,"dither_scale",(double)value,0);
}
FFmpeg::SwrDitherType FFmpeg::SwrContext::Dither::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"dither_method",0,&val))
	{
		return (FFmpeg::SwrDitherType)val;
	}
	return (FFmpeg::SwrDitherType)0;
}
void FFmpeg::SwrContext::Dither::set(SwrDitherType value)
{
	av_opt_set_int(m_pPointer,"dither_method",(int64_t)value,0);
}
int FFmpeg::SwrContext::FilterSize::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"filter_size",0,&val))
	{
		return (int)val;
	}
	return 0;
}
void FFmpeg::SwrContext::FilterSize::set(int value)
{
	av_opt_set_int(m_pPointer,"filter_size",(int64_t)value,0);
}
int FFmpeg::SwrContext::PhaseShift::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"phase_shift",0,&val))
	{
		return (int)val;
	}
	return 0;
}
void FFmpeg::SwrContext::PhaseShift::set(int value)
{
	av_opt_set_int(m_pPointer,"phase_shift",(int64_t)value,0);
}
bool FFmpeg::SwrContext::LinearInterp::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"linear_interp",0,&val))
	{
		return val != 0;
	}
	return false;
}
void FFmpeg::SwrContext::LinearInterp::set(bool value)
{
	av_opt_set_int(m_pPointer,"linear_interp",value ? 1 : 0,0);
}
double FFmpeg::SwrContext::CutOff::get()
{
	double val;
	if (0 >= av_opt_get_double(m_pPointer,"cutoff",0,&val))
	{
		return (double)val;
	}
	return (double)0;
}
void FFmpeg::SwrContext::CutOff::set(double value)
{
	av_opt_set_double(m_pPointer,"cutoff",(double)value,0);
}
int FFmpeg::SwrContext::OutChannelCount::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"out_channel_count",0,&val))
	{
		return (int)val;
	}
	return 0;
}
void FFmpeg::SwrContext::OutChannelCount::set(int value)
{
	av_opt_set_int(m_pPointer,"out_channel_count",(int64_t)value,0);
}
int FFmpeg::SwrContext::InChannelCount::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"in_channel_count",0,&val))
	{
		return (int)val;
	}
	return 0;
}
void FFmpeg::SwrContext::InChannelCount::set(int value)
{
	av_opt_set_int(m_pPointer,"in_channel_count",(int64_t)value,0);
}
int FFmpeg::SwrContext::UsedChannelCount::get()
{
	int64_t val;
	if (0 >= av_opt_get_int(m_pPointer,"used_channel_count",0,&val))
	{
		return (int)val;
	}
	return 0;
}
void FFmpeg::SwrContext::UsedChannelCount::set(int value)
{
	av_opt_set_int(m_pPointer,"used_channel_count",(int64_t)value,0);
}
FFmpeg::AVSampleFormat FFmpeg::SwrContext::InternalSampleFormat::get()
{
	::AVSampleFormat val;
	if (0 >= av_opt_get_sample_fmt(m_pPointer,"internal_sample_fmt",0,&val))
	{
		return (AVSampleFormat)val;
	}
	return (AVSampleFormat)0;
}
void FFmpeg::SwrContext::InternalSampleFormat::set(AVSampleFormat value)
{
	av_opt_set_sample_fmt(m_pPointer,"internal_sample_fmt",(::AVSampleFormat)value,0);
}
float FFmpeg::SwrContext::CenterMixLevel::get()
{
	double val;
	if (0 >= av_opt_get_double(m_pPointer,"center_mix_level",0,&val))
	{
		return (float)val;
	}
	return (float)0;
}
void FFmpeg::SwrContext::CenterMixLevel::set(float value)
{
	av_opt_set_double(m_pPointer,"center_mix_level",(double)value,0);
}
float FFmpeg::SwrContext::SurroundMixLevel::get()
{
	double val;
	if (0 >= av_opt_get_double(m_pPointer,"surround_mix_level",0,&val))
	{
		return (float)val;
	}
	return (float)0;
}
void FFmpeg::SwrContext::SurroundMixLevel::set(float value)
{
	av_opt_set_double(m_pPointer,"surround_mix_level",(double)value,0);
}
float FFmpeg::SwrContext::LfeMixLevel::get()
{
	double val;
	if (0 >= av_opt_get_double(m_pPointer,"lfe_mix_level",0,&val))
	{
		return (float)val;
	}
	return (float)0;
}
void FFmpeg::SwrContext::LfeMixLevel::set(float value)
{
	av_opt_set_double(m_pPointer,"lfe_mix_level",(double)value,0);
}
float FFmpeg::SwrContext::RematrixVolume::get()
{
	double val;
	if (0 >= av_opt_get_double(m_pPointer,"rematrix_volume",0,&val))
	{
		return (float)val;
	}
	return (float)0;
}
void FFmpeg::SwrContext::RematrixVolume::set(float value)
{
	av_opt_set_double(m_pPointer,"rematrix_volume",(double)value,0);
}
float FFmpeg::SwrContext::RematrixMaxval::get()
{
	double val;
	if (0 >= av_opt_get_double(m_pPointer,"rematrix_maxval",0,&val))
	{
		return (float)val;
	}
	return (float)0;
}
void FFmpeg::SwrContext::RematrixMaxval::set(float value)
{
	av_opt_set_double(m_pPointer,"rematrix_maxval",(double)value,0);
}
//////////////////////////////////////////////////////
bool FFmpeg::SwrContext::IsInitialized::get()
{
	return 0 < swr_is_initialized((::SwrContext*)m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::SwrContext::Init()
{
	return swr_init((::SwrContext*)m_pPointer);
}

void FFmpeg::SwrContext::Close()
{
	swr_close((::SwrContext*)m_pPointer);
}

FFmpeg::AVRESULT FFmpeg::SwrContext::Configure(FFmpeg::AVFrame^ out, FFmpeg::AVFrame^ in)
{
	return swr_config_frame((::SwrContext*)m_pPointer,(::AVFrame *)out->_Pointer.ToPointer(),(::AVFrame *)in->_Pointer.ToPointer());
}

int FFmpeg::SwrContext::Convert(IntPtr out, int out_count, IntPtr in, int in_count)
{
	uint8_t * _out[8] = { (uint8_t *)out.ToPointer(), 0, 0, 0, 0, 0, 0, 0 };
	uint8_t * _in[8] = { (uint8_t *)in.ToPointer(), 0, 0, 0, 0, 0, 0, 0 };
	return swr_convert((::SwrContext*)m_pPointer, _out, out_count, (const uint8_t**)_in, in_count);
}

int FFmpeg::SwrContext::Convert(array<IntPtr>^ out, int out_count, array<IntPtr>^ in, int in_count)
{
	uint8_t * _out[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t * _in[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < out->Length; i++){ _out[i] = (uint8_t *)out[i].ToPointer(); }
	if (in != nullptr)
	{
		for (int i = 0; i < in->Length; i++) { _in[i] = (uint8_t *)in[i].ToPointer(); }
	}
	return swr_convert((::SwrContext*)m_pPointer, _out, out_count, in != nullptr ? (const uint8_t**)_in : nullptr, in_count);
}

int FFmpeg::SwrContext::Convert(array<AVMemPtr^>^ out, int out_count, array<AVMemPtr^>^ in, int in_count)
{
	uint8_t * _out[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t * _in[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < out->Length; i++){ _out[i] = (uint8_t *)((IntPtr)out[i]).ToPointer(); }
	if (in != nullptr)
	{
		for (int i = 0; i < in->Length; i++) { _in[i] = (uint8_t *)((IntPtr)in[i]).ToPointer(); }
	}
	return swr_convert((::SwrContext*)m_pPointer, _out, out_count, in != nullptr ? (const uint8_t**)_in : nullptr, in_count);
}

int FFmpeg::SwrContext::Convert(array<IntPtr>^ out, int out_count, array<AVMemPtr^>^ in, int in_count)
{
	uint8_t * _out[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t * _in[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < out->Length; i++){ _out[i] = (uint8_t *)out[i].ToPointer(); }
	if (in != nullptr)
	{
		for (int i = 0; i < in->Length; i++) { _in[i] = (uint8_t *)((IntPtr)in[i]).ToPointer(); }
	}
	return swr_convert((::SwrContext*)m_pPointer, _out, out_count, in != nullptr ? (const uint8_t**)_in : nullptr, in_count);
}

int FFmpeg::SwrContext::Convert(AVArray<AVMemPtr^>^ out, int out_count, AVArray<AVMemPtr^>^ in, int in_count)
{
	uint8_t * _out[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t * _in[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < out->Count; i++){ _out[i] = (uint8_t *)((IntPtr)out[i]).ToPointer(); }
	if (in != nullptr)
	{
		for (int i = 0; i < in->Count; i++) { _in[i] = (uint8_t *)((IntPtr)in[i]).ToPointer(); }
	}
	return swr_convert((::SwrContext*)m_pPointer, _out, out_count, in != nullptr ? (const uint8_t**)_in : nullptr, in_count);
}

int FFmpeg::SwrContext::Convert(array<IntPtr>^ out, int out_count, AVArray<AVMemPtr^>^ in, int in_count)
{
	uint8_t * _out[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t * _in[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < out->Length; i++){ _out[i] = (uint8_t *)((IntPtr)out[i]).ToPointer(); }
	if (in != nullptr)
	{
		for (int i = 0; i < in->Count; i++) { _in[i] = (uint8_t *)((IntPtr)in[i]).ToPointer(); }
	}
	return swr_convert((::SwrContext*)m_pPointer, _out, out_count, in != nullptr ? (const uint8_t**)_in : nullptr, in_count);
}

int FFmpeg::SwrContext::Convert(AVArray<AVMemPtr^>^ out, int out_count, array<IntPtr>^ in, int in_count)
{
	uint8_t * _out[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t * _in[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < out->Count; i++){ _out[i] = (uint8_t *)((IntPtr)out[i]).ToPointer(); }
	if (in != nullptr)
	{
		for (int i = 0; i < in->Length; i++) { _in[i] = (uint8_t *)((IntPtr)in[i]).ToPointer(); }
	}
	return swr_convert((::SwrContext*)m_pPointer, _out, out_count, in != nullptr ? (const uint8_t**)_in : nullptr, in_count);
}

int FFmpeg::SwrContext::Convert(array<AVMemPtr^>^ out, int out_count, array<IntPtr>^ in, int in_count)
{
	uint8_t * _out[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t * _in[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < out->Length; i++){ _out[i] = (uint8_t *)((IntPtr)out[i]).ToPointer(); }
	if (in != nullptr)
	{
		for (int i = 0; i < in->Length; i++) { _in[i] = (uint8_t *)((IntPtr)in[i]).ToPointer(); }
	}
	return swr_convert((::SwrContext*)m_pPointer, _out, out_count, in != nullptr ? (const uint8_t**)_in : nullptr, in_count);
}

int FFmpeg::SwrContext::Convert(array<AVMemPtr^>^ out, int out_count, AVArray<AVMemPtr^>^ in, int in_count)
{
	uint8_t * _out[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t * _in[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < out->Length; i++){ _out[i] = (uint8_t *)((IntPtr)out[i]).ToPointer(); }
	if (in != nullptr)
	{
		for (int i = 0; i < in->Count; i++) { _in[i] = (uint8_t *)((IntPtr)in[i]).ToPointer(); }
	}
	return swr_convert((::SwrContext*)m_pPointer, _out, out_count, in != nullptr ? (const uint8_t**)_in : nullptr, in_count);
}

int FFmpeg::SwrContext::Convert(array<IntPtr>^ out, int out_count, AVFrame^ frame)
{
	return Convert(out, out_count, frame != nullptr ? frame->data : nullptr,frame != nullptr ? frame->nb_samples : 0);
}

int FFmpeg::SwrContext::Convert(AVArray<AVMemPtr^>^ out, int out_count, AVFrame^ frame)
{
	return Convert(out, out_count, frame != nullptr ? frame->data : nullptr,frame != nullptr ? frame->nb_samples : 0);
}

int FFmpeg::SwrContext::Convert(array<AVMemPtr^>^ out, int out_count, AVFrame^ frame)
{
	return Convert(out, out_count, frame != nullptr ? frame->data : nullptr,frame != nullptr ? frame->nb_samples : 0);
}

FFmpeg::AVRESULT FFmpeg::SwrContext::Convert(AVFrame^ output,AVFrame^ input)
{
	return swr_convert_frame((::SwrContext*)m_pPointer,(::AVFrame *)output->_Pointer.ToPointer(),(::AVFrame *)input->_Pointer.ToPointer());
}

Int64 FFmpeg::SwrContext::NextPts(Int64 pts)
{
	return swr_next_pts((::SwrContext*)m_pPointer,pts);
}

FFmpeg::AVRESULT FFmpeg::SwrContext::SetCompensation(int sample_delta, int compensation_distance)
{
	return swr_set_compensation((::SwrContext*)m_pPointer,sample_delta,compensation_distance);
}

FFmpeg::AVRESULT FFmpeg::SwrContext::SetChannelMapping(array<int>^ channel_map)
{
	if (channel_map == nullptr || channel_map->Length == 0) return -1;
	int _map[8];
	memset(_map,0x00,sizeof(_map));
	for (int i = 0; i < channel_map->Length && i < _countof(_map); i++) { _map[i] = channel_map[i]; }
	return swr_set_channel_mapping((::SwrContext*)m_pPointer, _map);
}

FFmpeg::AVRESULT FFmpeg::SwrContext::SetMatrix(array<double>^ matrix, int stride)
{
	if (matrix == nullptr || matrix->Length == 0) return -1;
	double * _matrix = (double *)av_mallocz(matrix->Length * sizeof(double));
	for (int i = 0; i < matrix->Length; i++) { _matrix[i] = matrix[i]; }
	try
	{
		return swr_set_matrix((::SwrContext*)m_pPointer,_matrix,stride);
	}
	finally
	{
		av_free(_matrix);
	}
}

FFmpeg::AVRESULT FFmpeg::SwrContext::DropOutput(int count)
{
	return swr_drop_output((::SwrContext*)m_pPointer,count);
}

FFmpeg::AVRESULT FFmpeg::SwrContext::InjectSilence(int count)
{
	return swr_inject_silence((::SwrContext*)m_pPointer,count);
}

Int64 FFmpeg::SwrContext::GetDelay(Int64 base)
{
	return swr_get_delay((::SwrContext*)m_pPointer,base);
}

int FFmpeg::SwrContext::GetOutSamples(int in_samples)
{
	return swr_get_out_samples((::SwrContext*)m_pPointer, in_samples);
}
//////////////////////////////////////////////////////
array<double>^ FFmpeg::SwrContext::BuildMatrix(AVChannelLayout in_layout,
	AVChannelLayout out_layout,
	double center_mix_level, double surround_mix_level,
	double lfe_mix_level, double rematrix_maxval,
	double rematrix_volume, int stride,
	AVMatrixEncoding matrix_encoding, IntPtr log_ctx)
{
	double * _matrix = (double *)av_mallocz(stride * stride * sizeof(double));
	try
	{
		array<double>^ matrix = nullptr;
		if (0 <= swr_build_matrix((uint64_t)in_layout,
			(uint64_t)out_layout,
			center_mix_level, surround_mix_level,
			lfe_mix_level, rematrix_maxval,
			rematrix_volume, _matrix,stride,
			(::AVMatrixEncoding)matrix_encoding,log_ctx.ToPointer()))
		{
			matrix = gcnew array<double>(stride * stride);
			for (int i = 0; i < matrix->Length; i++) { matrix[i] = _matrix[i]; }
		}
		return matrix;
	}
	finally
	{
		av_free(_matrix);
	}
}
//////////////////////////////////////////////////////