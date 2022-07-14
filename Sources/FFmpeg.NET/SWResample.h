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
#pragma once
//////////////////////////////////////////////////////
using namespace System;
//////////////////////////////////////////////////////
#include "AVCodec.h"
//////////////////////////////////////////////////////
namespace FFmpeg {
//////////////////////////////////////////////////////
#pragma region Enums
//////////////////////////////////////////////////////
public enum class SwrFlags
{
	NONE = 0,
	RESAMPLE = 1,///< Force resampling even if equal sample rate
};
//////////////////////////////////////////////////////
/// Dithering algorithms
public enum class SwrDitherType 
{
	NONE = 0,
	RECTANGULAR,
	TRIANGULAR,
	TRIANGULAR_HIGHPASS,

	NS = 64,         ///< not part of API/ABI
	NS_LIPSHITZ,
	NS_F_WEIGHTED,
	NS_MODIFIED_E_WEIGHTED,
	NS_IMPROVED_E_WEIGHTED,
	NS_SHIBATA,
	NS_LOW_SHIBATA,
	NS_HIGH_SHIBATA,
};
//////////////////////////////////////////////////////
/// Resampling Engines
public enum class SwrEngine 
{
	SWR,             ///< SW Resampler
	SOXR,            ///< SoX Resampler
};
//////////////////////////////////////////////////////
/// Resampling Filter Types
public enum class SwrFilterType 
{
	CUBIC,              ///< Cubic
	BLACKMAN_NUTTALL,   ///< Blackman Nuttall Windowed Sinc
	KAISER,             ///< Kaiser Windowed Sinc
};
//////////////////////////////////////////////////////
#pragma endregion 
//////////////////////////////////////////////////////
#pragma region Classes
//////////////////////////////////////////////////////
// LibSWResample
public ref class LibSWResample
{
private:
	LibSWResample();
public:
	// Return the @ref LIBSWRESAMPLE_VERSION_INT constant.
	static property UInt32 Version { UInt32 get(); }
	// Return the swr build-time configuration.
	static property String^ Configuration { String^ get(); }
	// Return the swr license.
	static property String^ License { String^ get(); }
};
//////////////////////////////////////////////////////
// SwrContext
public ref class SwrContext : public AVBase
{
public:
	/// Get the AVClass for SwrContext. It can be used in combination with
	/// AV_OPT_SEARCH_FAKE_OBJ for examining options.
	static property AVClass^ Class { AVClass^ get(); }
public:
	/// Allocate SwrContext.
	///
	/// If you use this function you will need to set the parameters (manually or
	/// with swr_alloc_set_opts()) before calling swr_init().
	///
	/// @see swr_alloc_set_opts(), swr_init(), swr_free()
	/// @return NULL on error, allocated context otherwise
	SwrContext();

	/// Allocate SwrContext if needed and set/reset common parameters.
	///
	/// This function does not require s to be allocated with swr_alloc(). On the
	/// other hand, swr_alloc() can use swr_alloc_set_opts() to set the parameters
	/// on the allocated context.
	///
	/// @param s               existing Swr context if available, or NULL if not
	/// @param out_ch_layout   output channel layout (AV_CH_LAYOUT_///)
	/// @param out_sample_fmt  output sample format (AV_SAMPLE_FMT_///).
	/// @param out_sample_rate output sample rate (frequency in Hz)
	/// @param in_ch_layout    input channel layout (AV_CH_LAYOUT_///)
	/// @param in_sample_fmt   input sample format (AV_SAMPLE_FMT_///).
	/// @param in_sample_rate  input sample rate (frequency in Hz)
	/// @param log_offset      logging level offset
	/// @param log_ctx         parent logging context, can be NULL
	///
	/// @see swr_init(), swr_free()
	/// @return NULL on error, allocated context otherwise
	SwrContext(AVChannelLayout out_ch_layout, AVSampleFormat out_sample_fmt, int out_sample_rate,
		AVChannelLayout in_ch_layout, AVSampleFormat  in_sample_fmt, int  in_sample_rate,
		int log_offset, IntPtr log_ctx);

	SwrContext(AVChannelLayout out_ch_layout, AVSampleFormat out_sample_fmt, int out_sample_rate,
		AVChannelLayout in_ch_layout, AVSampleFormat  in_sample_fmt, int  in_sample_rate);

private:
	void Init(AVChannelLayout out_ch_layout, AVSampleFormat out_sample_fmt, int out_sample_rate,
		AVChannelLayout in_ch_layout, AVSampleFormat  in_sample_fmt, int  in_sample_rate,
		int log_offset, IntPtr log_ctx);
public:
	property AVChannelLayout OutChannelLayout { AVChannelLayout get(); void set(AVChannelLayout); }
	property AVSampleFormat OutSampleFormat { AVSampleFormat get(); void set(AVSampleFormat); }
	property int OutSampleRate { int get(); void set(int); }
	property AVChannelLayout InChannelLayout { AVChannelLayout get(); void set(AVChannelLayout); }
	property AVSampleFormat InSampleFormat { AVSampleFormat get(); void set(AVSampleFormat); }
	property int InSampleRate { int get(); void set(int); }
	property SwrFlags Flags { SwrFlags get(); void set(SwrFlags); }
	property float DitherScale { float get(); void set(float); }
	property SwrDitherType Dither { SwrDitherType get(); void set(SwrDitherType); }
	property int FilterSize { int get(); void set(int); }
	property int PhaseShift { int get(); void set(int); }
	property bool LinearInterp { bool get(); void set(bool); }
	property double CutOff { double get(); void set(double); }

	property int OutChannelCount { int get(); void set(int); }
	property int InChannelCount { int get(); void set(int); }
	property int UsedChannelCount { int get(); void set(int); }
	property AVSampleFormat InternalSampleFormat { AVSampleFormat get(); void set(AVSampleFormat); }
	property float CenterMixLevel { float get(); void set(float); }
	property float SurroundMixLevel { float get(); void set(float); }
	property float LfeMixLevel { float get(); void set(float); }
	property float RematrixVolume { float get(); void set(float); }
	property float RematrixMaxval { float get(); void set(float); }

public:
	/// Check whether an swr context has been initialized or not.
	property bool IsInitialized { bool get(); }
	/// Initialize context after user parameters have been set.
	/// @note The context must be configured using the AVOption API.
	AVRESULT Init();

	/// Closes the context so that swr_is_initialized() returns 0.
	///
	/// The context can be brought back to life by running swr_init(),
	/// swr_init() can also be used without swr_close().
	/// This function is mainly provided for simplifying the usecase
	/// where one tries to support libavresample and libswresample.
	void Close();

	/// Configure or reconfigure the SwrContext using the information
	/// provided by the AVFrames.
	///
	/// The original resampling context is reset even on failure.
	/// The function calls swr_close() internally if the context is open.
	///
	/// @see swr_close();
	///
	/// @param swr             audio resample context
	/// @param output          output AVFrame
	/// @param input           input AVFrame
	/// @return                0 on success, AVERROR on failure.
	AVRESULT Configure(AVFrame^ out, AVFrame^ in);

	/// Convert audio.
	///
	/// in and in_count can be set to 0 to flush the last few samples out at the
	/// end.
	///
	/// If more input is provided than output space then the input will be buffered.
	/// You can avoid this buffering by providing more output space than input.
	/// Conversion will run directly without copying whenever possible.
	///
	/// @param s         allocated Swr context, with parameters set
	/// @param out       output buffers, only the first one need be set in case of packed audio
	/// @param out_count amount of space available for output in samples per channel
	/// @param in        input buffers, only the first one need to be set in case of packed audio
	/// @param in_count  number of input samples available in one channel
	///
	/// @return number of samples output per channel, negative value on error
	int Convert(IntPtr out, int out_count, IntPtr in, int in_count);

	int Convert(array<IntPtr>^ out, int out_count, array<IntPtr>^ in, int in_count);
	int Convert(AVArray<AVMemPtr^>^ out, int out_count, AVArray<AVMemPtr^>^ in, int in_count);
	int Convert(array<IntPtr>^ out, int out_count, AVArray<AVMemPtr^>^ in, int in_count);
	int Convert(AVArray<AVMemPtr^>^ out, int out_count, array<IntPtr>^ in, int in_count);

	int Convert(array<IntPtr>^ out, int out_count, array<AVMemPtr^>^ in, int in_count);
	int Convert(array<AVMemPtr^>^ out, int out_count, array<AVMemPtr^>^ in, int in_count);
	int Convert(array<AVMemPtr^>^ out, int out_count, array<IntPtr>^ in, int in_count);
	int Convert(array<AVMemPtr^>^ out, int out_count, AVArray<AVMemPtr^>^ in, int in_count);

	int Convert(array<IntPtr>^ out, int out_count, AVFrame^ frame);
	int Convert(AVArray<AVMemPtr^>^ out, int out_count, AVFrame^ frame);
	int Convert(array<AVMemPtr^>^ out, int out_count, AVFrame^ frame);


	/// Convert the samples in the input AVFrame and write them to the output AVFrame.
	///
	/// Input and output AVFrames must have channel_layout, sample_rate and format set.
	///
	/// If the output AVFrame does not have the data pointers allocated the nb_samples
	/// field will be set using av_frame_get_buffer()
	/// is called to allocate the frame.
	///
	/// The output AVFrame can be NULL or have fewer allocated samples than required.
	/// In this case, any remaining samples not written to the output will be added
	/// to an internal FIFO buffer, to be returned at the next call to this function
	/// or to swr_convert().
	///
	/// If converting sample rate, there may be data remaining in the internal
	/// resampling delay buffer. swr_get_delay() tells the number of
	/// remaining samples. To get this data as output, call this function or
	/// swr_convert() with NULL input.
	///
	/// If the SwrContext configuration does not match the output and
	/// input AVFrame settings the conversion does not take place and depending on
	/// which AVFrame is not matching AVERROR_OUTPUT_CHANGED, AVERROR_INPUT_CHANGED
	/// or the result of a bitwise-OR of them is returned.
	///
	/// @see swr_delay()
	/// @see swr_convert()
	/// @see swr_get_delay()
	///
	/// @param swr             audio resample context
	/// @param output          output AVFrame
	/// @param input           input AVFrame
	/// @return                0 on success, AVERROR on failure or nonmatching
	///                        configuration.
	AVRESULT Convert(AVFrame^ output,AVFrame^ input);

	/// Convert the next timestamp from input to output
	/// timestamps are in 1/(in_sample_rate /// out_sample_rate) units.
	///
	/// @note There are 2 slightly differently behaving modes.
	///       @li When automatic timestamp compensation is not used, (min_compensation >= FLT_MAX)
	///              in this case timestamps will be passed through with delays compensated
	///       @li When automatic timestamp compensation is used, (min_compensation < FLT_MAX)
	///              in this case the output timestamps will match output sample numbers.
	///              See ffmpeg-resampler(1) for the two modes of compensation.
	///
	/// @param s[in]     initialized Swr context
	/// @param pts[in]   timestamp for the next input sample, INT64_MIN if unknown
	/// @see swr_set_compensation(), swr_drop_output(), and swr_inject_silence() are
	///      function used internally for timestamp compensation.
	/// @return the output timestamp for the next output sample
	Int64 NextPts(Int64 pts);

	/// Activate resampling compensation ("soft" compensation). This function is
	/// internally called when needed in swr_next_pts().
	///
	/// @param[in,out] s             allocated Swr context. If it is not initialized,
	///                              or SWR_FLAG_RESAMPLE is not set, swr_init() is
	///                              called with the flag set.
	/// @param[in]     sample_delta  delta in PTS per sample
	/// @param[in]     compensation_distance number of samples to compensate for
	/// @return    >= 0 on success, AVERROR error codes if:
	///            @li @c s is NULL,
	///            @li @c compensation_distance is less than 0,
	///            @li @c compensation_distance is 0 but sample_delta is not,
	///            @li compensation unsupported by resampler, or
	///            @li swr_init() fails when called.
	AVRESULT SetCompensation(int sample_delta, int compensation_distance);

	/// Set a customized input channel mapping.
	///
	/// @param[in,out] s           allocated Swr context, not yet initialized
	/// @param[in]     channel_map customized input channel mapping (array of channel
	///                            indexes, -1 for a muted channel)
	/// @return >= 0 on success, or AVERROR error code in case of failure.
	AVRESULT SetChannelMapping(array<int>^ channel_map);

	/// Set a customized remix matrix.
	///
	/// @param s       allocated Swr context, not yet initialized
	/// @param matrix  remix coefficients; matrix[i + stride /// o] is
	///                the weight of input channel i in output channel o
	/// @param stride  offset between lines of the matrix
	/// @return  >= 0 on success, or AVERROR error code in case of failure.
	AVRESULT SetMatrix(array<double>^ matrix, int stride);

	/// Drops the specified number of output samples.
	///
	/// This function, along with swr_inject_silence(), is called by swr_next_pts()
	/// if needed for "hard" compensation.
	///
	/// @param s     allocated Swr context
	/// @param count number of samples to be dropped
	///
	/// @return >= 0 on success, or a negative AVERROR code on failure
	AVRESULT DropOutput(int count);

	/// Injects the specified number of silence samples.
	///
	/// This function, along with swr_drop_output(), is called by swr_next_pts()
	/// if needed for "hard" compensation.
	///
	/// @param s     allocated Swr context
	/// @param count number of samples to be dropped
	///
	/// @return >= 0 on success, or a negative AVERROR code on failure
	AVRESULT InjectSilence(int count);

	/// Gets the delay the next input sample will experience relative to the next output sample.
	///
	/// Swresample can buffer data if more input has been provided than available
	/// output space, also converting between sample rates needs a delay.
	/// This function returns the sum of all such delays.
	/// The exact delay is not necessarily an integer value in either input or
	/// output sample rate. Especially when downsampling by a large value, the
	/// output sample rate may be a poor choice to represent the delay, similarly
	/// for upsampling and the input sample rate.
	///
	/// @param s     swr context
	/// @param base  timebase in which the returned delay will be:
	///              @li if it's set to 1 the returned delay is in seconds
	///              @li if it's set to 1000 the returned delay is in milliseconds
	///              @li if it's set to the input sample rate then the returned
	///                  delay is in input samples
	///              @li if it's set to the output sample rate then the returned
	///                  delay is in output samples
	///              @li if it's the least common multiple of in_sample_rate and
	///                  out_sample_rate then an exact rounding-free delay will be
	///                  returned
	/// @returns     the delay in 1 / @c base units.
	Int64 GetDelay(Int64 base);

	 /// Find an upper bound on the number of samples that the next swr_convert
	 /// call will output, if called with in_samples of input samples. This
	 /// depends on the internal state, and anything changing the internal state
	 /// (like further swr_convert() calls) will may change the number of samples
	 /// swr_get_out_samples() returns for the same number of input samples.
	 ///
	 /// @param in_samples    number of input samples.
	 /// @note any call to swr_inject_silence(), swr_convert(), swr_next_pts()
	 ///       or swr_set_compensation() invalidates this limit
	 /// @note it is recommended to pass the correct available buffer size
	 ///       to all functions like swr_convert() even if swr_get_out_samples()
	 ///       indicates that less would be used.
	 /// @returns an upper bound on the number of samples that the next swr_convert
	 ///          will output or a negative value to indicate an error
	int GetOutSamples(int in_samples);
public: 
	 /// Generate a channel mixing matrix.
	 ///
	 /// This function is the one used internally by libswresample for building the
	 /// default mixing matrix. It is made public just as a utility function for
	 /// building custom matrices.
	 ///
	 /// @param in_layout           input channel layout
	 /// @param out_layout          output channel layout
	 /// @param center_mix_level    mix level for the center channel
	 /// @param surround_mix_level  mix level for the surround channel(s)
	 /// @param lfe_mix_level       mix level for the low-frequency effects channel
	 /// @param rematrix_maxval     if 1.0, coefficients will be normalized to prevent
	 ///                            overflow. if INT_MAX, coefficients will not be
	 ///                            normalized.
	 /// @param[out] matrix         mixing coefficients; matrix[i + stride /// o] is
	 ///                            the weight of input channel i in output channel o.
	 /// @param stride              distance between adjacent input channels in the
	 ///                            matrix array
	 /// @param matrix_encoding     matrixed stereo downmix mode (e.g. dplii)
	 /// @param log_ctx             parent logging context, can be NULL
	 /// @return                    0 on success, negative AVERROR code on failure
	static array<double>^ BuildMatrix(AVChannelLayout in_layout, 
		AVChannelLayout out_layout,
		double center_mix_level, double surround_mix_level,
		double lfe_mix_level, double rematrix_maxval,
		double rematrix_volume, int stride, 
		AVMatrixEncoding matrix_encoding, IntPtr log_ctx);
};
//////////////////////////////////////////////////////
#pragma endregion 
//////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////