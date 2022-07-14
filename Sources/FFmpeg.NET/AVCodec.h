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
// libavcodec
// Encoding/Decoding Library.
//////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////
using namespace System;
using namespace System::IO;
//////////////////////////////////////////////////////
#include "AVCore.h"
//////////////////////////////////////////////////////
namespace FFmpeg {
//////////////////////////////////////////////////////
#pragma region Enums
//////////////////////////////////////////////////////
public enum class FFCoderType : int
{
    VLC = 0,
    AC = 1,
    RAW = 2,
    RLE = 3,
    DEFLATE = 4,
};
//////////////////////////////////////////////////////
public enum class FFPred : int
{
    LEFT = 0,
    PLANE = 1,
    MEDIAN = 2,
};
//////////////////////////////////////////////////////
public enum class FFCmp : int
{
    SAD = 0,
    SSE = 1,
    SATD = 2,
    DCT = 3,
    PSNR = 4,
    BIT = 5,
    RD = 6,
    ZERO = 7,
    VSAD = 8,
    VSSE = 9,
    NSSE = 10,
    W53 = 11,
    W97 = 12,
    DCTMAX = 13,
    DCT264 = 14,
	MEDIAN_SAD = 15,
    CHROMA = 256,
};
//////////////////////////////////////////////////////
public enum class FFDtg : int
{
	None = 0,
    AFD_SAME = 8,
    AFD_4_3 = 9,
    AFD_16_9 = 10,
    AFD_14_9 = 11,
    AFD_4_3_SP_14_9 = 13,
    AFD_16_9_SP_14_9 = 14,
    AFD_SP_4_3 = 15,
};
//////////////////////////////////////////////////////
[Flags]
public enum class SliceFlag : int
{
	None = 0,
    CODED_ORDER = 0x0001, ///< draw_horiz_band() is called in coded order instead of display
    ALLOW_FIELD = 0x0002, ///< allow draw_horiz_band() with field slices (MPEG2 field pics)
    ALLOW_PLANE = 0x0004, ///< allow draw_horiz_band() with 1 component at a time (SVQ1)
};
//////////////////////////////////////////////////////
public enum class FFMbDecision : int
{
    SIMPLE = 0,        ///< uses mb_cmp
    BITS = 1,        ///< chooses the one which needs the fewest bits
    RD = 2,        ///< rate distortion
};
//////////////////////////////////////////////////////
///<summary>
///encoding support
///   These flags can be passed in AVCodecContext.flags before initialization.
///   Note: Not everything is supported yet.
///</summary>
// AV_CODEC_FLAG_*
[Flags]
public enum class AVCodecFlag : UInt32
{
	None = 0,
	/// Allow decoders to produce frames with data planes that are not aligned
	/// to CPU requirements (e.g. due to cropping).
	Unaligned = (1 << 0),
	///< Use fixed qscale.
    QScale = (1 << 1),  
	///< 4 MV per MB allowed / advanced prediction for H.263.
    _4MV = (1 << 2),  
	/// Output even those frames that might be corrupted.
	OUTPUT_CORRUPT  = (1 <<  3),
	///< Use qpel MC.
    QPEL = (1 << 4),  
	/// Don't output frames whose parameters differ from first
	/// decoded frame in stream.
	DROPCHANGED = (1 << 5),
	///< Use internal 2pass ratecontrol in first pass mode.
    PASS1 = (1 << 9),
	///< Use internal 2pass ratecontrol in second pass mode.
    PASS2 = (1 << 10), 
	///< loop filter
	LOOP_FILTER = (1 << 11), 
	///< Only decode/encode grayscale.
    GRAY = (1 << 13),   
	///< error[?] variables will be set during encoding.
	PSNR = (1 << 15),   
	/// Input bitstream might be truncated at a random
	/// location instead of only at frame boundaries.
	TRUNCATED = (1 << 16), 
	///< Use interlaced DCT.
	INTERLACED_DCT = (1 << 18),
	///< Force low delay.
	LOW_DELAY = (1 << 19), 
	/// Place global headers in extradata instead of every keyframe.
	GLOBAL_HEADER = (1 << 22),
	///< Use only bitexact stuff (except (I)DCT).
	BITEXACT = (1 << 23), 
	/// Fx : Flag for h263+ extra options 
	///< H.263 advanced intra coding / MPEG-4 AC prediction
	AC_PRED = (1 << 24), 
	///< interlaced motion estimation
	INTERLACED_ME = (1 << 29), 
	CLOSED_GOP = (1U << 31), 
};
//////////////////////////////////////////////////////
[Flags]
// AV_CODEC_FLAG2_*
public enum class AVCodecFlag2 : UInt32
{
	None = 0,
	///< Allow non spec compliant speedup tricks.
    FAST = (1 << 0), 
	///< Strictly enforce GOP size.
	STRICT_GOP = (1 << 1), 
	///< Skip bitstream encoding.
    NO_OUTPUT = (1 << 2), 
	///< Place global headers at every keyframe instead of in extradata.
    LOCAL_HEADER = (1 << 3), 
	///< timecode is in drop frame format. DEPRECATED!!!!
    DROP_FRAME_TIMECODE = (1 << 13), 
	///< Input bitstream might be truncated at a packet boundaries instead of only at frame boundaries.
	CHUNKS = (1 << 15),
	/// Discard cropping information from SPS.
	IGNORE_CROP = (1 << 16),
	///< Show all frames before the first keyframe
	SHOW_ALL = (1 << 22), 
	/// Export motion vectors through frame side data
	EXPORT_MVS = (1 << 28),
	/// Do not skip samples and export skip information as frame side data
	SKIP_MANUAL  = (1 << 29),
	/// Do not reset ASS ReadOrder field on flush (subtitles decoding)
	RO_FLUSH_NOOP  = (1 << 30),
};
//////////////////////////////////////////////////////
[Flags]
public enum class FFBug : int
{
	None = 0,
    AUTODETECT = 1,  ///< autodetection
    OLD_MSMPEG4 = 2,
    XVID_ILACE = 4,
    UMP4 = 8,
    NO_PADDING = 16,
    AMV = 32,
    AC_VLC = 0,  ///< Will be removed, libavcodec can now handle these non-compliant files by default.
    QPEL_CHROMA = 64,
    STD_QPEL = 128,
    QPEL_CHROMA2 = 256,
    DIRECT_BLOCKSIZE = 512,
    EDGE = 1024,
    HPEL_CHROMA = 2048,
    DC_CLIP = 4096,
    MS = 8192, ///< Work around various bugs in Microsoft's broken decoders.
    TRUNCATED = 16384,
	IEDGE     = 32768,
};
//////////////////////////////////////////////////////
public enum class FFCompliance : int
{
    COMPLIANCE_VERY_STRICT = 2, ///< Strictly conform to an older more strict version of the spec or reference software.
    COMPLIANCE_STRICT = 1, ///< Strictly conform to all the things in the spec no matter what consequences.
    COMPLIANCE_NORMAL = 0,
    COMPLIANCE_UNOFFICIAL = -1, ///< Allow unofficial extensions
    COMPLIANCE_EXPERIMENTAL = -2, ///< Allow nonstandardized experimental things.
};
//////////////////////////////////////////////////////
public enum class ErrorConcealment
{
	None = 0,
    GUESS_MVS = 1,
    DEBLOCK = 2,
	FAVOR_INTER = 256,
};
//////////////////////////////////////////////////////
[Flags]
public enum class FFDebug : int
{
	None = 0,
    PICT_INFO = 1,
    RC = 2,
    BITSTREAM = 4,
    MB_TYPE = 8,
    QP = 16,
    MV = 32,
    DCT_COEFF = 0x00000040,
    SKIP = 0x00000080,
    STARTCODE = 0x00000100,
    PTS = 0x00000200,
    ER = 0x00000400,
    MMCO = 0x00000800,
    BUGS = 0x00001000,
    VIS_QP = 0x00002000,
    VIS_MB_TYPE = 0x00004000,
    BUFFERS = 0x00008000,
    THREADS = 0x00010000,
	NOMC = 0x01000000,

};
//////////////////////////////////////////////////////
[Flags]
public enum class FFDebugMV : int
{
	None = 0,
    VIS_MV_P_FOR = 0x00000001, //visualize forward predicted MVs of P frames
    VIS_MV_B_FOR = 0x00000002, //visualize forward predicted MVs of B frames
    VIS_MV_B_BACK = 0x00000004, //visualize backward predicted MVs of B frames
};
//////////////////////////////////////////////////////
[Flags]
public enum class ErrorRecognition : int
{
	None = 0,
	/// Verify checksums embedded in the bitstream (could be of either encoded or
	/// decoded data, depending on the codec) and print an error message on mismatch.
	/// If AV_EF_EXPLODE is also set, a mismatching checksum will result in the
	/// decoder returning an error.
    CRCCHECK = (1 << 0),
	///< detect bitstream specification deviations
    BITSTREAM = (1 << 1),
	///< detect improper bitstream length
    BUFFER = (1 << 2),
	///< abort decoding on minor error detection
    EXPLODE = (1 << 3),
	///< ignore errors and continue
	IGNORE_ERR = (1<<15),
	///< consider things that violate the spec, are fast to calculate and have not been seen in the wild as errors
    CAREFUL = (1 << 16),
	///< consider all spec non compliances as errors
    COMPLIANT = (1 << 17),
	///< consider things that a sane encoder should not do as an error
    AGGRESSIVE = (1 << 18),
};
//////////////////////////////////////////////////////
public enum class FFDct : int
{
    AUTO = 0,
    FASTINT = 1,
    INT = 2,
    MMX = 3,
    ALTIVEC = 5,
    FAAN = 6,
};
//////////////////////////////////////////////////////
public enum class FFIdct : int
{
    AUTO = 0,
    INT = 1,
    SIMPLE = 2,
    SIMPLEMMX = 3,
    LIBMPEG2MMX = 4,
    MMI = 5,
    ARM = 7,
    ALTIVEC = 8,
    SH4 = 9,
    SIMPLEARM = 10,
    H264 = 11,
    VP3 = 12,
    IPP = 13,
    XVIDMMX = 14,
    CAVS = 15,
    SIMPLEARMV5TE = 16,
    SIMPLEARMV6 = 17,
    SIMPLEVIS = 18,
    WMV2 = 19,
    FAAN = 20,
    EA = 21,
    SIMPLENEON = 22,
    SIMPLEALPHA = 23,
    BINK = 24,
};
//////////////////////////////////////////////////////
public enum class ThreadType : int
{
    FRAME = 1, ///< Decode more than one frame at once
    SLICE = 2, ///< Decode more than one part of a single frame at once
};
//////////////////////////////////////////////////////
// AV_PKT_FLAG_*
[Flags]
public enum class AvPktFlag : int
{
	None = 0,
	///< The packet contains a keyframe
	KEY = 0x0001, 
	///< The packet content is corrupted
	CORRUPT = 0x0002, 
	///<summary>
	/// Flag is used to discard packets which are required to maintain valid
	/// decoder state but are not required for output and should be dropped
	/// after decoding.
	///</summary>
	DISCARD = 0x0004,
	///<summary>
	/// The packet comes from a trusted source.
	///
	/// Otherwise-unsafe constructs such as arbitrary pointers to data
	/// outside the packet may be followed.
	///</summary>
	TRUSTED = 0x0008,
	///<summary>
	/// Flag is used to indicate packets that contain frames that can
	/// be discarded by the decoder.  I.e. Non-reference frames.
	///</summary>
	DISPOSABLE = 0x0010,
};
//////////////////////////////////////////////////////
///<summary>
/// @defgroup lavc_packet AVPacket
/// AV_PKT_DATA_*
/// Types and functions for working with AVPacket.
/// @{
///</summary>
public enum class AVPacketSideDataType
{
	///<summary>
	/// An AV_PKT_DATA_PALETTE side data packet contains exactly AVPALETTE_SIZE
	/// bytes worth of palette. This side data signals that a new palette is
	/// present.
	///</summary>
	PALETTE,
	///<summary>
	/// The AV_PKT_DATA_NEW_EXTRADATA is used to notify the codec or the format
	/// that the extradata buffer was changed and the receiving side should
	/// act upon it appropriately. The new extradata is embedded in the side
	/// data buffer and should be immediately used for processing the current
	/// frame or packet.
	///</summary>
	NEW_EXTRADATA,

	///<summary>
	/// An PARAM_CHANGE side data packet is laid out as follows:
	/// @code
	/// u32le param_flags
	/// if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_COUNT)
	///     s32le channel_count
	/// if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_LAYOUT)
	///     u64le channel_layout
	/// if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_SAMPLE_RATE)
	///     s32le sample_rate
	/// if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_DIMENSIONS)
	///     s32le width
	///     s32le height
	/// @endcode
	///</summary>
	PARAM_CHANGE,

	///<summary>
	/// An H263_MB_INFO side data packet contains a number of
	/// structures with info about macroblocks relevant to splitting the
	/// packet into smaller packets on macroblock edges (e.g. as for RFC 2190).
	/// That is, it does not necessarily contain info about all macroblocks,
	/// as long as the distance between macroblocks in the info is smaller
	/// than the target payload size.
	/// Each MB info structure is 12 bytes, and is laid out as follows:
	/// @code
	/// u32le bit offset from the start of the packet
	/// u8    current quantizer at the start of the macroblock
	/// u8    GOB number
	/// u16le macroblock address within the GOB
	/// u8    horizontal MV predictor
	/// u8    vertical MV predictor
	/// u8    horizontal MV predictor for block number 3
	/// u8    vertical MV predictor for block number 3
	/// @endcode
	///</summary>
	H263_MB_INFO,

	///<summary>
	/// This side data should be associated with an audio stream and contains
	/// ReplayGain information in form of the AVReplayGain struct.
	///</summary>
	REPLAYGAIN,

	///<summary>
	/// This side data contains a 3x3 transformation matrix describing an affine
	/// transformation that needs to be applied to the decoded video frames for
	/// correct presentation.
	///
	/// See libavutil/display.h for a detailed description of the data.
	///</summary>
	DISPLAYMATRIX,

	///<summary>
	/// This side data should be associated with a video stream and contains
	/// Stereoscopic 3D information in form of the AVStereo3D struct.
	///</summary>
	STEREO3D,

	///<summary>
	/// This side data should be associated with an audio stream and corresponds
	/// to enum AVAudioServiceType.
	///</summary>
	AUDIO_SERVICE_TYPE,

	///<summary>
	/// This side data contains quality related information from the encoder.
	/// @code
	/// u32le quality factor of the compressed frame. Allowed range is between 1 (good) and FF_LAMBDA_MAX (bad).
	/// u8    picture type
	/// u8    error count
	/// u16   reserved
	/// u64le[error count] sum of squared differences between encoder in and output
	/// @endcode
	///</summary>
	QUALITY_STATS,

	///<summary>
	/// This side data contains an integer value representing the stream index
	/// of a "fallback" track.  A fallback track indicates an alternate
	/// track to use when the current track can not be decoded for some reason.
	/// e.g. no decoder available for codec.
	///</summary>
	FALLBACK_TRACK,

	///<summary>
	/// This side data corresponds to the AVCPBProperties struct.
	///</summary>
	CPB_PROPERTIES,

	///<summary>
	/// Recommmends skipping the specified number of samples
	/// @code
	/// u32le number of samples to skip from start of this packet
	/// u32le number of samples to skip from end of this packet
	/// u8    reason for start skip
	/// u8    reason for end   skip (0=padding silence, 1=convergence)
	/// @endcode
	///</summary>
	SKIP_SAMPLES,

	///<summary>
	/// An JP_DUALMONO side data packet indicates that
	/// the packet may contain "dual mono" audio specific to Japanese DTV
	/// and if it is true, recommends only the selected channel to be used.
	/// @code
	/// u8    selected channels (0=mail/left, 1=sub/right, 2=both)
	/// @endcode
	///</summary>
	JP_DUALMONO,

	///<summary>
	/// A list of zero terminated key/value strings. There is no end marker for
	/// the list, so it is required to rely on the side data size to stop.
	///</summary>
	STRINGS_METADATA,

	///<summary>
	/// Subtitle event position
	/// @code
	/// u32le x1
	/// u32le y1
	/// u32le x2
	/// u32le y2
	/// @endcode
	///</summary>
	SUBTITLE_POSITION,

	///<summary>
     /// Data found in BlockAdditional element of matroska container. There is
     /// no end marker for the data, so it is required to rely on the side data
     /// size to recognize the end. 8 byte id (as found in BlockAddId) followed
     /// by data.
     ///</summary>
    MATROSKA_BLOCKADDITIONAL,

    ///<summary>
     /// The optional first identifier line of a WebVTT cue.
     ///</summary>
    WEBVTT_IDENTIFIER,

    ///<summary>
     /// The optional settings (rendering instructions) that immediately
     /// follow the timestamp specifier of a WebVTT cue.
     ///</summary>
    WEBVTT_SETTINGS,

    ///<summary>
     /// A list of zero terminated key/value strings. There is no end marker for
     /// the list, so it is required to rely on the side data size to stop. This
     /// side data includes updated metadata which appeared in the stream.
     ///</summary>
    METADATA_UPDATE,

    ///<summary>
     /// MPEGTS stream ID as uint8_t, this is required to pass the stream ID
     /// information from the demuxer to the corresponding muxer.
     ///</summary>
    MPEGTS_STREAM_ID,

    ///<summary>
     /// Mastering display metadata (based on SMPTE-2086:2014). This metadata
     /// should be associated with a video stream and contains data in the form
     /// of the AVMasteringDisplayMetadata struct.
     ///</summary>
    MASTERING_DISPLAY_METADATA,

    ///<summary>
     /// This side data should be associated with a video stream and corresponds
     /// to the AVSphericalMapping structure.
     ///</summary>
    SPHERICAL,

    ///<summary>
     /// Content light level (based on CTA-861.3). This metadata should be
     /// associated with a video stream and contains data in the form of the
     /// AVContentLightMetadata struct.
     ///</summary>
    CONTENT_LIGHT_LEVEL,

    ///<summary>
     /// ATSC A53 Part 4 Closed Captions. This metadata should be associated with
     /// a video stream. A53 CC bitstream is stored as uint8_t in AVPacketSideData.data.
     /// The number of bytes of CC data is AVPacketSideData.size.
     ///</summary>
    A53_CC,

    ///<summary>
     /// This side data is encryption initialization data.
     /// The format is not part of ABI, use av_encryption_init_info_/// methods to
     /// access.
     ///</summary>
    ENCRYPTION_INIT_INFO,

    ///<summary>
     /// This side data contains encryption info for how to decrypt the packet.
     /// The format is not part of ABI, use av_encryption_info_/// methods to access.
     ///</summary>
    ENCRYPTION_INFO,

    ///<summary>
     /// Active Format Description data consisting of a single byte as specified
     /// in ETSI TS 101 154 using AVActiveFormatDescription enum.
     ///</summary>
    AFD,
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVFrameFlags : int
{
	None = 0,
	/// The frame data may be corrupted, e.g. due to decoding errors.
	CORRUPT = (1 << 0), 
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVIndexFlags : int
{
	None = 0,
	KeyFrame = 1,
	// Flag is used to indicate which frame should be discarded after decoding.
	DiscardFrame = 2,
};
//////////////////////////////////////////////////////
public enum class AVFrameSideDataType
{
    ///<summary>
    /// The data is the AVPanScan struct defined in libavcodec.
    ///</summary>
    PANSCAN,
    ///<summary>
    /// ATSC A53 Part 4 Closed Captions.
    /// A53 CC bitstream is stored as uint8_t in AVFrameSideData.data.
    /// The number of bytes of CC data is AVFrameSideData.size.
    ///</summary>
    A53_CC,
    ///<summary>
     /// Stereoscopic 3d metadata.
     /// The data is the AVStereo3D struct defined in libavutil/stereo3d.h.
     ///</summary>
    STEREO3D,
    ///<summary>
    /// The data is the AVMatrixEncoding enum defined in libavutil/channel_layout.h.
    ///</summary>
    MATRIXENCODING,
    ///<summary>
    /// Metadata relevant to a downmix procedure.
    /// The data is the AVDownmixInfo struct defined in libavutil/downmix_info.h.
    ///</summary>
    DOWNMIX_INFO,
    ///<summary>
    /// ReplayGain information in the form of the AVReplayGain struct.
    ///</summary>
    REPLAYGAIN,
    ///<summary>
    /// This side data contains a 3x3 transformation matrix describing an affine
    /// transformation that needs to be applied to the frame for correct
    /// presentation.
    ///
    /// See libavutil/display.h for a detailed description of the data.
    ///</summary>
    DISPLAYMATRIX,
    ///<summary>
    /// Active Format Description data consisting of a single byte as specified
    /// in ETSI TS 101 154 using AVActiveFormatDescription enum.
    ///</summary>
    AFD,
    ///<summary>
    /// Motion vectors exported by some codecs (on demand through the export_mvs
    /// flag set in the libavcodec AVCodecContext flags2 option).
    /// The data is the AVMotionVector struct defined in
    /// libavutil/motion_vector.h.
    ///</summary>
    MOTION_VECTORS,
    ///<summary>
    /// Recommmends skipping the specified number of samples. This is exported
    /// only if the "skip_manual" AVOption is set in libavcodec.
    /// This has the same format as AV_PKT_DATA_SKIP_SAMPLES.
    /// @code
    /// u32le number of samples to skip from start of this packet
    /// u32le number of samples to skip from end of this packet
    /// u8    reason for start skip
    /// u8    reason for end   skip (0=padding silence, 1=convergence)
    /// @endcode
    ///</summary>
    SKIP_SAMPLES,

    ///<summary>
    /// This side data must be associated with an audio frame and corresponds to
    /// enum AVAudioServiceType defined in avcodec.h.
    ///</summary>
    AUDIO_SERVICE_TYPE,
};
//////////////////////////////////////////////////////
// AVActiveFormatDescription
public enum class AVActiveFormatDescription
{
    AV_AFD_SAME         = 8,
    AV_AFD_4_3          = 9,
    AV_AFD_16_9         = 10,
    AV_AFD_14_9         = 11,
    AV_AFD_4_3_SP_14_9  = 13,
    AV_AFD_16_9_SP_14_9 = 14,
    AV_AFD_SP_4_3       = 15,
};
//////////////////////////////////////////////////////
public enum class AVSubtitleType
{
	NONE,

	BITMAP,                ///< A bitmap, pict will be set

	///<summary>
	/// Plain text, the text field must be set by the decoder and is
	/// authoritative. ass and pict fields may contain approximations.
	///</summary>
	TEXT,

	///<summary>
	/// Formatted text, the ass field must be set by the decoder and is
	/// authoritative. pict and text fields may contain approximations.
	///</summary>
	ASS,
};
//////////////////////////////////////////////////////
///<summary>
/// Lock operation used by lockmgr
///</summary>
public enum class AVLockOp
{
	CREATE,  ///< Create a mutex
	OBTAIN,  ///< Lock the mutex
	RELEASE, ///< Unlock the mutex
	DESTROY, ///< Free mutex resources
};
//////////////////////////////////////////////////////
[Flags]
public enum class ParserFlags : int
{
	COMPLETE_FRAMES = 0x0001,
	ONCE = 0x0002,
	/// Set if the parser has a valid file offset
	FETCHED_OFFSET = 0x0004,
	USE_CODEC_TS = 0x1000,
};
//////////////////////////////////////////////////////
// AV_PIX_FMT_FLAG
[Flags]
public enum class AV_PIX_FMT_FLAG : int
{
	/// Pixel format is big-endian.
	BE           = (1 << 0),
	 /// Pixel format has a palette in data[1], values are indexes in this palette.
	PAL          = (1 << 1),
	/// All values of a component are bit-wise packed end to end.
	BITSTREAM    = (1 << 2),
	/// Pixel format is an HW accelerated format.
	HWACCEL      = (1 << 3),
	/// At least one pixel component is not in the first data plane.
	PLANAR       = (1 << 4),
	/// The pixel format contains RGB-like data = (as opposed to YUV/grayscale),.
	RGB          = (1 << 5),
	/// The pixel format is "pseudo-paletted". This means that it contains a
	/// fixed palette in the 2nd plane but the palette is fixed/constant for each
	/// PIX_FMT. This allows interpreting the data as if it was PAL8, which can
	/// in some cases be simpler. Or the data can be interpreted purely based on
	/// the pixel format without using the palette.
	/// An example of a pseudo-paletted format is AV_PIX_FMT_GRAY8
	PSEUDOPAL    = (1 << 6),
	/// The pixel format has an alpha channel. This is set on all formats that
	/// support alpha in some way. The exception is AV_PIX_FMT_PAL8, which can
	/// carry alpha as part of the palette. Details are explained in the
	/// AVPixelFormat enum, and are also encoded in the corresponding
	/// AVPixFmtDescriptor.
	///
	/// The alpha is always straight, never pre-multiplied.
	///
	/// If a codec or a filter does not support alpha, it should set all alpha to
	/// opaque, or use the equivalent pixel formats without alpha component, e.g.
	/// AV_PIX_FMT_RGB0 (or AV_PIX_FMT_RGB24 etc.) instead of AV_PIX_FMT_RGBA.
	ALPHA        = (1 << 7),
};
//////////////////////////////////////////////////////
public enum class AVPictureStructure
{
    UNKNOWN,      //< unknown
    TOP_FIELD,    //< coded as top field
    BOTTOM_FIELD, //< coded as bottom field
    FRAME,        //< coded as frame
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVParserFlags
{
	NONE = 0,
	COMPLETE_FRAMES = 0x0001,
	ONCE = 0x0002,
	/// Set if the parser has a valid file offset
	FETCHED_OFFSET = 0x0004,
	USE_CODEC_TS = 0x1000,
};
//////////////////////////////////////////////////////
#pragma endregion
//////////////////////////////////////////////////////
#pragma region Classes
//////////////////////////////////////////////////////
ref class AVCodecContext;
ref class AVCodec;
ref class AVPicture;
ref class AVBSFList;
//////////////////////////////////////////////////////
// LibAVCodec
public ref class LibAVCodec
{
private:
	static bool s_bRegistered = false;
private:
	LibAVCodec();
public:
	// Return the LIBAVCODEC_VERSION_INT constant.
	static property UInt32 Version { UInt32 get(); }
	// Return the libavcodec build-time configuration.
	static property String^ Configuration { String^ get(); }
	// Return the libavcodec license.
	static property String^ License { String^ get(); }
public:
	// Register the codec codec and initialize libavcodec.
	static void Register(AVCodec^ _codec);
	// Register all the codecs, parsers and bitstream filters which were enabled at
	// configuration time. If you do not call this function you can select exactly
	// which formats you want to support, by using the individual registration
	// functions.
	static void RegisterAll();
};
//////////////////////////////////////////////////////
// AVCodecHWConfig
public ref class AVCodecHWConfig : public AVBase
{
internal:
	AVCodecHWConfig(void * _pointer,AVBase^ _parent);
public:
	/// A hardware pixel format which the codec can use.
	property AVPixelFormat pix_fmt { AVPixelFormat get(); }
	/// Bit set of AV_CODEC_HW_CONFIG_METHOD_/// flags, describing the possible
	/// setup methods which can be used with this configuration.
	property AV_CODEC_HW_CONFIG_METHOD methods { AV_CODEC_HW_CONFIG_METHOD get(); }
	///////
	/// The device type associated with the configuration.
	///
	/// Must be set for AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX and
	/// AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX, otherwise unused.
	property AVHWDeviceType device_type { AVHWDeviceType get(); }
};
//////////////////////////////////////////////////////
// AVCodec
public ref class AVCodec : public  AVBase
{
public:
	// Codecs Iterator
	ref class AVCodecs
		: public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<AVCodec^>
	{
	private:
		ref class AVCodecEnumerator 
			: public System::Collections::IEnumerator
			, public System::Collections::Generic::IEnumerator<AVCodec^>
		{
		protected:
			IntPtr		m_pOpaque;
			AVCodec^	m_pCurrent;
		public:
			AVCodecEnumerator();
			~AVCodecEnumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVCodec^ Current { AVCodec^ get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
		};
	public:
		AVCodecs();
	public:
		property AVCodec^ default[int] { AVCodec^ get(int); }
		property int Count { int get(); }
	public:
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { return gcnew AVCodecEnumerator(); }
		virtual System::Collections::Generic::IEnumerator<AVCodec^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVCodec^>::GetEnumerator { return gcnew AVCodecEnumerator(); }
	};
internal:
	AVCodec(void * _pointer,AVBase^ _parent);
public:
	///<summary>
	/// Name of the codec implementation.
	/// The name is globally unique among encoders and among decoders (but an
	/// encoder and a decoder can share the same name).
	/// This is the primary way to find a codec from the user perspective.
	///</summary>
	property String^ name { String^ get(); }
	///<summary>
	/// Descriptive name for the codec, meant to be more human readable than name.
	/// You should use the NULL_IF_CONFIG_SMALL() macro to define it.
	///</summary>
	property String^ long_name { String^ get(); }
	property AVMediaType type { AVMediaType get(); }
	property AVCodecID id { AVCodecID get(); }
	// Codec capabilities.
	property AVCodecCap capabilities { AVCodecCap get(); }
	///< array of supported framerates, or NULL if any, array is terminated by {0,0}
	property array<AVRational^>^ supported_framerates { array<AVRational^>^ get(); }
	///< array of supported pixel formats, or NULL if unknown, array is terminated by -1
	property array<AVPixelFormat>^ pix_fmts { array<FFmpeg::AVPixelFormat>^ get(); }
	///< array of supported audio samplerates, or NULL if unknown, array is terminated by 0
	property array<int>^ supported_samplerates { array<int>^ get(); }
	///< array of supported sample formats, or NULL if unknown, array is terminated by -1
	property array<AVSampleFormat>^ sample_fmts { array<AVSampleFormat>^ get(); }
	///< array of support channel layouts, or NULL if unknown. array is terminated by 0
	property array<AVChannelLayout>^ channel_layouts { array<AVChannelLayout>^ get(); }
	///< maximum value for lowres supported by the decoder, no direct access, use av_codec_get_max_lowres()
	property int max_lowres { int get(); }
	///< AVClass for the private context
	property AVClass^ priv_class { AVClass^ get(); }
	///< array of recognized profiles, or NULL if unknown, array is terminated by {FF_PROFILE_UNKNOWN}
	property array<AVProfile^>^ profiles { array<FFmpeg::AVProfile^>^ get(); }
	/// Group name of the codec implementation.
	/// This is a short symbolic name of the wrapper backing this codec. A
	/// wrapper uses some kind of external implementation for the codec, such
	/// as an external library, or a codec implementation provided by the OS or
	/// the hardware.
	/// If this field is NULL, this is a builtin, libavcodec native codec.
	/// If non-NULL, this will be the suffix in AVCodec.name in most cases
	/// (usually AVCodec.name will be of the form	"<codec_name>_<wrapper_name>").
	property String^ wrapper_name { String^ get(); }
public:
	/// Retrieve supported hardware configurations for a codec.
	property array<AVCodecHWConfig^>^ hw_configs { array<AVCodecHWConfig^>^ get(); }
public:
	virtual String^ ToString() override;
public:
	property int _StructureSize { virtual int get() override; }
public:
	AVCodec^ Next();
	bool IsDecoder();
	bool IsEncoder();
	 /// Allocate an AVCodecContext and set its fields to default values. The
	 /// resulting struct should be freed with avcodec_free_context().
	 ///
	 /// @param codec if non-NULL, allocate private data and initialize defaults
	 ///              for the given codec. It is illegal to then call avcodec_open2()
	 ///              with a different codec.
	 ///              If NULL, then the codec-specific defaults won't be initialized,
	 ///              which may result in suboptimal default settings (this is
	 ///              important mainly for encoders, e.g. libx264).
	 ///
	 /// @return An AVCodecContext filled with default values or NULL on failure.
	AVCodecContext^ AllocContext();
public:
	String^ GetProfileName(Profile _profile);
public:
	// Accessing AVCodecs Collection
	static property AVCodecs^ Codecs { AVCodecs^ get() { return gcnew AVCodecs(); } }
public:
	static AVCodec^ Next(AVCodec^ _prev);
	static AVCodec^ FindDecoder(AVCodecID _id);
	static AVCodec^ FindDecoder(String^ _name);
	static AVCodec^ FindEncoder(AVCodecID _id);
	static AVCodec^ FindEncoder(String^ _name);
	static int GetBitsPerSample(AVCodecID codec_id);
	static AVCodecID GetPCMCodec(AVSampleFormat fmt, int be);
	static int GetExactBitsPerSample(AVCodecID codec_id);
	static AVMediaType GetCodecType(AVCodecID codec_id);
	static String^ GetCodecName(AVCodecID id);
	static String^ GetColorRangeName(AVColorRange range);
	static String^ GetColorPrimariesName(AVColorPrimaries primaries);
	static String^ GetColorTransferName(AVColorTransferCharacteristic transfer);
	static String^ GetColorSpaceName(AVColorSpace space);
	static String^ GetChromaLocationName(AVChromaLocation location);
public:
	/// Converts AVChromaLocation to swscale x/y chroma position.
	/// 
	/// The positions represent the chroma (0,0) position in a coordinates system
	/// with luma (0,0) representing the origin and luma(1,1) representing 256,256
	/// 
	/// @param xpos  horizontal chroma sample position
	/// @param ypos  vertical   chroma sample position
	static System::Drawing::Size^ EnumToChromaPos(AVChromaLocation pos);
	/// Converts swscale x/y chroma position to AVChromaLocation.
	/// 
	/// The positions represent the chroma (0,0) position in a coordinates system
	/// with luma (0,0) representing the origin and luma(1,1) representing 256,256
	/// 
	/// @param xpos  horizontal chroma sample position
	/// @param ypos  vertical   chroma sample position
	static AVChromaLocation ChromaPosToEnum(System::Drawing::Size^ pos);
};
//////////////////////////////////////////////////////
// AVComponentDescriptor
public ref class AVComponentDescriptor : public  AVBase
{
internal:
	AVComponentDescriptor(void * _pointer,AVBase^ _parent);
public:
	property int _StructureSize { virtual int get() override; }
public:
	 ///Which of the 4 planes contains the component.
	property UInt16 plane { UInt16 get(); }
     ///Number of elements between 2 horizontally consecutive pixels.
     ///Elements are bits for bitstream formats, bytes otherwise.
    property UInt16 step { UInt16 get(); }
	///Number of elements before the component of the first pixel.
	///Elements are bits for bitstream formats, bytes otherwise.
    property UInt16 offset { UInt16 get(); }
    /// Number of least significant bits that must be shifted away
    /// to get the value.
    property UInt16 shift { UInt16 get(); }
    /// Number of bits in the component.
    property UInt16 depth { UInt16 get(); }
};
//////////////////////////////////////////////////////
// AVPixFmtDescriptor
public ref class AVPixFmtDescriptor : public  AVBase
{
internal:
	AVPixFmtDescriptor(void * _pointer,AVBase^ _parent);
public:
	property int _StructureSize { virtual int get() override; }
public:
	virtual String^ ToString() override;
public:
	property String^ name { String^ get(); }
	///< The number of components each pixel has, (1-4)
	property byte nb_components { byte get(); }
	/// Amount to shift the luma width right to find the chroma width.
	/// For YV12 this is 1 for example.
	/// chroma_width = -((-luma_width) >> log2_chroma_w)
	/// The note above is needed to ensure rounding up.
	/// This value only refers to the chroma components.
	///< chroma_width = -((-luma_width )>>log2_chroma_w)
	property byte log2_chroma_w { byte get(); }
	/// Amount to shift the luma height right to find the chroma height.
	/// For YV12 this is 1 for example.
	/// chroma_height= -((-luma_height) >> log2_chroma_h)
	/// The note above is needed to ensure rounding up.
	/// This value only refers to the chroma components.
	property byte log2_chroma_h { byte get(); }
	property AV_PIX_FMT_FLAG flags { AV_PIX_FMT_FLAG get(); }
	/// Parameters that describe how pixels are packed.
    /// If the format has 2 or 4 components, then alpha is last.
    /// If the format has 1 or 2 components, then luma is 0.
    /// If the format has 3 or 4 components:
    ///   if the RGB flag is set then 0 is red, 1 is green and 2 is blue;
    ///   otherwise 0 is luma, 1 is chroma-U and 2 is chroma-V.
	property array<AVComponentDescriptor^>^ comp { array<AVComponentDescriptor^>^ get(); }
     /// Alternative comma-separated names.
	property String^ alias { String^ get(); }
public:
	/// Return the number of bits per pixel used by the pixel format
	/// described by pixdesc. Note that this is not the same as the number
	/// of bits per sample.
	///
	/// The returned number of bits refers to the number of bits actually
	/// used for storing the pixel information, that is padding bits are
	/// not counted.
	property int bits_per_pixel { int get(); }
	/// Return the number of bits per pixel for the pixel format
	/// described by pixdesc, including any padding or unused bits.
	property int padded_bits_per_pixel { int get(); }
	/// @return an AVPixelFormat id described by desc, or AV_PIX_FMT_NONE if desc
	/// is not a valid pointer to a pixel format descriptor.
	property AVPixelFormat id { AVPixelFormat get(); }
public:
	/// @return a pixel format descriptor for provided pixel format or NULL if
	/// this pixel format is unknown.
	static AVPixFmtDescriptor^ pix_fmt_desc_get(AVPixelFormat pix_fmt);
	/// Iterate over all pixel format descriptors known to libavutil.
	/// 
	/// @param prev previous descriptor. NULL to get the first descriptor.
	/// 
	/// @return next descriptor or NULL after the last descriptor
	static AVPixFmtDescriptor^ Next(AVPixFmtDescriptor^ _desc);
};
//////////////////////////////////////////////////////
// AVPixel
public ref class AVPixel
{
private:
	AVPixel() {}
public:
	///  Utility function to access log2_chroma_w log2_chroma_h from
	///  the pixel format AVPixFmtDescriptor.
	/// 
	///  This function asserts that pix_fmt is valid. See av_pix_fmt_get_chroma_sub_sample
	///  for one that returns a failure code and continues in case of invalid
	///  pix_fmts.
	/// 
	///  @param[in]  pix_fmt the pixel format
	///  @param[out] h_shift store log2_chroma_w
	///  @param[out] v_shift store log2_chroma_h
	static System::Drawing::Size^ get_chroma_sub_sample(AVPixelFormat pix_fmt);
	/// Return a value representing the fourCC code associated to the
	/// pixel format pix_fmt, or 0 if no associated fourCC code can be
	/// found.
	static UInt32 pix_fmt_to_codec_tag(AVPixelFormat pix_fmt);
	/// Return the short name for a pixel format, NULL in case pix_fmt is
	/// unknown.
	/// 
	/// @see av_get_pix_fmt(), av_get_pix_fmt_string()
	static String^ get_pix_fmt_name(AVPixelFormat pix_fmt);
	/// @return number of planes in pix_fmt, a negative AVERROR if pix_fmt is not a
	/// valid pixel format.
	static int pix_fmt_count_planes(AVPixelFormat pix_fmt);
	/// Utility function to swap the endianness of a pixel format.
	///
	/// @param[in]  pix_fmt the pixel format
	///
	/// @return pixel format with swapped endianness if it exists,
	/// otherwise AV_PIX_FMT_NONE
	static AVPixelFormat pix_fmt_swap_endianness(AVPixelFormat pix_fmt);
	/// Compute what kind of losses will occur when converting from one specific
	/// pixel format to another.
	/// When converting from one pixel format to another, information loss may occur.
	/// For example, when converting from RGB24 to GRAY, the color information will
	/// be lost. Similarly, other losses occur when converting from some formats to
	/// other formats. These losses can involve loss of chroma, but also loss of
	/// resolution, loss of color depth, loss due to the color space conversion, loss
	/// of the alpha bits or loss due to color quantization.
	/// av_get_fix_fmt_loss() informs you about the various types of losses
	/// which will occur when converting from one pixel format to another.
	///
	/// @param[in] dst_pix_fmt destination pixel format
	/// @param[in] src_pix_fmt source pixel format
	/// @param[in] has_alpha Whether the source pixel format alpha channel is used.
	/// @return Combination of flags informing you what kind of losses will occur
	/// (maximum loss for an invalid dst_pix_fmt).
	static FFLoss get_pix_fmt_loss(AVPixelFormat dst_pix_fmt,AVPixelFormat src_pix_fmt,bool has_alpha);
	/// Find the best pixel format to convert to given a certain source pixel
	/// format.  When converting from one pixel format to another, information loss
	/// may occur.  For example, when converting from RGB24 to GRAY, the color
	/// information will be lost. Similarly, other losses occur when converting from
	/// some formats to other formats. avcodec_find_best_pix_fmt_of_2() searches which of
	/// the given pixel formats should be used to suffer the least amount of loss.
	/// The pixel formats from which it chooses one, are determined by the
	/// pix_fmt_list parameter.
	static AVPixelFormat find_best_pix_fmt_of_list(array<AVPixelFormat>^ pix_fmt_list,AVPixelFormat src_pix_fmt,bool has_alpha, FFLoss % loss_ptr);
	/// Compute what kind of losses will occur when converting from one specific
	/// pixel format to another.
	/// When converting from one pixel format to another, information loss may occur.
	/// For example, when converting from RGB24 to GRAY, the color information will
	/// be lost. Similarly, other losses occur when converting from some formats to
	/// other formats. These losses can involve loss of chroma, but also loss of
	/// resolution, loss of color depth, loss due to the color space conversion, loss
	/// of the alpha bits or loss due to color quantization.
	/// av_get_fix_fmt_loss() informs you about the various types of losses
	/// which will occur when converting from one pixel format to another.
	///
	/// @param[in] dst_pix_fmt destination pixel format
	/// @param[in] src_pix_fmt source pixel format
	/// @param[in] has_alpha Whether the source pixel format alpha channel is used.
	/// @return Combination of flags informing you what kind of losses will occur
	/// (maximum loss for an invalid dst_pix_fmt).
	static AVPixelFormat find_best_pix_fmt_of_2(AVPixelFormat dst_pix_fmt1,AVPixelFormat dst_pix_fmt2,AVPixelFormat src_pix_fmt,bool has_alpha,FFLoss % loss_ptr);
};
//////////////////////////////////////////////////////
// RcOverride
public ref class RcOverride : public  AVBase
{
internal:
	RcOverride(void * _pointer,AVBase^ _parent);
public:
	RcOverride();
public:
	property int _StructureSize { virtual int get() override; }
public:
	property int start_frame  { int get(); void set(int); }
	property int end_frame  { int get(); void set(int); }
	// If this is 0 then quality_factor will be used instead.
	property int qscale { int get(); void set(int); }
	property float quality_factor  { float get(); void set(float); }
};
//////////////////////////////////////////////////////
// AVSubtitleRect
public ref class AVSubtitleRect : public  AVBase
{
internal:
	AVSubtitleRect(void * _pointer,AVBase^ _parent);
public:
	AVSubtitleRect();
public:
	property int _StructureSize { virtual int get() override; }
public:
	///< top left corner  of pict, undefined when pict is not set
	property int x { int get(); void set(int); }
	///< top left corner  of pict, undefined when pict is not set
	property int y { int get(); void set(int); }
	///< width            of pict, undefined when pict is not set
	property int w { int get(); void set(int); }
	///< height           of pict, undefined when pict is not set
	property int h { int get(); void set(int); }
	///< number of colors in pict, undefined when pict is not set
	property int nb_colors { int get(); void set(int); }
	/// data+linesize for the bitmap of this subtitle.
	/// can be set for text/ass as well once they are rendered
	property AVPicture^ pict { AVPicture^ get(); }
	/// data+linesize for the bitmap of this subtitle.
	/// Can be set for text/ass as well once they are rendered.
	property AVArray<IntPtr>^ data { AVArray<IntPtr>^ get(); }
	///< number of bytes per line
	property AVArray<int>^ linesize { AVArray<int>^ get(); }
	property AVSubtitleType type { AVSubtitleType get(); void set(AVSubtitleType); }
	///< 0 terminated plain UTF-8 text
	property String^ text { String^ get(); void set(String^); }
	/// 0 terminated ASS/SSA compatible event line.
	/// The presentation of this is unaffected by the other values in this
	/// struct.
	property String^ ass { String^ get(); void set(String^); }
	property int flags { int get(); void set(int); }
public:
	/// @return AVClass for AVSubtitleRect.
	///
	/// @see av_opt_find().
	static AVClass^ GetClass();
};
//////////////////////////////////////////////////////
// AVSubtitle
public ref class AVSubtitle : public AVBase
{
internal:
	AVSubtitle(void * _pointer,AVBase^ _parent);
public:
	AVSubtitle();
public:
	property int _StructureSize { virtual int get() override; }
public:
	/// 0 = graphics
	property UInt16 format { UInt16 get(); void set(UInt16); }
	/// relative to packet pts, in ms
	property UInt32 start_display_time { UInt32 get(); void set(UInt32); }
	/// relative to packet pts, in ms
	property UInt32 end_display_time { UInt32 get(); void set(UInt32); }
	property array<AVSubtitleRect^>^ rects { array<AVSubtitleRect^>^ get(); void set(array<AVSubtitleRect^>^); }
	///< Same as packet pts, in AV_TIME_BASE
	property Int64 pts { Int64 get(); void set(Int64); }
};
//////////////////////////////////////////////////////
public delegate void AVBufferFreeCB(Object^ opaque, IntPtr buf);
//////////////////////////////////////////////////////
// AVBufferRef
public ref class AVBufferRef : public AVBase
							 , public ICloneable
{
private:
	Object^			m_pOpaque;
	AVBufferFreeCB^	m_pFreeCB;
internal:
	AVBufferRef(void * _pointer,AVBase^ _parent);
public:
	AVBufferRef(int _size);
	AVBufferRef(AVBufferRef^ _buffer);
	AVBufferRef(IntPtr _data, int _size, AVBufferFreeCB^ free_cb, Object^ opaque);
public:
	property int _StructureSize { virtual int get() override; }
public:
	///
    ///The data buffer. It is considered writable if and only if
    ///this is the only reference to the buffer, in which case
    ///av_buffer_is_writable() returns 1.
    ///
    property IntPtr data { IntPtr get(); }
    ///
    ///Size of data in bytes.
    ///
    property UInt64 size { UInt64 get(); }
public:
	property int RefCount { int get(); }
	property IntPtr Opaque { IntPtr get(); }
	property bool IsWritable { bool get(); }
	AVRESULT MakeWritable();
	AVRESULT Realloc(int _size);
public:
	// ICloneable
	virtual Object^ Clone();
internal:
	virtual void FreeCB(IntPtr data);
};
//////////////////////////////////////////////////////
// AVCodecParameters
// This struct describes the properties of an encoded stream.
public ref class AVCodecParameters : public AVBase
									, public ICloneable
{
internal:
	AVCodecParameters(void * _pointer,AVBase^ _parent);
public:
	AVCodecParameters();
	AVCodecParameters(AVCodecContext^ codec);
	AVCodecParameters(AVCodecParameters^ src);
public:
	///<summary>
	/// General type of the encoded data.
	///</summary>
	property AVMediaType codec_type { AVMediaType get(); void set(AVMediaType); }
	///<summary>
	/// Specific type of the encoded data (the codec used).
	///</summary>
	property AVCodecID codec_id { AVCodecID get(); void set(AVCodecID); }
	///<summary>
	/// Additional information about the codec (corresponds to the AVI FOURCC).
	///</summary>
	property unsigned int codec_tag { unsigned int get(); void set(unsigned int); }

	///<summary>
	/// Extra binary data needed for initializing the decoder, codec-dependent.
	///
	/// Must be allocated with av_malloc() and will be freed by
	/// avcodec_parameters_free(). The allocated size of extradata must be at
	/// least extradata_size + AV_INPUT_BUFFER_PADDING_SIZE, with the padding
	/// bytes zeroed.
	///</summary>
	property IntPtr extradata { IntPtr get(); void set(IntPtr); }
	///<summary>
	/// Size of the extradata content in bytes.
	///</summary>
	property int extradata_size { int get(); void set(int); }

	///<summary>
	/// - video: the pixel format, the value corresponds to enum AVPixelFormat.
	/// - audio: the sample format, the value corresponds to enum AVSampleFormat.
	///</summary>
	property int format { int get(); void set(int); }

	///<summary>
	/// The average bitrate of the encoded data (in bits per second).
	///</summary>
	property Int64 bit_rate { Int64 get(); void set(Int64);}

	///<summary>
	/// The number of bits per sample in the codedwords.
	///
	/// This is basically the bitrate per sample. It is mandatory for a bunch of
	/// formats to actually decode them. It's the number of bits for one sample in
	/// the actual coded bitstream.
	///
	/// This could be for example 4 for ADPCM
	/// For PCM formats this matches bits_per_raw_sample
	/// Can be 0
	///</summary>
	property int bits_per_coded_sample { int get(); void set(int); }

	///<summary>
	/// This is the number of valid bits in each output sample. If the
	/// sample format has more bits, the least significant bits are additional
	/// padding bits, which are always 0. Use right shifts to reduce the sample
	/// to its actual size. For example, audio formats with 24 bit samples will
	/// have bits_per_raw_sample set to 24, and format set to AV_SAMPLE_FMT_S32.
	/// To get the original sample use "(int32_t)sample >> 8"."
	///
	/// For ADPCM this might be 12 or 16 or similar
	/// Can be 0
	///</summary>
	property int bits_per_raw_sample { int get(); void set(int); }

	///<summary>
	/// Codec-specific bitstream restrictions that the stream conforms to.
	///</summary>
	property Profile profile { Profile get(); void set(Profile); }
	property int level { int get(); void set(int); }

	///<summary>
	/// Video only. The dimensions of the video frame in pixels.
	///</summary>
	property int width { int get(); void set(int); }
	property int height { int get(); void set(int); }

	///<summary>
	/// Video only. The aspect ratio (width / height) which a single pixel
	/// should have when displayed.
	///
	/// When the aspect ratio is unknown / undefined, the numerator should be
	/// set to 0 (the denominator may have any value).
	///</summary>
	property AVRational^ sample_aspect_ratio { AVRational^ get(); void set(AVRational^); }

	///<summary>
	/// Video only. The order of the fields in interlaced video.
	///</summary>
	property AVFieldOrder field_order { AVFieldOrder get(); void set(AVFieldOrder); }

	///<summary>
	/// Video only. Additional colorspace characteristics.
	///</summary>
	property AVColorRange color_range { AVColorRange get(); void set(AVColorRange); }
	property AVColorPrimaries color_primaries { AVColorPrimaries get(); void set(AVColorPrimaries); }
	property AVColorTransferCharacteristic color_trc { AVColorTransferCharacteristic get(); void set(AVColorTransferCharacteristic); }
	property AVColorSpace color_space { AVColorSpace get(); void set(AVColorSpace); }
	property AVChromaLocation chroma_location { AVChromaLocation get(); void set(AVChromaLocation); }

	///<summary>
	/// Video only. Number of delayed frames.
	///</summary>
	property int video_delay { int get(); void set(int); }

	///<summary>
	/// Audio only. The channel layout bitmask. May be 0 if the channel layout is
	/// unknown or unspecified, otherwise the number of bits set must be equal to
	/// the channels field.
	///</summary>
	property AVChannelLayout channel_layout { AVChannelLayout get(); void set(AVChannelLayout); }
	///<summary>
	/// Audio only. The number of audio channels.
	///</summary>
	property int channels { int get(); void set(int); }
	///<summary>
	/// Audio only. The number of audio samples per second.
	///</summary>

	property int sample_rate { int get(); void set(int); }
	///<summary>
	/// Audio only. The number of bytes per coded audio frame, required by some
	/// formats.
	///
	/// Corresponds to nBlockAlign in WAVEFORMATEX.
	///</summary>
	property int block_align { int get(); void set(int); }
	///<summary>
	/// Audio only. Audio frame size, if known. Required by some formats to be static.
	///</summary>
	property int frame_size { int get(); void set(int); }

	///<summary>
	/// Audio only. The amount of padding (in samples) inserted by the encoder at
	/// the beginning of the audio. I.e. this number of leading decoded samples
	/// must be discarded by the caller to get the original audio without leading
	/// padding.
	///</summary>
	property int initial_padding { int get(); void set(int); }

	///<summary>
	/// Audio only. The amount of padding (in samples) appended by the encoder to
	/// the end of the audio. I.e. this number of decoded samples must be
	/// discarded by the caller from the end of the stream to get the original
	/// audio without any trailing padding.
	///</summary>
	property int trailing_padding { int get(); void set(int); }
	///<summary>
	/// Audio only. Number of samples to skip after a discontinuity.
	///</summary>
	property int seek_preroll { int get(); void set(int); }
public:
	/// Fill the parameters struct based on the values from the supplied codec
	/// context. Any allocated fields in par are freed and replaced with duplicates
	/// of the corresponding fields in codec.
	///
	/// @return >= 0 on success, a negative AVERROR code on failure
	AVRESULT FromContext(AVCodecContext^ codec);
	/// Copy the contents of src to dst. Any allocated fields in dst are freed and
	/// replaced with newly allocated duplicates of the corresponding fields in src.
	///
	/// @return >= 0 on success, a negative AVERROR code on failure.
	AVRESULT CopyTo(AVCodecParameters^ dst);
	AVRESULT CopyTo(AVCodecContext^ dst);
public:
	/// Return audio frame duration.
	///
	/// @param avctx        codec context
	/// @param frame_bytes  size of the frame, or 0 if unknown
	/// @return             frame duration, in samples, if known. 0 if not able to
	///                     determine.
	int GetAudioFrameDuration(int frame_bytes);
public:
	// Inherited via ICloneable
	virtual System::Object ^ Clone();
};
//////////////////////////////////////////////////////
// AVCPBProperties
/// This structure describes the bitrate properties of an encoded bitstream. It
/// roughly corresponds to a subset the VBV parameters for MPEG-2 or HRD
/// parameters for H.264/HEVC.
public ref class AVCPBProperties : public AVBase
{
protected:
	int		m_nSize;
internal:
	AVCPBProperties(void * _pointer,AVBase^ _parent);
public:
	/// Allocate a CPB properties structure and initialize its fields to default
	/// values.
	AVCPBProperties();
public:
	property int size { int get(); }
public:
	property int _StructureSize { virtual int get() override; }
public:
	/// Maximum bitrate of the stream, in bits per second.
	/// Zero if unknown or unspecified.
	property Int64 max_bitrate { Int64 get(); }
	/// Minimum bitrate of the stream, in bits per second.
    /// Zero if unknown or unspecified.
	property Int64 min_bitrate { Int64 get(); }
	/// Average bitrate of the stream, in bits per second.
	/// Zero if unknown or unspecified.
	property Int64 avg_bitrate { Int64 get(); }
	/// The size of the buffer to which the ratecontrol is applied, in bits.
	/// Zero if unknown or unspecified.
	property Int64 buffer_size { Int64 get(); }
	/// The delay between the time the packet this structure is associated with
    /// is received and the time when it should be decoded, in periods of a 27MHz
    /// clock.
    ///
    /// UINT64_MAX when unknown or unspecified.
	property UInt64 vbv_delay { UInt64 get(); }
};
//////////////////////////////////////////////////////
// AVPacketSideData
public ref class AVPacketSideData : public AVBase
{
internal:
	AVPacketSideData(void * _pointer,AVBase^ _parent);
public:
	property int _StructureSize { virtual int get() override; }
public:
	virtual String^ ToString() override;
public:
	property IntPtr data { IntPtr get(); }
	property UInt64 size { UInt64 get(); }
	property AVPacketSideDataType type { AVPacketSideDataType get(); }
public:
	property String^ name { String^ get();}
public:
	static String^ SideDataName(AVPacketSideDataType _type);
};
//////////////////////////////////////////////////////
// AVFrameSideData
public ref class AVFrameSideData : public AVBase
{
internal:
	AVFrameSideData(void * _pointer,AVBase^ _parent);
public:
	property int _StructureSize { virtual int get() override; }
public:
	virtual String^ ToString() override;
public:
	property IntPtr data { IntPtr get(); }
	property UInt64 size { UInt64 get(); }
	property AVFrameSideDataType type { AVFrameSideDataType get(); }
	property AVDictionary^ metadata { AVDictionary^ get(); void set(AVDictionary^); }
	property AVBufferRef^ buf { AVBufferRef^ get(); void set(AVBufferRef^); }
public:
	static String^ SideDataName(AVFrameSideDataType _type);
};
//////////////////////////////////////////////////////
/// <summary>
///This structure stores compressed data. It is typically exported by demuxers
///and then passed as input to decoders, or received as output from encoders and
///then passed to muxers.
///
///For video, it should typically contain one compressed frame. For audio it may
///contain several compressed frames.
///
///AVPacket is one of the few structs in FFmpeg, whose size is a part of public
///ABI. Thus it may be allocated on stack and no new fields can be added to it
///without libavcodec and libavformat major bump.
///
///The semantics of data ownership depends on the destruct field.
///If it is set, the packet data is dynamically allocated and is valid
///indefinitely until av_free_packet() is called (which in turn calls the
///destruct callback to free the data). If destruct is not set, the packet data
///is typically backed by some static buffer somewhere and is only valid for a
///limited time (e.g. until the next read call when demuxing).
///
///The side data is always allocated with av_malloc() and is freed in
///av_free_packet().
/// </summary>
// AVPacket
public ref class AVPacket : public AVBase
						  , public ICloneable
{
private:
	Object^			m_pOpaque;
	AVBufferFreeCB^	m_pFreeCB;
internal:
	AVPacket(void * _pointer,AVBase^ _parent);
public:
	/// Allocate an AVPacket and set its fields to default values. 
	AVPacket();
	// Allocate the payload of a packet and initialize its fields with default values.
	AVPacket(int _size);
	/// Initialize a reference-counted packet from allocated data.
	/// Data Must be freed separately
	AVPacket(IntPtr _data,int _size);
	/// Initialize a reference-counted packet from allocated data.
	/// With callback For data Free
	AVPacket(IntPtr _data,int _size,AVBufferFreeCB^ free_cb,Object^ opaque);
	/// Initialize a reference-counted packet with given buffer
	AVPacket(AVBufferRef^ buf);
	// Create a new packet that references the same data as src.
	AVPacket(AVPacket^ _packet);
	~AVPacket();
public:
	property int _StructureSize { virtual int get() override; }
public:
    ///A reference to the reference-counted buffer where the packet data is
    ///stored.
    ///May be NULL, then the packet data is not reference-counted.
	property AVBufferRef^ buf { AVBufferRef^ get(); }

	///<summary>
	///Presentation timestamp in AVStream->time_base units; the time at which
	///the decompressed packet will be presented to the user.
	///Can be AV_NOPTS_VALUE if it is not stored in the file.
	///pts MUST be larger or equal to dts as presentation cannot happen before
	///decompression, unless one wants to view hex dumps. Some formats misuse
	///the terms dts and pts/cts to mean something different. Such timestamps
	///must be converted to true pts/dts before they are stored in AVPacket.
	///</summary>
	property Int64 pts { Int64 get(); void set(Int64); }

	///<summary>
	///Decompression timestamp in AVStream->time_base units; the time at which
	///the packet is decompressed.
	///Can be AV_NOPTS_VALUE if it is not stored in the file.
	///</summary>
	property Int64 dts { Int64 get(); void set(Int64); }
	property AVMemPtr^ data { AVMemPtr^ get(); void set(AVMemPtr^); }
	property int size { int get(); void set(int); }
	property int stream_index { int get(); void set(int); }

	///<summary>
	///A combination of AV_PKT_FLAG values
	///</summary>
	property AvPktFlag flags { AvPktFlag get(); void set(AvPktFlag); }
	
	///<summary>
	/// Additional packet data that can be provided by the container.
	/// Packet can contain several types of side information.
	///</summary>
	property array<AVPacketSideData^>^ side_data { array<AVPacketSideData^>^ get(); }

	///<summary>
	///Duration of this packet in AVStream->time_base units, 0 if unknown.
	///Equals next_pts - this_pts in presentation order.
	///</summary>
	property Int64 duration { Int64 get(); void set(Int64 ); }

	///< byte position in stream, -1 if unknown
	property Int64 pos { Int64 get(); void set(Int64); }

	///<summary>
	///Time difference in AVStream->time_base units from the pts of this
	///packet to the point at which the output from the decoder has converged
	///independent from the availability of previous frames. That is, the
	///frames are virtually identical no matter if decoding started from
	///the very first frame or from this keyframe.
	///Is AV_NOPTS_VALUE if unknown.
	///This field is not the display duration of the current packet.
	///This field has no meaning if the packet does not have AV_PKT_FLAG_KEY
	///set.
	///
	///The purpose of this field is to allow seeking in streams that have no
	///keyframes in the conventional sense. It corresponds to the
	///recovery point SEI in H.264 and match_time_delta in NUT. It is also
	///essential for some types of subtitle streams to ensure that all
	///subtitles are correctly displayed after seeking.
	///</summary>
	property Int64 convergence_duration { Int64 get(); void set(Int64); }
public:
	// Unref Packet
	void Free();
	// Reduce packet size, correctly zeroing padding
	void Shrink(int _size);
	AVRESULT Alloc(int _size);
	// Increase packet size, correctly zeroing padding
	AVRESULT Grow(int _size);
	void RescaleTS(AVRational^ tb_src, AVRational^ tb_dst);
	bool CopyProps(AVPacket^ _dest);
	void MoveTo(AVPacket^ _packet);
	// Allocate new information of a packet.
	AVPacketSideData^ NewSideData(AVPacketSideDataType _type, int _size);
	IntPtr NewSideDataPtr(AVPacketSideDataType _type, int _size);
	IntPtr GetSideDataPtr(AVPacketSideDataType _type,Int64 % _size);
	bool AddSideData(AVPacketSideDataType _type, IntPtr data, int _size);
	bool AddSideData(AVPacketSideDataType _type, array<byte>^ data);
	bool ShrinkSideData(AVPacketSideDataType _type, int _size);
	bool CopySideData(AVPacket^ _dest);
	void FreeSideData();
	AVPacketSideData^ GetSideData(AVPacketSideDataType _type);
public:
	/// Create a writable reference for the data described by a given packet,
	/// avoiding data copy if possible.
	bool MakeWritable();
protected:
	// deprecated
	bool MergeSideData();
	// deprecated
	bool SplitSideData();
public:
	bool Dump(String^ sFileName,bool bAppend);
	bool Dump(String^ sFileName);
	bool Dump(Stream^ _stream);
public:
	array<byte>^ ToArray();
public:
	static AVPacket^ FromArray(array<byte>^ _data);
	static AVPacket^ FromArray(array<int>^ _data);
	static AVPacket^ FromArray(array<unsigned int>^ _data);
	static AVPacket^ FromArray(array<Int64>^ _data);
	static AVPacket^ FromArray(array<short>^ _data);
	static AVPacket^ FromArray(array<unsigned short>^ _data);
	static AVPacket^ FromArray(array<float>^ _data);
	static AVPacket^ FromArray(array<double>^ _data);
public:
	// ICloneable
	virtual Object^ Clone();
internal:
	virtual void FreeCB(IntPtr data);
};
//////////////////////////////////////////////////////
// AVPicture
public ref class AVPicture : public AVBase
{
public:
	static const int NUM_DATA_POINTERS = AV_NUM_DATA_POINTERS;
private:
	// Width Of the Allocated Data
	int				m_nWidth;
	// Height Of the Allocated Data
	int				m_nHeight;
	// Format Of the Allocated Data
	AVPixelFormat	m_nFormat;
protected:
	int  m_nNumDataPointers;
	int	 m_nDataAllocated;
internal:
	AVPicture(void * _pointer,AVBase^ _parent);
	AVPicture(uint8_t ** _data,int* linesize,int cch ,AVBase^ _parent);
public:
	AVPicture();
	AVPicture(AVPixelFormat pix_fmt, int width, int height);
	AVPicture(AVPixelFormat pix_fmt, int width, int height,int align);
	~AVPicture();
	!AVPicture();
public:
	property int _StructureSize { virtual int get() override; }
public:
	///< pointers to the image data planes
	property AVArray<AVMemPtr^>^ data { AVArray<AVMemPtr^>^ get(); }
	///< number of bytes per line
	property AVArray<int>^ linesize { AVArray<int>^ get(); }
public:
	/// Gets allocated Buffer size / 0 - if not allocated
	property int AllocatedSize { int get() { return m_nDataAllocated; } }
public:
	/// width and height of the video frame
	property int width  { virtual int get(); virtual void set(int); }
	property int height { virtual int get(); virtual void set(int); }
	/// AVPixelFormat
	property int format { virtual int get(); virtual void set(int); }
public:
	/// Allocate memory for the pixels of a picture and setup the AVPicture fields for it.
	virtual int Alloc(AVPixelFormat pix_fmt, int width, int height);
	/// Allocate an image with size w and h and pixel format pix_fmt, and
	/// fill pointers and linesizes accordingly.
	/// The allocated image buffer has to be freed by using
	virtual int Alloc(AVPixelFormat pix_fmt, int width, int height,int align);
	/// Free a picture previously allocated by avpicture_alloc().
	/// The data buffer used by the AVPicture is freed, but the AVPicture structure
	/// itself is not.
	virtual void Free();
	/// Setup the picture fields based on the specified image parameters
	/// and the provided image data buffer.
	/// 
	/// The picture fields are filled in by using the image data buffer
	/// pointed to by ptr.
	/// 
	/// If ptr is NULL, the function will fill only the picture linesize
	/// array and return the required size for the image buffer.
	/// 
	/// To allocate an image buffer and fill the picture data in one call,
	/// use Alloc().
	/// @return the size in bytes required for src, a negative error code
	/// in case of failure
	int Fill(IntPtr ptr,AVPixelFormat pix_fmt, int width, int height);
	/// Copy pixel data from an AVPicture into a buffer.
	/// GetSize() can be used to compute the required size for
	/// the buffer to fill.
	int Layout(AVPixelFormat pix_fmt,int width, int height,IntPtr dest, int dest_size);
	/// Copy image to dst
	void CopyTo(AVPicture^ dst,AVPixelFormat pix_fmt, int width, int height);
	/// Copy image data from an image into a buffer.
	///
	/// av_image_get_buffer_size() can be used to compute the required size
	/// for the buffer to fill.
	///
	/// @param dst           a buffer into which picture data will be copied
	/// @param dst_size      the size in bytes of dst
	/// @param src_data      pointers containing the source image data
	/// @param src_linesizes linesizes for the image in src_data
	/// @param pix_fmt       the pixel format of the source image
	/// @param width         the width of the source image in pixels
	/// @param height        the height of the source image in pixels
	/// @param align         the assumed linesize alignment for dst
	/// @return the number of bytes written to dst, or a negative value
	/// (error code) on error
	int CopyTo(IntPtr dst,int dst_size,AVPixelFormat pix_fmt, int width, int height, int align);
	/// Crop image top and left side.
	AVRESULT Crop(AVPicture^ dst,AVPixelFormat pix_fmt, int top_band, int left_band);
	/// Pad image.
	AVRESULT Pad(AVPicture^ dst,AVPixelFormat pix_fmt, int width, int height,
		int padtop, int padbottom, int padleft, int padright, array<int>^ color);
public:
	/// Calculate the size in bytes that a picture of the given width and height
	/// would occupy if stored in the given picture format.
	static int GetSize(AVPixelFormat pix_fmt, int width, int height);

	/// Return the size in bytes of the amount of data required to store an
	/// image with the given parameters.
	///
	/// @param[in] align the assumed linesize alignment
	static int GetSize(AVPixelFormat pix_fmt, int width, int height,int align);

	/// Compute the max pixel step for each plane of an image with a
	/// format described by pixdesc.
	///
	/// The pixel step is the distance in bytes between the first byte of
	/// the group of bytes which describe a pixel component and the first
	/// byte of the successive group in the same plane for the same
	/// component.
	///
	/// @param max_pixsteps an array which is filled with the max pixel step
	/// for each plane. Since a plane may contain different pixel
	/// components, the computed max_pixsteps[plane] is relative to the
	/// component in the plane with the max pixel step.
	/// @param max_pixstep_comps an array which is filled with the component
	/// for each plane which has the max pixel step. May be NULL.
	static void FillMaxPixsteps(array<int>^ max_pixsteps,array<int>^ max_pixstep_comps,AVPixFmtDescriptor^ pixdesc);

	/// Compute the size of an image line with format pix_fmt and width
	/// width for the plane plane.
	///
	/// @return the computed size in bytes
	static int GetLinesize(AVPixelFormat pix_fmt, int width, int plane);

	/// Fill plane linesizes for an image with pixel format pix_fmt and
	/// width width.
	///
	/// @param linesizes array to be filled with the linesize for each plane
	/// @return >= 0 in case of success, a negative error code otherwise
	static int FillLinesizes(array<int>^ linesizes, AVPixelFormat pix_fmt, int width);
	static int FillLinesizes(AVArray<int>^ linesizes, AVPixelFormat pix_fmt, int width);

	/// Fill plane data pointers for an image with pixel format pix_fmt and
	/// height height.
	///
	/// @param data pointers array to be filled with the pointer for each image plane
	/// @param ptr the pointer to a buffer which will contain the image
	/// @param linesizes the array containing the linesize for each
	/// plane, should be filled by av_image_fill_linesizes()
	/// @return the size in bytes required for the image buffer, a negative
	/// error code in case of failure
	static int FillPointers(array<IntPtr>^ pointers, AVPixelFormat pix_fmt, int height,IntPtr ptr,array<int>^ linesizes);
	static int FillPointers(AVArray<IntPtr>^ pointers, AVPixelFormat pix_fmt, int height, IntPtr ptr, AVArray<int>^ linesizes);

	/// Check if the given dimension of an image is valid, meaning that all
	/// bytes of the image can be addressed with a signed int.
	///
	/// @param w the width of the picture
	/// @param h the height of the picture
	/// @param log_offset the offset to sum to the log level for logging with log_ctx
	/// @param log_ctx the parent logging context, it may be NULL
	/// @return >= 0 if valid, a negative error code otherwise
	static bool CheckSize(UInt32 w, UInt32 h);

	/// Check if the given sample aspect ratio of an image is valid.
	///
	/// It is considered invalid if the denominator is 0 or if applying the ratio
	/// to the image size would make the smaller dimension less than 1. If the
	/// sar numerator is 0, it is considered unknown and will return as valid.
	///
	/// @param w width of the image
	/// @param h height of the image
	/// @param sar sample aspect ratio of the image
	/// @return 0 if valid, a negative AVERROR code otherwise
	static bool CheckSAR(UInt32 w, UInt32 h, AVRational^ sar);

	/// Copy image plane from src to dst.
	/// That is, copy "height" number of lines of "bytewidth" bytes each.
	/// The first byte of each successive line is separated by ///_linesize
	/// bytes.
	///
	/// bytewidth must be contained by both absolute values of dst_linesize
	/// and src_linesize, otherwise the function behavior is undefined.
	///
	/// @param dst_linesize linesize for the image plane in dst
	/// @param src_linesize linesize for the image plane in src
	static void CopyPlane(IntPtr dst, int dst_linesize,IntPtr src, int src_linesize,int bytewidth, int height);
};
//////////////////////////////////////////////////////
// AVPanScan
public ref class AVPanScan : public AVBase
{
public:
	ref class AVPosition
	{
	private:
		AVPanScan^ m_Parent;
	internal:
		AVPosition(AVPanScan^ _parent): m_Parent(_parent) {}
	public:
		property short default[int,int] { short get(int x,int y) { return m_Parent->GetPosition(x,y); } void set(int x,int y,short value) { m_Parent->SetPosition(x,y,value);} }
	};
internal:
	AVPanScan(void * _pointer,AVBase^ _parent);
public:
	property int _StructureSize { virtual int get() override; }
public:
	///<summary>
	/// id
	/// - encoding: Set by user.
	/// - decoding: Set by libavcodec.
	///</summary>
	property int id { int get(); void set(int); }
	///<summary>
	/// width and height in 1/16 pel
	/// - encoding: Set by user.
	/// - decoding: Set by libavcodec.
	///</summary>
	property int width { int get(); void set(int); }
	property int height { int get(); void set(int); }
	///<summary>
	/// position of the top left corner in 1/16 pel for up to 3 fields/frames
	/// - encoding: Set by user.
	/// - decoding: Set by libavcodec.
	///</summary>
	property AVPosition^ position { AVPosition^ get(); }
private:
	short GetPosition(int x,int y);
	void SetPosition(int x,int y,short position);
};
//////////////////////////////////////////////////////
/// 
 /// This structure describes decoded (raw) audio or video data.
 /// 
 /// AVFrame must be allocated using av_frame_alloc(). Note that this only
 /// allocates the AVFrame itself, the buffers for the data must be managed
 /// through other means (see below).
 /// AVFrame must be freed with av_frame_free().
 /// 
 /// AVFrame is typically allocated once and then reused multiple times to hold
 /// different data (e.g. a single AVFrame to hold frames received from a
 /// decoder). In such a case, av_frame_unref() will free any references held by
 /// the frame and reset it to its original clean state before it
 /// is reused again.
 /// 
 /// The data described by an AVFrame is usually reference counted through the
 /// AVBuffer API. The underlying buffer references are stored in AVFrame.buf /
 /// AVFrame.extended_buf. An AVFrame is considered to be reference counted if at
 /// least one reference is set, i.e. if AVFrame.buf[0] != NULL. In such a case,
 /// every single data plane must be contained in one of the buffers in
 /// AVFrame.buf or AVFrame.extended_buf.
 /// There may be a single buffer for all the data, or one separate buffer for
 /// each plane, or anything in between.
 /// 
 /// sizeof(AVFrame) is not a part of the public ABI, so new fields may be added
 /// to the end with a minor bump.
 /// Similarly fields that are marked as to be only accessed by
 /// av_opt_ptr() can be reordered. This allows 2 forks to add fields
 /// without breaking compatibility with each other.
 /// 
public ref class AVFrame : public AVPicture
						 , public ICloneable
{
private:
	ref class AVImage : public AVBase
	{
	private:
		System::Drawing::Bitmap^ m_pBitmap;
		System::Drawing::Imaging::BitmapData^ m_pData;
		bool m_bDeleteBitmap;
	public:
		AVImage(System::Drawing::Bitmap^ _image,AVFrame^ _frame,bool bDeleteBitmap);
		~AVImage();
	public:
		property System::Drawing::Imaging::BitmapData^ data { System::Drawing::Imaging::BitmapData^ get(); }
	};
public:
	ref class AVBufferRefArray
	{
	private:
		AVFrame^ m_pParent;
	internal:
		AVBufferRefArray(AVFrame^ _parent):m_pParent(_parent) {}
	public:
		property AVBufferRef^ default[int] { AVBufferRef^ get(int index) { return m_pParent->GetBuf(index); } void set(int index,AVBufferRef^ value) { m_pParent->SetBuf(index,value); } }
	};
internal:
	AVFrame(void * _pointer,AVBase^ _parent);
public:
	AVFrame();
public:
	property int _StructureSize { virtual int get() override; }
public:
	/// pointers to the data planes/channels.
	/// For video, this should simply point to data[].
	/// 
	/// For planar audio, each channel has a separate data pointer, and
	/// linesize[0] contains the size of each channel buffer.
	/// For packed audio, there is just one data pointer, and linesize[0]
	/// contains the total size of the buffer for all channels.
	/// 
	/// Note: Both data and extended_data should always be set in a valid frame,
	/// but for planar audio with more channels that can fit in data,
	/// extended_data must be used in order to access all channels.

	property IntPtr extended_data { IntPtr get(); }

	/// width and height of the video frame
	property int width  { int get() override; void set(int) override; }
	property int height { int get() override; void set(int) override; }
	/// number of audio samples (per channel) described by this frame
	property int nb_samples { int get(); void set(int); }

	/// format of the frame, -1 if unknown or unset
	/// Values correspond to enum AVPixelFormat for video frames,
	/// enum AVSampleFormat for audio)
	/// 
	property int format { int get() override; void set(int) override; }
	/// 
	/// 1 -> keyframe, 0-> not
	/// 
	property bool key_frame { bool get(); void set(bool ); }
	/// 
	/// Picture type of the frame.
	/// 
	property AVPictureType pict_type { AVPictureType get(); void set(AVPictureType); }

	/// 
	/// Sample aspect ratio for the video frame, 0/1 if unknown/unspecified.
	/// 
	property AVRational^ sample_aspect_ratio { AVRational^ get(); void set(AVRational^); }

	/// 
	/// Presentation timestamp in time_base units (time when frame should be shown to user).
	/// 
	property Int64 pts { Int64 get(); void set(Int64); }

	/// 
	/// PTS copied from the AVPacket that was decoded to produce this frame.
	/// 
	property Int64 pkt_pts { Int64 get(); void set(Int64); }

	/// 
	/// DTS copied from the AVPacket that triggered returning this frame. (if frame threading isn't used)
	/// This is also the Presentation time of this AVFrame calculated from
	/// only AVPacket.dts values without pts values.
	/// 
	property Int64 pkt_dts { Int64 get(); void set(Int64); }

	/// 
	/// picture number in bitstream order
	/// 
	property int coded_picture_number { int get(); void set(int); }
	/// 
	/// picture number in display order
	/// 
	property int display_picture_number { int get(); void set(int); }

	/// 
	/// quality (between 1 (good) and FF_LAMBDA_MAX (bad))
	/// 
	property int quality { int get(); void set(int); }

	/// 
	/// for some private data of the user
	/// 
	property IntPtr opaque { IntPtr get(); void set(IntPtr); }

	/// 
	/// error
	/// 
	property AVArray<UInt64>^ error{ AVArray<UInt64>^ get(); }

	/// 
	/// When decoding, this signals how much the picture must be delayed.
	/// extra_delay = repeat_pict / (2*fps)
	/// 
	property int repeat_pict { int get(); void set(int); }

	/// 
	/// The content of the picture is interlaced.
	/// 
	property int interlaced_frame { int get(); void set(int); }

	/// 
	/// If the content is interlaced, is top field displayed first.
	/// 
	property int top_field_first { int get(); void set(int); }

	/// 
	/// Tell user application that palette has changed from previous frame.
	/// 
	property int palette_has_changed { int get(); void set(int); }

	/// 
	/// reordered opaque 64bit (generally an integer or a double precision float
	/// PTS but can be anything).
	/// The user sets AVCodecContext.reordered_opaque to represent the input at
	/// that time,
	/// the decoder reorders values as needed and sets AVFrame.reordered_opaque
	/// to exactly one of the values provided by the user through AVCodecContext.reordered_opaque
	/// @deprecated in favor of pkt_pts
	/// 
	property Int64 reordered_opaque { Int64 get(); void set(Int64); }

	/// 
	/// Sample rate of the audio data.
	/// 
	property int sample_rate { int get(); void set(int); }

	/// 
	/// Channel layout of the audio data.
	/// 
	property AVChannelLayout channel_layout { AVChannelLayout get(); void set(AVChannelLayout); }

	/// 
	/// AVBuffer references backing the data for this frame. If all elements of
	/// this array are NULL, then this frame is not reference counted. This array
	/// must be filled contiguously -- if buf[i] is non-NULL then buf[j] must
	/// also be non-NULL for all j < i.
	/// 
	/// There may be at most one AVBuffer per data plane, so for video this array
	/// always contains all the references. For planar audio with more than
	/// AV_NUM_DATA_POINTERS channels, there may be more buffers than can fit in
	/// this array. Then the extra AVBufferRef pointers are stored in the
	/// extended_buf array.
	/// 
	property AVBufferRefArray^ buf { AVBufferRefArray^ get(); }

	/// 
	/// For planar audio which requires more than AV_NUM_DATA_POINTERS
	/// AVBufferRef pointers, this array will hold all the references which
	/// cannot fit into AVFrame.buf.
	/// 
	/// Note that this is different from AVFrame.extended_data, which always
	/// contains all the pointers. This array only contains the extra pointers,
	/// which cannot fit into AVFrame.buf.
	/// 
	/// This array is always allocated using av_malloc() by whoever constructs
	/// the frame. It is freed in av_frame_unref().
	/// 
	property array<AVBufferRef^>^ extended_buf { array<AVBufferRef^>^ get(); void set(array<AVBufferRef^>^ value); }

	/// 
	/// Number of elements in extended_buf.
	/// 
	property int nb_extended_buf { int get(); }

	property array<AVFrameSideData^>^ side_data { array<AVFrameSideData^>^ get(); }

	/// 
	/// Frame flags, a combination of @ref lavu_frame_flags
	/// 
	property AVFrameFlags flags { AVFrameFlags get(); void set(AVFrameFlags); }

	/// 
	/// MPEG vs JPEG YUV range.
	/// It must be accessed using av_frame_get_color_range() and
	/// av_frame_set_color_range().
	/// - encoding: Set by user
	/// - decoding: Set by libavcodec
	/// 
	property AVColorRange color_range { AVColorRange get(); void set(AVColorRange); }

	property AVColorPrimaries color_primaries { AVColorPrimaries get(); void set(AVColorPrimaries); }

	property AVColorTransferCharacteristic color_trc { AVColorTransferCharacteristic get(); void set(AVColorTransferCharacteristic); }

	/// 
	/// YUV colorspace type.
	/// It must be accessed using av_frame_get_colorspace() and
	/// av_frame_set_colorspace().
	/// - encoding: Set by user
	/// - decoding: Set by libavcodec
	/// 
	property AVColorSpace colorspace { AVColorSpace get(); void set(AVColorSpace); }

	property AVChromaLocation chroma_location { AVChromaLocation get(); void set(AVChromaLocation); }

	/// 
	/// frame timestamp estimated using various heuristics, in stream time base
	/// Code outside libavcodec should access this field using:
	/// av_frame_get_best_effort_timestamp(frame)
	/// - encoding: unused
	/// - decoding: set by libavcodec, read by user.
	/// 
	property Int64 best_effort_timestamp { Int64 get(); void set(Int64); }

	/// 
	/// reordered pos from the last AVPacket that has been input into the decoder
	/// Code outside libavcodec should access this field using:
	/// av_frame_get_pkt_pos(frame)
	/// - encoding: unused
	/// - decoding: Read by user.
	/// 
	property Int64 pkt_pos { Int64 get(); void set(Int64); }

	/// 
	/// duration of the corresponding packet, expressed in
	/// AVStream->time_base units, 0 if unknown.
	/// Code outside libavcodec should access this field using:
	/// av_frame_get_pkt_duration(frame)
	/// - encoding: unused
	/// - decoding: Read by user.
	/// 
	property Int64 pkt_duration { Int64 get(); void set(Int64); }

	/// 
	/// metadata.
	/// Code outside libavcodec should access this field using:
	/// av_frame_get_metadata(frame)
	/// - encoding: Set by user.
	/// - decoding: Set by libavcodec.
	/// 
	property AVDictionary^ metadata { AVDictionary^ get(); void set(AVDictionary^); }

	/// 
	/// decode error flags of the frame, set to a combination of
	/// FF_DECODE_ERROR_xxx flags if the decoder produced a frame, but there
	/// were errors during the decoding.
	/// Code outside libavcodec should access this field using:
	/// av_frame_get_decode_error_flags(frame)
	/// - encoding: unused
	/// - decoding: set by libavcodec, read by user.
	/// 
	property FFDecodeError decode_error_flags { FFDecodeError get(); void set(FFDecodeError); }

	/// 
	/// number of audio channels, only used for audio.
	/// Code outside libavcodec should access this field using:
	/// av_frame_get_channels(frame)
	/// - encoding: unused
	/// - decoding: Read by user.
	/// 
	property int channels { int get(); void set(int); }

	/// 
	/// size of the corresponding packet containing the compressed
	/// frame. It must be accessed using av_frame_get_pkt_size() and
	/// av_frame_set_pkt_size().
	/// It is set to a negative value if unknown.
	/// - encoding: unused
	/// - decoding: set by libavcodec, read by user.
	/// 
	property int pkt_size { int get(); void set(int); }
private:
	AVBufferRef^ GetBuf(int index);
	void SetBuf(int index,AVBufferRef^ value);
public:
	int Alloc(AVPixelFormat pix_fmt, int width, int height)override;
	int Alloc(AVPixelFormat pix_fmt, int width, int height, int align)override;

	int Alloc(AVSampleFormat sample_fmt, AVChannelLayout ch, int nb_samples);
	int Alloc(AVSampleFormat sample_fmt, AVChannelLayout ch, int nb_samples, int align);

	/// Check if the frame data is writable.
	///
	/// @return A positive value if the frame data is writable (which is true if and
	/// only if each of the underlying buffers has only one reference, namely the one
	/// stored in this frame). Return 0 otherwise.
	///
	/// If 1 is returned the answer is valid until av_buffer_ref() is called on any
	/// of the underlying AVBufferRefs (e.g. through av_frame_ref() or directly).
	property bool IsWritable{ bool get(); }

	/// Ensure that the frame data is writable, avoiding data copy if possible.
	///
	/// Do nothing if the frame is writable, allocate new buffers and copy the data
	/// if it is not.
	///
	/// @return 0 on success, a negative AVERROR on error.
	bool MakeWritable();

	/// Move everything contained in src to dst and reset src.
	void Move(AVFrame^ dst);

	/// Allocate new buffer(s) for audio or video data.
	///
	/// The following fields must be set on frame before calling this function:
	/// - format (pixel format for video, sample format for audio)
	/// - width and height for video
	/// - nb_samples and channel_layout for audio
	///
	/// This function will fill AVFrame.data and AVFrame.buf arrays and, if
	/// necessary, allocate and fill AVFrame.extended_data and AVFrame.extended_buf.
	/// For planar formats, one buffer will be allocated for each plane.
	///
	/// @param frame frame in which to store the new buffers.
	/// @param align required buffer size alignment
	///
	/// @return 0 on success, a negative AVERROR on error.
	AVRESULT AllocateBuffers();
	AVRESULT AllocateBuffers(int align);

	/// Copy the frame data from src to dst.
	///
	/// This function does not allocate anything, dst must be already initialized and
	/// allocated with the same parameters as src.
	///
	/// This function only copies the frame data (i.e. the contents of the data /
	/// extended data arrays), not any other properties.
	///
	/// @return >= 0 on success, a negative AVERROR on error.
	AVRESULT Copy(AVFrame^ dst);
	
	/// Copy Video Frame Data 
	void CopyTo(AVPicture^ dst);

	/// Copy only "metadata" fields from src to dst.
	///
	/// Metadata for the purpose of this function are those fields that do not affect
	/// the data layout in the buffers.  E.g. pts, sample rate (for audio) or sample
	/// aspect ratio (for video), but not width/height or channel layout.
	/// Side data is also copied.
	AVRESULT CopyProps(AVFrame^ dst);

	/// Get the buffer reference a given data plane is stored in.
	///
	/// @param plane index of the data plane of interest in frame->extended_data.
	///
	/// @return the buffer reference that contains the plane or NULL if the input
	/// frame is not valid.
	AVBufferRef^ GetPlaneBuffer(int plane);

	/// Add a new side data to a frame.
	///
	/// @param frame a frame to which the side data should be added
	/// @param type type of the added side data
	/// @param size size of the side data
	///
	/// @return newly added side data on success, NULL on error
	AVFrameSideData^ NewSideData(AVFrameSideDataType type,int size);

	/// @return a pointer to the side data of a given type on success, NULL if there
	/// is no side data with such type in this frame.
	AVFrameSideData^ GetSideData(AVFrameSideDataType type);

	/// If side data of the supplied type exists in the frame, free it and remove it
	/// from the frame.
	void RemoveSideData(AVFrameSideDataType type);

	/// Fill AVFrame audio data and linesize pointers.
	///
	/// The buffer buf must be a preallocated buffer with a size big enough
	/// to contain the specified samples amount. The filled AVFrame data
	/// pointers will point to this buffer.
	///
	/// AVFrame extended_data channel pointers are allocated if necessary for
	/// planar audio.
	///
	/// @param frame       the AVFrame
	///                    frame->nb_samples must be set prior to calling the
	///                    function. This function fills in frame->data,
	///                    frame->extended_data, frame->linesize[0].
	/// @param nb_channels channel count
	/// @param sample_fmt  sample format
	/// @param buf         buffer to use for frame data
	/// @param buf_size    size of buffer
	/// @param align       plane size sample alignment (0 = default)
	/// @return            >=0 on success, negative error code on failure
	/// @todo return the size in bytes required to store the samples in
	/// case of success, at the next libavutil bump
	int FillAudio(int nb_channels,AVSampleFormat sample_fmt,IntPtr buf,int buf_size, int align);
	/// Unreference all the buffers referenced by frame and reset the frame fields.
	 void Free() override;
public:
	// ICloneable
	virtual Object^ Clone();
public:
	System::Drawing::Bitmap^ ToBitmap();
public:
	static String^ GetColorspaceName(AVColorSpace val);
public:
	// Create Frame From Image
	static AVFrame^ FromImage(System::Drawing::Bitmap^ _image,AVPixelFormat _format);
	static AVFrame^ FromImage(System::Drawing::Bitmap^ _image);
	static AVFrame^ FromImage(System::Drawing::Bitmap^ _image, AVPixelFormat _format,bool bDeleteBitmap);
	static AVFrame^ FromImage(System::Drawing::Bitmap^ _image, bool bDeleteBitmap);
	// Convert Frame To Different Colorspace
	static AVFrame^ ConvertVideoFrame(AVFrame^ _source,AVPixelFormat _format);
	// Convert Frame To Bitmap
	static System::Drawing::Bitmap^ ToBitmap(AVFrame^ _frame);
public:
	/// @return AVClass for AVFrame.
	///
	/// @see av_opt_find().
	static AVClass^ GetClass();
};
//////////////////////////////////////////////////////
public delegate void DrawHorizBandHandler(AVCodecContext^ s, AVFrame^ src, array<int>^ offset,int y, int type, int height);
public delegate AVPixelFormat GetFormatHandler(AVCodecContext^ s, array<AVPixelFormat>^ fmt);
public delegate AVRESULT GetBuffer2Handler(AVCodecContext^ s, AVFrame^ frame, int flags);
//////////////////////////////////////////////////////
// AVHWAccel
public ref class AVHWAccel : public AVBase
{
internal:
	AVHWAccel(void * _pointer,AVBase^ _parent);
public:
	virtual String^ ToString() override;
public:
	/// Name of the hardware accelerated codec.
	/// The name is globally unique among encoders and among decoders (but an
	/// encoder and a decoder can share the same name).
	property String^ name { String^ get(); }
	/// Type of codec implemented by the hardware accelerator.
	///
	/// See AVMEDIA_TYPE_xxx
	property AVMediaType type { AVMediaType get(); }
	/// Codec implemented by the hardware accelerator.
	///
	/// See AV_CODEC_ID_xxx
	property AVCodecID id { AVCodecID get(); }
	/// Supported pixel format.
	///
	/// Only hardware accelerated formats are supported here.
	property AVPixelFormat pix_fmt  { AVPixelFormat get(); }
	/// Hardware accelerated codec capabilities.
	property AV_HWACCEL_CODEC_CAP capabilities { AV_HWACCEL_CODEC_CAP get(); }
public:
	AVHWAccel^ Next();
public:
	/// If hwaccel is NULL, returns the first registered hardware accelerator,
	/// if hwaccel is non-NULL, returns the next registered hardware accelerator
	/// after hwaccel, or NULL if hwaccel is the last one.
	static AVHWAccel^ Next(AVHWAccel^ hwaccel);
};
//////////////////////////////////////////////////////
/// AVCodecContext
/// main external API structure.
/// New fields can be added to the end with minor version bumps.
/// Removal, reordering and changes to existing fields require a major
/// version bump.
/// You can use AVOptions (av_opt* / av_set/get*()) to access these fields from user
/// applications.
/// sizeof(AVCodecContext) must not be used outside libav///.
/// 
public ref class AVCodecContext : public AVBase, ICloneable
{
protected:
	GetBuffer2Handler^		m_pGetBuffer2;
	DrawHorizBandHandler^	m_pDrawHorizBand;
	GetFormatHandler^		m_pGetFormat;
	IntPtr					m_pOpaque;
	IntPtr					m_pGetFormatNative;
	IntPtr					m_pDrawHorizBandNative;
	IntPtr					m_pGetBuffer2Native;
	GCHandle				m_ThisPtr;
internal:
	AVCodecContext(void * _pointer,AVBase^ _parent);
public:
	AVCodecContext(AVCodec^ _codec);
public:
	virtual String^ ToString() override;
public:
	property int _StructureSize { virtual int get() override; }
public:
	/// information on struct for av_log
	/// - set by avcodec_alloc_context3
	property AVClass^ av_class { AVClass^ get(); }
	property int log_level_offset { int get(); void set(int); }
	property AVMediaType codec_type { AVMediaType get(); void set(AVMediaType); }
	property AVCodec^ codec { AVCodec^ get(); void set(AVCodec^); }
	property AVCodecID codec_id { AVCodecID get(); void set(AVCodecID); }
	/// fourcc (LSB first, so "ABCD" -> ('D'<<24) + ('C'<<16) + ('B'<<8) + 'A').
	/// This is used to work around some encoder bugs.
	/// A demuxer should set this to what is stored in the field used to identify the codec.
	/// If there are multiple such fields in a container then the demuxer should choose the one
	/// which maximizes the information about the used codec.
	/// If the codec tag field in a container is larger than 32 bits then the demuxer should
	/// remap the longer ID to 32 bits with a table or other structure. Alternatively a new
	/// extra_codec_tag + size could be added but for this a clear advantage must be demonstrated
	/// first.
	/// - encoding: Set by user, if not then the default based on codec_id will be used.
	/// - decoding: Set by user, will be converted to uppercase by libavcodec during init.
	property UInt32 codec_tag { UInt32 get(); void set(UInt32); }
	property IntPtr priv_data { IntPtr get(); }
	/// Private data of the user, can be used to carry app specific stuff.
	/// - encoding: Set by user.
	/// - decoding: Set by user.
	property IntPtr opaque { IntPtr get(); void set(IntPtr); }
	/// the average bitrate
	/// - encoding: Set by user; unused for constant quantizer encoding.
	/// - decoding: Set by libavcodec. 0 or some bitrate if this info is available in the stream.
	property Int64 bit_rate { Int64 get(); void set(Int64); }
	/// number of bits the bitstream is allowed to diverge from the reference.
	///           the reference can be CBR (for CBR pass1) or VBR (for pass2)
	/// - encoding: Set by user; unused for constant quantizer encoding.
	/// - decoding: unused
	property int bit_rate_tolerance { int get(); void set(int); }
	/// Global quality for codecs which cannot change it per frame.
	/// This should be proportional to MPEG-1/2/4 qscale.
	/// - encoding: Set by user.
	/// - decoding: unused
	property int global_quality { int get(); void set(int); }
	/// - encoding: Set by user.
	/// - decoding: unused
	property int compression_level { int get(); void set(int); }
	/// AV_CODEC_FLAG_*.
	/// - encoding: Set by user.
	/// - decoding: Set by user.
	property AVCodecFlag flags { AVCodecFlag get(); void set(AVCodecFlag); }
	/// AV_CODEC_FLAG2_*
	/// - encoding: Set by user.
	/// - decoding: Set by user.
	property AVCodecFlag2 flags2 { AVCodecFlag2 get(); void set(AVCodecFlag2); }
	/// some codecs need / can use extradata like Huffman tables.
	/// mjpeg: Huffman tables
	/// rv10: additional flags
	/// mpeg4: global headers (they can be in the bitstream or here)
	/// The allocated memory should be FF_INPUT_BUFFER_PADDING_SIZE bytes larger
	/// than extradata_size to avoid problems if it is read with the bitstream reader.
	/// The bytewise contents of extradata must not depend on the architecture or CPU endianness.
	/// - encoding: Set/allocated/freed by libavcodec.
	/// - decoding: Set/allocated/freed by user.
	property array<byte>^ extra_data { array<byte>^ get(); void set(array<byte>^); }
	/// This is the fundamental unit of time (in seconds) in terms
	/// of which frame timestamps are represented. For fixed-fps content,
	/// timebase should be 1/framerate and timestamp increments should be
	/// identically 1.
	/// This often, but not always is the inverse of the frame rate or field rate
	/// for video.
	/// - encoding: MUST be set by user.
	/// - decoding: the use of this field for decoding is deprecated.
	///             Use framerate instead.
	property AVRational^ time_base { AVRational^ get(); void set(AVRational^); }
	/// For some codecs, the time base is closer to the field rate than the frame rate.
	/// Most notably, H.264 and MPEG-2 specify time_base as half of frame duration
	/// if no telecine is used ...
	///
	/// Set to time_base ticks per frame. Default 1, e.g., H.264/MPEG-2 set it to 2.
	property int ticks_per_frame { int get(); }
	/// Codec delay.
	///
	/// Encoding: Number of frames delay there will be from the encoder input to
	///           the decoder output. (we assume the decoder matches the spec)
	/// Decoding: Number of frames delay in addition to what a standard decoder
	///           as specified in the spec would produce.
	///
	/// Video:
	///   Number of frames the decoded output will be delayed relative to the
	///   encoded input.
	///
	/// Audio:
	///   For encoding, this field is unused (see initial_padding).
	///
	///   For decoding, this is the number of samples the decoder needs to
	///   output before the decoder's output is valid. When seeking, you should
	///   start decoding this many samples prior to your desired seek point.
	///
	/// - encoding: Set by libavcodec.
	/// - decoding: Set by libavcodec.
	property int delay { int get(); }
	/// picture width / height.
	/// - encoding: MUST be set by user.
	/// - decoding: May be set by the user before opening the decoder if known e.g.
	///             from the container. Some decoders will require the dimensions
	///             to be set by the caller. During decoding, the decoder may
	///             overwrite those values as required.
	property int width { int get(); void set(int); }
	property int height { int get(); void set(int); }
	/// Bitstream width / height, may be different from width/height e.g. when
	/// the decoded frame is cropped before being output or lowres is enabled.
	/// - encoding: unused
	/// - decoding: May be set by the user before opening the decoder if known
	///             e.g. from the container. During decoding, the decoder may
	///             overwrite those values as required.
	property int coded_width { int get(); void set(int); }
	property int coded_height { int get(); void set(int); }
	/// the number of pictures in a group of pictures, or 0 for intra_only
	/// - encoding: Set by user.
	/// - decoding: unused
	property int gop_size { int get(); void set(int); }
	/// Pixel format, see AV_PIX_FMT_xxx.
	/// May be set by the demuxer if known from headers.
	/// May be overridden by the decoder if it knows better.
	/// - encoding: Set by user.
	/// - decoding: Set by user if known, overridden by libavcodec if known
	property AVPixelFormat pix_fmt { AVPixelFormat get(); void set(AVPixelFormat); }
	/// If non NULL, 'draw_horiz_band' is called by the libavcodec
	/// decoder to draw a horizontal band. It improves cache usage. Not
	/// all codecs can do that. You must check the codec capabilities
	/// beforehand.
	/// When multithreading is used, it may be called from multiple threads
	/// at the same time; threads might draw different parts of the same AVFrame,
	/// or multiple AVFrames, and there is no guarantee that slices will be drawn
	/// in order.
	/// The function is also used by hardware acceleration APIs.
	/// It is called at least once during frame decoding to pass
	/// the data needed for hardware render.
	/// In that mode instead of pixel data, AVFrame points to
	/// a structure specific to the acceleration API. The application
	/// reads the structure and can change some fields to indicate progress
	/// or mark state.
	/// - encoding: unused
	/// - decoding: Set by user.
	/// @param height the height of the slice
	/// @param y the y position of the slice
	/// @param type 1->top field, 2->bottom field, 3->frame
	/// @param offset offset into the AVFrame.data from which the slice should be read
	property DrawHorizBandHandler^ draw_horiz_band { DrawHorizBandHandler^ get(); void set(DrawHorizBandHandler^); }
    /// callback to negotiate the pixelFormat
    /// @param fmt is the list of formats which are supported by the codec,
    /// it is terminated by -1 as 0 is a valid format, the formats are ordered by quality.
    /// The first is always the native one.
    /// @note The callback may be called again immediately if initialization for
    /// the selected (hardware-accelerated) pixel format failed.
    /// @warning Behavior is undefined if the callback returns a value not
    /// in the fmt list of formats.
    /// @return the chosen format
    /// - encoding: unused
    /// - decoding: Set by user, if not set the native format will be chosen.
	property GetFormatHandler^ get_format { GetFormatHandler^ get(); void set(GetFormatHandler^); }
	/// maximum number of B-frames between non-B-frames
	/// Note: The output will be delayed by max_b_frames+1 relative to the input.
	/// - encoding: Set by user.
	/// - decoding: unused
	property int max_b_frames { int get(); void set(int); }
	/// qscale factor between IP and B-frames
	/// If > 0 then the last P-frame quantizer will be used (q= lastp_q///factor+offset).
	/// If < 0 then normal ratecontrol will be done (q= -normal_q///factor+offset).
	/// - encoding: Set by user.
	/// - decoding: unused
	property float b_quant_factor { float get(); void set(float); }
	property int b_frame_strategy { int get(); void set(int); }
	/// qscale offset between IP and B-frames
	/// - encoding: Set by user.
	/// - decoding: unused
	property float b_quant_offset { float get(); void set(float); }
	/// Size of the frame reordering buffer in the decoder.
	/// For MPEG-2 it is 1 IPB or 0 low delay IP.
	/// - encoding: Set by libavcodec.
	/// - decoding: Set by libavcodec.
	property int has_b_frames { int get(); }
	/// 0-> h263 quant 1-> mpeg quant
	/// - encoding: Set by user.
	/// - decoding: unused
	property int mpeg_quant { int get(); void set(int); }
	/// qscale factor between P and I-frames
	/// If > 0 then the last p frame quantizer will be used (q= lastp_q///factor+offset).
	/// If < 0 then normal ratecontrol will be done (q= -normal_q///factor+offset).
	/// - encoding: Set by user.
	/// - decoding: unused
	property float i_quant_factor { float get(); void set(float); }
	/// qscale offset between P and I-frames
	/// - encoding: Set by user.
	/// - decoding: unused
	property float i_quant_offset { float get(); void set(float); }
	/// luminance masking (0-> disabled)
	/// - encoding: Set by user.
	/// - decoding: unused
	property float lumi_masking { float get(); void set(float); }
	/// temporary complexity masking (0-> disabled)
	/// - encoding: Set by user.
	/// - decoding: unused
	property float temporal_cplx_masking { float get(); void set(float); }
	/// spatial complexity masking (0-> disabled)
	/// - encoding: Set by user.
	/// - decoding: unused
	property float spatial_cplx_masking { float get(); void set(float); }
	/// p block masking (0-> disabled)
	/// - encoding: Set by user.
	/// - decoding: unused
	property float p_masking { float get(); void set(float); }
	/// darkness masking (0-> disabled)
	/// - encoding: Set by user.
	/// - decoding: unused
	property float dark_masking { float get(); void set(float); }
	/// slice count
	/// - encoding: Set by libavcodec.
	/// - decoding: Set by user (or 0).
	property int slice_count { int get(); void set(int); }
	/// prediction method (needed for huffyuv)
	/// - encoding: Set by user.
	/// - decoding: unused
	property FFPred prediction_method { FFPred get(); void set(FFPred); }
	/// slice offsets in the frame in bytes
	/// - encoding: Set/allocated by libavcodec.
	/// - decoding: Set/allocated by user (or NULL).
	property IntPtr slice_offset { IntPtr get(); void set(IntPtr ); }
	/// sample aspect ratio (0 if unknown)
	/// That is the width of a pixel divided by the height of the pixel.
	/// Numerator and denominator must be relatively prime and smaller than 256 for some video standards.
	/// - encoding: Set by user.
	/// - decoding: Set by libavcodec.
	property AVRational^ sample_aspect_ratio { AVRational^ get(); void set(AVRational^); }
	/// motion estimation comparison function
	/// - encoding: Set by user.
	/// - decoding: unused
	property FFCmp me_cmp { FFCmp get(); void set(FFCmp); }
	/// subpixel motion estimation comparison function
	/// - encoding: Set by user.
	/// - decoding: unused
	property FFCmp me_sub_cmp { FFCmp get(); void set(FFCmp); }
	/// macroblock comparison function (not supported yet)
	/// - encoding: Set by user.
	/// - decoding: unused
	property FFCmp mb_cmp { FFCmp get(); void set(FFCmp); }
	/// interlaced DCT comparison function
	/// - encoding: Set by user.
	/// - decoding: unused
	property FFCmp ildct_cmp { FFCmp get(); void set(FFCmp); }
	/// ME diamond size & shape
	/// - encoding: Set by user.
	/// - decoding: unused
	property int dia_size { int get(); void set(int); }
	/// amount of previous MV predictors (2a+1 x 2a+1 square)
	/// - encoding: Set by user.
	/// - decoding: unused
	property int last_predictor_count { int get(); void set(int); }
	/// prepass for motion estimation
	/// - encoding: Set by user.
	/// - decoding: unused
	property int pre_me { int get(); void set(int); }
	/// motion estimation prepass comparison function
	/// - encoding: Set by user.
	/// - decoding: unused
	property FFCmp me_pre_cmp { FFCmp get(); void set(FFCmp); }
	/// ME prepass diamond size & shape
	/// - encoding: Set by user.
	/// - decoding: unused
	property int pre_dia_size { int get(); void set(int); }
	/// subpel ME quality
	/// - encoding: Set by user.
	/// - decoding: unused
	property int me_subpel_quality { int get(); void set(int); }
	/// maximum motion estimation search range in subpel units
	/// If 0 then no limit.
	///
	/// - encoding: Set by user.
	/// - decoding: unused
	property int me_range { int get(); void set(int); }
	/// slice flags
	/// - encoding: unused
	/// - decoding: Set by user.
	property SliceFlag slice_flags { SliceFlag get(); void set(SliceFlag); } 
	/// macroblock decision mode
	/// - encoding: Set by user.
	/// - decoding: unused
	property FFMbDecision mb_decision { FFMbDecision get(); void set(FFMbDecision); } 
	/// custom intra quantization matrix
	/// - encoding: Set by user, can be NULL.
	/// - decoding: Set by libavcodec.
	property IntPtr intra_matrix { IntPtr get(); void set(IntPtr); } 
	/// custom inter quantization matrix
	/// - encoding: Set by user, can be NULL.
	/// - decoding: Set by libavcodec.
	property IntPtr inter_matrix { IntPtr get(); void set(IntPtr); } 
	/// scene change detection threshold
	/// 0 is default, larger means fewer detected scene changes.
	/// - encoding: Set by user.
	/// - decoding: unused
	property int scenechange_threshold { int get(); void set(int); } 
	/// noise reduction strength
	/// - encoding: Set by user.
	/// - decoding: unused
	property int noise_reduction { int get(); void set(int); } 
	/// precision of the intra DC coefficient - 8
	/// - encoding: Set by user.
	/// - decoding: Set by libavcodec
	property int intra_dc_precision { int get(); void set(int); } 
	/// Number of macroblock rows at the top which are skipped.
	/// - encoding: unused
	/// - decoding: Set by user.
	property int skip_top { int get(); void set(int); } 
	/// Number of macroblock rows at the bottom which are skipped.
	/// - encoding: unused
	/// - decoding: Set by user.
	property int skip_bottom { int get(); void set(int); } 
	/// minimum MB lagrange multipler
	/// - encoding: Set by user.
	/// - decoding: unused
	property int mb_lmin { int get(); void set(int); } 
	/// maximum MB lagrange multipler
	/// - encoding: Set by user.
	/// - decoding: unused
	property int mb_lmax { int get(); void set(int); } 
	/// - encoding: Set by user.
	/// - decoding: unused
	property int me_penalty_compensation { int get(); void set(int); } 
	/// - encoding: Set by user.
	/// - decoding: unused
	property int bidir_refine { int get(); void set(int); } 
	/// - encoding: Set by user.
	/// - decoding: unused
	property int brd_scale { int get(); void set(int); } 
	/// minimum GOP size
	/// - encoding: Set by user.
	/// - decoding: unused
	property int keyint_min { int get(); void set(int); } 
	/// number of reference frames
	/// - encoding: Set by user.
	/// - decoding: Set by lavc.
	property int refs { int get(); void set(int); } 
	/// chroma qp offset from luma
	/// - encoding: Set by user.
	/// - decoding: unused
	property int chromaoffset { int get(); void set(int); } 
	/// Note: Value depends upon the compare function used for fullpel ME.
	/// - encoding: Set by user.
	/// - decoding: unused
	property int mv0_threshold { int get(); void set(int); } 
	/// Adjust sensitivity of b_frame_strategy 1.
	/// - encoding: Set by user.
	/// - decoding: unused
	property int b_sensitivity { int get(); void set(int); } 
	/// Chromaticity coordinates of the source primaries.
	/// - encoding: Set by user
	/// - decoding: Set by libavcodec
	property AVColorPrimaries color_primaries { AVColorPrimaries get(); void set(AVColorPrimaries); } 
	/// Color Transfer Characteristic.
	/// - encoding: Set by user
	/// - decoding: Set by libavcodec
	property AVColorTransferCharacteristic color_trc { AVColorTransferCharacteristic get(); void set(AVColorTransferCharacteristic); } 
	/// YUV colorspace type.
	/// - encoding: Set by user
	/// - decoding: Set by libavcodec
	property AVColorSpace colorspace { AVColorSpace get(); void set(AVColorSpace); } 
	/// MPEG vs JPEG YUV range.
	/// - encoding: Set by user
	/// - decoding: Set by libavcodec
	property AVColorRange color_range { AVColorRange get(); void set(AVColorRange); } 
	/// This defines the location of chroma samples.
	/// - encoding: Set by user
	/// - decoding: Set by libavcodec
	property AVChromaLocation chroma_sample_location { AVChromaLocation get(); void set(AVChromaLocation); } 
	/// Number of slices.
	/// Indicates number of picture subdivisions. Used for parallelized
	/// decoding.
	/// - encoding: Set by user
	/// - decoding: unused
	property int slices { int get(); void set(int); }
	/// Field order
	/// - encoding: set by libavcodec
	/// - decoding: Set by user.
	property AVFieldOrder field_order { AVFieldOrder get(); void set(AVFieldOrder); }
	///< samples per second
	property int sample_rate { int get(); void set(int); }
	///< number of audio channels
	property int channels { int get(); void set(int); }
	/// audio sample format
	/// - encoding: Set by user.
	/// - decoding: Set by libavcodec.
	property AVSampleFormat sample_fmt { AVSampleFormat get(); void set(AVSampleFormat); }
	/// Number of samples per channel in an audio frame.
	///
	/// - encoding: set by libavcodec in avcodec_open2(). Each submitted frame
	///   except the last must contain exactly frame_size samples per channel.
	///   May be 0 when the codec has CODEC_CAP_VARIABLE_FRAME_SIZE set, then the
	///   frame size is not restricted.
	/// - decoding: may be set by some decoders to indicate constant frame size
	property int frame_size { int get(); }
	/// Frame counter, set by libavcodec.
	///
	/// - decoding: total number of frames returned from the decoder so far.
	/// - encoding: total number of frames passed to the encoder so far.
	///
	///   @note the counter is not incremented if encoding/decoding resulted in
	///   an error.
	property int frame_number { int get(); }
	/// number of bytes per packet if constant and known or 0
	/// Used by some WAV based audio codecs.
	property int block_align { int get(); void set(int); }
	/// Audio cutoff bandwidth (0 means "automatic")
	/// - encoding: Set by user.
	/// - decoding: unused
	property int cutoff { int get(); void set(int); }
	/// Audio channel layout.
	/// - encoding: set by user.
	/// - decoding: set by user, may be overwritten by libavcodec.
	property AVChannelLayout channel_layout { AVChannelLayout get(); void set(AVChannelLayout); }
	/// Request decoder to use this channel layout if it can (0 for default)
	/// - encoding: unused
	/// - decoding: Set by user.
	property AVChannelLayout request_channel_layout { AVChannelLayout get(); void set(AVChannelLayout); }
	/// Type of service that the audio stream conveys.
	/// - encoding: Set by user.
	/// - decoding: Set by libavcodec.
	property AVAudioServiceType audio_service_type { AVAudioServiceType get(); void set(AVAudioServiceType); }
	/// desired sample format
	/// - encoding: Not used.
	/// - decoding: Set by user.
	/// Decoder will decode to this format if it can.
	property AVSampleFormat request_sample_fmt { AVSampleFormat get(); void set(AVSampleFormat); }

	/// This callback is called at the beginning of each frame to get data
	/// buffer(s) for it. There may be one contiguous buffer for all the data or
	/// there may be a buffer per each data plane or anything in between. What
	/// this means is, you may set however many entries in buf[] you feel necessary.
	/// Each buffer must be reference-counted using the AVBuffer API (see description
	/// of buf[] below).
	///
	/// The following fields will be set in the frame before this callback is
	/// called:
	/// - format
	/// - width, height (video only)
	/// - sample_rate, channel_layout, nb_samples (audio only)
	/// Their values may differ from the corresponding values in
	/// AVCodecContext. This callback must use the frame values, not the codec
	/// context values, to calculate the required buffer size.
	///
	/// This callback must fill the following fields in the frame:
	/// - data[]
	/// - linesize[]
	/// - extended_data:
	///   /// if the data is planar audio with more than 8 channels, then this
	///     callback must allocate and fill extended_data to contain all pointers
	///     to all data planes. data[] must hold as many pointers as it can.
	///     extended_data must be allocated with av_malloc() and will be freed in
	///     av_frame_unref().
	///   /// otherwise exended_data must point to data
	/// - buf[] must contain one or more pointers to AVBufferRef structures. Each of
	///   the frame's data and extended_data pointers must be contained in these. That
	///   is, one AVBufferRef for each allocated chunk of memory, not necessarily one
	///   AVBufferRef per data[] entry. See: av_buffer_create(), av_buffer_alloc(),
	///   and av_buffer_ref().
	/// - extended_buf and nb_extended_buf must be allocated with av_malloc() by
	///   this callback and filled with the extra buffers if there are more
	///   buffers than buf[] can hold. extended_buf will be freed in
	///   av_frame_unref().
	///
	/// If CODEC_CAP_DR1 is not set then get_buffer2() must call
	/// avcodec_default_get_buffer2() instead of providing buffers allocated by
	/// some other means.
	///
	/// Each data plane must be aligned to the maximum required by the target
	/// CPU.
	///
	/// @see avcodec_default_get_buffer2()
	///
	/// Video:
	///
	/// If AV_GET_BUFFER_FLAG_REF is set in flags then the frame may be reused
	/// (read and/or written to if it is writable) later by libavcodec.
	///
	/// avcodec_align_dimensions2() should be used to find the required width and
	/// height, as they normally need to be rounded up to the next multiple of 16.
	///
	/// Some decoders do not support linesizes changing between frames.
	///
	/// If frame multithreading is used and thread_safe_callbacks is set,
	/// this callback may be called from a different thread, but not from more
	/// than one at once. Does not need to be reentrant.
	///
	/// @see avcodec_align_dimensions2()
	///
	/// Audio:
	///
	/// Decoders request a buffer of a particular size by setting
	/// AVFrame.nb_samples prior to calling get_buffer2(). The decoder may,
	/// however, utilize only part of the buffer by setting AVFrame.nb_samples
	/// to a smaller value in the output frame.
	///
	/// As a convenience, av_samples_get_buffer_size() and
	/// av_samples_fill_arrays() in libavutil may be used by custom get_buffer2()
	/// functions to find the required data size and to fill data pointers and
	/// linesize. In AVFrame.linesize, only linesize[0] may be set for audio
	/// since all planes must be the same size.
	///
	/// @see av_samples_get_buffer_size(), av_samples_fill_arrays()
	///
	/// - encoding: unused
	/// - decoding: Set by libavcodec, user can override.
	property GetBuffer2Handler^ get_buffer2 { GetBuffer2Handler^ get(); void set(GetBuffer2Handler^); }

	/// If non-zero, the decoded audio and video frames returned from
	/// avcodec_decode_video2() and avcodec_decode_audio4() are reference-counted
	/// and are valid indefinitely. The caller must free them with
	/// av_frame_unref() when they are not needed anymore.
	/// Otherwise, the decoded frames must not be freed by the caller and are
	/// only valid until the next decode call.
	///
	/// - encoding: unused
	/// - decoding: set by the caller before avcodec_open2().
	property int refcounted_frames { int get(); void set(int); }
	///< amount of qscale change between easy & hard scenes (0.0-1.0)
	property float qcompress { float get(); void set(float); }
	///< amount of qscale smoothing over time (0.0-1.0)
	property float qblur { float get(); void set(float); }
	/// minimum quantizer
	/// - encoding: Set by user.
	/// - decoding: unused
	property int qmin { int get(); void set(int); }
	/// maximum quantizer
	/// - encoding: Set by user.
	/// - decoding: unused
	property int qmax { int get(); void set(int); }
	/// maximum quantizer difference between frames
	/// - encoding: Set by user.
	/// - decoding: unused
	property int max_qdiff { int get(); void set(int); }
	/// decoder bitstream buffer size
	/// - encoding: Set by user.
	/// - decoding: unused
	property int rc_buffer_size { int get(); void set(int); }
	/// ratecontrol override, see RcOverride
	/// - encoding: set by user.
	/// - decoding: unused
	property array<RcOverride^>^ rc_override { array<RcOverride^>^ get(); void set(array<RcOverride^>^); }
	/// maximum bitrate
	/// - encoding: Set by user.
	/// - decoding: Set by libavcodec.
	property Int64 rc_max_rate { Int64 get(); void set(Int64); }
	/// minimum bitrate
	/// - encoding: Set by user.
	/// - decoding: unused
	property Int64 rc_min_rate { Int64 get(); void set(Int64); }
	/// Ratecontrol attempt to use, at maximum, <value> of what can be used without an underflow.
	/// - encoding: Set by user.
	/// - decoding: unused.
	property float rc_max_available_vbv_use { float get(); void set(float); }
	/// Ratecontrol attempt to use, at least, <value> times the amount needed to prevent a vbv overflow.
	/// - encoding: Set by user.
	/// - decoding: unused.
	property float rc_min_vbv_overflow_use { float get(); void set(float); }
	/// Number of bits which should be loaded into the rc buffer before decoding starts.
	/// - encoding: Set by user.
	/// - decoding: unused
	property int rc_initial_buffer_occupancy { int get(); void set(int); }
	/// coder type
	/// - encoding: Set by user.
	/// - decoding: unused
	property FFCoderType coder_type { FFCoderType get(); void set(FFCoderType); }
	/// context model
	/// - encoding: Set by user.
	/// - decoding: unused
	property int context_model { int get(); void set(int); }
	/// frame skip threshold
	/// - encoding: Set by user.
	/// - decoding: unused
	property int frame_skip_threshold { int get(); void set(int); }
	/// frame skip factor
	/// - encoding: Set by user.
	/// - decoding: unused
	property int frame_skip_factor { int get(); void set(int); }
	/// frame skip exponent
	/// - encoding: Set by user.
	/// - decoding: unused
	property int frame_skip_exp { int get(); void set(int); }
	/// frame skip comparison function
	/// - encoding: Set by user.
	/// - decoding: unused
	property int frame_skip_cmp { int get(); void set(int); }
	/// trellis RD quantization
	/// - encoding: Set by user.
	/// - decoding: unused
	property int trellis { int get(); void set(int); }
	/// - encoding: Set by user.
	/// - decoding: unused
	property int min_prediction_order { int get(); void set(int); }
	/// - encoding: Set by user.
	/// - decoding: unused
	property int max_prediction_order { int get(); void set(int); }
	/// GOP timecode frame start number
	/// - encoding: Set by user, in non drop frame format
	/// - decoding: Set by libavcodec (timecode in the 25 bits format, -1 if unset)
	property Int64 timecode_frame_start { Int64 get(); void set(Int64); }
	/// The size of the RTP payload: the coder will
	/// do its best to deliver a chunk with size     
	/// below rtp_payload_size, the chunk will start 
	/// with a start code on some codecs like H.263. 
	/// This doesn't take account of any particular  
	/// headers inside the transmitted RTP payload.  
	property int rtp_payload_size { int get(); void set(int); }
	/// statistics, used for 2-pass encoding
	property int mv_bits { int get(); }
	property int header_bits { int get(); }
	property int i_tex_bits { int get(); }
	property int p_tex_bits { int get(); }
	property int i_count { int get(); }
	property int p_count { int get(); }
	property int skip_count { int get(); }
	property int misc_bits { int get(); }
	/// number of bits used for the previously encoded frame
	/// - encoding: Set by libavcodec.
	/// - decoding: unused
	property int frame_bits { int get(); }
	/// pass1 encoding statistics output buffer
	/// - encoding: Set by libavcodec.
	/// - decoding: unused
	property IntPtr stats_out { IntPtr get(); }
	/// pass2 encoding statistics input buffer
	/// Concatenated stuff from stats_out of pass1 should be placed here.
	/// - encoding: Allocated/set/freed by user.
	/// - decoding: unused
	property IntPtr stats_in { IntPtr get(); void set(IntPtr); }
	/// Work around bugs in encoders which sometimes cannot be detected automatically.
	/// - encoding: Set by user
	/// - decoding: Set by user
	property FFBug workaround_bugs { FFBug get(); void set(FFBug); }
	/// strictly follow the standard (MPEG4, ...).
	/// - encoding: Set by user.
	/// - decoding: Set by user.
	/// Setting this to STRICT or higher means the encoder and decoder will
	/// generally do stupid things, whereas setting it to unofficial or lower
	/// will mean the encoder might produce output that is not supported by all
	/// spec-compliant decoders. Decoders don't differentiate between normal,
	/// unofficial and experimental (that is, they always try to decode things
	/// when they can) unless they are explicitly asked to behave stupidly
	/// (=strictly conform to the specs)
	property FFCompliance strict_std_compliance { FFCompliance get(); void set(FFCompliance); }
	/// error concealment flags
	/// - encoding: unused
	/// - decoding: Set by user.
	property ErrorConcealment error_concealment { ErrorConcealment get(); void set(ErrorConcealment); }
	/// debug
	/// - encoding: Set by user.
	/// - decoding: Set by user.
	property FFDebug debug { FFDebug get(); void set(FFDebug); }
	/// debug
	/// Code outside libavcodec should access this field using AVOptions
	/// - encoding: Set by user.
	/// - decoding: Set by user.
	property FFDebugMV debug_mv { FFDebugMV get(); void set(FFDebugMV); }
	/// Error recognition; may misdetect some more or less valid parts as errors.
	/// - encoding: unused
	/// - decoding: Set by user.
	property ErrorRecognition err_recognition { ErrorRecognition get(); void set(ErrorRecognition); }
	/// opaque 64bit number (generally a PTS) that will be reordered and
	/// output in AVFrame.reordered_opaque
	/// - encoding: unused
	/// - decoding: Set by user.
	property Int64 reordered_opaque { Int64 get(); void set(Int64); }
	/// Hardware accelerator in use
	/// - encoding: unused.
	/// - decoding: Set by libavcodec
	property AVHWAccel^ hwaccel { AVHWAccel^ get(); }
	/// Hardware accelerator context.
	/// For some hardware accelerators, a global context needs to be
	/// provided by the user. In that case, this holds display-dependent
	/// data FFmpeg cannot instantiate itself. Please refer to the
	/// FFmpeg HW accelerator documentation to know how to fill this
	/// is. e.g. for VA API, this is a struct vaapi_context.
	/// - encoding: unused
	/// - decoding: Set by user
	property IntPtr hwaccel_context { IntPtr get(); void set(IntPtr); }
	/// error
	/// - encoding: Set by libavcodec if flags&CODEC_FLAG_PSNR.
	/// - decoding: unused
	property AVArray<UInt64>^ error{ AVArray<UInt64>^ get(); }
	/// DCT algorithm, see FF_DCT_/// below
	/// - encoding: Set by user.
	/// - decoding: unused
	property FFDct dct_algo { FFDct get(); void set(FFDct); }
	/// IDCT algorithm.
	/// - encoding: Set by user.
	/// - decoding: Set by user.
	property FFIdct idct_algo { FFIdct get(); void set(FFIdct); }
	/// bits per sample/pixel from the demuxer (needed for huffyuv).
	/// - encoding: Set by libavcodec.
	/// - decoding: Set by user.
	property int bits_per_coded_sample { int get(); void set(int); }
	/// Bits per sample/pixel of internal libavcodec pixel/sample format.
	/// - encoding: set by user.
	/// - decoding: set by libavcodec.
	property int bits_per_raw_sample { int get(); void set(int); }
	/// low resolution decoding, 1-> 1/2 size, 2->1/4 size
	/// - encoding: unused
	/// - decoding: Set by user.
	/// Code outside libavcodec should access this field using:
	/// av_codec_{get,set}_lowres(avctx)
	property int lowres { int get(); void set(int); }
	/// the picture in the bitstream
	/// - encoding: Set by libavcodec.
	/// - decoding: unused
	property AVFrame^ coded_frame { AVFrame^ get(); }
	/// thread count
	/// is used to decide how many independent tasks should be passed to execute()
	/// - encoding: Set by user.
	/// - decoding: Set by user.
	property int thread_count { int get(); void set(int); }
	/// Which multithreading methods to use.
	/// Use of FF_THREAD_FRAME will increase decoding delay by one frame per thread,
	/// so clients which cannot provide future frames should not use it.
	///
	/// - encoding: Set by user, otherwise the default is used.
	/// - decoding: Set by user, otherwise the default is used.
	property ThreadType thread_type { ThreadType get(); void set(ThreadType); }
	/// Which multithreading methods are in use by the codec.
	/// - encoding: Set by libavcodec.
	/// - decoding: Set by libavcodec.
	property ThreadType active_thread_type { ThreadType get(); }
	/// Set by the client if its custom get_buffer() callback can be called
	/// synchronously from another thread, which allows faster multithreaded decoding.
	/// draw_horiz_band() will be called from other threads regardless of this setting.
	/// Ignored if the default get_buffer() is used.
	/// - encoding: Set by user.
	/// - decoding: Set by user.
	property int thread_safe_callbacks { int get(); void set(int); }
	/// noise vs. sse weight for the nsse comparison function
	/// - encoding: Set by user.
	/// - decoding: unused
	property int nsse_weight { int get(); void set(int); }
	/// profile
	/// - encoding: Set by user.
	/// - decoding: Set by libavcodec.
	property Profile profile { Profile get(); void set(Profile); }
	/// level
	/// - encoding: Set by user.
	/// - decoding: Set by libavcodec.
	property int level { int get(); void set(int); }
	/// Skip loop filtering for selected frames.
	/// - encoding: unused
	/// - decoding: Set by user.
	property AVDiscard skip_loop_filter { AVDiscard get(); void set(AVDiscard); }
	/// Skip IDCT/dequantization for selected frames.
	/// - encoding: unused
	/// - decoding: Set by user.
	property AVDiscard skip_idct { AVDiscard get(); void set(AVDiscard); }
	/// Skip decoding for selected frames.
	/// - encoding: unused
	/// - decoding: Set by user.
	property AVDiscard skip_frame { AVDiscard get(); void set(AVDiscard); }
	/// Header containing style information for text subtitles.
	/// For SUBTITLE_ASS subtitle type, it should contain the whole ASS
	/// [Script Info] and [V4+ Styles] section, plus the [Events] line and
	/// the Format line following. It shouldn't include any Dialogue line.
	/// - encoding: Set/allocated/freed by user (before avcodec_open2())
	/// - decoding: Set/allocated/freed by libavcodec (by avcodec_open2())
	property array<byte>^ subtitle_header { array<byte>^ get(); void set(array<byte>^); }
	/// VBV delay coded in the last frame (in periods of a 27 MHz clock).
	/// Used for compliant TS muxing.
	/// - encoding: Set by libavcodec.
	/// - decoding: unused.
	property UInt64 vbv_delay { UInt64 get(); }
	/// Encoding only. Allow encoders to output packets that do not contain any
	/// encoded data, only side data.
	///
	/// Some encoders need to output such packets, e.g. to update some stream
	/// parameters at the end of encoding.
	///
	/// All callers are strongly recommended to set this option to 1 and update
	/// their code to deal with such packets, since this behaviour may become
	/// always enabled in the future (then this option will be deprecated and
	/// later removed). To avoid ABI issues when this happens, the callers should
	/// use AVOptions to set this field.
	property int side_data_only_packets { int get(); void set(int); }
	/// Audio only. The number of "priming" samples (padding) inserted by the
	/// encoder at the beginning of the audio. I.e. this number of leading
	/// decoded samples must be discarded by the caller to get the original audio
	/// without leading padding.
	///
	/// - decoding: unused
	/// - encoding: Set by libavcodec. The timestamps on the output packets are
	///             adjusted by the encoder so that they always refer to the
	///             first sample of the data actually contained in the packet,
	///             including any added padding.  E.g. if the timebase is
	///             1/samplerate and the timestamp of the first input sample is
	///             0, the timestamp of the first output packet will be
	///             -initial_padding.
	property int initial_padding { int get(); }
	/// - decoding: For codecs that store a framerate value in the compressed
	///             bitstream, the decoder may export it here. { 0, 1} when
	///             unknown.
	/// - encoding: unused
	property AVRational^ framerate { AVRational^ get(); void set(AVRational^); }
	/// Nominal unaccelerated pixel format, see AV_PIX_FMT_xxx.
	/// - encoding: unused.
	/// - decoding: Set by libavcodec before calling get_format()
	property AVPixelFormat sw_pix_fmt { AVPixelFormat get(); }
	/// Timebase in which pkt_dts/pts and AVPacket.dts/pts are.
	/// Code outside libavcodec should access this field using:
	/// av_codec_{get,set}_pkt_timebase(avctx)
	/// - encoding unused.
	/// - decoding set by user.
	property AVRational^ pkt_timebase { AVRational^ get(); void set(AVRational^); }
	/// AVCodecDescriptor
	/// Code outside libavcodec should access this field using:
	/// av_codec_{get,set}_codec_descriptor(avctx)
	/// - encoding: unused.
	/// - decoding: set by libavcodec.
	property AVCodecDescriptor^ codec_descriptor { AVCodecDescriptor^ get(); void set(AVCodecDescriptor^); }
	/// Current statistics for PTS correction.
	/// - decoding: maintained and used by libavcodec, not intended to be used by user apps
	/// - encoding: unused
	property Int64 pts_correction_num_faulty_pts { Int64 get(); } /// Number of incorrect PTS values so far
	property Int64 pts_correction_num_faulty_dts { Int64 get(); } /// Number of incorrect DTS values so far
	property Int64 pts_correction_last_pts { Int64 get(); }       /// PTS of the last frame
	property Int64 pts_correction_last_dts { Int64 get(); }       /// DTS of the last frame
	/// Character encoding of the input subtitles file.
	/// - decoding: set by user
	/// - encoding: unused
	property String^ sub_charenc { String^ get(); void set(String^ ); }
	/// Subtitles character encoding mode. Formats or codecs might be adjusting
	/// this setting (if they are doing the conversion themselves for instance).
	/// - decoding: set by libavcodec
	/// - encoding: unused
	property FF_SUB_CHARENC_MODE sub_charenc_mode { FF_SUB_CHARENC_MODE get(); }
	/// Skip processing alpha if supported by codec.
	/// Note that if the format uses pre-multiplied alpha (common with VP6,
	/// and recommended due to better video quality/compression)
	/// the image will look as if alpha-blended onto a black background.
	/// However for formats that do not use pre-multiplied alpha
	/// there might be serious artefacts (though e.g. libswscale currently
	/// assumes pre-multiplied alpha anyway).
	/// Code outside libavcodec should access this field using AVOptions
	///
	/// - decoding: set by user
	/// - encoding: unused
	property int skip_alpha { int get(); void set(int); }
	/// Number of samples to skip after a discontinuity
	/// - decoding: unused
	/// - encoding: set by libavcodec
	property int seek_preroll { int get(); void set(int); }
	/// custom intra quantization matrix
	/// Code outside libavcodec should access this field using av_codec_g/set_chroma_intra_matrix()
	/// - encoding: Set by user, can be NULL.
	/// - decoding: unused.
	property IntPtr chroma_intra_matrix { IntPtr get(); void set(IntPtr); }
	/// dump format separator.
	/// can be ", " or "\n      " or anything else
	/// Code outside libavcodec should access this field using AVOptions
	/// (NO direct access).
	/// - encoding: Set by user.
	/// - decoding: Set by user.
	property String^ dump_separator { String^ get(); void set(String^); }
	/// ',' separated list of allowed decoders.
	/// If NULL then all are allowed
	/// - encoding: unused
	/// - decoding: set by user through AVOPtions (NO direct access)
	property String^ codec_whitelist { String^ get(); void set(String^); }
	/// Properties of the stream that gets decoded
	/// - encoding: unused
	/// - decoding: set by libavcodec
	property FF_CODEC_PROPERTY properties { FF_CODEC_PROPERTY get(); }
	/// Additional data associated with the entire coded stream.
	///
	/// - decoding: unused
	/// - encoding: may be set by libavcodec after avcodec_open2().
	property array<AVPacketSideData^>^ coded_side_data { array<AVPacketSideData^>^ get(); }
	/// A reference to the AVHWFramesContext describing the input (for encoding)
    /// or output (decoding) frames. The reference is set by the caller and
    /// afterwards owned (and freed) by libavcodec - it should never be read by
    /// the caller after being set.
    ///
    /// - decoding: This field should be set by the caller from the get_format()
    ///             callback. The previous reference (if any) will always be
    ///             unreffed by libavcodec before the get_format() call.
    ///
    ///             If the default get_buffer2() is used with a hwaccel pixel
    ///             format, then this AVHWFramesContext will be used for
    ///             allocating the frame buffers.
    ///
    /// - encoding: For hardware encoders configured to use a hwaccel pixel
    ///             format, this field should be set by the caller to a reference
    ///             to the AVHWFramesContext describing input frames.
    ///             AVHWFramesContext.format must be equal to
    ///             AVCodecContext.pix_fmt.
    ///
    ///             This field should be set before avcodec_open2() is called.
	property AVBufferRef^ hw_frames_ctx { AVBufferRef^ get(); void set(AVBufferRef^); }
	/// Control the form of AVSubtitle.rects[N]->ass
	/// - decoding: set by user
	/// - encoding: unused
	property FF_SUB_TEXT_FMT sub_text_format  { FF_SUB_TEXT_FMT get(); void set(FF_SUB_TEXT_FMT); } 
	/// Audio only. The amount of padding (in samples) appended by the encoder to
    /// the end of the audio. I.e. this number of decoded samples must be
    /// discarded by the caller from the end of the stream to get the original
    /// audio without any trailing padding.
    ///
    /// - decoding: unused
    /// - encoding: unused
	property int trailing_padding { int get(); } 
	/// The number of pixels per image to maximally accept.
    ///
    /// - decoding: set by user
    /// - encoding: set by user
	property Int64 max_pixels { Int64 get(); void set(Int64); } 
	/// A reference to the AVHWDeviceContext describing the device which will
    /// be used by a hardware encoder/decoder.  The reference is set by the
    /// caller and afterwards owned (and freed) by libavcodec.
    ///
    /// This should be used if either the codec device does not require
    /// hardware frames or any that are used are to be allocated internally by
    /// libavcodec.  If the user wishes to supply any of the frames used as
    /// encoder input or decoder output then hw_frames_ctx should be used
    /// instead.  When hw_frames_ctx is set in get_format() for a decoder, this
    /// field will be ignored while decoding the associated stream segment, but
    /// may again be used on a following one after another get_format() call.
    ///
    /// For both encoders and decoders this field should be set before
    /// avcodec_open2() is called and must not be written to thereafter.
    ///
    /// Note that some decoders may require this field to be set initially in
    /// order to support hw_frames_ctx at all - in that case, all frames
    /// contexts used must be created on the same device.
	property AVBufferRef^ hw_device_ctx { AVBufferRef^ get(); void set(AVBufferRef^); }
	/// Bit set of AV_HWACCEL_FLAG_/// flags, which affect hardware accelerated
	/// decoding (if active).
	/// - encoding: unused
	/// - decoding: Set by user (either before avcodec_open2(), or in the
	///             AVCodecContext.get_format callback)
	property AV_HWACCEL_FLAG hwaccel_flags { AV_HWACCEL_FLAG get(); void set(AV_HWACCEL_FLAG); }
	/// Video decoding only. Certain video codecs support cropping, meaning that
    /// only a sub-rectangle of the decoded frame is intended for display.  This
    /// option controls how cropping is handled by libavcodec.
    ///
    /// When set to 1 (the default), libavcodec will apply cropping internally.
    /// I.e. it will modify the output frame width/height fields and offset the
    /// data pointers (only by as much as possible while preserving alignment, or
    /// by the full amount if the AV_CODEC_FLAG_UNALIGNED flag is set) so that
    /// the frames output by the decoder refer only to the cropped area. The
    /// crop_/// fields of the output frames will be zero.
    ///
    /// When set to 0, the width/height fields of the output frames will be set
    /// to the coded dimensions and the crop_/// fields will describe the cropping
    /// rectangle. Applying the cropping is left to the caller.
    ///
    /// @warning When hardware acceleration with opaque output frames is used,
    /// libavcodec is unable to apply cropping from the top/left border.
    ///
    /// @note when this option is set to zero, the width/height fields of the
    /// AVCodecContext and output AVFrames have different meanings. The codec
    /// context fields store display dimensions (with the coded dimensions in
    /// coded_width/height), while the frame fields store the coded dimensions
    /// (with the display dimensions being determined by the crop_/// fields).
	property bool apply_cropping { bool get (); void set(bool); }
	/// Video decoding only.  Sets the number of extra hardware frames which
    /// the decoder will allocate for use by the caller.  This must be set
    /// before avcodec_open2() is called.
    ///
    /// Some hardware decoders require all frames that they will use for
    /// output to be defined in advance before decoding starts.  For such
    /// decoders, the hardware frame pool must therefore be of a fixed size.
    /// The extra frames set here are on top of any number that the decoder
    /// needs internally in order to operate normally (for example, frames
    /// used as reference pictures).
	property int extra_hw_frames { int get(); void set(int); } 
	/// The percentage of damaged samples to discard a frame.
    ///
    /// - decoding: set by user
    /// - encoding: unused
	property int discard_damaged_percentage { int get(); void set(int); } 
public:
	/// DTG active format information (additional aspect ratio
	/// information only used in DVB MPEG-2 transport streams)
	/// 0 if not set.
	///
	/// - encoding: unused
	/// - decoding: Set by decoder.
	/// @deprecated Deprecated in favor of AVSideData
	property FFDtg dtg_active_format { FFDtg get(); }
	///<summary>
	/// @return a positive value if s is open (i.e. avcodec_open2() was called on it
	/// with no corresponding avcodec_close()), 0 otherwise.
	///</summary>
	property bool IsOpened { bool get(); }
public:
	/// Copy the settings of the source AVCodecContext into the destination
	/// AVCodecContext. The resulting destination codec context will be
	/// unopened, i.e. you are required to call avcodec_open2() before you
	/// can use this AVCodecContext to decode/encode video/audio data.
	///
	/// @param dest target codec context, should be initialized with
	///            avcodec_alloc_context3(NULL), but otherwise uninitialized
	/// @param src source codec context
	/// @return AVERROR() on error (e.g. memory allocation error), 0 on success
	AVRESULT CopyTo(AVCodecContext^ dest);
	/// Initialize the AVCodecContext to use the given AVCodec. Prior to using this
	/// function the context has to be allocated with avcodec_alloc_context3().
	/// 
	/// The functions avcodec_find_decoder_by_name(), avcodec_find_encoder_by_name(),
	/// avcodec_find_decoder() and avcodec_find_encoder() provide an easy way for
	/// retrieving a codec.
	/// 
	/// @warning This function is not thread safe!
	/// 
	/// @note Always call this function before using decoding routines (such as
	/// @ref avcodec_decode_video2()).
	/// 
	/// @code
	/// avcodec_register_all();
	/// av_dict_set(&opts, "b", "2.5M", 0);
	/// codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	/// if (!codec)
	///     exit(1);
	/// 
	/// context = avcodec_alloc_context3(codec);
	/// 
	/// if (avcodec_open2(context, codec, opts) < 0)
	///     exit(1);
	/// @endcode
	/// 
	/// @param avctx The context to initialize.
	/// @param codec The codec to open this context for. If a non-NULL codec has been
	///              previously passed to avcodec_alloc_context3() or
	///              avcodec_get_context_defaults3() for this context, then this
	///              parameter MUST be either NULL or equal to the previously passed
	///              codec.
	/// @param options A dictionary filled with AVCodecContext and codec-private options.
	///                On return this object will be filled with options that were not found.
	/// 
	/// @return zero on success, a negative value on error
	/// @see avcodec_alloc_context3(), avcodec_find_decoder(), avcodec_find_encoder(),
	///      av_dict_set(), av_opt_find().
	AVRESULT Open(AVCodec^ codec, AVDictionary^ options);
	AVRESULT Open(AVCodec^ codec);

	/// Close a given AVCodecContext and free all the data associated with it
	/// (but not the AVCodecContext itself).
	///
	/// Calling this function on an AVCodecContext that hasn't been opened will free
	/// the codec-specific data allocated in avcodec_alloc_context3() with a non-NULL
	/// codec. Subsequent calls will do nothing.
	///
	/// @note Do not use this function. Use avcodec_free_context() to destroy a
	/// codec context (either open or closed). Opening and closing a codec context
	/// multiple times is not supported anymore -- use multiple codec contexts
	/// instead.
	AVRESULT Close();
	/// Reset the internal decoder state / flush internal buffers. Should be called
	/// e.g. when seeking or when switching to a different stream.
	///
	/// @note when refcounted frames are not used (i.e. avctx->refcounted_frames is 0),
	/// this invalidates the frames previously returned from the decoder. When
	/// refcounted frames are used, the decoder just releases any references it might
	/// keep internally, but the caller's reference remains valid.
	void Flush();
	/// Modify width and height values so that they will result in a memory
	/// buffer that is acceptable for the codec if you do not use any horizontal
	/// padding.
	/// 
	/// May only be used if a codec with CODEC_CAP_DR1 has been opened.
	void AlignDimentions(int % _width,int % _height);
	/// Modify width and height values so that they will result in a memory
	/// buffer that is acceptable for the codec if you also ensure that all
	/// line sizes are a multiple of the respective linesize_align[i].
	/// 
	/// May only be used if a codec with CODEC_CAP_DR1 has been opened.
	void AlignDimentions(int % _width,int % _height,array<int>^ % linesize_align);
	/// Return audio frame duration.
	///
	/// @param avctx        codec context
	/// @param frame_bytes  size of the frame, or 0 if unknown
	/// @return             frame duration, in samples, if known. 0 if not able to
	///                     determine.
	int GetAudioFrameDuration(int frame_bytes);
	/// Fill the codec context based on the values from the supplied codec
	/// parameters. Any allocated fields in codec that have a corresponding field in
	/// par are freed and replaced with duplicates of the corresponding field in par.
	/// Fields in codec that do not have a counterpart in par are not touched.
	///
	/// @return >= 0 on success, a negative AVERROR code on failure.
	AVRESULT SetParameters(AVCodecParameters^ par);
public:
	/// Decode the audio frame of size avpkt->size from avpkt->data into frame.
	/// 
	/// Some decoders may support multiple frames in a single AVPacket. Such
	/// decoders would then just decode the first frame and the return value would be
	/// less than the packet size. In this case, avcodec_decode_audio4 has to be
	/// called again with an AVPacket containing the remaining data in order to
	/// decode the second frame, etc...  Even if no frames are returned, the packet
	/// needs to be fed to the decoder with remaining data until it is completely
	/// consumed or an error occurs.
	/// 
	/// Some decoders (those marked with CODEC_CAP_DELAY) have a delay between input
	/// and output. This means that for some packets they will not immediately
	/// produce decoded output and need to be flushed at the end of decoding to get
	/// all the decoded data. Flushing is done by calling this function with packets
	/// with avpkt->data set to NULL and avpkt->size set to 0 until it stops
	/// returning samples. It is safe to flush even those decoders that are not
	/// marked with CODEC_CAP_DELAY, then no samples will be returned.
	/// 
	/// @warning The input buffer, avpkt->data must be FF_INPUT_BUFFER_PADDING_SIZE
	///          larger than the actual read bytes because some optimized bitstream
	///          readers read 32 or 64 bits at once and could read over the end.
	/// 
	/// @note The AVCodecContext MUST have been opened with @ref avcodec_open2()
	/// before packets may be fed to the decoder.
	/// 
	/// @param      avctx the codec context
	/// @param[out] frame The AVFrame in which to store decoded audio samples.
	///                   The decoder will allocate a buffer for the decoded frame by
	///                   calling the AVCodecContext.get_buffer2() callback.
	///                   When AVCodecContext.refcounted_frames is set to 1, the frame is
	///                   reference counted and the returned reference belongs to the
	///                   caller. The caller must release the frame using av_frame_unref()
	///                   when the frame is no longer needed. The caller may safely write
	///                   to the frame if av_frame_is_writable() returns 1.
	///                   When AVCodecContext.refcounted_frames is set to 0, the returned
	///                   reference belongs to the decoder and is valid only until the
	///                   next call to this function or until closing or flushing the
	///                   decoder. The caller may not write to it.
	/// @param[out] got_frame_ptr Zero if no frame could be decoded, otherwise it is
	///                           non-zero. Note that this field being set to zero
	///                           does not mean that an error has occurred. For
	///                           decoders with CODEC_CAP_DELAY set, no given decode
	///                           call is guaranteed to produce a frame.
	/// @param[in]  avpkt The input AVPacket containing the input buffer.
	///                   At least avpkt->data and avpkt->size should be set. Some
	///                   decoders might also require additional fields to be set.
	/// @return A negative error code is returned if an error occurred during
	///         decoding, otherwise the number of bytes consumed from the input
	///         AVPacket is returned.
	int DecodeAudio(AVFrame^ frame,bool % got_frame_ptr,AVPacket^ avpkt);
	///  Decode the video frame of size avpkt->size from avpkt->data into picture.
	///  Some decoders may support multiple frames in a single AVPacket, such
	///  decoders would then just decode the first frame.
	/// 
	///  @warning The input buffer must be FF_INPUT_BUFFER_PADDING_SIZE larger than
	///  the actual read bytes because some optimized bitstream readers read 32 or 64
	///  bits at once and could read over the end.
	/// 
	///  @warning The end of the input buffer buf should be set to 0 to ensure that
	///  no overreading happens for damaged MPEG streams.
	/// 
	///  @note Codecs which have the CODEC_CAP_DELAY capability set have a delay
	///  between input and output, these need to be fed with avpkt->data=NULL,
	///  avpkt->size=0 at the end to return the remaining frames.
	/// 
	///  @note The AVCodecContext MUST have been opened with @ref avcodec_open2()
	///  before packets may be fed to the decoder.
	/// 
	///  @param avctx the codec context
	///  @param[out] picture The AVFrame in which the decoded video frame will be stored.
	///              Use av_frame_alloc() to get an AVFrame. The codec will
	///              allocate memory for the actual bitmap by calling the
	///              AVCodecContext.get_buffer2() callback.
	///              When AVCodecContext.refcounted_frames is set to 1, the frame is
	///              reference counted and the returned reference belongs to the
	///              caller. The caller must release the frame using av_frame_unref()
	///              when the frame is no longer needed. The caller may safely write
	///              to the frame if av_frame_is_writable() returns 1.
	///              When AVCodecContext.refcounted_frames is set to 0, the returned
	///              reference belongs to the decoder and is valid only until the
	///              next call to this function or until closing or flushing the
	///              decoder. The caller may not write to it.
	/// 
	///  @param[in] avpkt The input AVPacket containing the input buffer.
	///             You can create such packet with av_init_packet() and by then setting
	///             data and size, some decoders might in addition need other fields like
	///             flags&AV_PKT_FLAG_KEY. All decoders are designed to use the least
	///             fields possible.
	///  @param[in,out] got_picture_ptr Zero if no frame could be decompressed, otherwise, it is nonzero.
	///  @return On error a negative value is returned, otherwise the number of bytes
	///  used or zero if no frame could be decompressed.
	int DecodeVideo(AVFrame^ frame,bool % got_picture_ptr,AVPacket^ avpkt);
	/// Decode a subtitle message.
	/// Return a negative value on error, otherwise return the number of bytes used.
	/// If no subtitle could be decompressed, got_sub_ptr is zero.
	/// Otherwise, the subtitle is stored in *sub.
	/// Note that CODEC_CAP_DR1 is not available for subtitle codecs. This is for
	/// simplicity, because the performance difference is expect to be negligible
	/// and reusing a get_buffer written for video codecs would probably perform badly
	/// due to a potentially very different allocation pattern.
	/// 
	/// Some decoders (those marked with CODEC_CAP_DELAY) have a delay between input
	/// and output. This means that for some packets they will not immediately
	/// produce decoded output and need to be flushed at the end of decoding to get
	/// all the decoded data. Flushing is done by calling this function with packets
	/// with avpkt->data set to NULL and avpkt->size set to 0 until it stops
	/// returning subtitles. It is safe to flush even those decoders that are not
	/// marked with CODEC_CAP_DELAY, then no subtitles will be returned.
	/// 
	/// @note The AVCodecContext MUST have been opened with @ref avcodec_open2()
	/// before packets may be fed to the decoder.
	/// 
	/// @param avctx the codec context
	/// @param[out] sub The Preallocated AVSubtitle in which the decoded subtitle will be stored,
	///                 must be freed with avsubtitle_free if *got_sub_ptr is set.
	/// @param[in,out] got_sub_ptr Zero if no subtitle could be decompressed, otherwise, it is nonzero.
	/// @param[in] avpkt The input AVPacket containing the input buffer.
	AVRESULT DecodeSubtitle(AVSubtitle^ sub,bool % got_sub_ptr,AVPacket^ avpkt);
	/// Encode a frame of audio.
	/// 
	/// Takes input samples from frame and writes the next output packet, if
	/// available, to avpkt. The output packet does not necessarily contain data for
	/// the most recent frame, as encoders can delay, split, and combine input frames
	/// internally as needed.
	/// 
	/// @param avctx     codec context
	/// @param avpkt     output AVPacket.
	///                  The user can supply an output buffer by setting
	///                  avpkt->data and avpkt->size prior to calling the
	///                  function, but if the size of the user-provided data is not
	///                  large enough, encoding will fail. If avpkt->data and
	///                  avpkt->size are set, avpkt->destruct must also be set. All
	///                  other AVPacket fields will be reset by the encoder using
	///                  av_init_packet(). If avpkt->data is NULL, the encoder will
	///                  allocate it. The encoder will set avpkt->size to the size
	///                  of the output packet.
	/// 
	///                  If this function fails or produces no output, avpkt will be
	///                  freed using av_free_packet() (i.e. avpkt->destruct will be
	///                  called to free the user supplied buffer).
	/// @param[in] frame AVFrame containing the raw audio data to be encoded.
	///                  May be NULL when flushing an encoder that has the
	///                  CODEC_CAP_DELAY capability set.
	///                  If CODEC_CAP_VARIABLE_FRAME_SIZE is set, then each frame
	///                  can have any number of samples.
	///                  If it is not set, frame->nb_samples must be equal to
	///                  avctx->frame_size for all frames except the last.
	///                  The final frame may be smaller than avctx->frame_size.
	/// @param[out] got_packet_ptr This field is set to 1 by libavcodec if the
	///                            output packet is non-empty, and to 0 if it is
	///                            empty. If the function returns an error, the
	///                            packet can be assumed to be invalid, and the
	///                            value of got_packet_ptr is undefined and should
	///                            not be used.
	/// @return          0 on success, negative error code on failure
	AVRESULT EncodeAudio(AVPacket^ avpkt,AVFrame^ frame,bool % got_packet_ptr);

	/// Encode a frame of video.
	///
	/// Takes input raw video data from frame and writes the next output packet, if
	/// available, to avpkt. The output packet does not necessarily contain data for
	/// the most recent frame, as encoders can delay and reorder input frames
	/// internally as needed.
	/// 
	/// @param avctx     codec context
	/// @param avpkt     output AVPacket.
	///                  The user can supply an output buffer by setting
	///                  avpkt->data and avpkt->size prior to calling the
	///                  function, but if the size of the user-provided data is not
	///                  large enough, encoding will fail. All other AVPacket fields
	///                  will be reset by the encoder using av_init_packet(). If
	///                  avpkt->data is NULL, the encoder will allocate it.
	///                  The encoder will set avpkt->size to the size of the
	///                  output packet. The returned data (if any) belongs to the
	///                  caller, he is responsible for freeing it.
	/// 
	///                  If this function fails or produces no output, avpkt will be
	///                  freed using av_free_packet() (i.e. avpkt->destruct will be
	///                  called to free the user supplied buffer).
	/// @param[in] frame AVFrame containing the raw video data to be encoded.
	///                  May be NULL when flushing an encoder that has the
	///                  CODEC_CAP_DELAY capability set.
	/// @param[out] got_packet_ptr This field is set to 1 by libavcodec if the
	///                            output packet is non-empty, and to 0 if it is
	///                            empty. If the function returns an error, the
	///                            packet can be assumed to be invalid, and the
	///                            value of got_packet_ptr is undefined and should
	///                            not be used.
	/// @return          0 on success, negative error code on failure
	AVRESULT EncodeVideo(AVPacket^ avpkt,AVFrame^ frame,bool % got_packet_ptr);

	AVRESULT EncodeSubtitle(IntPtr buf,int buf_size,AVSubtitle^ sub);
public:
	///<summary>
	/// Supply raw packet data as input to a decoder.
	///
	/// Internally, this call will copy relevant AVCodecContext fields, which can
	/// influence decoding per-packet, and apply them when the packet is actually
	/// decoded. (For example AVCodecContext.skip_frame, which might direct the
	/// decoder to drop the frame contained by the packet sent with this function.)
	///
	/// @warning The input buffer, avpkt->data must be AV_INPUT_BUFFER_PADDING_SIZE
	///          larger than the actual read bytes because some optimized bitstream
	///          readers read 32 or 64 bits at once and could read over the end.
	///
	/// @warning Do not mix this API with the legacy API (like avcodec_decode_video2())
	///          on the same AVCodecContext. It will return unexpected results now
	///          or in future libavcodec versions.
	///
	/// @note The AVCodecContext MUST have been opened with @ref avcodec_open2()
	///       before packets may be fed to the decoder.
	///
	/// @param avctx codec context
	/// @param[in] avpkt The input AVPacket. Usually, this will be a single video
	///                  frame, or several complete audio frames.
	///                  Ownership of the packet remains with the caller, and the
	///                  decoder will not write to the packet. The decoder may create
	///                  a reference to the packet data (or copy it if the packet is
	///                  not reference-counted).
	///                  Unlike with older APIs, the packet is always fully consumed,
	///                  and if it contains multiple frames (e.g. some audio codecs),
	///                  will require you to call avcodec_receive_frame() multiple
	///                  times afterwards before you can send a new packet.
	///                  It can be NULL (or an AVPacket with data set to NULL and
	///                  size set to 0); in this case, it is considered a flush
	///                  packet, which signals the end of the stream. Sending the
	///                  first flush packet will return success. Subsequent ones are
	///                  unnecessary and will return AVERROR_EOF. If the decoder
	///                  still has frames buffered, it will return them after sending
	///                  a flush packet.
	///
	/// @return 0 on success, otherwise negative error code:
	///      AVERROR(EAGAIN):   input is not accepted in the current state - user
	///                         must read output with avcodec_receive_frame() (once
	///                         all output is read, the packet should be resent, and
	///                         the call will not fail with EAGAIN).
	///      AVERROR_EOF:       the decoder has been flushed, and no new packets can
	///                         be sent to it (also returned if more than 1 flush
	///                         packet is sent)
	///      AVERROR(EINVAL):   codec not opened, it is an encoder, or requires flush
	///      AVERROR(ENOMEM):   failed to add packet to internal queue, or similar
	///      other errors: legitimate decoding errors
	///</summary>
	AVRESULT SendPacket(AVPacket^ avpkt);
	///<summary>
	/// Return decoded output data from a decoder.
	///
	/// @param avctx codec context
	/// @param frame This will be set to a reference-counted video or audio
	///              frame (depending on the decoder type) allocated by the
	///              decoder. Note that the function will always call
	///              av_frame_unref(frame) before doing anything else.
	///
	/// @return
	///      0:                 success, a frame was returned
	///      AVERROR(EAGAIN):   output is not available in this state - user must try
	///                         to send new input
	///      AVERROR_EOF:       the decoder has been fully flushed, and there will be
	///                         no more output frames
	///      AVERROR(EINVAL):   codec not opened, or it is an encoder
	///      AVERROR_INPUT_CHANGED:   current decoded frame has changed parameters
	///                               with respect to first decoded frame. Applicable
	///                               when flag AV_CODEC_FLAG_DROPCHANGED is set.
	///      other negative values: legitimate decoding errors
	///</summary>
	AVRESULT ReceiveFrame(AVFrame^ frame);
public:
	///<summary>
	/// Supply a raw video or audio frame to the encoder. Use avcodec_receive_packet()
	/// to retrieve buffered output packets.
	///
	/// @param avctx     codec context
	/// @param[in] frame AVFrame containing the raw audio or video frame to be encoded.
	///                  Ownership of the frame remains with the caller, and the
	///                  encoder will not write to the frame. The encoder may create
	///                  a reference to the frame data (or copy it if the frame is
	///                  not reference-counted).
	///                  It can be NULL, in which case it is considered a flush
	///                  packet.  This signals the end of the stream. If the encoder
	///                  still has packets buffered, it will return them after this
	///                  call. Once flushing mode has been entered, additional flush
	///                  packets are ignored, and sending frames will return
	///                  AVERROR_EOF.
	///
	///                  For audio:
	///                  If AV_CODEC_CAP_VARIABLE_FRAME_SIZE is set, then each frame
	///                  can have any number of samples.
	///                  If it is not set, frame->nb_samples must be equal to
	///                  avctx->frame_size for all frames except the last.
	///                  The final frame may be smaller than avctx->frame_size.
	/// @return 0 on success, otherwise negative error code:
	///      AVERROR(EAGAIN):   input is not accepted in the current state - user
	///                         must read output with avcodec_receive_packet() (once
	///                         all output is read, the packet should be resent, and
	///                         the call will not fail with EAGAIN).
	///      AVERROR_EOF:       the encoder has been flushed, and no new frames can
	///                         be sent to it
	///      AVERROR(EINVAL):   codec not opened, refcounted_frames not set, it is a
	///                         decoder, or requires flush
	///      AVERROR(ENOMEM):   failed to add packet to internal queue, or similar
	///      other errors: legitimate decoding errors
	///</summary>
	AVRESULT SendFrame(AVFrame^ frame);
	///<summary>
	/// Read encoded data from the encoder.
	///
	/// @param avctx codec context
	/// @param avpkt This will be set to a reference-counted packet allocated by the
	///              encoder. Note that the function will always call
	///              av_frame_unref(frame) before doing anything else.
	/// @return 0 on success, otherwise negative error code:
	///      AVERROR(EAGAIN):   output is not available in the current state - user
	///                         must try to send input
	///      AVERROR_EOF:       the encoder has been fully flushed, and there will be
	///                         no more output packets
	///      AVERROR(EINVAL):   codec not opened, or it is an encoder
	///      other errors: legitimate decoding errors
	///</summary>
	AVRESULT ReceivePacket(AVPacket^ avpkt);
public:
	/// Get Default format from list
	AVPixelFormat default_get_format(array<AVPixelFormat>^ fmt);
	// Default Get Buffer
	AVRESULT default_get_buffer2(AVFrame^ frame, int flags);
internal:
	// Callback Handlers
	virtual void DrawHorizBand(AVFrame^ src, array<int>^ offset,int y, int type, int height);
	virtual AVPixelFormat GetFormat(array<AVPixelFormat>^ fmt);
	virtual AVRESULT GetBuffer2(AVFrame^ frame, int flags);
public:
	// ICloneable
	virtual Object^ Clone();
public:
	/// @return AVClass for AVCodecContext.
	///
	/// @see av_opt_find().
	static AVClass^ GetClass();
};
//////////////////////////////////////////////////////
// AVCodecParser
public ref class AVCodecParser : public AVBase
{
public:
	// Parsers Iterator
	ref class AVCodecParsers
		: public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<AVCodecParser^>
	{
	private:
		ref class Enumerator
			: public System::Collections::IEnumerator
			, public System::Collections::Generic::IEnumerator<AVCodecParser^>
		{
		protected:
			IntPtr				m_pOpaque;
			AVCodecParser^	m_pCurrent;
		public:
			Enumerator();
			~Enumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVCodecParser^ Current { AVCodecParser^ get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
		};
	internal:
		AVCodecParsers();
	public:
		property AVCodecParser^ default[int] { AVCodecParser^ get(int); }
		property int Count { int get(); }
	public:
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { return gcnew Enumerator(); }
		virtual System::Collections::Generic::IEnumerator<AVCodecParser^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVCodecParser^>::GetEnumerator { return gcnew Enumerator(); }
	};
internal:
	AVCodecParser(void * _pointer,AVBase^ _parent);
public:
	// several codec IDs are permitted 
	property array<AVCodecID>^ codec_ids { array<AVCodecID>^ get(); }
public:
	static AVCodecParser^ Next(AVCodecParser^ _current);
	static void Register(AVCodecParser^ _parser);
public:
	// Enumerate All Filters
	static property AVCodecParsers^ Parsers { AVCodecParsers^ get() { return gcnew AVCodecParsers(); } }
};
//////////////////////////////////////////////////////
// AVCodecParserContext
public ref class AVCodecParserContext : public AVBase
{
internal:
	AVCodecParserContext(void * _pointer,AVBase^ _parent);
public:
	AVCodecParserContext(AVCodecID codec_id);
public:
	property IntPtr priv_data { IntPtr get(); }
	property AVCodecParser^ parser { AVCodecParser^ get(); }
	/// offset of the current frame
	property Int64 frame_offset { Int64 get(); }
	/// current offset (incremented by each av_parser_parse())
    property Int64 cur_offset { Int64 get(); }
	/// offset of the next frame
	property Int64 next_frame_offset { Int64 get(); }
    /// video info
    property AVPictureType pict_type { AVPictureType get(); } /// XXX: Put it back in AVCodecContext.
	/// This field is used for proper frame duration computation in lavf.
	/// It signals, how much longer the frame duration of the current frame
	/// is compared to normal frame duration.
	///
	/// frame_duration = (1 + repeat_pict) * time_base
	///
	/// It is used by codecs like H.264 to display telecined material.
    property int repeat_pict { int get(); } /// XXX: Put it back in AVCodecContext.
	/// pts of the current frame
    property Int64 pts { Int64 get(); }
	/// dts of the current frame
    property Int64 dts { Int64 get(); }
    /// private data
    property Int64 last_pts { Int64 get(); }
    property Int64 last_dts { Int64 get(); }
    property int fetch_timestamp { int get(); }

	property int cur_frame_start_index { int get(); }
    property array<Int64>^ cur_frame_offset { array<Int64>^ get(); }
    property array<Int64>^ cur_frame_pts { array<Int64>^ get(); }
    property array<Int64>^ cur_frame_dts { array<Int64>^ get(); }

	property AVParserFlags flags { AVParserFlags get(); }
	///< byte offset from starting packet start
	property Int64 offset { Int64 get(); }
    property array<Int64>^ cur_frame_end { array<Int64>^ get(); }

	/// Set by parser to 1 for key frames and 0 for non-key frames.
	/// It is initialized to -1, so if the parser doesn't set this flag,
	/// old-style fallback using AV_PICTURE_TYPE_I picture type as key frames
	/// will be used.
	property int key_frame { int get(); }

	/// Time difference in stream time base units from the pts of this
	/// packet to the point at which the output from the decoder has converged
	/// independent from the availability of previous frames. That is, the
	/// frames are virtually identical no matter if decoding started from
	/// the very first frame or from this keyframe.
	/// Is AV_NOPTS_VALUE if unknown.
	/// This field is not the display duration of the current frame.
	/// This field has no meaning if the packet does not have AV_PKT_FLAG_KEY
	/// set.
	///
	/// The purpose of this field is to allow seeking in streams that have no
	/// keyframes in the conventional sense. It corresponds to the
	/// recovery point SEI in H.264 and match_time_delta in NUT. It is also
	/// essential for some types of subtitle streams to ensure that all
	/// subtitles are correctly displayed after seeking.
	property Int64 convergence_duration { Int64 get(); }

	// Timestamp generation support:
	/// Synchronization point for start of timestamp generation.
	///
	/// Set to >0 for sync point, 0 for no sync point and <0 for undefined
	/// (default).
	///
	/// For example, this corresponds to presence of H.264 buffering period
	/// SEI message.
	property int dts_sync_point { int get(); }

	/// Offset of the current timestamp against last timestamp sync point in
	/// units of AVCodecContext.time_base.
	///
	/// Set to INT_MIN when dts_sync_point unused. Otherwise, it must
	/// contain a valid timestamp offset.
	///
	/// Note that the timestamp of sync point has usually a nonzero
	/// dts_ref_dts_delta, which refers to the previous sync point. Offset of
	/// the next frame after timestamp sync point will be usually 1.
	///
	/// For example, this corresponds to H.264 cpb_removal_delay.
	property int dts_ref_dts_delta { int get(); }

	/// Presentation delay of current frame in units of AVCodecContext.time_base.
	///
	/// Set to INT_MIN when dts_sync_point unused. Otherwise, it must
	/// contain valid non-negative timestamp delta (presentation time of a frame
	/// must not lie in the past).
	///
	/// This delay represents the difference between decoding and presentation
	/// time of the frame.
	///
	/// For example, this corresponds to H.264 dpb_output_delay.
	property int pts_dts_delta { int get(); }

	/// Position of the packet in file.
	///
	/// Analogous to cur_frame_pts/dts
	property array<Int64>^ cur_frame_pos { array<Int64>^ get(); }

	/// Byte position of currently parsed frame in stream.
	property Int64 pos { Int64 get(); }

	/// Previous frame byte position.
	property Int64 last_pos { Int64 get(); }

	/// Duration of the current frame.
	/// For audio, this is in units of 1 / AVCodecContext.sample_rate.
	/// For all other types, this is in units of AVCodecContext.time_base.
	property int duration { int get(); }

	property AVFieldOrder field_order { AVFieldOrder get(); }

	/// Indicate whether a picture is coded as a frame, top field or bottom field.
	///
	/// For example, H.264 field_pic_flag equal to 0 corresponds to
	/// AV_PICTURE_STRUCTURE_FRAME. An H.264 picture with field_pic_flag
	/// equal to 1 and bottom_field_flag equal to 0 corresponds to
	/// AV_PICTURE_STRUCTURE_TOP_FIELD.
	property AVPictureStructure picture_structure { AVPictureStructure get(); }

	/// Picture number incremented in presentation or output order.
	/// This field may be reinitialized at the first picture of a new sequence.
	///
	/// For example, this corresponds to H.264 PicOrderCnt.
	property int output_picture_number { int get(); }

	/// Dimensions of the decoded video intended for presentation.
	property int width { int get(); }
	property int height { int get(); }

	/// Dimensions of the coded video.
	property int coded_width { int get(); }
	property int coded_height { int get(); }

	/// The format of the coded data, corresponds to enum AVPixelFormat for video
	/// and for enum AVSampleFormat for audio.
	///
	/// Note that a decoder can have considerable freedom in how exactly it
	/// decodes the data, so the format reported here might be different from the
	/// one returned by a decoder.
	property int format { int get(); }
public:
	/// Parse a packet.
	///
	/// @param s             parser context.
	/// @param avctx         codec context.
	/// @param poutbuf       set to pointer to parsed buffer or NULL if not yet finished.
	/// @param poutbuf_size  set to size of parsed buffer or zero if not yet finished.
	/// @param buf           input buffer.
	/// @param buf_size      input length, to signal EOF, this should be 0 (so that the last frame can be output).
	/// @param pts           input presentation timestamp.
	/// @param dts           input decoding timestamp.
	/// @param pos           input byte position in stream.
	/// @return the number of bytes of the input bitstream used.
	///
	/// Example:
	/// @code
	///   while(in_len){
	///       len = av_parser_parse2(myparser, AVCodecContext, &data, &size,
	///                                        in_data, in_len,
	///                                        pts, dts, pos);
	///       in_data += len;
	///       in_len  -= len;
	///
	///       if(size)
	///          decode_frame(data, size);
	///   }
	/// @endcode
	int Parse(AVCodecContext^ avctx,
                     IntPtr % poutbuf, int % poutbuf_size,
                     IntPtr buf, int buf_size,
                     Int64 pts, Int64 dts,
                     Int64 pos);

	int Parse(AVCodecContext^ avctx,
					IntPtr % poutbuf, int % poutbuf_size,
					array<byte>^ buf,
					Int64 pts, Int64 dts,
					Int64 pos);

	int Parse(AVCodecContext^ avctx,
					AVPacket^ pkt,
					array<byte>^ buf,
					Int64 pts, Int64 dts,
					Int64 pos);

	int Parse(AVCodecContext^ avctx,
					AVPacket^ pkt,
					IntPtr buf, int buf_size,
					Int64 pts, Int64 dts,
					Int64 pos);

	int Parse(AVCodecContext^ avctx,
                     IntPtr % poutbuf, int % poutbuf_size,
                     IntPtr buf, int buf_size,
                     Int64 pts, Int64 dts);

	int Parse(AVCodecContext^ avctx,
					IntPtr % poutbuf, int % poutbuf_size,
					array<byte>^ buf,
					Int64 pts, Int64 dts);

	int Parse(AVCodecContext^ avctx,
					AVPacket^ pkt,
					array<byte>^ buf,
					Int64 pts, Int64 dts);

	int Parse(AVCodecContext^ avctx,
					AVPacket^ pkt,
					IntPtr buf, int buf_size,
					Int64 pts, Int64 dts);

	int Parse(AVCodecContext^ avctx,
                     IntPtr % poutbuf, int % poutbuf_size,
                     IntPtr buf, int buf_size,
                     Int64 pos);

	int Parse(AVCodecContext^ avctx,
					IntPtr % poutbuf, int % poutbuf_size,
					array<byte>^ buf,
					Int64 pos);

	int Parse(AVCodecContext^ avctx,
					AVPacket^ pkt,
					array<byte>^ buf,
					Int64 pos);

	int Parse(AVCodecContext^ avctx,
					AVPacket^ pkt,
					IntPtr buf, int buf_size,
					Int64 pos);

	int Parse(AVCodecContext^ avctx,
                     IntPtr % poutbuf, int % poutbuf_size,
                     IntPtr buf, int buf_size);

	int Parse(AVCodecContext^ avctx,
					IntPtr % poutbuf, int % poutbuf_size,
					array<byte>^ buf);

	int Parse(AVCodecContext^ avctx,
					AVPacket^ pkt,
					array<byte>^ buf);

	int Parse(AVCodecContext^ avctx,
					AVPacket^ pkt,
					IntPtr buf, int buf_size);

	int Parse(AVCodecContext^ avctx,
					AVPacket^ out_pkt,
					AVPacket^ in_pkt);

	/// @return 0 if the output buffer is a subset of the input, 1 if it is allocated and must be freed
	/// @deprecated use AVBitStreamFilter
	int Change(AVCodecContext^ avctx,
                     IntPtr % poutbuf, int % poutbuf_size,
                     IntPtr buf, int buf_size, int keyframe);
};
//////////////////////////////////////////////////////
// AVBitStreamFilter
public ref class AVBitStreamFilter : public AVBase
{
public:
	// Filters Iterator
	ref class AVBitStreamFilters
		: public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<AVBitStreamFilter^>
	{
	private:
		ref class AVBitStreamFilterEnumerator
			: public System::Collections::IEnumerator
			, public System::Collections::Generic::IEnumerator<AVBitStreamFilter^>
		{
		protected:
			IntPtr				m_pOpaque;
			AVBitStreamFilter^	m_pCurrent;
		public:
			AVBitStreamFilterEnumerator();
			~AVBitStreamFilterEnumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVBitStreamFilter^ Current { AVBitStreamFilter^ get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
		};
	internal:
		AVBitStreamFilters();
	public:
		property AVBitStreamFilter^ default[int] { AVBitStreamFilter^ get(int); }
		property int Count { int get(); }
	public:
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { return gcnew AVBitStreamFilterEnumerator(); }
		virtual System::Collections::Generic::IEnumerator<AVBitStreamFilter^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVBitStreamFilter^>::GetEnumerator { return gcnew AVBitStreamFilterEnumerator(); }
	};
internal:
	AVBitStreamFilter(void * _pointer,AVBase^ _parent);
public:
	property String^ name { String^ get(); }
	/// A list of codec ids supported by the filter, terminated by
	/// AV_CODEC_ID_NONE.
	/// May be NULL, in that case the bitstream filter works with any codec id.
	property array<AVCodecID>^ codec_ids { array<AVCodecID>^ get(); }
	/// A class for the private data, used to declare bitstream filter private
    /// AVOptions. This field is NULL for bitstream filters that do not declare
    /// any options.
    ///
    /// If this field is non-NULL, the first member of the filter private data
    /// must be a pointer to AVClass, which will be set by libavcodec generic
    /// code to this class.
	property AVClass^ priv_class { AVClass^ get(); }
public:
	// Enumerate All Filters
	static property AVBitStreamFilters^ Filters { AVBitStreamFilters^ get(); }
public:
	virtual String^ ToString() override;
public:
	/// Register a bitstream filter.
	///
	/// The filter will be accessible to the application code through
	/// av_bitstream_filter_next() or can be directly initialized with
	/// av_bitstream_filter_init().
	///
	/// @see avcodec_register_all()
	static void Register(AVBitStreamFilter^ bsf);
	/// If f is NULL, return the first registered bitstream filter,
	/// if f is non-NULL, return the next registered bitstream filter
	/// after f, or NULL if f is the last one.
	///
	/// This function can be used to iterate over all registered bitstream
	/// filters.
	static AVBitStreamFilter^ Next(AVBitStreamFilter^ f);
	///<summary>
	/// @return a bitstream filter with the specified name or NULL if no such
	///         bitstream filter exists.
	///</summary>
	static AVBitStreamFilter^ GetByName(String^ value);
};
//////////////////////////////////////////////////////
// AVBitStreamFilterContext
public ref class AVBitStreamFilterContext : public AVBase
{
internal:
	AVBitStreamFilterContext(void * _pointer,AVBase^ _parent);
public:
	AVBitStreamFilterContext(String^ name);
public:
	property IntPtr priv_data { IntPtr get(); }
    property AVBitStreamFilter^ filter { AVBitStreamFilter^ get(); }
    property AVCodecParserContext^ parser { AVCodecParserContext^ get(); }
	property AVBitStreamFilterContext^ next { AVBitStreamFilterContext^ get(); }
public:
	/// Filter bitstream.
	///
	/// This function filters the buffer buf with size buf_size, and places the
	/// filtered buffer in the buffer pointed to by poutbuf.
	///
	/// The output buffer must be freed by the caller.
	///
	/// @param bsfc            bitstream filter context created by av_bitstream_filter_init()
	/// @param avctx           AVCodecContext accessed by the filter, may be NULL.
	///                        If specified, this must point to the encoder context of the
	///                        output stream the packet is sent to.
	/// @param args            arguments which specify the filter configuration, may be NULL
	/// @param poutbuf         pointer which is updated to point to the filtered buffer
	/// @param poutbuf_size    pointer which is updated to the filtered buffer size in bytes
	/// @param buf             buffer containing the data to filter
	/// @param buf_size        size in bytes of buf
	/// @param keyframe        set to non-zero if the buffer to filter corresponds to a key-frame packet data
	/// @return >= 0 in case of success, or a negative error code in case of failure
	///
	/// If the return value is positive, an output buffer is allocated and
	/// is available in *poutbuf, and is distinct from the input buffer.
	///
	/// If the return value is 0, the output buffer is not allocated and
	/// should be considered identical to the input buffer, or in case
	/// *poutbuf was set it points to the input buffer (not necessarily to
	/// its starting address).
	AVRESULT Filter(AVCodecContext^ avctx, String^ args,
                               IntPtr % poutbuf, int % poutbuf_size,
                               IntPtr buf, int buf_size, int keyframe);
};
//////////////////////////////////////////////////////
// AVBSFContext
/// The bitstream filter state.
public ref class AVBSFContext : public AVBase
{
public:
	/// Get the AVClass for AVBSFContext. It can be used in combination with
	/// AV_OPT_SEARCH_FAKE_OBJ for examining options.
	static property AVClass^ Class { AVClass^ get(); }
internal:
	AVBSFContext(void * _pointer,AVBase^ _parent);
public:
	AVBSFContext(AVBitStreamFilter^ filter);
	///<summary>
	/// Finalize list of bitstream filters.
	///
	/// This function will transform @ref AVBSFList to single @ref AVBSFContext,
	/// so the whole chain of bitstream filters can be treated as single filter
	/// freshly allocated by av_bsf_alloc().
	/// If the call is successful, @ref AVBSFList structure is freed and lst
	/// will be set to NULL. In case of failure, caller is responsible for
	/// freeing the structure by av_bsf_list_free()
	///
	/// @param      lst Filter list structure to be transformed
	/// @param[out] bsf Pointer to be set to newly created @ref AVBSFContext structure
	///                 representing the chain of bitstream filters
	///
	/// @return >=0 on success, negative AVERROR in case of failure
	///</summary>
	AVBSFContext(AVBSFList^ lst);
public:
	///<summary>
	/// A class for logging and AVOptions
	///</summary>
	property AVClass^ av_class { AVClass^ get(); }

	///<summary>
	/// The bitstream filter this context is an instance of.
	///</summary>
	property AVBitStreamFilter^ filter { AVBitStreamFilter^ get(); }

	///<summary>
	/// Opaque filter-specific private data. If filter->priv_class is non-NULL,
	/// this is an AVOptions-enabled struct.
	///</summary>
	property IntPtr priv_data { IntPtr get(); }

	///<summary>
	/// Parameters of the input stream. This field is allocated in
	/// av_bsf_alloc(), it needs to be filled by the caller before
	/// av_bsf_init().
	///</summary>
	property AVCodecParameters^ par_in { AVCodecParameters^ get(); }

	///<summary>
	/// Parameters of the output stream. This field is allocated in
	/// av_bsf_alloc(), it is set by the filter in av_bsf_init().
	///</summary>
	property AVCodecParameters^ par_out { AVCodecParameters^ get(); }

	///<summary>
	/// The timebase used for the timestamps of the input packets. Set by the
	/// caller before av_bsf_init().
	///</summary>
	property AVRational^ time_base_in { AVRational^ get();  void set(AVRational^); }

	///<summary>
	/// The timebase used for the timestamps of the output packets. Set by the
	/// filter in av_bsf_init().
	///</summary>
	property AVRational^ time_base_out { AVRational^ get(); }
public:
	virtual String^ ToString() override;
public:
	///<summary>
	/// Prepare the filter for use, after all the parameters and options have been
	/// set.
	///</summary>
	AVRESULT Init();

	///<summary>
	/// Submit a packet for filtering.
	///
	/// After sending each packet, the filter must be completely drained by calling
	/// av_bsf_receive_packet() repeatedly until it returns AVERROR(EAGAIN) or
	/// AVERROR_EOF.
	///
	/// @param pkt the packet to filter. The bitstream filter will take ownership of
	/// the packet and reset the contents of pkt. pkt is not touched if an error occurs.
	/// This parameter may be NULL, which signals the end of the stream (i.e. no more
	/// packets will be sent). That will cause the filter to output any packets it
	/// may have buffered internally.
	///
	/// @return 0 on success, a negative AVERROR on error.
	///</summary>
	AVRESULT SendPacket(AVPacket^ pkt);

	///<summary>
	/// Retrieve a filtered packet.
	///
	/// @param[out] pkt this struct will be filled with the contents of the filtered
	///                 packet. It is owned by the caller and must be freed using
	///                 av_packet_unref() when it is no longer needed.
	///                 This parameter should be "clean" (i.e. freshly allocated
	///                 with av_packet_alloc() or unreffed with av_packet_unref())
	///                 when this function is called. If this function returns
	///                 successfully, the contents of pkt will be completely
	///                 overwritten by the returned data. On failure, pkt is not
	///                 touched.
	///
	/// @return 0 on success. AVERROR(EAGAIN) if more packets need to be sent to the
	/// filter (using av_bsf_send_packet()) to get more output. AVERROR_EOF if there
	/// will be no further output from the filter. Another negative AVERROR value if
	/// an error occurs.
	///
	/// @note one input packet may result in several output packets, so after sending
	/// a packet with av_bsf_send_packet(), this function needs to be called
	/// repeatedly until it stops returning 0. It is also possible for a filter to
	/// output fewer packets than were sent to it, so this function may return
	/// AVERROR(EAGAIN) immediately after a successful av_bsf_send_packet() call.
	///</summary>
	AVRESULT ReceivePacket(AVPacket^ pkt);

	///<summary>
	/// Reset the internal bitstream filter state / flush internal buffers.
	///</summary>
	void Flush();
public:
	///<summary>
	/// Parse string describing list of bitstream filters and create single
	/// @ref AVBSFContext describing the whole chain of bitstream filters.
	/// Resulting @ref AVBSFContext can be treated as any other @ref AVBSFContext freshly
	/// allocated by av_bsf_alloc().
	///
	/// @param      str String describing chain of bitstream filters in format
	///                 `bsf1[=opt1=val1:opt2=val2][,bsf2]`
	/// @param[out] bsf Pointer to be set to newly created @ref AVBSFContext structure
	///                 representing the chain of bitstream filters
	///
	/// @return >=0 on success, negative AVERROR in case of failure
	///</summary>
	static AVRESULT Parse(String^ str, [Out] AVBSFContext^ % bsf);

	///<summary>
	/// Get null/pass-through bitstream filter.
	///
	/// @param[out] bsf Pointer to be set to new instance of pass-through bitstream filter
	///
	/// @return
	///</summary>
	static AVRESULT get_null_filter([Out] AVBSFContext^ % bsf);
};
//////////////////////////////////////////////////////
// AVBSFList
public ref class AVBSFList : public AVBase
{
public:
	AVBSFList();
public:
	///<summary>
	/// Append bitstream filter to the list of bitstream filters.
	///
	/// @param lst List to append to
	/// @param bsf Filter context to be appended
	///
	/// @return >=0 on success, negative AVERROR in case of failure
	///</summary>
	AVRESULT Append(AVBSFContext^ bsf);

	///<summary>
	/// Construct new bitstream filter context given it's name and options
	/// and append it to the list of bitstream filters.
	///
	/// @param lst      List to append to
	/// @param bsf_name Name of the bitstream filter
	/// @param options  Options for the bitstream filter, can be set to NULL
	///
	/// @return >=0 on success, negative AVERROR in case of failure
	///</summary>
	AVRESULT Append(String^ bsf_name);
	AVRESULT Append(String^ bsf_name,AVDictionary^ options);
};
//////////////////////////////////////////////////////
#pragma endregion 
//////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////