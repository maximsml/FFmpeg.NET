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
// Core classes and utils wrapper.
//////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////
using namespace System;
using namespace System::IO;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;  
//////////////////////////////////////////////////////
namespace FFmpeg {
//////////////////////////////////////////////////////
ref class AVPixFmtDescriptor;
//////////////////////////////////////////////////////
#pragma region Enums
//////////////////////////////////////////////////////
/// <summary>
/// AVPicture types, pixel formats and basic image planes manipulation.
/// </summary>
public enum class AVPictureType
{
	NONE = 0, ///< Undefined
	I,     ///< Intra
	P,     ///< Predicted
	B,     ///< Bi-dir predicted
	S,     ///< S(GMC)-VOP MPEG4
	SI,    ///< Switching Intra
	SP,    ///< Switching Predicted
	BI,    ///< BI type
	QP2,   ///< MPEG2 style QScale
};
//////////////////////////////////////////////////////
// FFDecodeError
[Flags]
public enum class FFDecodeError : int
{
	None = 0,
	InvalidBitstream = 1,
	MissingReference = 2,
};
//////////////////////////////////////////////////////
/// AV_MATRIX_ENCODING_*
public enum class AVMatrixEncoding
{
	NONE,
	DOLBY,
	DPLII,
	DPLIIX,
	DPLIIZ,
	DOLBYEX,
	DOLBYHEADPHONE,
};
//////////////////////////////////////////////////////
/// AV_ROUND_*
public enum class AVRounding
{
	ZERO = 0, ///< Round toward zero.
	INF = 1, ///< Round away from zero.
	DOWN = 2, ///< Round toward -infinity.
	UP = 3, ///< Round toward +infinity.
	NEAR_INF = 5, ///< Round to nearest and halfway cases away from zero.
	PASS_MINMAX = 8192, ///< Flag to pass INT64_MIN/MAX through instead of rescaling, this avoids special cases for AV_NOPTS_VALUE
};
//////////////////////////////////////////////////////
// AVClassCategory
/// AV_CLASS_CATEGORY_*
public enum class AVClassCategory
{
	NA = 0,
	INPUT,
	OUTPUT,
	MUXER,
	DEMUXER,
	ENCODER,
	DECODER,
	FILTER,
	BITSTREAM_FILTER,
	SWSCALER,
	SWRESAMPLER,
	DEVICE_VIDEO_OUTPUT = 40,
	DEVICE_VIDEO_INPUT,
	DEVICE_AUDIO_OUTPUT,
	DEVICE_AUDIO_INPUT,
	DEVICE_OUTPUT,
	DEVICE_INPUT,
	NB, ///< not part of ABI/API
};
//////////////////////////////////////////////////////
public enum class AVMediaType
{
	UNKNOWN = -1,  ///< Usually treated as DATA
	VIDEO,
	AUDIO,
	DATA,          ///< Opaque data information usually continuous
	SUBTITLE,
	ATTACHMENT,    ///< Opaque data information usually sparse
	NB
};
//////////////////////////////////////////////////////
///<summary>
/// Identify the syntax and semantics of the bitstream.
/// The principle is roughly:
/// Two decoders with the same ID can decode the same streams.
/// Two encoders with the same ID can encode compatible streams.
/// There may be slight deviations from the principle due to implementation
/// details.
///
/// If you add a codec ID to this list, add it so that
/// 1. no value of a existing codec ID changes (that would break ABI),
/// 2. Give it a value which when taken as ASCII is recognized uniquely by a human as this specific codec.
///    This ensures that 2 forks can independently add AVCodecIDs without producing conflicts.
///
/// After adding new codec IDs, do not forget to add an entry to the codec
/// descriptor list and bump libavcodec minor version.
///</summary>
/// AV_CODEC_ID_*
public enum class AVCodecID : UInt32
{
	NONE,

	MPEG1VIDEO,
	MPEG2VIDEO, ///< preferred ID for MPEG-1/2 video decoding
	H261,
	H263,
	RV10,
	RV20,
	MJPEG,
	MJPEGB,
	LJPEG,
	SP5X,
	JPEGLS,
	MPEG4,
	RAWVIDEO,
	MSMPEG4V1,
	MSMPEG4V2,
	MSMPEG4V3,
	WMV1,
	WMV2,
	H263P,
	H263I,
	FLV1,
	SVQ1,
	SVQ3,
	DVVIDEO,
	HUFFYUV,
	CYUV,
	H264,
	INDEO3,
	VP3,
	THEORA,
	ASV1,
	ASV2,
	FFV1,
	_4XM,
	VCR1,
	CLJR,
	MDEC,
	ROQ,
	INTERPLAY_VIDEO,
	XAN_WC3,
	XAN_WC4,
	RPZA,
	CINEPAK,
	WS_VQA,
	MSRLE,
	MSVIDEO1,
	IDCIN,
	_8BPS,
	SMC,
	FLIC,
	TRUEMOTION1,
	VMDVIDEO,
	MSZH,
	ZLIB,
	QTRLE,
	TSCC,
	ULTI,
	QDRAW,
	VIXL,
	QPEG,
	PNG,
	PPM,
	PBM,
	PGM,
	PGMYUV,
	PAM,
	FFVHUFF,
	RV30,
	RV40,
	VC1,
	WMV3,
	LOCO,
	WNV1,
	AASC,
	INDEO2,
	FRAPS,
	TRUEMOTION2,
	BMP,
	CSCD,
	MMVIDEO,
	ZMBV,
	AVS,
	SMACKVIDEO,
	NUV,
	KMVC,
	FLASHSV,
	CAVS,
	JPEG2000,
	VMNC,
	VP5,
	VP6,
	VP6F,
	TARGA,
	DSICINVIDEO,
	TIERTEXSEQVIDEO,
	TIFF,
	GIF,
	DXA,
	DNXHD,
	THP,
	SGI,
	C93,
	BETHSOFTVID,
	PTX,
	TXD,
	VP6A,
	AMV,
	VB,
	PCX,
	SUNRAST,
	INDEO4,
	INDEO5,
	MIMIC,
	RL2,
	ESCAPE124,
	DIRAC,
	BFI,
	CMV,
	MOTIONPIXELS,
	TGV,
	TGQ,
	TQI,
	AURA,
	AURA2,
	V210X,
	TMV,
	V210,
	DPX,
	MAD,
	FRWU,
	FLASHSV2,
	CDGRAPHICS,
	R210,
	ANM,
	BINKVIDEO,
	IFF_ILBM,
	KGV1,
	YOP,
	VP8,
	PICTOR,
	ANSI,
	A64_MULTI,
	A64_MULTI5,
	R10K,
	MXPEG,
	LAGARITH,
	PRORES,
	JV,
	DFA,
	WMV3IMAGE,
	VC1IMAGE,
	UTVIDEO,
	BMV_VIDEO,
	VBLE,
	DXTORY,
	V410,
	XWD,
	CDXL,
	XBM,
	ZEROCODEC,
	MSS1,
	MSA1,
	TSCC2,
	MTS2,
	CLLC,
	MSS2,
	VP9,
	AIC,
	ESCAPE130,
	G2M,
	WEBP,
	HNM4_VIDEO,
	HEVC,
	FIC,
	ALIAS_PIX,
	BRENDER_PIX,
	PAF_VIDEO,
	EXR,
	VP7,
	SANM,
	SGIRLE,
	MVC1,
	MVC2,
	HQX,
	TDSC,
	HQ_HQA,
	HAP,
	DDS,
	DXV,
	SCREENPRESSO,
	RSCC,
	AVS2,

	Y41P = 0x8000,
	AVRP,
	_012V,
	AVUI,
	AYUV,
	TARGA_Y216,
	V308,
	V408,
	YUV4,
	AVRN,
	CPIA,
	XFACE,
	SNOW,
	SMVJPEG,
	APNG,
	DAALA,
	CFHD,
	TRUEMOTION2RT,
	M101,
	MAGICYUV,
	SHEERVIDEO,
	YLC,
	PSD,
	PIXLET,
	SPEEDHQ,
	FMVC,
	SCPR,
	CLEARVIDEO,
	XPM,
	AV1,
	BITPACKED,
	MSCC,
	SRGC,
	SVG,
	GDV,
	FITS,
	IMM4,
	PROSUMER,
	MWSC,
	WCMV,
	RASC,
	HYMT,
	ARBC,
	AGM,
	LSCR,
	VP4,

	/* various PCM "codecs" */
	FIRST_AUDIO = 0x10000,     ///< A dummy id pointing at the start of audio codecs
	PCM_S16LE = 0x10000,
	PCM_S16BE,
	PCM_U16LE,
	PCM_U16BE,
	PCM_S8,
	PCM_U8,
	PCM_MULAW,
	PCM_ALAW,
	PCM_S32LE,
	PCM_S32BE,
	PCM_U32LE,
	PCM_U32BE,
	PCM_S24LE,
	PCM_S24BE,
	PCM_U24LE,
	PCM_U24BE,
	PCM_S24DAUD,
	PCM_ZORK,
	PCM_S16LE_PLANAR,
	PCM_DVD,
	PCM_F32BE,
	PCM_F32LE,
	PCM_F64BE,
	PCM_F64LE,
	PCM_BLURAY,
	PCM_LXF,
	S302M,
	PCM_S8_PLANAR,
	PCM_S24LE_PLANAR,
	PCM_S32LE_PLANAR,
	PCM_S16BE_PLANAR,

	PCM_S64LE = 0x10800,
	PCM_S64BE,
	PCM_F16LE,
	PCM_F24LE,
	PCM_VIDC,

	/* various ADPCM codecs */
	ADPCM_IMA_QT = 0x11000,
	ADPCM_IMA_WAV,
	ADPCM_IMA_DK3,
	ADPCM_IMA_DK4,
	ADPCM_IMA_WS,
	ADPCM_IMA_SMJPEG,
	ADPCM_MS,
	ADPCM_4XM,
	ADPCM_XA,
	ADPCM_ADX,
	ADPCM_EA,
	ADPCM_G726,
	ADPCM_CT,
	ADPCM_SWF,
	ADPCM_YAMAHA,
	ADPCM_SBPRO_4,
	ADPCM_SBPRO_3,
	ADPCM_SBPRO_2,
	ADPCM_THP,
	ADPCM_IMA_AMV,
	ADPCM_EA_R1,
	ADPCM_EA_R3,
	ADPCM_EA_R2,
	ADPCM_IMA_EA_SEAD,
	ADPCM_IMA_EA_EACS,
	ADPCM_EA_XAS,
	ADPCM_EA_MAXIS_XA,
	ADPCM_IMA_ISS,
	ADPCM_G722,
	ADPCM_IMA_APC,
	ADPCM_VIMA,

	ADPCM_AFC = 0x11800,
	ADPCM_IMA_OKI,
	ADPCM_DTK,
	ADPCM_IMA_RAD,
	ADPCM_G726LE,
	ADPCM_THP_LE,
	ADPCM_PSX,
	ADPCM_AICA,
	ADPCM_IMA_DAT4,
	ADPCM_MTAF,
	ADPCM_AGM,

	/* AMR */
	AMR_NB = 0x12000,
	AMR_WB,

	/* RealAudio codecs*/
	RA_144 = 0x13000,
	RA_288,

	/* various DPCM codecs */
	ROQ_DPCM = 0x14000,
	INTERPLAY_DPCM,
	XAN_DPCM,
	SOL_DPCM,

	SDX2_DPCM = 0x14800,
	GREMLIN_DPCM,

	/* audio codecs */
	MP2 = 0x15000,
	MP3, ///< preferred ID for decoding MPEG audio layer 1, 2 or 3
	AAC,
	AC3,
	DTS,
	VORBIS,
	DVAUDIO,
	WMAV1,
	WMAV2,
	MACE3,
	MACE6,
	VMDAUDIO,
	FLAC,
	MP3ADU,
	MP3ON4,
	SHORTEN,
	ALAC,
	WESTWOOD_SND1,
	GSM, ///< as in Berlin toast format
	QDM2,
	COOK,
	TRUESPEECH,
	TTA,
	SMACKAUDIO,
	QCELP,
	WAVPACK,
	DSICINAUDIO,
	IMC,
	MUSEPACK7,
	MLP,
	GSM_MS, /* as found in WAV */
	ATRAC3,
	APE,
	NELLYMOSER,
	MUSEPACK8,
	SPEEX,
	WMAVOICE,
	WMAPRO,
	WMALOSSLESS,
	ATRAC3P,
	EAC3,
	SIPR,
	MP1,
	TWINVQ,
	TRUEHD,
	MP4ALS,
	ATRAC1,
	BINKAUDIO_RDFT,
	BINKAUDIO_DCT,
	AAC_LATM,
	QDMC,
	CELT,
	G723_1,
	G729,
	_8SVX_EXP,
	_8SVX_FIB,
	BMV_AUDIO,
	RALF,
	IAC,
	ILBC,
	OPUS,
	COMFORT_NOISE,
	TAK,
	METASOUND,
	PAF_AUDIO,
	ON2AVC,
	DSS_SP,
	CODEC2,

	FFWAVESYNTH = 0x15800,
	SONIC,
	SONIC_LS,
	EVRC,
	SMV,
	DSD_LSBF,
	DSD_MSBF,
	DSD_LSBF_PLANAR,
	DSD_MSBF_PLANAR,
	_4GV,
	INTERPLAY_ACM,
	XMA1,
	XMA2,
	DST,
	ATRAC3AL,
	ATRAC3PAL,
	DOLBY_E,
	APTX,
	APTX_HD,
	SBC,
	ATRAC9,
	HCOM,

	/* subtitle codecs */
	FIRST_SUBTITLE = 0x17000,          ///< A dummy ID pointing at the start of subtitle codecs.
	DVD_SUBTITLE = 0x17000,
	DVB_SUBTITLE,
	TEXT,  ///< raw UTF-8 text
	XSUB,
	SSA,
	MOV_TEXT,
	HDMV_PGS_SUBTITLE,
	DVB_TELETEXT,
	SRT,

	MICRODVD   = 0x17800,
	EIA_608,
	JACOSUB,
	SAMI,
	REALTEXT,
	STL,
	SUBVIEWER1,
	SUBVIEWER,
	SUBRIP,
	WEBVTT,
	MPL2,
	VPLAYER,
	PJS,
	ASS,
	HDMV_TEXT_SUBTITLE,
	TTML,
	ARIB_CAPTION,

	/* other specific kind of codecs (generally used for attachments) */
	FIRST_UNKNOWN = 0x18000,           ///< A dummy ID pointing at the start of various fake codecs.
	TTF = 0x18000,

	SCTE_35, ///< Contain timestamp estimated through PCR of program stream.
	BINTEXT    = 0x18800,
	XBIN,
	IDF,
	OTF,
	SMPTE_KLV,
	DVD_NAV,
	TIMED_ID3,
	BIN_DATA,


	PROBE = 0x19000, ///< codec_id is not known (like NONE) but lavf should attempt to identify it

	MPEG2TS = 0x20000, /**< _FAKE_ codec to indicate a raw MPEG-2 TS
								   * stream (only used by libavformat) */
	MPEG4SYSTEMS = 0x20001, /**< _FAKE_ codec to indicate a MPEG-4 Systems
										* stream (only used by libavformat) */
	FFMETADATA = 0x21000,   ///< Dummy codec for streams containing only metadata information.
	WRAPPED_AVFRAME = 0x21001, ///< Passthrough codec, AVFrames	wrapped in AVPacket

};
//////////////////////////////////////////////////////
[Flags]
//AV_CODEC_PROP_
public enum class AVCodecProp: int
{
	///<summary>
	/// Codec uses only intra compression.
	/// Video codecs only.
	///</summary>
	IntraOnly = (1 << 0),
	///<summary>
	/// Codec supports lossy compression. Audio and video codecs only.
	/// @note a codec may support both lossy and lossless
	/// compression modes
	///</summary>
	Lossy = (1 << 1),
	///<summary>
	/// Codec supports lossless compression. Audio and video codecs only.
	///</summary>
	Lossless = (1 << 2),
	/**
		* Codec supports frame reordering. That is, the coded order (the order in which
		* the encoded packets are output by the encoders / stored / input to the
		* decoders) may be different from the presentation order of the corresponding
		* frames.
		*
		* For codecs that do not have this property set, PTS and DTS should always be
		* equal.
		*/
	Reorder       = (1 << 3),
	/**
		* Subtitle codec is bitmap based
		* Decoded AVSubtitle data can be read from the AVSubtitleRect->pict field.
		*/
	BitmapSub    = (1 << 16),
	/**
		* Subtitle codec is text based.
		* Decoded AVSubtitle data can be read from the AVSubtitleRect->ass field.
		*/
	TextSub      = (1 << 17)
};
//////////////////////////////////////////////////////
///<summary>
/// @ingroup lavc_encoding
/// motion estimation type.
///</summary>
/// FF_ME_*
public enum class ME
{
    ZERO = 0,
    ME_EPZS,        ///< enhanced predictive zonal search
    ME_XONE,
};
//////////////////////////////////////////////////////
///<summary>
/// @ingroup lavc_decoding
///</summary>
/// AVDISCARD_*
public enum class AVDiscard
{
    //// We leave some space between them for extensions (drop some
    /// keyframes for intra-only or drop just some bidir frames). ///</summary>
	NONE    =-16, ///< discard nothing
	DEFAULT =  0, ///< discard useless packets like 0 size packets in avi
	NONREF  =  8, ///< discard all non reference
	BIDIR   = 16, ///< discard all bidirectional frames
	NONINTRA= 24, ///< discard all non intra frames
	NONKEY  = 32, ///< discard all frames except keyframes
	ALL     = 48, ///< discard all
};
//////////////////////////////////////////////////////
// AVCOL_PRI_*
public enum class AVColorPrimaries
{
    BT709 = 1, ///< also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP177 Annex B
    UNSPECIFIED = 2,
    BT470M = 4,
    BT470BG = 5, ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM
    SMPTE170M = 6, ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
    SMPTE240M = 7, ///< functionally identical to above
    FILM = 8,
};
//////////////////////////////////////////////////////
/// AVCOL_TRC_*
public enum class AVColorTransferCharacteristic
{
    BT709 = 1, ///< also ITU-R BT1361
    UNSPECIFIED = 2,
    GAMMA22 = 4, ///< also ITU-R BT470M / ITU-R BT1700 625 PAL & SECAM
    GAMMA28 = 5, ///< also ITU-R BT470BG
    SMPTE240M = 7,
};
//////////////////////////////////////////////////////
/**
* YUV colorspace type.
* These values match the ones defined by ISO/IEC 23001-8_2013 § 7.3.
*/
/// AVCOL_SPC_*
public value class AVColorSpace
{
public:
	static const AVColorSpace RGB = AVCOL_SPC_RGB;
	static const AVColorSpace BT709 = AVCOL_SPC_BT709; ///< also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / SMPTE RP177 Annex B
	static const AVColorSpace UNSPECIFIED = AVCOL_SPC_UNSPECIFIED;
	static const AVColorSpace RESERVED = AVCOL_SPC_RESERVED;
	static const AVColorSpace FCC = AVCOL_SPC_FCC;
	static const AVColorSpace BT470BG = AVCOL_SPC_BT470BG; ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
	static const AVColorSpace SMPTE170M = AVCOL_SPC_SMPTE170M; ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC / functionally identical to above
	static const AVColorSpace SMPTE240M = AVCOL_SPC_SMPTE240M;
	static const AVColorSpace YCOCG = AVCOL_SPC_YCOCG; ///< Used by Dirac / VC-2 and H.264 FRext, see ITU-T SG16
	static const AVColorSpace YCGCO = AVCOL_SPC_YCOCG;
	static const AVColorSpace BT2020_NCL  = AVCOL_SPC_BT2020_NCL; ///< ITU-R BT2020 non-constant luminance system
	static const AVColorSpace BT2020_CL   = AVCOL_SPC_BT2020_CL; ///< ITU-R BT2020 constant luminance system
	static const AVColorSpace SMPTE2085   = AVCOL_SPC_SMPTE2085; ///< SMPTE 2085, Y'D'zD'x
	static const AVColorSpace CHROMA_DERIVED_NCL = AVCOL_SPC_CHROMA_DERIVED_NCL; ///< Chromaticity-derived non-constant luminance system
	static const AVColorSpace CHROMA_DERIVED_CL = AVCOL_SPC_CHROMA_DERIVED_CL; ///< Chromaticity-derived constant luminance system
	static const AVColorSpace ICTCP       = AVCOL_SPC_ICTCP; ///< ITU-R BT.2100-0, ICtCp
protected:
	/// Value
	int			m_nValue;
public:
	AVColorSpace(int value);
internal:
	AVColorSpace(::AVColorSpace value);
protected:
	//property int value { int get() { return m_nValue; } }
public:
	/// Return the name of sample_fmt, or NULL if sample_fmt is not
	/// recognized.
	property String^ name { String^ get() { return ToString(); } }
public:
	static operator int(AVColorSpace a) { return a.m_nValue; }
	static operator AVColorSpace(int a) { return AVColorSpace(a); }
internal:
	static operator ::AVColorSpace(AVColorSpace a) { return (::AVColorSpace)a.m_nValue; }
	static operator AVColorSpace(::AVColorSpace a) { return AVColorSpace((int)a); }
public:
	virtual String^ ToString() override;
public:
	/// @return the AVColorSpace value for name or an AVError if not found.
	static AVColorSpace get_colorspace(String^ name);
};
//////////////////////////////////////////////////////
// AVCOL_RANGE_*
public enum class AVColorRange
{
    UNSPECIFIED = 0,
    MPEG = 1, ///< the normal 219///2^(n-8) "MPEG" YUV ranges
    JPEG = 2, ///< the normal     2^n-1   "JPEG" YUV ranges
};
//////////////////////////////////////////////////////
///<summary>
///  X   X      3 4 X      X are luma samples,
///             1 2        1-6 are possible chroma positions
///  X   X      5 6 X      0 is undefined/unknown position
///</summary>
///<remarks> AVCHROMA_LOC_*</remarks> 
public enum class AVChromaLocation
{
    UNSPECIFIED = 0,
    LEFT = 1, ///< mpeg2/4, h264 default
    CENTER = 2, ///< mpeg1, jpeg, h263
    TOPLEFT = 3, ///< DV
    TOP = 4,
    BOTTOMLEFT = 5,
    BOTTOM = 6,
};
//////////////////////////////////////////////////////
/// AV_AUDIO_SERVICE_TYPE_*
public enum class AVAudioServiceType
{
    MAIN = 0,
    EFFECTS = 1,
    VISUALLY_IMPAIRED = 2,
    HEARING_IMPAIRED = 3,
    DIALOGUE = 4,
    COMMENTARY = 5,
    EMERGENCY = 6,
    VOICE_OVER = 7,
    KARAOKE = 8,
};
//////////////////////////////////////////////////////
[Flags]
public enum class FFLoss : int
{
	NONE = 0x0000,
	RESOLUTION = 0x0001, ///<summary>< loss due to resolution change ///</summary>
	DEPTH = 0x0002, ///<summary>< loss due to color depth change ///</summary>
	COLORSPACE = 0x0004, ///<summary>< loss due to color space conversion ///</summary>
	ALPHA = 0x0008, ///<summary>< loss of alpha bits ///</summary>
	COLORQUANT = 0x0010, ///<summary>< loss due to color quantization ///</summary>
	CHROMA = 0x0020, ///<summary>< loss of chroma (e.g. RGB to gray conversion) ///</summary>
};
//////////////////////////////////////////////////////
// AV_SIDE_DATA_PARAM_CHANGE_*
[Flags]
public enum class AVSideDataParamChangeFlags
{
    CHANNEL_COUNT = 0x0001,
    CHANNEL_LAYOUT = 0x0002,
    SAMPLE_RATE = 0x0004,
    DIMENSIONS = 0x0008,
};
//////////////////////////////////////////////////////
///<summary>
/// Pixel format.
///
/// @note
/// PIX_FMT_RGB32 is handled in an endian-specific manner. An RGBA
/// color is put together as:
///  (A << 24) | (R << 16) | (G << 8) | B
/// This is stored as BGRA on little-endian CPU architectures and ARGB on
/// big-endian CPUs.
///
/// @par
/// When the pixel format is palettized RGB (PIX_FMT_PAL8), the palettized
/// image data is stored in AVFrame.data[0]. The palette is transported in
/// AVFrame.data[1], is 1024 bytes long (256 4-byte entries) and is
/// formatted the same as in PIX_FMT_RGB32 described above (i.e., it is
/// also endian-specific). Note also that the individual RGB palette
/// components stored in AVFrame.data[1] should be in the range 0..255.
/// This is important as many custom PAL8 video codecs that were designed
/// to run on the IBM VGA graphics adapter use 6-bit palette components.
///
/// @par
/// For all the 8bit per pixel formats, an RGB32 palette is in data[1] like
/// for pal8. This palette is filled in automatically by the function
/// allocating the picture.
///
/// @note
/// Make sure that all newly added big-endian formats have pix_fmt & 1 == 1
/// and that all newly added little-endian formats have pix_fmt & 1 == 0.
/// This allows simpler detection of big vs little-endian.
///</summary>
/// AV_PIX_FMT_*
public value class AVPixelFormat
{
public:
	static const AVPixelFormat NONE = AV_PIX_FMT_NONE;
	///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
	static const AVPixelFormat YUV420P = AV_PIX_FMT_YUV420P;
	///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
	static const AVPixelFormat YUYV422 = AV_PIX_FMT_YUYV422;
	///< packed RGB 8:8:8, 24bpp, RGBRGB...
	static const AVPixelFormat RGB24 = AV_PIX_FMT_RGB24;
	///< packed RGB 8:8:8, 24bpp, BGRBGR...
	static const AVPixelFormat BGR24 = AV_PIX_FMT_BGR24;
	///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
	static const AVPixelFormat YUV422P = AV_PIX_FMT_YUV422P;
	///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
	static const AVPixelFormat YUV444P = AV_PIX_FMT_YUV444P;
	///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
	static const AVPixelFormat YUV410P = AV_PIX_FMT_YUV410P;
	///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
	static const AVPixelFormat YUV411P = AV_PIX_FMT_YUV411P;
	///<        Y        ,  8bpp
	static const AVPixelFormat GRAY8 = AV_PIX_FMT_GRAY8;
	///<        Y        ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
	static const AVPixelFormat MONOWHITE = AV_PIX_FMT_MONOWHITE;
	///<        Y        ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
	static const AVPixelFormat MONOBLACK = AV_PIX_FMT_MONOBLACK;
	///< 8 bit with RGB32 palette
	static const AVPixelFormat PAL8 = AV_PIX_FMT_PAL8;
	///< planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of YUV420P and setting color_range
	static const AVPixelFormat YUVJ420P = AV_PIX_FMT_YUVJ420P;
	///< planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of YUV422P and setting color_range
	static const AVPixelFormat YUVJ422P = AV_PIX_FMT_YUVJ422P;
	///< planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of YUV444P and setting color_range
	static const AVPixelFormat YUVJ444P = AV_PIX_FMT_YUVJ444P;
	///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
	static const AVPixelFormat UYVY422 = AV_PIX_FMT_UYVY422;
	///< packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
	static const AVPixelFormat UYYVYY411 = AV_PIX_FMT_UYYVYY411;
	static const AVPixelFormat BGR8 = AV_PIX_FMT_BGR8;      ///< packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
	static const AVPixelFormat BGR4 = AV_PIX_FMT_BGR4;      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
	static const AVPixelFormat BGR4_BYTE = AV_PIX_FMT_BGR4_BYTE; ///< packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
	static const AVPixelFormat RGB8 = AV_PIX_FMT_RGB8;      ///< packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
	static const AVPixelFormat RGB4 = AV_PIX_FMT_RGB4;      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
	static const AVPixelFormat RGB4_BYTE = AV_PIX_FMT_RGB4_BYTE; ///< packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)
	static const AVPixelFormat NV12 = AV_PIX_FMT_NV12;      ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
	static const AVPixelFormat NV21 = AV_PIX_FMT_NV21;      ///< as above, but U and V bytes are swapped

	static const AVPixelFormat ARGB = AV_PIX_FMT_ARGB;      ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
	static const AVPixelFormat RGBA = AV_PIX_FMT_RGBA;      ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
	static const AVPixelFormat ABGR = AV_PIX_FMT_ABGR;      ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
	static const AVPixelFormat BGRA = AV_PIX_FMT_BGRA;      ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...

	static const AVPixelFormat GRAY16BE = AV_PIX_FMT_GRAY16BE;  ///<        Y        , 16bpp, big-endian
	static const AVPixelFormat GRAY16LE = AV_PIX_FMT_GRAY16LE;  ///<        Y        , 16bpp, little-endian
	static const AVPixelFormat YUV440P = AV_PIX_FMT_YUV440P;   ///< planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
	static const AVPixelFormat YUVJ440P = AV_PIX_FMT_YUVJ440P;  ///< planar YUV 4:4:0 full scale (JPEG), deprecated in favor of YUV440P and setting color_range
	static const AVPixelFormat YUVA420P = AV_PIX_FMT_YUVA420P;  ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
	static const AVPixelFormat RGB48BE = AV_PIX_FMT_RGB48BE;   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian
	static const AVPixelFormat RGB48LE = AV_PIX_FMT_RGB48LE;   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian

	static const AVPixelFormat RGB565BE = AV_PIX_FMT_RGB565BE;  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
	static const AVPixelFormat RGB565LE = AV_PIX_FMT_RGB565LE;  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
	static const AVPixelFormat RGB555BE = AV_PIX_FMT_RGB555BE;  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), big-endian   , X=unused/undefined
	static const AVPixelFormat RGB555LE = AV_PIX_FMT_RGB555LE;  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), little-endian, X=unused/undefined

	static const AVPixelFormat BGR565BE = AV_PIX_FMT_BGR565BE;  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
	static const AVPixelFormat BGR565LE = AV_PIX_FMT_BGR565LE;  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
	static const AVPixelFormat BGR555BE = AV_PIX_FMT_BGR555BE;  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), big-endian   , X=unused/undefined
	static const AVPixelFormat BGR555LE = AV_PIX_FMT_BGR555LE;  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), little-endian, X=unused/undefined
#if FF_API_VAAPI
	static const AVPixelFormat VAAPI_MOCO = AV_PIX_FMT_VAAPI_MOCO; ///< HW acceleration through VA API at motion compensation entry-point, Picture.data[3] contains a vaapi_render_state struct which contains macroblocks as well as various fields extracted from headers
	static const AVPixelFormat VAAPI_IDCT = AV_PIX_FMT_VAAPI_IDCT; ///< HW acceleration through VA API at IDCT entry-point, Picture.data[3] contains a vaapi_render_state struct which contains fields extracted from headers
	static const AVPixelFormat VAAPI_VLD = AV_PIX_FMT_VAAPI_VLD;  ///< HW decoding through VA API, Picture.data[3] contains a vaapi_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
#else
	/**
	*  Hardware acceleration through VA-API, data[3] contains a
	*  VASurfaceID.
	*/
	static const AVPixelFormat VAAPI = AV_PIX_FMT_VAAPI;
#endif
	static const AVPixelFormat YUV420P16LE = AV_PIX_FMT_YUV420P16LE;  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	static const AVPixelFormat YUV420P16BE = AV_PIX_FMT_YUV420P16BE;  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	static const AVPixelFormat YUV422P16LE = AV_PIX_FMT_YUV422P16LE;  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	static const AVPixelFormat YUV422P16BE = AV_PIX_FMT_YUV422P16BE;  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	static const AVPixelFormat YUV444P16LE = AV_PIX_FMT_YUV444P16LE;  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	static const AVPixelFormat YUV444P16BE = AV_PIX_FMT_YUV444P16BE;  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	static const AVPixelFormat DXVA2_VLD = AV_PIX_FMT_DXVA2_VLD;    ///< HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer

	static const AVPixelFormat RGB444LE = AV_PIX_FMT_RGB444LE;  ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), little-endian, X=unused/undefined
	static const AVPixelFormat RGB444BE = AV_PIX_FMT_RGB444BE;  ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), big-endian,    X=unused/undefined
	static const AVPixelFormat BGR444LE = AV_PIX_FMT_BGR444LE;  ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), little-endian, X=unused/undefined
	static const AVPixelFormat BGR444BE = AV_PIX_FMT_BGR444BE;  ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), big-endian,    X=unused/undefined
	static const AVPixelFormat YA8 = AV_PIX_FMT_YA8;       ///< 8bit gray, 8bit alpha

	static const AVPixelFormat Y400A = YA8; ///< alias for YA8
	static const AVPixelFormat GRAY8A = YA8; ///< alias for YA8

	static const AVPixelFormat BGR48BE = AV_PIX_FMT_BGR48BE;   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian
	static const AVPixelFormat BGR48LE = AV_PIX_FMT_BGR48LE;   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian

	/**
	* The following 12 formats have the disadvantage of needing 1 format for each bit depth.
	* Notice that each 9/10 bits sample is stored in 16 bits with extra padding.
	* If you want to support multiple bit depths, then using YUV420P16* with the bpp stored separately is better.
	*/
	static const AVPixelFormat YUV420P9BE = AV_PIX_FMT_YUV420P9BE; ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	static const AVPixelFormat YUV420P9LE = AV_PIX_FMT_YUV420P9LE; ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	static const AVPixelFormat YUV420P10BE = AV_PIX_FMT_YUV420P10BE;///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	static const AVPixelFormat YUV420P10LE = AV_PIX_FMT_YUV420P10LE;///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	static const AVPixelFormat YUV422P10BE = AV_PIX_FMT_YUV422P10BE;///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	static const AVPixelFormat YUV422P10LE = AV_PIX_FMT_YUV422P10LE;///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	static const AVPixelFormat YUV444P9BE = AV_PIX_FMT_YUV444P9BE; ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	static const AVPixelFormat YUV444P9LE = AV_PIX_FMT_YUV444P9LE; ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	static const AVPixelFormat YUV444P10BE = AV_PIX_FMT_YUV444P10BE;///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	static const AVPixelFormat YUV444P10LE = AV_PIX_FMT_YUV444P10LE;///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	static const AVPixelFormat YUV422P9BE = AV_PIX_FMT_YUV422P9BE; ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	static const AVPixelFormat YUV422P9LE = AV_PIX_FMT_YUV422P9LE; ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	static const AVPixelFormat GBRP = AV_PIX_FMT_GBRP;      ///< planar GBR 4:4:4 24bpp
	static const AVPixelFormat GBR24P = GBRP;      ///< alias for AV_PIX_FMT_GBRP
	static const AVPixelFormat GBRP9BE = AV_PIX_FMT_GBRP9BE;   ///< planar GBR 4:4:4 27bpp, big-endian
	static const AVPixelFormat GBRP9LE = AV_PIX_FMT_GBRP9LE;   ///< planar GBR 4:4:4 27bpp, little-endian
	static const AVPixelFormat GBRP10BE = AV_PIX_FMT_GBRP10BE;  ///< planar GBR 4:4:4 30bpp, big-endian
	static const AVPixelFormat GBRP10LE = AV_PIX_FMT_GBRP10LE;  ///< planar GBR 4:4:4 30bpp, little-endian
	static const AVPixelFormat GBRP16BE = AV_PIX_FMT_GBRP16BE;  ///< planar GBR 4:4:4 48bpp, big-endian
	static const AVPixelFormat GBRP16LE = AV_PIX_FMT_GBRP16LE;  ///< planar GBR 4:4:4 48bpp, little-endian

	static const AVPixelFormat YUVA420P9BE = AV_PIX_FMT_YUVA420P9BE;  ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian
	static const AVPixelFormat YUVA420P9LE = AV_PIX_FMT_YUVA420P9LE;  ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian
	static const AVPixelFormat YUVA422P9BE = AV_PIX_FMT_YUVA422P9BE;  ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian
	static const AVPixelFormat YUVA422P9LE = AV_PIX_FMT_YUVA422P9LE;  ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian
	static const AVPixelFormat YUVA444P9BE = AV_PIX_FMT_YUVA444P9BE;  ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian
	static const AVPixelFormat YUVA444P9LE = AV_PIX_FMT_YUVA444P9LE;  ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
	static const AVPixelFormat YUVA420P10BE = AV_PIX_FMT_YUVA420P10BE; ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
	static const AVPixelFormat YUVA420P10LE = AV_PIX_FMT_YUVA420P10LE; ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
	static const AVPixelFormat YUVA422P10BE = AV_PIX_FMT_YUVA422P10BE; ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
	static const AVPixelFormat YUVA422P10LE = AV_PIX_FMT_YUVA422P10LE; ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
	static const AVPixelFormat YUVA444P10BE = AV_PIX_FMT_YUVA444P10BE; ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
	static const AVPixelFormat YUVA444P10LE = AV_PIX_FMT_YUVA444P10LE; ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)
	static const AVPixelFormat YUVA420P16BE = AV_PIX_FMT_YUVA420P16BE; ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
	static const AVPixelFormat YUVA420P16LE = AV_PIX_FMT_YUVA420P16LE; ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
	static const AVPixelFormat YUVA422P16BE = AV_PIX_FMT_YUVA422P16BE; ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
	static const AVPixelFormat YUVA422P16LE = AV_PIX_FMT_YUVA422P16LE; ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
	static const AVPixelFormat YUVA444P16BE = AV_PIX_FMT_YUVA444P16BE; ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
	static const AVPixelFormat YUVA444P16LE = AV_PIX_FMT_YUVA444P16LE; ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)

	static const AVPixelFormat VDPAU = AV_PIX_FMT_VDPAU;     ///< HW acceleration through VDPAU, Picture.data[3] contains a VdpVideoSurface

	static const AVPixelFormat XYZ12LE = AV_PIX_FMT_XYZ12LE;      ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as little-endian, the 4 lower bits are set to 0
	static const AVPixelFormat XYZ12BE = AV_PIX_FMT_XYZ12BE;      ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as big-endian, the 4 lower bits are set to 0
	static const AVPixelFormat NV16 = AV_PIX_FMT_NV16;         ///< interleaved chroma YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
	static const AVPixelFormat NV20LE = AV_PIX_FMT_NV20LE;       ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	static const AVPixelFormat NV20BE = AV_PIX_FMT_NV20BE;       ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian

	static const AVPixelFormat RGBA64BE = AV_PIX_FMT_RGBA64BE;     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
	static const AVPixelFormat RGBA64LE = AV_PIX_FMT_RGBA64LE;     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
	static const AVPixelFormat BGRA64BE = AV_PIX_FMT_BGRA64BE;     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
	static const AVPixelFormat BGRA64LE = AV_PIX_FMT_BGRA64LE;     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian

	static const AVPixelFormat YVYU422 = AV_PIX_FMT_YVYU422;   ///< packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb

	static const AVPixelFormat YA16BE = AV_PIX_FMT_YA16BE;       ///< 16bit gray, 16bit alpha (big-endian)
	static const AVPixelFormat YA16LE = AV_PIX_FMT_YA16LE;       ///< 16bit gray, 16bit alpha (little-endian)

	static const AVPixelFormat GBRAP = AV_PIX_FMT_GBRAP;        ///< planar GBRA 4:4:4:4 32bpp
	static const AVPixelFormat GBRAP16BE = AV_PIX_FMT_GBRAP16BE;    ///< planar GBRA 4:4:4:4 64bpp, big-endian
	static const AVPixelFormat GBRAP16LE = AV_PIX_FMT_GBRAP16LE;    ///< planar GBRA 4:4:4:4 64bpp, little-endian
	/**
	*  HW acceleration through QSV, data[3] contains a pointer to the
	*  mfxFrameSurface1 structure.
	*/
	static const AVPixelFormat QSV = AV_PIX_FMT_QSV;
	/**
	* HW acceleration though MMAL, data[3] contains a pointer to the
	* MMAL_BUFFER_HEADER_T structure.
	*/
	static const AVPixelFormat MMAL = AV_PIX_FMT_MMAL;
	///< HW decoding through Direct3D11, Picture.data[3] contains a ID3D11VideoDecoderOutputView pointer
	static const AVPixelFormat D3D11VA_VLD = AV_PIX_FMT_D3D11VA_VLD;

	/**
	* HW acceleration through CUDA. data[i] contain CUdeviceptr pointers
	* exactly as for system memory frames.
	*/
	static const AVPixelFormat CUDA = AV_PIX_FMT_CUDA;

	static const AVPixelFormat ORGB = AV_PIX_FMT_0RGB;///< packed RGB 8:8:8, 32bpp, XRGBXRGB...   X=unused/undefined
	static const AVPixelFormat RGB0 = AV_PIX_FMT_RGB0;        ///< packed RGB 8:8:8, 32bpp, RGBXRGBX...   X=unused/undefined
	static const AVPixelFormat OBGR = AV_PIX_FMT_0BGR;        ///< packed BGR 8:8:8, 32bpp, XBGRXBGR...   X=unused/undefined
	static const AVPixelFormat BGR0 = AV_PIX_FMT_RGB0;        ///< packed BGR 8:8:8, 32bpp, BGRXBGRX...   X=unused/undefined

	static const AVPixelFormat YUVA444P = AV_PIX_FMT_YUVA444P;  ///< planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)
	static const AVPixelFormat YUVA422P = AV_PIX_FMT_YUVA422P;  ///< planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)

	static const AVPixelFormat YUV420P12BE = AV_PIX_FMT_YUV420P12BE; ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	static const AVPixelFormat YUV420P12LE = AV_PIX_FMT_YUV420P12LE; ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	static const AVPixelFormat YUV420P14BE = AV_PIX_FMT_YUV420P14BE; ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	static const AVPixelFormat YUV420P14LE = AV_PIX_FMT_YUV420P14LE; ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	static const AVPixelFormat YUV422P12BE = AV_PIX_FMT_YUV422P12BE; ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	static const AVPixelFormat YUV422P12LE = AV_PIX_FMT_YUV422P12LE; ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	static const AVPixelFormat YUV422P14BE = AV_PIX_FMT_YUV422P14BE; ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	static const AVPixelFormat YUV422P14LE = AV_PIX_FMT_YUV422P14LE; ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	static const AVPixelFormat YUV444P12BE = AV_PIX_FMT_YUV444P12BE; ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	static const AVPixelFormat YUV444P12LE = AV_PIX_FMT_YUV444P12LE; ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	static const AVPixelFormat YUV444P14BE = AV_PIX_FMT_YUV444P14BE; ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	static const AVPixelFormat YUV444P14LE = AV_PIX_FMT_YUV444P14LE; ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian

	static const AVPixelFormat GBRP12BE = AV_PIX_FMT_GBRP12BE;    ///< planar GBR 4:4:4 36bpp, big-endian
	static const AVPixelFormat GBRP12LE = AV_PIX_FMT_GBRP12LE;    ///< planar GBR 4:4:4 36bpp, little-endian
	static const AVPixelFormat GBRP14BE = AV_PIX_FMT_GBRP14BE;    ///< planar GBR 4:4:4 42bpp, big-endian
	static const AVPixelFormat GBRP14LE = AV_PIX_FMT_GBRP14LE;    ///< planar GBR 4:4:4 42bpp, little-endian
	static const AVPixelFormat YUVJ411P = AV_PIX_FMT_YUVJ411P;    ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) full scale (JPEG), deprecated in favor of YUV411P and setting color_range

	static const AVPixelFormat BAYER_BGGR8 = AV_PIX_FMT_BAYER_BGGR8;    ///< bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples */
	static const AVPixelFormat BAYER_RGGB8 = AV_PIX_FMT_BAYER_RGGB8;    ///< bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples */
	static const AVPixelFormat BAYER_GBRG8 = AV_PIX_FMT_BAYER_GBRG8;    ///< bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples */
	static const AVPixelFormat BAYER_GRBG8 = AV_PIX_FMT_BAYER_GRBG8;    ///< bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples */
	static const AVPixelFormat BAYER_BGGR16LE = AV_PIX_FMT_BAYER_BGGR16LE; ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, little-endian */
	static const AVPixelFormat BAYER_BGGR16BE = AV_PIX_FMT_BAYER_BGGR16BE; ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, big-endian */
	static const AVPixelFormat BAYER_RGGB16LE = AV_PIX_FMT_BAYER_RGGB16LE; ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, little-endian */
	static const AVPixelFormat BAYER_RGGB16BE = AV_PIX_FMT_BAYER_RGGB16BE; ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, big-endian */
	static const AVPixelFormat BAYER_GBRG16LE = AV_PIX_FMT_BAYER_GBRG16LE; ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, little-endian */
	static const AVPixelFormat BAYER_GBRG16BE = AV_PIX_FMT_BAYER_GBRG16BE; ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, big-endian */
	static const AVPixelFormat BAYER_GRBG16LE = AV_PIX_FMT_BAYER_GRBG16LE; ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, little-endian */
	static const AVPixelFormat BAYER_GRBG16BE = AV_PIX_FMT_BAYER_GRBG16BE; ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, big-endian */

	static const AVPixelFormat XVMC = AV_PIX_FMT_XVMC;///< XVideo Motion Acceleration via common packet passing

	static const AVPixelFormat YUV440P10LE = AV_PIX_FMT_YUV440P10LE; ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
	static const AVPixelFormat YUV440P10BE = AV_PIX_FMT_YUV440P10BE; ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
	static const AVPixelFormat YUV440P12LE = AV_PIX_FMT_YUV440P12LE; ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
	static const AVPixelFormat YUV440P12BE = AV_PIX_FMT_YUV440P12BE; ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian

	static const AVPixelFormat  AYUV64LE = AV_PIX_FMT_AYUV64LE;    ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
	static const AVPixelFormat  AYUV64BE = AV_PIX_FMT_AYUV64BE;    ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), big-endian

	static const AVPixelFormat  VIDEOTOOLBOX = AV_PIX_FMT_VIDEOTOOLBOX; ///< hardware decoding through Videotoolbox

	static const AVPixelFormat P010LE = AV_PIX_FMT_P010LE; ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, little-endian
	static const AVPixelFormat P010BE = AV_PIX_FMT_P010BE; ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, big-endian

	static const AVPixelFormat GBRAP12BE = AV_PIX_FMT_GBRAP12BE;  ///< planar GBR 4:4:4:4 48bpp, big-endian
	static const AVPixelFormat GBRAP12LE = AV_PIX_FMT_GBRAP12LE;  ///< planar GBR 4:4:4:4 48bpp, little-endian

	static const AVPixelFormat GBRAP10BE = AV_PIX_FMT_GBRAP10BE;  ///< planar GBR 4:4:4:4 40bpp, big-endian
	static const AVPixelFormat GBRAP10LE = AV_PIX_FMT_GBRAP10LE;  ///< planar GBR 4:4:4:4 40bpp, little-endian

	static const AVPixelFormat MEDIACODEC = AV_PIX_FMT_MEDIACODEC; ///< hardware decoding through MediaCodec

	static const AVPixelFormat GRAY12BE = AV_PIX_FMT_GRAY12BE;   ///<        Y        , 12bpp, big-endian
	static const AVPixelFormat GRAY12LE = AV_PIX_FMT_GRAY12LE;   ///<        Y        , 12bpp, little-endian
	static const AVPixelFormat GRAY10BE = AV_PIX_FMT_GRAY10BE;   ///<        Y        , 10bpp, big-endian
	static const AVPixelFormat GRAY10LE = AV_PIX_FMT_GRAY10LE;   ///<        Y        , 10bpp, little-endian

	static const AVPixelFormat P016LE = AV_PIX_FMT_P016LE; ///< like NV12, with 16bpp per component, little-endian
	static const AVPixelFormat P016BE = AV_PIX_FMT_P016BE; ///< like NV12, with 16bpp per component, big-endian

	/**
	* Hardware surfaces for Direct3D11.
	*
	* This is preferred over the legacy static const AVPixelFormat   = AV_PIX_FMT_D3D11VA_VLD. The new D3D11
	* hwaccel API and filtering support static const AVPixelFormat   = AV_PIX_FMT_D3D11 only.
	*
	* data[0] contains a ID3D11Texture2D pointer, and data[1] contains the
	* texture array index of the frame as intptr_t if the ID3D11Texture2D is
	* an array texture (or always 0 if it's a normal texture).
	*/
	static const AVPixelFormat D3D11 = AV_PIX_FMT_D3D11;

	static const AVPixelFormat GRAY9BE = AV_PIX_FMT_GRAY9BE;   ///<        Y        , 9bpp, big-endian
	static const AVPixelFormat GRAY9LE = AV_PIX_FMT_GRAY9LE;   ///<        Y        , 9bpp, little-endian

	static const AVPixelFormat GBRPF32BE = AV_PIX_FMT_GBRPF32BE;  ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, big-endian
	static const AVPixelFormat GBRPF32LE = AV_PIX_FMT_GBRPF32LE;  ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, little-endian
	static const AVPixelFormat GBRAPF32BE = AV_PIX_FMT_GBRAPF32BE; ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, big-endian
	static const AVPixelFormat GBRAPF32LE = AV_PIX_FMT_GBRAPF32LE; ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, little-endian

	/**
	* DRM-managed buffers exposed through PRIME buffer sharing.
	*
	* data[0] points to an AVDRMFrameDescriptor.
	*/
	static const AVPixelFormat DRM_PRIME = AV_PIX_FMT_DRM_PRIME;
	/**
	* Hardware surfaces for OpenCL.
	*
	* data[i] contain 2D image objects (typed in C as cl_mem, used
	* in OpenCL as image2d_t) for each plane of the surface.
	*/
	static const AVPixelFormat OPENCL = AV_PIX_FMT_OPENCL;

	static const AVPixelFormat GRAY14BE = AV_PIX_FMT_GRAY14BE;   ///<        Y        , 14bpp, big-endian
	static const AVPixelFormat GRAY14LE = AV_PIX_FMT_GRAY14LE;   ///<        Y        , 14bpp, little-endian

	static const AVPixelFormat GRAYF32BE = AV_PIX_FMT_GRAYF32BE;  ///< IEEE-754 single precision Y, 32bpp, big-endian
	static const AVPixelFormat GRAYF32LE = AV_PIX_FMT_GRAYF32LE;  ///< IEEE-754 single precision Y, 32bpp, little-endian

	static const AVPixelFormat YUVA422P12BE = AV_PIX_FMT_YUVA422P12BE; ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), 12b alpha, big-endian
	static const AVPixelFormat YUVA422P12LE = AV_PIX_FMT_YUVA422P12LE; ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), 12b alpha, little-endian
	static const AVPixelFormat YUVA444P12BE = AV_PIX_FMT_YUVA444P12BE; ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), 12b alpha, big-endian
	static const AVPixelFormat YUVA444P12LE = AV_PIX_FMT_YUVA444P12LE; ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), 12b alpha, little-endian

	static const AVPixelFormat NV24 = AV_PIX_FMT_NV24;      ///< planar YUV 4:4:4, 24bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
	static const AVPixelFormat NV42 = AV_PIX_FMT_NV42;      ///< as above, but U and V bytes are swapped

	static const AVPixelFormat RGB32 = AV_PIX_FMT_RGB32;
	static const AVPixelFormat RGB32_1 = AV_PIX_FMT_RGB32_1;
	static const AVPixelFormat BGR32 = AV_PIX_FMT_BGR32;
	static const AVPixelFormat BGR32_1 = AV_PIX_FMT_BGR32_1;
	static const AVPixelFormat ORGB32 = AV_PIX_FMT_0RGB32;
	static const AVPixelFormat OBGR32 = AV_PIX_FMT_0BGR32;

	static const AVPixelFormat GRAY9 = AV_PIX_FMT_GRAY9;
	static const AVPixelFormat GRAY10 = AV_PIX_FMT_GRAY10;
	static const AVPixelFormat GRAY12 = AV_PIX_FMT_GRAY12;
	static const AVPixelFormat GRAY14 = AV_PIX_FMT_GRAY14;
	static const AVPixelFormat GRAY16 = AV_PIX_FMT_GRAY16;
	static const AVPixelFormat YA16 = AV_PIX_FMT_YA16;
	static const AVPixelFormat RGB48 = AV_PIX_FMT_RGB48;
	static const AVPixelFormat RGB565 = AV_PIX_FMT_RGB565;
	static const AVPixelFormat RGB555 = AV_PIX_FMT_RGB555;
	static const AVPixelFormat RGB444 = AV_PIX_FMT_RGB444;
	static const AVPixelFormat RGBA64 = AV_PIX_FMT_RGBA64;
	static const AVPixelFormat BGR48 = AV_PIX_FMT_BGR48;
	static const AVPixelFormat BGR565 = AV_PIX_FMT_BGR565;
	static const AVPixelFormat BGR555 = AV_PIX_FMT_BGR555;
	static const AVPixelFormat BGR444 = AV_PIX_FMT_BGR444;
	static const AVPixelFormat BGRA64 = AV_PIX_FMT_BGRA64;

	static const AVPixelFormat YUV420P9 = AV_PIX_FMT_YUV420P9;
	static const AVPixelFormat YUV422P9 = AV_PIX_FMT_YUV422P9;
	static const AVPixelFormat YUV444P9 = AV_PIX_FMT_YUV444P9;
	static const AVPixelFormat YUV420P10 = AV_PIX_FMT_YUV420P10;
	static const AVPixelFormat YUV422P10 = AV_PIX_FMT_YUV422P10;
	static const AVPixelFormat YUV440P10 = AV_PIX_FMT_YUV440P10;
	static const AVPixelFormat YUV444P10 = AV_PIX_FMT_YUV444P10;
	static const AVPixelFormat YUV420P12 = AV_PIX_FMT_YUV420P12;
	static const AVPixelFormat YUV422P12 = AV_PIX_FMT_YUV422P12;
	static const AVPixelFormat YUV440P12 = AV_PIX_FMT_YUV440P12;
	static const AVPixelFormat YUV444P12 = AV_PIX_FMT_YUV444P12;
	static const AVPixelFormat YUV420P14 = AV_PIX_FMT_YUV420P14;
	static const AVPixelFormat YUV422P14 = AV_PIX_FMT_YUV422P14;
	static const AVPixelFormat YUV444P14 = AV_PIX_FMT_YUV444P14;
	static const AVPixelFormat YUV420P16 = AV_PIX_FMT_YUV420P16;
	static const AVPixelFormat YUV422P16 = AV_PIX_FMT_YUV422P16;
	static const AVPixelFormat YUV444P16 = AV_PIX_FMT_YUV444P16;

	static const AVPixelFormat GBRP9 = AV_PIX_FMT_GBRP9;
	static const AVPixelFormat GBRP10 = AV_PIX_FMT_GBRP10;
	static const AVPixelFormat GBRP12 = AV_PIX_FMT_GBRP12;
	static const AVPixelFormat GBRP14 = AV_PIX_FMT_GBRP14;
	static const AVPixelFormat GBRP16 = AV_PIX_FMT_GBRP16;
	static const AVPixelFormat GBRAP10 = AV_PIX_FMT_GBRAP10;
	static const AVPixelFormat GBRAP12 = AV_PIX_FMT_GBRAP12;
	static const AVPixelFormat GBRAP16 = AV_PIX_FMT_GBRAP16;

	static const AVPixelFormat BGGR16 = AV_PIX_FMT_BAYER_BGGR16;
	static const AVPixelFormat RGGB16 = AV_PIX_FMT_BAYER_RGGB16;
	static const AVPixelFormat GBRG16 = AV_PIX_FMT_BAYER_GBRG16;
	static const AVPixelFormat GRBG16 = AV_PIX_FMT_BAYER_GRBG16;

	static const AVPixelFormat GBRPF32 = AV_PIX_FMT_GBRPF32;
	static const AVPixelFormat GBRAPF32 = AV_PIX_FMT_GBRAPF32;

	static const AVPixelFormat GRAYF32 = AV_PIX_FMT_GRAYF32;

	static const AVPixelFormat YUVA420P9 = AV_PIX_FMT_YUVA420P9;
	static const AVPixelFormat YUVA422P9 = AV_PIX_FMT_YUVA422P9;
	static const AVPixelFormat YUVA444P9 = AV_PIX_FMT_YUVA444P9;
	static const AVPixelFormat YUVA420P10 = AV_PIX_FMT_YUVA420P10;
	static const AVPixelFormat YUVA422P10 = AV_PIX_FMT_YUVA422P10;
	static const AVPixelFormat YUVA444P10 = AV_PIX_FMT_YUVA444P10;
	static const AVPixelFormat YUVA422P12 = AV_PIX_FMT_YUVA422P12;
	static const AVPixelFormat YUVA444P12 = AV_PIX_FMT_YUVA444P12;
	static const AVPixelFormat YUVA420P16 = AV_PIX_FMT_YUVA420P16;
	static const AVPixelFormat YUVA422P16 = AV_PIX_FMT_YUVA422P16;
	static const AVPixelFormat YUVA444P16 = AV_PIX_FMT_YUVA444P16;

	static const AVPixelFormat XYZ12 = AV_PIX_FMT_XYZ12;
	static const AVPixelFormat NV20 = AV_PIX_FMT_NV20;
	static const AVPixelFormat AYUV64 = AV_PIX_FMT_AYUV64;
	static const AVPixelFormat P010 = AV_PIX_FMT_P010;
	static const AVPixelFormat P016 = AV_PIX_FMT_P016;

protected:
	/// Pixel Format
	int			m_nValue;
public:
	AVPixelFormat(int value);
	explicit AVPixelFormat(unsigned int value);
	explicit AVPixelFormat(__int64 value);
internal:
	AVPixelFormat(::AVPixelFormat value);
protected:
	//property int Value { int get() { return m_nValue; } }
public:
	/// Return the name of sample_fmt, or NULL if sample_fmt is not
	/// recognized.
	property String^ name { String^ get(); }
	///  Utility function to access log2_chroma_w log2_chroma_h from
	///  the pixel format AVPixFmtDescriptor.
	/// 
	///  This function asserts that pix_fmt is valid. See av_pix_fmt_get_chroma_sub_sample
	///  for one that returns a failure code and continues in case of invalid
	///  pix_fmts.
	property System::Drawing::Size^ chroma_sub_sample { System::Drawing::Size^ get(); }
	/// @return number of planes in pix_fmt, a negative AVERROR if pix_fmt is not a
	/// valid pixel format.
	property int planes { int get(); }
	/// Return a value representing the fourCC code associated to the
	/// pixel format pix_fmt, or 0 if no associated fourCC code can be
	/// found.
	property UInt32 codec_tag { UInt32 get(); };
	/**
	* @return a pixel format descriptor for provided pixel format or NULL if
	* this pixel format is unknown.
	*/
	property AVPixFmtDescriptor^ format { AVPixFmtDescriptor^ get(); }
public:
	static operator int(AVPixelFormat a) { return a.m_nValue; }
	static explicit operator __int64(AVPixelFormat a) { return a.m_nValue; }
	static explicit operator unsigned int(AVPixelFormat a) { return (unsigned int)a.m_nValue; }
	static operator AVPixelFormat(int a) { return AVPixelFormat(a); }
	static explicit operator AVPixelFormat(unsigned int a) { return AVPixelFormat((int)a); }
	static explicit operator AVPixelFormat(__int64 a) { return AVPixelFormat((int)a); }
internal:
	static operator ::AVPixelFormat(AVPixelFormat a) { return (::AVPixelFormat)a.m_nValue; }
	static operator AVPixelFormat(::AVPixelFormat a) { return AVPixelFormat((int)a); }
public:
	static bool operator == (AVPixelFormat a, AVPixelFormat b) { return a.m_nValue == b.m_nValue; }
	static bool operator != (AVPixelFormat a, AVPixelFormat b) { return a.m_nValue != b.m_nValue; }
public:
	/**
	* Print in buf the string corresponding to the pixel format with
	* number pix_fmt, or a header if pix_fmt is negative.
	*
	* @param buf the buffer where to write the string
	* @param buf_size the size of buf
	* @param pix_fmt the number of the pixel format to print the
	* corresponding info string, or a negative value to print the
	* corresponding header.
	*/
	virtual String^ ToString() override;
public:
	/// Utility function to swap the endianness of a pixel format.
	///
	/// @param[in]  pix_fmt the pixel format
	///
	/// @return pixel format with swapped endianness if it exists,
	/// otherwise AV_PIX_FMT_NONE
	AVPixelFormat swap_endianness();
public:
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

	/**
	* Return the pixel format corresponding to name.
	*
	* If there is no pixel format with name name, then looks for a
	* pixel format with the name corresponding to the native endian
	* format of name.
	* For example in a little-endian system, first looks for "gray16",
	* then for "gray16le".
	*
	* Finally if no pixel format has been found, returns AV_PIX_FMT_NONE.
	*/
	static AVPixelFormat get_pix_fmt(String^ name);
};
//////////////////////////////////////////////////////
// AV_FIELD_*
public enum class AVFieldOrder
{
    UNKNOWN,
    PROGRESSIVE,
    TT,          //< Top coded_first, top displayed first
    BB,          //< Bottom coded first, bottom displayed first
    TB,          //< Top coded first, bottom displayed first
    BT,          //< Bottom coded first, top displayed first
};
//////////////////////////////////////////////////////
/// AV_SAMPLE_FMT_*
public value class AVSampleFormat
{
public:
	static const AVSampleFormat NONE	= ::AV_SAMPLE_FMT_NONE;
	///< unsigned 8 bits
	static const AVSampleFormat U8 		= ::AV_SAMPLE_FMT_U8;
	///< signed 16 bits
	static const AVSampleFormat S16		= ::AV_SAMPLE_FMT_S16;
	///< signed 32 bits
	static const AVSampleFormat S32		= ::AV_SAMPLE_FMT_S32;
	///< float
	static const AVSampleFormat FLT		= ::AV_SAMPLE_FMT_FLT;
	///< double
	static const AVSampleFormat DBL		= ::AV_SAMPLE_FMT_DBL;
	///< unsigned 8 bits, planar
	static const AVSampleFormat U8P		= ::AV_SAMPLE_FMT_U8P;
	///< signed 16 bits, planar
	static const AVSampleFormat S16P	= ::AV_SAMPLE_FMT_S16P;
	///< signed 32 bits, planar
	static const AVSampleFormat S32P	= ::AV_SAMPLE_FMT_S32P;
	///< float, planar
	static const AVSampleFormat FLTP	= ::AV_SAMPLE_FMT_FLTP;
	///< double, planar
	static const AVSampleFormat DBLP	= ::AV_SAMPLE_FMT_DBLP;
protected:
	/// Sample Format
	int			m_nValue;
public:
	AVSampleFormat(int value);
	explicit AVSampleFormat(unsigned int value);
protected:
	//property int value { int get() { return m_nValue; } }
public:
	/// Return the name of sample_fmt, or NULL if sample_fmt is not
	/// recognized.
	property String^ name { String^ get() { return ToString(); } }
	/// Return number of bytes per sample.
	///
	/// @param sample_fmt the sample format
	/// @return number of bytes per sample or zero if unknown for the given
	/// sample format
	property int bytes_per_sample { int get(); }

	/// Check if the sample format is planar.
	///
	/// @param sample_fmt the sample format to inspect
	/// @return 1 if the sample format is planar, 0 if it is interleaved
	property bool is_planar { bool get(); }
public:
	static operator int(AVSampleFormat a) { return a.m_nValue; }
	static explicit operator unsigned int(AVSampleFormat a) { return (unsigned int)a.m_nValue; }
	static operator AVSampleFormat(int a) { return AVSampleFormat(a); }
	static explicit operator AVSampleFormat(unsigned int a) { return AVSampleFormat((int)a); }
internal:
	static operator ::AVSampleFormat(AVSampleFormat a) { return (::AVSampleFormat)a.m_nValue; }
	static operator AVSampleFormat(::AVSampleFormat a) { return AVSampleFormat((int)a); }
public:
	virtual String^ ToString() override;
public:
	/// Return the planar<->packed alternative form of the given sample format, or
	/// AV_SAMPLE_FMT_NONE on error. If the passed sample_fmt is already in the
	/// requested planar/packed format, the format returned is the same as the
	/// input.
	AVSampleFormat get_alt_sample_fmt(bool planar);
	
	/// Get the packed alternative form of the given sample format.
	///
	/// If the passed sample_fmt is already in packed format, the format returned is
	/// the same as the input.
	///
	/// @return  the packed alternative form of the given sample format or
	/// AV_SAMPLE_FMT_NONE on error.
	AVSampleFormat get_packed_sample_fmt();

	/// Get the planar alternative form of the given sample format.
	///
	/// If the passed sample_fmt is already in planar format, the format returned is
	/// the same as the input.
	///
	/// @return  the planar alternative form of the given sample format or
	/// AV_SAMPLE_FMT_NONE on error.
	AVSampleFormat get_planar_sample_fmt();
public:
	/// Return a sample format corresponding to name, or AV_SAMPLE_FMT_NONE
	/// on error.
	static AVSampleFormat get_sample_fmt(String^ name);
};
//////////////////////////////////////////////////////
public enum class FFQScaleType : int
{
    MPEG1 = 0,
    MPEG2 = 1,
    H264 = 2,
    VP56 = 3,
};
//////////////////////////////////////////////////////
public enum class FFBufferType : int
{
    INTERNAL = 1,
    USER = 2, ///< direct rendering buffers (image is (de)allocated by user)
    SHARED = 4, ///< Buffer from somewhere else, don't deallocate image (data/base), all other tables are not shared.
    COPY = 8, ///< Just a (modified) copy of some other buffer, don't deallocate anything.
};
//////////////////////////////////////////////////////
public enum class FFBufferHints : int
{
    VALID = 0x01, // Buffer hints value is meaningful (if 0 ignore).
    READABLE = 0x02, // Codec will read from buffer.
    PRESERVE = 0x04, // User must not alter buffer content.
    REUSABLE = 0x08, // Codec will reuse the buffer (update).
};
//////////////////////////////////////////////////////
//The following defines may change, don't expect compatibility if you use them.
public enum class MbType : int
{
    INTRA4x4 = 0x0001,
    INTRA16x16 = 0x0002, //FIXME H.264-specific
    INTRA_PCM = 0x0004, //FIXME H.264-specific
    MB_16x16 = 0x0008,
    MB_16x8 = 0x0010,
    MB_8x16 = 0x0020,
    MB_8x8 = 0x0040,
    INTERLACED = 0x0080,
    DIRECT2 = 0x0100, //FIXME
    ACPRED = 0x0200,
    GMC = 0x0400,
    SKIP = 0x0800,
    P0L0 = 0x1000,
    P1L0 = 0x2000,
    P0L1 = 0x4000,
    P1L1 = 0x8000,
    L0 = (P0L0 | P1L0),
    L1 = (P0L1 | P1L1),
    L0L1 = (L0 | L1),
    QUANT = 0x00010000,
    CBP = 0x00020000,
};
//////////////////////////////////////////////////////
///<summary>
/// codec capabilities 
///</summary>
// AV_CODEC_CAP_*
[Flags]
public enum class AVCodecCap : UInt32
{
	///< Decoder can use draw_horiz_band callback.
	DRAW_HORIZ_BAND = (1 <<  0), 
	///<summary>
	/// Codec uses get_buffer() for allocating buffers and supports custom allocators.
	/// If not set, it might not use get_buffer() at all or use operations that
	/// assume the buffer was allocated by avcodec_default_get_buffer.
	///</summary>
	DR1 = (1 <<  1),
	TRUNCATED = (1 <<  3),
	///<summary> Codec can export data for HW decoding (XvMC). ///</summary>
	HWACCEL = (1 <<  4),
	///<summary>
	/// Encoder or decoder requires flushing with NULL input at the end in order to
	/// give the complete and correct output.
	///
	/// NOTE: If this flag is not set, the codec is guaranteed to never be fed with
	///       with NULL data. The user can still send NULL data to the public encode
	///       or decode function, but libavcodec will not pass it along to the codec
	///       unless this flag is set.
	///
	/// Decoders:
	/// The decoder has a non-zero delay and needs to be fed with avpkt->data=NULL,
	/// avpkt->size=0 at the end to get the delayed data until the decoder no longer
	/// returns frames.
	///
	/// Encoders:
	/// The encoder needs to be fed with NULL data at the end of encoding until the
	/// encoder no longer returns data.
	///
	/// NOTE: For encoders implementing the AVCodec.encode2() function, setting this
	///       flag also means that the encoder must set the pts and duration for
	///       each output packet. If this flag is not set, the pts and duration will
	///       be determined by libavcodec from the input frame.
	///</summary>
	DELAY = (1 <<  5),
	///<summary>
	/// Codec can be fed a final frame with a smaller size.
	/// This can be used to prevent truncation of the last audio samples.
	///</summary>
	SMALL_LAST_FRAME = (1 <<  6),
	///<summary>
	/// Codec can export data for HW decoding (VDPAU).
	///</summary>
	HWACCEL_VDPAU = 0x0080,
	///<summary>
	/// Codec can output multiple frames per AVPacket
	/// Normally demuxers return one frame at a time, demuxers which do not do
	/// are connected to a parser to split what they return into proper frames.
	/// This flag is reserved to the very rare category of codecs which have a
	/// bitstream that cannot be split into frames without timeconsuming
	/// operations like full decoding. Demuxers carring such bitstreams thus
	/// may return multiple frames in a packet. This has many disadvantages like
	/// prohibiting stream copy in many cases thus it should only be considered
	/// as a last resort.
	///</summary>
	SUBFRAMES = (1 <<  8),
	///<summary>
	/// Codec is experimental and is thus avoided in favor of non experimental
	/// encoders
	///</summary>
	EXPERIMENTAL = (1 <<  9),
	///<summary>
	/// Codec should fill in channel configuration and samplerate instead of container
	///</summary>
	CHANNEL_CONF = (1 << 10),
	///<summary>
	/// Codec is able to deal with negative linesizes
	///</summary>
	NEG_LINESIZES = (1 << 11),
	///<summary>
	/// Codec supports frame-level multithreading.
	///</summary>
	FRAME_THREADS = (1 << 12),
	///<summary>
	/// Codec supports slice-based (or partition-based) multithreading.
	///</summary>
	SLICE_THREADS = (1 << 13),
	///<summary>
	/// Codec supports changed parameters at any point.
	///</summary>
	PARAM_CHANGE = (1 << 14),
	///<summary>
	/// Codec supports avctx->thread_count == 0 (auto).
	///</summary>
	AUTO_THREADS = (1 << 15),
	///<summary>
	/// Audio encoder supports receiving a different number of samples in each call.
	///</summary>
	VARIABLE_FRAME_SIZE = (1 << 16),
	///<summary>
	/// Decoder is not a preferred choice for probing.
	/// This indicates that the decoder is not a good choice for probing.
	/// It could for example be an expensive to spin up hardware decoder,
	/// or it could simply not provide a lot of useful information about
	/// the stream.
	/// A decoder marked with this flag should only be used as last resort
	/// choice for probing.
	///</summary>
	AVOID_PROBING = (1 << 17),
	///<summary>
	/// Codec is intra only.
	///</summary>
	INTRA_ONLY = 0x40000000,
	///<summary>
	/// Codec is lossless.
	///</summary>
	LOSSLESS = 0x80000000,
	/// Codec is backed by a hardware implementation. Typically used to
	/// identify a non-hwaccel hardware decoder. For information about hwaccels, use
	/// avcodec_get_hw_config() instead.
	HARDWARE           = (1 << 18),
	/// Codec is potentially backed by a hardware implementation, but not
	/// necessarily. This is used instead of AV_CODEC_CAP_HARDWARE, if the
	/// implementation provides some sort of internal fallback.
	HYBRID             = (1 << 19),
	/// This codec takes the reordered_opaque field from input AVFrames
	/// and returns it in the corresponding field in AVCodecContext after
	/// encoding.
	ENCODER_REORDERED_OPAQUE = (1 << 20),
};
//////////////////////////////////////////////////////
// AV_CH_*
public value class AVChannelLayout
{
public:
	static const AVChannelLayout UNKNOWN = 0;
	static const AVChannelLayout FRONT_LEFT = 0x00000001;
	static const AVChannelLayout FRONT_RIGHT = 0x00000002;
	static const AVChannelLayout FRONT_CENTER = 0x00000004;
	static const AVChannelLayout LOW_FREQUENCY = 0x00000008;
	static const AVChannelLayout BACK_LEFT = 0x00000010;
	static const AVChannelLayout BACK_RIGHT = 0x00000020;
	static const AVChannelLayout FRONT_LEFT_OF_CENTER = 0x00000040;
	static const AVChannelLayout FRONT_RIGHT_OF_CENTER = 0x00000080;
	static const AVChannelLayout BACK_CENTER = 0x00000100;
	static const AVChannelLayout SIDE_LEFT = 0x00000200;
	static const AVChannelLayout SIDE_RIGHT = 0x00000400;
	static const AVChannelLayout TOP_CENTER = 0x00000800;
	static const AVChannelLayout TOP_FRONT_LEFT = 0x00001000;
	static const AVChannelLayout TOP_FRONT_CENTER = 0x00002000;
	static const AVChannelLayout TOP_FRONT_RIGHT = 0x00004000;
	static const AVChannelLayout TOP_BACK_LEFT = 0x00008000;
	static const AVChannelLayout TOP_BACK_CENTER = 0x00010000;
	static const AVChannelLayout TOP_BACK_RIGHT = 0x00020000;
	static const AVChannelLayout STEREO_LEFT = 0x20000000; ///< Stereo downmix.
	static const AVChannelLayout STEREO_RIGHT = 0x40000000;  ///< See STEREO_LEFT.
	static const AVChannelLayout WIDE_LEFT = 0x0000000080000000;
	static const AVChannelLayout WIDE_RIGHT = 0x0000000100000000;
	static const AVChannelLayout SURROUND_DIRECT_LEFT = 0x0000000200000000;
	static const AVChannelLayout SURROUND_DIRECT_RIGHT = 0x0000000400000000;
	static const AVChannelLayout LOW_FREQUENCY_2 = 0x0000000800000000;

	// Channel mask value used for AVCodecContext.request_channel_layout
	// to indicate that the user requests the channel order of the decoder output
	// to be the native codec channel order. 
	static const AVChannelLayout LAYOUT_NATIVE = 0x8000000000000000;

	// defgroup channel_mask_c Audio channel convenience macros
	static const AVChannelLayout LAYOUT_MONO = (FRONT_CENTER);
	static const AVChannelLayout LAYOUT_STEREO = (FRONT_LEFT | FRONT_RIGHT);
	static const AVChannelLayout LAYOUT_2POINT1 = (LAYOUT_STEREO | LOW_FREQUENCY);
	static const AVChannelLayout LAYOUT_2_1 = (LAYOUT_STEREO | BACK_CENTER);
	static const AVChannelLayout LAYOUT_SURROUND = (LAYOUT_STEREO | FRONT_CENTER);
	static const AVChannelLayout LAYOUT_3POINT1 = (LAYOUT_SURROUND | LOW_FREQUENCY);
	static const AVChannelLayout LAYOUT_4POINT0 = (LAYOUT_SURROUND | BACK_CENTER);
	static const AVChannelLayout LAYOUT_4POINT1 = (LAYOUT_4POINT0 | LOW_FREQUENCY);
	static const AVChannelLayout LAYOUT_2_2 = (LAYOUT_STEREO | SIDE_LEFT | SIDE_RIGHT);
	static const AVChannelLayout LAYOUT_QUAD = (LAYOUT_STEREO | BACK_LEFT | BACK_RIGHT);
	static const AVChannelLayout LAYOUT_5POINT0 = (LAYOUT_SURROUND | SIDE_LEFT | SIDE_RIGHT);
	static const AVChannelLayout LAYOUT_5POINT1 = (LAYOUT_5POINT0 | LOW_FREQUENCY);
	static const AVChannelLayout LAYOUT_5POINT0_BACK = (LAYOUT_SURROUND | BACK_LEFT | BACK_RIGHT);
	static const AVChannelLayout LAYOUT_5POINT1_BACK = (LAYOUT_5POINT0_BACK | LOW_FREQUENCY);
	static const AVChannelLayout LAYOUT_6POINT0 = (LAYOUT_5POINT0 | BACK_CENTER);
	static const AVChannelLayout LAYOUT_6POINT0_FRONT = (LAYOUT_2_2 | FRONT_LEFT_OF_CENTER | FRONT_RIGHT_OF_CENTER);
	static const AVChannelLayout LAYOUT_HEXAGONAL = (LAYOUT_5POINT0_BACK | BACK_CENTER);
	static const AVChannelLayout LAYOUT_6POINT1 = (LAYOUT_5POINT1 | BACK_CENTER);
	static const AVChannelLayout LAYOUT_6POINT1_BACK = (LAYOUT_5POINT1_BACK | BACK_CENTER);
	static const AVChannelLayout LAYOUT_6POINT1_FRONT = (LAYOUT_6POINT0_FRONT | LOW_FREQUENCY);
	static const AVChannelLayout LAYOUT_7POINT0 = (LAYOUT_5POINT0 | BACK_LEFT | BACK_RIGHT);
	static const AVChannelLayout LAYOUT_7POINT0_FRONT = (LAYOUT_5POINT0 | FRONT_LEFT_OF_CENTER | FRONT_RIGHT_OF_CENTER);
	static const AVChannelLayout LAYOUT_7POINT1 = (LAYOUT_5POINT1 | BACK_LEFT | BACK_RIGHT);
	static const AVChannelLayout LAYOUT_7POINT1_WIDE = (LAYOUT_5POINT1 | FRONT_LEFT_OF_CENTER | FRONT_RIGHT_OF_CENTER);
	static const AVChannelLayout LAYOUT_7POINT1_WIDE_BACK = (LAYOUT_5POINT1_BACK | FRONT_LEFT_OF_CENTER | FRONT_RIGHT_OF_CENTER);
	static const AVChannelLayout LAYOUT_OCTAGONAL = (LAYOUT_5POINT0 | BACK_LEFT | BACK_CENTER | BACK_RIGHT);
	static const AVChannelLayout LAYOUT_STEREO_DOWNMIX = (STEREO_LEFT | STEREO_RIGHT);
protected:
	/// Layout Value
	UInt64			m_nValue;
public:
	AVChannelLayout(unsigned __int64 value);
	explicit AVChannelLayout(__int64 value);
protected:
	//property UInt64 Value { UInt64 get() { return m_nValue; } }
public:
	/// Return the number of channels in the channel layout.
	property int channels { int get(); }
	/// Get the name of a given channel.
	property String^ name { String^ get(); }
	/// Return a description of a channel layout.
	property String^ description { String^ get(); }
	/// Get the channel with the given index in channel_layout.
	property AVChannelLayout default[int]{ AVChannelLayout get(int); }
		/// Get the index of a channel in channel_layout.
	property int default[AVChannelLayout] { int get(AVChannelLayout); }
public:
	static operator UInt64(AVChannelLayout a) { return a.m_nValue; }
	static explicit operator Int64(AVChannelLayout a) { return (Int64)a.m_nValue; }
	static operator AVChannelLayout(UInt64 a) { return AVChannelLayout(a); }
	static explicit operator AVChannelLayout(Int64 a) { return AVChannelLayout((UInt64)a); }
	static explicit operator AVChannelLayout(int a) { return AVChannelLayout((Int64)a); }
public:
	virtual String^ ToString() override;
public:
	/// Get the index of a channel in channel_layout.
	/// 
	/// @param channel a channel layout describing exactly one channel which must be
	///                present in channel_layout.
	/// 
	/// @return index of channel in channel_layout on success, a negative AVERROR
	///         on error.
	int get_channel_index(AVChannelLayout channel);

	/// Get the channel with the given index in channel_layout.
	AVChannelLayout extract_channel(int index);
public:
	/// Return a channel layout id that matches name, or 0 if no match is found.
	/// 
	/// name can be one or several of the following notations,
	/// separated by '+' or '|':
	/// - the name of an usual channel layout (mono, stereo, 4.0, quad, 5.0,
	///   5.0(side), 5.1, 5.1(side), 7.1, 7.1(wide), downmix);
	/// - the name of a single channel (FL, FR, FC, LFE, BL, BR, FLC, FRC, BC,
	///   SL, SR, TC, TFL, TFC, TFR, TBL, TBC, TBR, DL, DR);
	/// - a number of channels, in decimal, optionally followed by 'c', yielding
	///   the default channel layout for that number of channels (@see
	///   av_get_default_channel_layout);
	/// - a channel layout mask, in hexadecimal starting with "0x" (see the
	///   AV_CH_* macros).
	/// 
	/// @warning Starting from the next major bump the trailing character
	/// 'c' to specify a number of channels will be required, while a
	/// channel layout mask could also be specified as a decimal number
	/// (if and only if not followed by "c").
	/// 
	/// Example: "stereo+FC" = "2c+FC" = "2c+1c" = "0x7"
	static AVChannelLayout get_channel_layout(String^ name);

	/// Return default channel layout for a given number of channels.
	static AVChannelLayout get_default_channel_layout(int nb_channels);

	/// Get the value and name of a standard channel layout.
	/// 
	/// @param[in]  index   index in an internal list, starting at 0
	/// @param[out] layout  channel layout mask
	/// @param[out] name    name of the layout
	/// @return  0  if the layout exists,
	///          <0 if index is beyond the limits
	static bool get_standard_channel_layout(UInt32 index, [Out] AVChannelLayout % layout, String^ % name);
};
//////////////////////////////////////////////////////
public enum class Profile : int
{
	UNKNOWN = -99,
	RESERVED = -100,

	AAC_MAIN = 0,
	AAC_LOW = 1,
	AAC_SSR = 2,
	AAC_LTP = 3,
	AAC_HE = 4,
	AAC_HE_V2 = 28,
	AAC_LD = 22,
	AAC_ELD = 38,
	MPEG2_AAC_LOW = 128,
	MPEG2_AAC_HE = 131,

	DNXHD         = 0,
	DNXHR_LB      = 1,
	DNXHR_SQ      = 2,
	DNXHR_HQ      = 3,
	DNXHR_HQX     = 4,
	DNXHR_444     = 5,

	DTS = 20,
	DTS_ES = 30,
	DTS_96_24 = 40,
	DTS_HD_HRA = 50,
	DTS_HD_MA = 60,
	DTS_EXPRESS = 70,

	MPEG2_422 = 0,
	MPEG2_HIGH = 1,
	MPEG2_SS = 2,
	MPEG2_SNR_SCALABLE = 3,
	MPEG2_MAIN = 4,
	MPEG2_SIMPLE = 5,

	H264_CONSTRAINED = (1 << 9),  // 8+1, constraint_set1_flag
	H264_INTRA = (1 << 11), // 8+3, constraint_set3_flag

	H264_BASELINE = 66,
	H264_CONSTRAINED_BASELINE = (66 | H264_CONSTRAINED),
	H264_MAIN = 77,
	H264_EXTENDED = 88,
	H264_HIGH = 100,
	H264_HIGH_10 = 110,
	H264_HIGH_10_INTRA = (110 | H264_INTRA),
	H264_MULTIVIEW_HIGH = 118,
	H264_HIGH_422 = 122,
	H264_HIGH_422_INTRA = (122 | H264_INTRA),
	H264_STEREO_HIGH = 128,
	H264_HIGH_444 = 144,
	H264_HIGH_444_PREDICTIVE = 244,
	H264_HIGH_444_INTRA = (244 | H264_INTRA),
	H264_CAVLC_444 = 44,

	VC1_SIMPLE = 0,
	VC1_MAIN = 1,
	VC1_COMPLEX = 2,
	VC1_ADVANCED = 3,

	MPEG4_SIMPLE = 0,
	MPEG4_SIMPLE_SCALABLE = 1,
	MPEG4_CORE = 2,
	MPEG4_MAIN = 3,
	MPEG4_N_BIT = 4,
	MPEG4_SCALABLE_TEXTURE = 5,
	MPEG4_SIMPLE_FACE_ANIMATION = 6,
	MPEG4_BASIC_ANIMATED_TEXTURE = 7,
	MPEG4_HYBRID = 8,
	MPEG4_ADVANCED_REAL_TIME = 9,
	MPEG4_CORE_SCALABLE = 10,
	MPEG4_ADVANCED_CODING = 11,
	MPEG4_ADVANCED_CORE = 12,
	MPEG4_ADVANCED_SCALABLE_TEXTURE = 13,
	MPEG4_SIMPLE_STUDIO = 14,
	MPEG4_ADVANCED_SIMPLE = 15,

	JPEG2000_CSTREAM_RESTRICTION_0 = 1,
	JPEG2000_CSTREAM_RESTRICTION_1 = 2,
	JPEG2000_CSTREAM_NO_RESTRICTION = 32768,
	JPEG2000_DCINEMA_2K = 3,
	JPEG2000_DCINEMA_4K = 4,

	VP9_0 = 0,
	VP9_1 = 1,
	VP9_2 = 2,
	VP9_3 = 3,

	HEVC_MAIN = 1,
	HEVC_MAIN_10 = 2,
	HEVC_MAIN_STILL_PICTURE = 3,
	HEVC_REXT = 4,

	AV1_MAIN = 0,
	AV1_HIGH = 1,
	AV1_PROFESSIONAL = 2,

	MJPEG_HUFFMAN_BASELINE_DCT            = 0xc0,
	MJPEG_HUFFMAN_EXTENDED_SEQUENTIAL_DCT = 0xc1,
	MJPEG_HUFFMAN_PROGRESSIVE_DCT         = 0xc2,
	MJPEG_HUFFMAN_LOSSLESS                = 0xc3,
	MJPEG_JPEG_LS                         = 0xf7,

	SBC_MSBC                        =  1,

	PRORES_PROXY     = 0,
	PRORES_LT        = 1,
	PRORES_STANDARD  = 2,
	PRORES_HQ        = 3,
	PRORES_4444      = 4,
	PRORES_XQ        = 5,

	ARIB_PROFILE_A = 0,
	ARIB_PROFILE_C = 1,
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVDictFlags : int
{
	NONE = 0,
	MATCH_CASE = 1,
	IGNORE_SUFFIX = 2,
	DONT_STRDUP_KEY = 4,   // < Take ownership of a key that's been allocated with av_malloc() and children. 
	DONT_STRDUP_VAL = 8,   // Take ownership of a value that's been allocated with av_malloc() and chilren. 
	DONT_OVERWRITE = 16,   ///< Don't overwrite existing entries.
	APPEND = 32,
};
//////////////////////////////////////////////////////
public enum class AVOptionType : UInt32
{
	FLAGS,
	INT,
	INT64,
	DOUBLE,
	FLOAT,
	STRING,
	RATIONAL,
	BINARY,  ///< offset must point to a pointer immediately followed by an int for the length
	DICT,
	OPT_CONST = 128,
	IMAGE_SIZE = MKBETAG('S', 'I', 'Z', 'E'), ///< offset must point to two consecutive integers
	PIXEL_FMT = MKBETAG('P', 'F', 'M', 'T'),
	SAMPLE_FMT = MKBETAG('S', 'F', 'M', 'T'),
	VIDEO_RATE = MKBETAG('V','R','A','T'), ///< offset must point to AVRational
	DURATION   = MKBETAG('D','U','R',' '),
	COLOR      = MKBETAG('C','O','L','R'),
	CHANNEL_LAYOUT = MKBETAG('C','H','L','A'),
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVOptFlag
{
	None            = 0,
	All				= 0xff,
	ENCODING_PARAM  = 1,   ///< a generic parameter which can be set by the user for muxing or encoding
	DECODING_PARAM  = 2,   ///< a generic parameter which can be set by the user for demuxing or decoding
	METADATA        = 4,   ///< some data extracted or inserted into the file like title, comment, ...
	AUDIO_PARAM     = 8,
	VIDEO_PARAM     = 16,
	SUBTITLE_PARAM  = 32,
	FILTERING_PARAM = (1<<16), ///< a generic parameter which can be set by the user for filtering
	EXPORT          = 64,
	READONLY        = 128,
};
//////////////////////////////////////////////////////
public enum class AVOptFlag2 : UInt32
{
    None            = 0,
    /**
    * Accept to parse a value without a key; the key will then be returned
    * as NULL.
    */
    IMPLICIT_KEY = 1,
};
//////////////////////////////////////////////////////
public enum class AVOptSearch
{
    None   = 0,

    CHILDREN   = 0x0001, /**< Search in possible children of the
                                                    given object first. */
    /**
        *  The obj passed to av_opt_find() is fake -- only a double pointer to AVClass
        *  instead of a required pointer to a struct containing AVClass. This is
        *  useful for searching for options without needing to allocate the corresponding
        *  object.
        */
    FAKE_OBJ   = 0x0002,
};
//////////////////////////////////////////////////////
public enum class FF_SUB_CHARENC_MODE
{
	///< do nothing (demuxer outputs a stream supposed to be already in UTF-8, or the codec is bitmap for instance)
	DO_NOTHING  = -1,
	///< libavcodec will select the mode itself
	AUTOMATIC   =  0,
	///< the AVPacket data needs to be recoded to UTF-8 before being fed to the decoder, requires iconv
	PRE_DECODER =  1,
};
//////////////////////////////////////////////////////
// FF_CODEC_PROPERTY_*
public enum class FF_CODEC_PROPERTY : unsigned 
{
	NONE = 0,
	LOSSLESS = 0x00000001,
	CLOSED_CAPTIONS = 0x00000002,
};
//////////////////////////////////////////////////////
// FF_SUB_TEXT_FMT_*
public enum class FF_SUB_TEXT_FMT : int
{
	ASS = 0,
	ASS_WITH_TIMINGS = 1,
};
//////////////////////////////////////////////////////
// AV_HWACCEL_FLAG_
[Flags]
public enum class AV_HWACCEL_FLAG : int
{
	NONE = 0,
	///<summary>
	/// Hardware acceleration should be used for decoding even if the codec level
	/// used is unknown or higher than the maximum supported level reported by the
	/// hardware driver.
	///
	/// It's generally a good idea to pass this flag unless you have a specific
	/// reason not to, as hardware tends to under-report supported levels.
	///</summary>
	IGNORE_LEVEL = (1 << 0),
	///<summary>
	/// Hardware acceleration can output YUV pixel formats with a different chroma
	/// sampling than 4:2:0 and/or other than 8 bits per component.
	///</summary>
	ALLOW_HIGH_DEPTH = (1 << 1),
	///<summary>
	/// Hardware acceleration should still be attempted for decoding when the
	/// codec profile does not match the reported capabilities of the hardware.
	///
	/// For example, this can be used to try to decode baseline profile H.264
	/// streams in hardware - it will often succeed, because many streams marked
	/// as baseline profile actually conform to constrained baseline profile.
	///
	/// @warning If the stream is actually not supported then the behaviour is
	///          undefined, and may include returning entirely incorrect output
	///          while indicating success.
	///</summary>
	ALLOW_PROFILE_MISMATCH = (1 << 2),
};
//////////////////////////////////////////////////////
// AV_HWACCEL_CODEC_CAP_*
[Flags]
public enum class AV_HWACCEL_CODEC_CAP : int 
{
	NONE = 0,
	/// HWAccel is experimental and is thus avoided in favor of non experimental
	/// codecs
	EXPERIMENTAL = 0x0200,
};
//////////////////////////////////////////////////////
// AV_CPU_FLAG
public enum class AV_CPU_FLAG : UInt32
{
	NONE		= 0x0000,
	FORCE		= 0x80000000, /// force usage of selected flags (OR)
	/// lower 16 bits - CPU features
	MMX			= 0x0001, ///< standard MMX
	MMXEXT		= 0x0002, ///< SSE integer functions or AMD MMX ext
	MMX2		= 0x0002, ///< SSE integer functions or AMD MMX ext
	f3DNOW		= 0x0004, ///< AMD 3DNOW
	SSE			= 0x0008, ///< SSE functions
	SSE2		= 0x0010, ///< PIV SSE2 functions
	SSE2SLOW	= 0x40000000, ///< SSE2 supported, but usually not faster
	///< than regular MMX/SSE (e.g. Core1)
	f3DNOWEXT   = 0x0020, ///< AMD 3DNowExt
	SSE3		= 0x0040, ///< Prescott SSE3 functions
	SSE3SLOW	= 0x20000000, ///< SSE3 supported, but usually not faster
	///< than regular MMX/SSE (e.g. Core1)
	SSSE3		= 0x0080, ///< Conroe SSSE3 functions
	ATOM		= 0x10000000, ///< Atom processor, some SSSE3 instructions are slower
	SSE4        = 0x0100, ///< Penryn SSE4.1 functions
	SSE42       = 0x0200, ///< Nehalem SSE4.2 functions
	AVX         = 0x4000, ///< AVX functions: requires OS support even if YMM registers aren't used
	AVXSLOW		= 0x8000000, ///< AVX supported, but slow when using YMM registers (e.g. Bulldozer)
	XOP         = 0x0400, ///< Bulldozer XOP functions
	FMA4        = 0x0800, ///< Bulldozer FMA4 functions
	CMOV        = 0x1000, ///< supports cmov instruction
	AVX2        = 0x8000, ///< AVX2 functions: requires OS support even if YMM registers aren't used
	FMA3        = 0x10000, ///< Haswell FMA3 functions
	BMI1        = 0x20000, ///< Bit Manipulation Instruction Set 1
	BMI2        = 0x40000, ///< Bit Manipulation Instruction Set 2
	ALTIVEC      = 0x0001, ///< standard
	VSX          = 0x0002, ///< ISA 2.06
	POWER8       = 0x0004, ///< ISA 2.07
	ARMV5TE      = (1 << 0),
	ARMV6        = (1 << 1),
	ARMV6T2      = (1 << 2),
	VFP          = (1 << 3),
	VFPV3        = (1 << 4),
	NEON         = (1 << 5),
	ARMV8        = (1 << 6),
	SETEND       = (1 <<16),
};
//////////////////////////////////////////////////////
// AVLogLevel
public enum class AVLogLevel : int
{
	// Print no output.
	Quiet = -8,
	//
	//Something went really wrong and we will crash now.
	//
	Panic = 0,

	//
	//Something went wrong and recovery is not possible.
	//For example, no header was found for a format which depends
	//on headers or an illegal combination of parameters is used.
	//
	Fatal = 8,

	//
	//Something went wrong and cannot losslessly be recovered.
	//However, not all future data is affected.
	//
	Error = 16,
	
	//
	//Something somehow does not look correct. This may or may not
	//lead to problems. An example would be the use of '-vstrict -2'.
	//
	Warning = 24,

	//
	//Standard information.
	//
	Info = 32,

	//
	//Detailed information.
	//
	Verbose = 40,

	//
	//Stuff which is only useful for libav//developers.
	//
	Debug   = 48,

	//
	//Extremely verbose debugging, useful for libav//development.
	//
	Trace    = 56,
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVLogFlags : int
{
	None = 0,
	// 
	 //  Skip repeated messages, this requires the user app to use av_log() instead of
	 //  (f)printf as the 2 would otherwise interfere and lead to
	 //  "Last message repeated x times" messages below (f)printf messages with some
	 //  bad luck.
	 //  Also to receive the last, "last repeated" line if any, the user app must
	 //  call av_log(NULL, AV_LOG_QUIET, "%s", ""); at the end
	 // 
	SKIP_REPEATED = 1,

	// 
	 //  Include the log severity in messages originating from codecs.
	 // 
	 //  Results in messages such as:
	 //  [rawvideo @ 0xDEADBEEF] [error] encode did not produce valid pts
	 // 
	PRINT_LEVEL = 2,
};
//////////////////////////////////////////////////////
public enum class AVCRCId : int
{
    AV_CRC_8_ATM,
    AV_CRC_16_ANSI,
    AV_CRC_16_CCITT,
    AV_CRC_32_IEEE,
	//< reversed bitorder version of AV_CRC_32_IEEE
    AV_CRC_32_IEEE_LE,
	//< reversed bitorder version of AV_CRC_16_ANSI
    AV_CRC_16_ANSI_LE,
    AV_CRC_24_IEEE = 12,
};
//////////////////////////////////////////////////////
// List of possible 3D Types
// AV_STEREO3D_*
public enum class AVStereo3DType : int 
{
	///<summary>
	 /// Video is not stereoscopic (and metadata has to be there).
	 ///</summary>
	_2D,

	///<summary>
	 /// Views are next to each other.
	 ///
	 ///    LLLLRRRR
	 ///    LLLLRRRR
	 ///    LLLLRRRR
	 ///    ...
	 ///</summary>
	SIDEBYSIDE,

	///<summary>
	 /// Views are on top of each other.
	 ///
	 ///    LLLLLLLL
	 ///    LLLLLLLL
	 ///    RRRRRRRR
	 ///    RRRRRRRR
	 ///</summary>
	TOPBOTTOM,

	///<summary>
	 /// Views are alternated temporally.
	 ///
	 ///     frame0   frame1   frame2   ...
	 ///    LLLLLLLL RRRRRRRR LLLLLLLL
	 ///    LLLLLLLL RRRRRRRR LLLLLLLL
	 ///    LLLLLLLL RRRRRRRR LLLLLLLL
	 ///    ...      ...      ...
	 ///</summary>
	FRAMESEQUENCE,

	///<summary>
	 /// Views are packed in a checkerboard-like structure per pixel.
	 ///
	 ///    LRLRLRLR
	 ///    RLRLRLRL
	 ///    LRLRLRLR
	 ///    ...
	 ///</summary>
	CHECKERBOARD,

	///<summary>
	 /// Views are next to each other, but when upscaling
	 /// apply a checkerboard pattern.
	 ///
	 ///     LLLLRRRR          L L L L    R R R R
	 ///     LLLLRRRR    =>     L L L L  R R R R
	 ///     LLLLRRRR          L L L L    R R R R
	 ///     LLLLRRRR           L L L L  R R R R
	 ///</summary>
	SIDEBYSIDE_QUINCUNX,

	///<summary>
	 /// Views are packed per line, as if interlaced.
	 ///
	 ///    LLLLLLLL
	 ///    RRRRRRRR
	 ///    LLLLLLLL
	 ///    ...
	 ///</summary>
	LINES,

	///<summary>
	 /// Views are packed per column.
	 ///
	 ///    LRLRLRLR
	 ///    LRLRLRLR
	 ///    LRLRLRLR
	 ///    ...
	 ///</summary>
	COLUMNS,
};
//////////////////////////////////////////////////////
public enum class AVStereo3DFlags
{
	None = 0,
	// Inverted views, Right/Bottom represents the left view.
	Invert  = (1 << 0),
};
//////////////////////////////////////////////////////
public enum class AVOptSerializeFlags : int
{
	None = 0,
	///< Serialize options that are not set to default values only.
	SKIP_DEFAULTS = 0x00000001,
	///< Serialize options that exactly match opt_flags only.
	OPT_FLAGS_EXACT = 0x00000002,
};

//////////////////////////////////////////////////////
//AV_CODEC_HW_CONFIG_METHOD_*
[Flags]
public enum class AV_CODEC_HW_CONFIG_METHOD : int
{
	///<summary>
	/// The codec supports this format via the hw_device_ctx interface.
	///
	/// When selecting this format, AVCodecContext.hw_device_ctx should
	/// have been set to a device of the specified type before calling
	/// avcodec_open2().
	///</summary>
	HW_DEVICE_CTX = 0x01,
	///<summary>
	/// The codec supports this format via the hw_frames_ctx interface.
	///
	/// When selecting this format for a decoder,
	/// AVCodecContext.hw_frames_ctx should be set to a suitable frames
	/// context inside the get_format() callback.  The frames context
	/// must have been created on a device of the specified type.
	///</summary>
	HW_FRAMES_CTX = 0x02,
	///<summary>
	/// The codec supports this format by some internal method.
	///
	/// This format can be selected without any additional configuration -
	/// no device or frames context is required.
	///</summary>
	INTERNAL      = 0x04,
	///<summary>
	/// The codec supports this format by some ad-hoc method.
	///
	/// Additional settings and/or function calls are required.  See the
	/// codec-specific documentation for details.  (Methods requiring
	/// this sort of configuration are deprecated and others should be
	/// used in preference.)
	///</summary>
	AD_HOC        = 0x08,
};
//////////////////////////////////////////////////////
public enum class AVHWDeviceType 
{
	NONE,
	VDPAU,
	CUDA,
	VAAPI,
	DXVA2,
	QSV,
	VIDEOTOOLBOX,
	D3D11VA,
	DRM,
	OPENCL,
	MEDIACODEC,
};
//////////////////////////////////////////////////////
public enum class RDFTransformType {
	DFT_R2C,
	IDFT_C2R,
	IDFT_R2C,
	DFT_C2R,
};
//////////////////////////////////////////////////////
public enum class DCTTransformType {
	DCT_II = 0,
	DCT_III,
	DCT_I,
	DST_I,
};
//////////////////////////////////////////////////////
#pragma endregion
//////////////////////////////////////////////////////
#pragma region Classes
//////////////////////////////////////////////////////
#ifdef _INC_STDIO
//////////////////////////////////////////////////////
#pragma push_macro("EOF")
//////////////////////////////////////////////////////
#undef EOF
//////////////////////////////////////////////////////
#endif // _INC_STDIO
//////////////////////////////////////////////////////
#ifdef _INC_ERRNO
//////////////////////////////////////////////////////
#pragma push_macro("EPERM")
#pragma push_macro("ENOENT")
#pragma push_macro("ESRCH")
#pragma push_macro("EINTR")
#pragma push_macro("EIO")
#pragma push_macro("ENXIO")
#pragma push_macro("E2BIG")
#pragma push_macro("ENOEXEC")
#pragma push_macro("EBADF")
#pragma push_macro("ECHILD")
#pragma push_macro("EAGAIN")
#pragma push_macro("ENOMEM")
#pragma push_macro("EACCES")
#pragma push_macro("EFAULT")
#pragma push_macro("EBUSY")
#pragma push_macro("EEXIST")
#pragma push_macro("EXDEV")
#pragma push_macro("ENODEV")
#pragma push_macro("ENOTDIR")
#pragma push_macro("EISDIR")
#pragma push_macro("ENFILE")
#pragma push_macro("EMFILE")
#pragma push_macro("ENOTTY")
#pragma push_macro("EFBIG")
#pragma push_macro("ENOSPC")
#pragma push_macro("ESPIPE")
#pragma push_macro("EROFS")
#pragma push_macro("EMLINK")
#pragma push_macro("EPIPE")
#pragma push_macro("EDOM")
#pragma push_macro("EDEADLK")
#pragma push_macro("ENAMETOOLONG")
#pragma push_macro("ENOLCK")
#pragma push_macro("ENOSYS")
#pragma push_macro("ENOTEMPTY")
#pragma push_macro("EINVAL")
#pragma push_macro("ERANGE")
#pragma push_macro("EILSEQ")
#pragma push_macro("STRUNCATE")
#pragma push_macro("EADDRINUSE")
#pragma push_macro("EADDRNOTAVAIL")
#pragma push_macro("EAFNOSUPPORT")
#pragma push_macro("EALREADY")
#pragma push_macro("EBADMSG")
#pragma push_macro("ECANCELED")
#pragma push_macro("ECONNABORTED")
#pragma push_macro("ECONNREFUSED")
#pragma push_macro("ECONNRESET")
#pragma push_macro("EDESTADDRREQ")
#pragma push_macro("EHOSTUNREACH")
#pragma push_macro("EIDRM")
#pragma push_macro("EINPROGRESS")
#pragma push_macro("EISCONN")
#pragma push_macro("ELOOP")
#pragma push_macro("EMSGSIZE")
#pragma push_macro("ENETDOWN")
#pragma push_macro("ENETRESET")
#pragma push_macro("ENETUNREACH")
#pragma push_macro("ENOBUFS")
#pragma push_macro("ENODATA")
#pragma push_macro("ENOLINK")
#pragma push_macro("ENOMSG")
#pragma push_macro("ENOPROTOOPT")
#pragma push_macro("ENOSR")
#pragma push_macro("ENOSTR")
#pragma push_macro("ENOTCONN")
#pragma push_macro("ENOTRECOVERABLE")
#pragma push_macro("ENOTSOCK")
#pragma push_macro("ENOTSUP")
#pragma push_macro("EOPNOTSUPP")
#pragma push_macro("EOTHER")
#pragma push_macro("EOVERFLOW")
#pragma push_macro("EOWNERDEAD")
#pragma push_macro("EPROTO")
#pragma push_macro("EPROTONOSUPPORT")
#pragma push_macro("EPROTOTYPE")
#pragma push_macro("ETIME")
#pragma push_macro("ETIMEDOUT")
#pragma push_macro("ETXTBSY")
#pragma push_macro("EWOULDBLOCK")
//////////////////////////////////////////////////////
#undef EPERM
#undef ENOENT
#undef ESRCH
#undef EINTR
#undef EIO
#undef ENXIO
#undef E2BIG
#undef ENOEXEC
#undef EBADF
#undef ECHILD
#undef EAGAIN
#undef ENOMEM
#undef EACCES
#undef EFAULT
#undef EBUSY
#undef EEXIST
#undef EXDEV
#undef ENODEV
#undef ENOTDIR
#undef EISDIR
#undef ENFILE
#undef EMFILE
#undef ENOTTY
#undef EFBIG
#undef ENOSPC
#undef ESPIPE
#undef EROFS
#undef EMLINK
#undef EPIPE
#undef EDOM
#undef EDEADLK
#undef ENAMETOOLONG
#undef ENOLCK
#undef ENOSYS
#undef ENOTEMPTY
#undef EINVAL
#undef ERANGE
#undef EILSEQ
#undef STRUNCATE
#undef EADDRINUSE
#undef EADDRNOTAVAIL
#undef EAFNOSUPPORT
#undef EALREADY
#undef EBADMSG
#undef ECANCELED
#undef ECONNABORTED
#undef ECONNREFUSED
#undef ECONNRESET
#undef EDESTADDRREQ
#undef EHOSTUNREACH
#undef EIDRM
#undef EINPROGRESS
#undef EISCONN
#undef ELOOP
#undef EMSGSIZE
#undef ENETDOWN
#undef ENETRESET
#undef ENETUNREACH
#undef ENOBUFS
#undef ENODATA
#undef ENOLINK
#undef ENOMSG
#undef ENOPROTOOPT
#undef ENOSR
#undef ENOSTR
#undef ENOTCONN
#undef ENOTRECOVERABLE
#undef ENOTSOCK
#undef ENOTSUP
#undef EOPNOTSUPP
#undef EOTHER
#undef EOVERFLOW
#undef EOWNERDEAD
#undef EPROTO
#undef EPROTONOSUPPORT
#undef EPROTOTYPE
#undef ETIME
#undef ETIMEDOUT
#undef ETXTBSY
#undef EWOULDBLOCK
//////////////////////////////////////////////////////
#endif // _INC_ERRNO
//////////////////////////////////////////////////////
// AVERROR
public value class AVRESULT: public IComparable, public ICloneable
{
public:
	static property AVRESULT  OK				{ AVRESULT get() { return AVRESULT(0); } }
	static property AVRESULT  EPERM				{ AVRESULT get() { return AVRESULT(-1); } }
	static property AVRESULT  ENOENT			{ AVRESULT get() { return AVRESULT(-2); } }
	static property AVRESULT  ESRCH				{ AVRESULT get() { return AVRESULT(-3); } }
	static property AVRESULT  EINTR				{ AVRESULT get() { return AVRESULT(-4); } }
	static property AVRESULT  EIO				{ AVRESULT get() { return AVRESULT(-5); } }
	static property AVRESULT  ENXIO				{ AVRESULT get() { return AVRESULT(-6); } }
	static property AVRESULT  E2BIG				{ AVRESULT get() { return AVRESULT(-7); } }
	static property AVRESULT  ENOEXEC			{ AVRESULT get() { return AVRESULT(-8); } }
	static property AVRESULT  EBADF				{ AVRESULT get() { return AVRESULT(-9); } }
	static property AVRESULT  ECHILD			{ AVRESULT get() { return AVRESULT(-10); } }
	static property AVRESULT  EAGAIN			{ AVRESULT get() { return AVRESULT(-11); } }
	static property AVRESULT  ENOMEM			{ AVRESULT get() { return AVRESULT(-12); } }
	static property AVRESULT  EACCES			{ AVRESULT get() { return AVRESULT(-13); } }
	static property AVRESULT  EFAULT			{ AVRESULT get() { return AVRESULT(-14); } }
	static property AVRESULT  EBUSY				{ AVRESULT get() { return AVRESULT(-16); } }
	static property AVRESULT  EEXIST			{ AVRESULT get() { return AVRESULT(-17); } }
	static property AVRESULT  EXDEV				{ AVRESULT get() { return AVRESULT(-18); } }
	static property AVRESULT  ENODEV			{ AVRESULT get() { return AVRESULT(-19); } }
	static property AVRESULT  ENOTDIR			{ AVRESULT get() { return AVRESULT(-20); } }
	static property AVRESULT  EISDIR			{ AVRESULT get() { return AVRESULT(-21); } }
	static property AVRESULT  ENFILE			{ AVRESULT get() { return AVRESULT(-23); } }
	static property AVRESULT  EMFILE			{ AVRESULT get() { return AVRESULT(-24); } }
	static property AVRESULT  ENOTTY			{ AVRESULT get() { return AVRESULT(-25); } }
	static property AVRESULT  EFBIG				{ AVRESULT get() { return AVRESULT(-27); } }
	static property AVRESULT  ENOSPC			{ AVRESULT get() { return AVRESULT(-28); } }
	static property AVRESULT  ESPIPE			{ AVRESULT get() { return AVRESULT(-29); } }
	static property AVRESULT  EROFS				{ AVRESULT get() { return AVRESULT(-30); } }
	static property AVRESULT  EMLINK			{ AVRESULT get() { return AVRESULT(-31); } }
	static property AVRESULT  EPIPE				{ AVRESULT get() { return AVRESULT(-32); } }
	static property AVRESULT  EDOM				{ AVRESULT get() { return AVRESULT(-33); } }
	static property AVRESULT  EDEADLK			{ AVRESULT get() { return AVRESULT(-36); } }
	static property AVRESULT  ENAMETOOLONG		{ AVRESULT get() { return AVRESULT(-38); } }
	static property AVRESULT  ENOLCK			{ AVRESULT get() { return AVRESULT(-39); } }
	static property AVRESULT  ENOSYS			{ AVRESULT get() { return AVRESULT(-40); } }
	static property AVRESULT  ENOTEMPTY			{ AVRESULT get() { return AVRESULT(-41); } }
	static property AVRESULT  EINVAL			{ AVRESULT get() { return AVRESULT(-22); } }
	static property AVRESULT  ERANGE			{ AVRESULT get() { return AVRESULT(-34); } }
	static property AVRESULT  EILSEQ			{ AVRESULT get() { return AVRESULT(-42); } }
	static property AVRESULT  STRUNCATE			{ AVRESULT get() { return AVRESULT(-80); } }
	static property AVRESULT  EADDRINUSE		{ AVRESULT get() { return AVRESULT(-100); } }
	static property AVRESULT  EADDRNOTAVAIL		{ AVRESULT get() { return AVRESULT(-101); } }
	static property AVRESULT  EAFNOSUPPORT		{ AVRESULT get() { return AVRESULT(-102); } }
	static property AVRESULT  EALREADY			{ AVRESULT get() { return AVRESULT(-103); } }
	static property AVRESULT  EBADMSG			{ AVRESULT get() { return AVRESULT(-104); } }
	static property AVRESULT  ECANCELED			{ AVRESULT get() { return AVRESULT(-105); } }
	static property AVRESULT  ECONNABORTED		{ AVRESULT get() { return AVRESULT(-106); } }
	static property AVRESULT  ECONNREFUSED		{ AVRESULT get() { return AVRESULT(-107); } }
	static property AVRESULT  ECONNRESET		{ AVRESULT get() { return AVRESULT(-108); } }
	static property AVRESULT  EDESTADDRREQ		{ AVRESULT get() { return AVRESULT(-109); } }
	static property AVRESULT  EHOSTUNREACH		{ AVRESULT get() { return AVRESULT(-110); } }
	static property AVRESULT  EIDRM				{ AVRESULT get() { return AVRESULT(-111); } }
	static property AVRESULT  EINPROGRESS		{ AVRESULT get() { return AVRESULT(-112); } }
	static property AVRESULT  EISCONN			{ AVRESULT get() { return AVRESULT(-113); } }
	static property AVRESULT  ELOOP				{ AVRESULT get() { return AVRESULT(-114); } }
	static property AVRESULT  EMSGSIZE			{ AVRESULT get() { return AVRESULT(-115); } }
	static property AVRESULT  ENETDOWN			{ AVRESULT get() { return AVRESULT(-116); } }
	static property AVRESULT  ENETRESET			{ AVRESULT get() { return AVRESULT(-117); } }
	static property AVRESULT  ENETUNREACH		{ AVRESULT get() { return AVRESULT(-118); } }
	static property AVRESULT  ENOBUFS			{ AVRESULT get() { return AVRESULT(-119); } }
	static property AVRESULT  ENODATA			{ AVRESULT get() { return AVRESULT(-120); } }
	static property AVRESULT  ENOLINK			{ AVRESULT get() { return AVRESULT(-121); } }
	static property AVRESULT  ENOMSG			{ AVRESULT get() { return AVRESULT(-122); } }
	static property AVRESULT  ENOPROTOOPT		{ AVRESULT get() { return AVRESULT(-123); } }
	static property AVRESULT  ENOSR				{ AVRESULT get() { return AVRESULT(-124); } }
	static property AVRESULT  ENOSTR			{ AVRESULT get() { return AVRESULT(-125); } }
	static property AVRESULT  ENOTCONN			{ AVRESULT get() { return AVRESULT(-126); } }
	static property AVRESULT  ENOTRECOVERABLE	{ AVRESULT get() { return AVRESULT(-127); } }
	static property AVRESULT  ENOTSOCK			{ AVRESULT get() { return AVRESULT(-128); } }
	static property AVRESULT  ENOTSUP			{ AVRESULT get() { return AVRESULT(-129); } }
	static property AVRESULT  EOPNOTSUPP		{ AVRESULT get() { return AVRESULT(-130); } }
	static property AVRESULT  EOTHER			{ AVRESULT get() { return AVRESULT(-131); } }
	static property AVRESULT  EOVERFLOW			{ AVRESULT get() { return AVRESULT(-132); } }
	static property AVRESULT  EOWNERDEAD		{ AVRESULT get() { return AVRESULT(-133); } }
	static property AVRESULT  EPROTO			{ AVRESULT get() { return AVRESULT(-134); } }
	static property AVRESULT  EPROTONOSUPPORT	{ AVRESULT get() { return AVRESULT(-135); } }
	static property AVRESULT  EPROTOTYPE		{ AVRESULT get() { return AVRESULT(-136); } }
	static property AVRESULT  ETIME				{ AVRESULT get() { return AVRESULT(-137); } }
	static property AVRESULT  ETIMEDOUT			{ AVRESULT get() { return AVRESULT(-138); } }
	static property AVRESULT  ETXTBSY			{ AVRESULT get() { return AVRESULT(-139); } }
	static property AVRESULT  EWOULDBLOCK		{ AVRESULT get() { return AVRESULT(-140); } }
	///< Bitstream filter not found
	static property AVRESULT BSF_NOT_FOUND		{ AVRESULT get() { return AVRESULT(AVERROR_BSF_NOT_FOUND); } }
	///< Internal bug, also see AVERROR_BUG2
	static property AVRESULT BUG				{ AVRESULT get() { return AVRESULT(AVERROR_BUG); } }
	///< Buffer too small
	static property AVRESULT BUFFER_TOO_SMALL	{ AVRESULT get() { return AVRESULT(AVERROR_BUFFER_TOO_SMALL);	} }
	///< Decoder not found
	static property AVRESULT DECODER_NOT_FOUND	{ AVRESULT get() { return AVRESULT(AVERROR_DECODER_NOT_FOUND);	} }
	///< Demuxer not found
	static property AVRESULT DEMUXER_NOT_FOUND	{ AVRESULT get() { return AVRESULT(AVERROR_DEMUXER_NOT_FOUND);	} }
	///< Encoder not found
	static property AVRESULT ENCODER_NOT_FOUND	{ AVRESULT get() { return AVRESULT(AVERROR_ENCODER_NOT_FOUND);	} }
	///< End of file
	static property AVRESULT EOF				{ AVRESULT get() { return AVRESULT(AVERROR_EOF);				} }
	///< Immediate exit was requested; the called function should not be restarted
	static property AVRESULT EXIT				{ AVRESULT get() { return AVRESULT(AVERROR_EXIT);				} }
	///< Generic error in an external library
	static property AVRESULT EXTERNAL			{ AVRESULT get() { return AVRESULT(AVERROR_EXTERNAL);			} }
	///< Filter not found
	static property AVRESULT FILTER_NOT_FOUND	{ AVRESULT get() { return AVRESULT(AVERROR_FILTER_NOT_FOUND);	} }
	///< Invalid data found when processing input
	static property AVRESULT INVALIDDATA		{ AVRESULT get() { return AVRESULT(AVERROR_INVALIDDATA);		} }
	///< Muxer not found
	static property AVRESULT MUXER_NOT_FOUND	{ AVRESULT get() { return AVRESULT(AVERROR_MUXER_NOT_FOUND);	} }
	///< Option not found
	static property AVRESULT OPTION_NOT_FOUND	{ AVRESULT get() { return AVRESULT(AVERROR_OPTION_NOT_FOUND);	} }
	///< Not yet implemented in FFmpeg, patches welcome
	static property AVRESULT PATCHWELCOME		{ AVRESULT get() { return AVRESULT(AVERROR_PATCHWELCOME);		} }
	///< Protocol not found
	static property AVRESULT PROTOCOL_NOT_FOUND	{ AVRESULT get() { return AVRESULT(AVERROR_PROTOCOL_NOT_FOUND); } }
	///< Stream not found
	static property AVRESULT STREAM_NOT_FOUND	{ AVRESULT get() { return AVRESULT(AVERROR_STREAM_NOT_FOUND);	} }
	/// This is semantically identical to AVERROR_BUG
	/// it has been introduced in Libav after our AVERROR_BUG and with a modified value.
	static property AVRESULT BUG2				{ AVRESULT get() { return AVRESULT(AVERROR_BUG2);				} }
	///< Unknown error, typically from an external library
	static property AVRESULT UNKNOWN			{ AVRESULT get() { return AVRESULT(AVERROR_UNKNOWN);			} }
	///< Requested feature is flagged experimental. Set strict_std_compliance if you really want to use it.
	static property AVRESULT EXPERIMENTAL		{ AVRESULT get() { return AVRESULT(AVERROR_EXPERIMENTAL);		} }
	///< Input changed between calls. Reconfiguration is required. (can be OR-ed with AVERROR_OUTPUT_CHANGED)
	static property AVRESULT INPUT_CHANGED		{ AVRESULT get() { return AVRESULT(AVERROR_INPUT_CHANGED);		} }
	///< Output changed between calls. Reconfiguration is required. (can be OR-ed with AVERROR_INPUT_CHANGED)
	static property AVRESULT OUTPUT_CHANGED		{ AVRESULT get() { return AVRESULT(AVERROR_OUTPUT_CHANGED);		} }
	// HTTP & RTSP errors
	static property AVRESULT HTTP_BAD_REQUEST	{ AVRESULT get() { return AVRESULT(AVERROR_HTTP_BAD_REQUEST);	} }
	static property AVRESULT HTTP_UNAUTHORIZED	{ AVRESULT get() { return AVRESULT(AVERROR_HTTP_UNAUTHORIZED);	} }
	static property AVRESULT HTTP_FORBIDDEN		{ AVRESULT get() { return AVRESULT(AVERROR_HTTP_FORBIDDEN);		} }
	static property AVRESULT HTTP_NOT_FOUND		{ AVRESULT get() { return AVRESULT(AVERROR_HTTP_NOT_FOUND);		} }
	static property AVRESULT HTTP_OTHER_4XX		{ AVRESULT get() { return AVRESULT(AVERROR_HTTP_OTHER_4XX);		} }
	static property AVRESULT HTTP_SERVER_ERROR	{ AVRESULT get() { return AVRESULT(AVERROR_HTTP_SERVER_ERROR);	} }
protected:
	/// Error Code
	int			m_nValue;
public:
	explicit AVRESULT(int error);
	explicit AVRESULT(unsigned int error);
public:
	property int Value { int get() { return m_nValue; } }
	property String^ Text{ String^ get(); }
public:
	property bool Succeeded { bool get() { return m_nValue >= 0; } }
	property bool Failed { bool get() { return m_nValue < 0; } }
public:
	static explicit operator bool(AVRESULT a) { return a.Succeeded; }
	static operator int(AVRESULT a) { return a.m_nValue; }
	static explicit operator unsigned int(AVRESULT a) { return (unsigned int)a.m_nValue; }
	static operator AVRESULT(int a) { return AVRESULT(a); }
	static explicit operator AVRESULT(unsigned int a) { return AVRESULT((int)a); }
	static bool operator !(AVRESULT a) { return !a.Succeeded; }
public:
	String^ ToString() override { return Text; }
public:
	// IComparable
	virtual int CompareTo(System::Object ^obj);
public:
	// ICloneable
	virtual System::Object ^ Clone();
};
//////////////////////////////////////////////////////
#ifdef _INC_STDIO
//////////////////////////////////////////////////////
#pragma pop_macro("EOF")
//////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////
#ifdef _INC_ERRNO
//////////////////////////////////////////////////////
#pragma pop_macro("EPERM")
#pragma pop_macro("ENOENT")
#pragma pop_macro("ESRCH")
#pragma pop_macro("EINTR")
#pragma pop_macro("EIO")
#pragma pop_macro("ENXIO")
#pragma pop_macro("E2BIG")
#pragma pop_macro("ENOEXEC")
#pragma pop_macro("EBADF")
#pragma pop_macro("ECHILD")
#pragma pop_macro("EAGAIN")
#pragma pop_macro("ENOMEM")
#pragma pop_macro("EACCES")
#pragma pop_macro("EFAULT")
#pragma pop_macro("EBUSY")
#pragma pop_macro("EEXIST")
#pragma pop_macro("EXDEV")
#pragma pop_macro("ENODEV")
#pragma pop_macro("ENOTDIR")
#pragma pop_macro("EISDIR")
#pragma pop_macro("ENFILE")
#pragma pop_macro("EMFILE")
#pragma pop_macro("ENOTTY")
#pragma pop_macro("EFBIG")
#pragma pop_macro("ENOSPC")
#pragma pop_macro("ESPIPE")
#pragma pop_macro("EROFS")
#pragma pop_macro("EMLINK")
#pragma pop_macro("EPIPE")
#pragma pop_macro("EDOM")
#pragma pop_macro("EDEADLK")
#pragma pop_macro("ENAMETOOLONG")
#pragma pop_macro("ENOLCK")
#pragma pop_macro("ENOSYS")
#pragma pop_macro("ENOTEMPTY")
#pragma pop_macro("EINVAL")
#pragma pop_macro("ERANGE")
#pragma pop_macro("EILSEQ")
#pragma pop_macro("STRUNCATE")
#pragma pop_macro("EADDRINUSE")
#pragma pop_macro("EADDRNOTAVAIL")
#pragma pop_macro("EAFNOSUPPORT")
#pragma pop_macro("EALREADY")
#pragma pop_macro("EBADMSG")
#pragma pop_macro("ECANCELED")
#pragma pop_macro("ECONNABORTED")
#pragma pop_macro("ECONNREFUSED")
#pragma pop_macro("ECONNRESET")
#pragma pop_macro("EDESTADDRREQ")
#pragma pop_macro("EHOSTUNREACH")
#pragma pop_macro("EIDRM")
#pragma pop_macro("EINPROGRESS")
#pragma pop_macro("EISCONN")
#pragma pop_macro("ELOOP")
#pragma pop_macro("EMSGSIZE")
#pragma pop_macro("ENETDOWN")
#pragma pop_macro("ENETRESET")
#pragma pop_macro("ENETUNREACH")
#pragma pop_macro("ENOBUFS")
#pragma pop_macro("ENODATA")
#pragma pop_macro("ENOLINK")
#pragma pop_macro("ENOMSG")
#pragma pop_macro("ENOPROTOOPT")
#pragma pop_macro("ENOSR")
#pragma pop_macro("ENOSTR")
#pragma pop_macro("ENOTCONN")
#pragma pop_macro("ENOTRECOVERABLE")
#pragma pop_macro("ENOTSOCK")
#pragma pop_macro("ENOTSUP")
#pragma pop_macro("EOPNOTSUPP")
#pragma pop_macro("EOTHER")
#pragma pop_macro("EOVERFLOW")
#pragma pop_macro("EOWNERDEAD")
#pragma pop_macro("EPROTO")
#pragma pop_macro("EPROTONOSUPPORT")
#pragma pop_macro("EPROTOTYPE")
#pragma pop_macro("ETIME")
#pragma pop_macro("ETIMEDOUT")
#pragma pop_macro("ETXTBSY")
#pragma pop_macro("EWOULDBLOCK")
//////////////////////////////////////////////////////
#endif // _INC_ERRNO
//////////////////////////////////////////////////////
ref class AVRational;
ref class AVPicture;
ref class AVFrame;
ref class AVClass;
ref class AVStream;
ref class AVPacket;
ref class AVDictionary;
//////////////////////////////////////////////////////
#define LOAD_API(lib,result,api,...) \
	typedef result (WINAPIV *PFN_##api)(__VA_ARGS__); \
	PFN_##api api = (AVBase::m_hLib##lib != nullptr ? (PFN_##api)GetProcAddress(AVBase::m_hLib##lib,#api) : nullptr);
//////////////////////////////////////////////////////
#define DYNAMIC_API(lib,result,api,...) \
		LOAD_API(lib,result,api,__VA_ARGS__); \
		if (api)
//////////////////////////////////////////////////////
#define DYNAMIC_DEF_API(lib,result,_default,api,...) \
	LOAD_API(lib,result,api,__VA_ARGS__); \
	if (!api) return _default;

#define DYNAMIC_DEF_SYM(lib,result,_default,sym) \
	void * pSym = (AVBase::m_hLib##lib != nullptr ? GetProcAddress(AVBase::m_hLib##lib,#sym) : nullptr); \
	if (!pSym) return _default; \
	result sym = (result)pSym;
//////////////////////////////////////////////////////
#define VOID_API(lib,api,...) DYNAMIC_API(lib,void,api,__VA_ARGS__)
#define PTR_API(lib,api,...)  DYNAMIC_API(lib,void *,api,__VA_ARGS__)
#define INT_API(lib,api,...)  DYNAMIC_API(lib,int,api,__VA_ARGS__)
#define INT_API2(lib,_default,api,...) DYNAMIC_DEF_API(lib,int,_default,api,__VA_ARGS__)
//////////////////////////////////////////////////////
// Memory 
public ref class AVMemory
{
protected:
	static Int64 s_MemoryReference = 0;
	static Generic::List<IntPtr>^ m_AllocatedMemory = gcnew Generic::List<IntPtr>();
protected:
	AVMemory();
public:
	virtual ~AVMemory();
	!AVMemory();
};
//////////////////////////////////////////////////////
// AVBase
public ref class AVBase : AVMemory
{
private:
	// Allocation Flag
	bool			m_bAllocated;
	// References
	volatile long	m_lRefCount;
	// References
	volatile long	m_lInternalRefCount;
	// Disposed Flag
	bool			m_bDisposed;
internal:
	static bool s_bDllLoaded = false;
	static HMODULE m_hLibAVUtil = nullptr;
	static HMODULE m_hLibAVCodec = nullptr;
	static HMODULE m_hLibAVFormat = nullptr;
	static HMODULE m_hLibAVFilter = nullptr;
	static HMODULE m_hLibAVDevice = nullptr;
	static HMODULE m_hLibPostproc = nullptr;
	static HMODULE m_hLibSwscale = nullptr;
	static HMODULE m_hLibSwresample = nullptr;
private:
	Generic::SortedList<String^,IntPtr>^ m_ObjectMemory;
	Generic::List<AVBase^>^ m_Objects;
internal:
	typedef void TFreeFN(void *);
	typedef void TFreeFNP(void **);
	// Function to free object
	TFreeFN * m_pFree;
	// Object Destructor Function (may be set along with any free function)
	TFreeFN * m_pDescructor;
	// Free Pointer Function
	TFreeFNP * m_pFreep;
protected:
	// Target Object Pointer
	void *	m_pPointer;
	// Parent Object
	AVBase^ m_pParent;
internal:
	AVBase(void * _pointer,AVBase^ _parent);
protected:
	AVBase();
public:
	virtual ~AVBase();
	!AVBase();
internal:
	long AddRef();
	long Release();
private:
	long InternalAddRef();
	long InternalRelease();
internal:
	static void EnsureLibraryLoaded();
protected:
	// Validate object not disposed
	void ValidateObject();
internal:
	void ChangePointer(void * p) { m_pPointer = p; }
public:
	virtual bool Equals(Object^ obj) override;
public:
	// Access To Underlaying Object 
	property IntPtr _Pointer { virtual IntPtr get(); internal: void set(IntPtr _ptr); }
	// Check whatever object pointer non zero
	property bool _IsValid { bool get(); }
	// Check whatever object memory allocated internally
	property bool _IsAllocated { bool get(); }
	// Rerieve underlying structure size if available
	property int _StructureSize { virtual int get(); }
internal:
	// Internal access to parent object
	property AVBase^ _Parent { AVBase^ get(); }
internal:
	virtual void _Free();
	virtual void _FreeObjects();
protected:
	// Check that object is availeble to be accessed
	// throw exception if not
	bool _EnsurePointer();
	bool _EnsurePointer(bool bThrow);
internal:
	// Create or Access Child objects
	template <class T>
	static T^ _CreateChildObject(const void * p, AVBase^ _parent) { return _CreateChildObject((void*)p,_parent); }
	template <class T>
	static T^ _CreateChildObject(void * p,AVBase^ _parent) {  
		if (p == nullptr) return nullptr; 
		T^ o = (_parent != nullptr ? (T^)_parent->GetObject((IntPtr)p) : nullptr); 
		if (o == nullptr) o = gcnew T(p,_parent); return o; 
	}
	template <class T>
	T^ _CreateObject(const void * p) { return _CreateChildObject<T>(p,this); }
	template <class T>
	T^ _CreateObject(void * p) { return _CreateChildObject<T>(p,this); }
protected:
	// Accessing children 
	AVBase^ GetObject(IntPtr p);
	bool AddObject(IntPtr p,AVBase^ _object);
	bool AddObject(AVBase^ _object);
	void RemoveObject(IntPtr p);
protected:
	// Object Memory Allocation
	void AllocPointer(int _size);
	IntPtr GetMemory(String^ _key);
	void SetMemory(String^ _key,IntPtr _pointer);
	IntPtr AllocMemory(String^ _key,int _size);
	IntPtr AllocString(String^ _key,String^ _value);
	IntPtr AllocString(String^ _key,String^ _value,bool bUnicode);
	void FreeMemory(String^ _key);
	bool IsAllocated(String^ _key);
public:
	static bool operator == (AVBase^,AVBase^);
	static bool operator != (AVBase^,AVBase^);
	static bool operator == (AVBase^, IntPtr);
	static bool operator != (AVBase^, IntPtr);
public:
	// Base Memory allocation
	static IntPtr AllocMemory(int _size);
	static IntPtr AllocString(String^ _value);
	static IntPtr AllocString(String^ _value,bool bUnicode);
	static void FreeMemory(IntPtr _memory);
public:
	// Trace helping
	bool TraceValues() { return TraceValues(this); }
	static bool TraceValues(AVBase^ _object);
};
//////////////////////////////////////////////////////
// AVArrayBase
public ref class AVArrayBase : public AVBase
{
protected:
	bool	m_bValidate;
	int		m_nItemSize;
	int		m_nCount;
protected:
	AVArrayBase(void * _pointer,AVBase^ _parent,int nItemSize,int nCount) 
		: AVBase(_pointer,_parent) , m_nCount(nCount), m_nItemSize(nItemSize), m_bValidate(true) { }
	AVArrayBase(void * _pointer,AVBase^ _parent,int nItemSize,int nCount,bool bValidate) 
		: AVBase(_pointer,_parent) , m_nCount(nCount), m_nItemSize(nItemSize), m_bValidate(bValidate) { }
protected:
	void ValidateIndex(int index) { if (index < 0 || index >= m_nCount) throw gcnew ArgumentOutOfRangeException(); }
	void * GetValue(int index)
	{
		if (m_bValidate) ValidateIndex(index);
		return (((LPBYTE)m_pPointer) + m_nItemSize * index);
	}
	void SetValue(int index,void * value)
	{
		if (m_bValidate) ValidateIndex(index);
		memcpy(((LPBYTE)m_pPointer) + m_nItemSize * index,value,m_nItemSize);
	}
public:
	property int Count { int get() { return m_nCount; } }
};
//////////////////////////////////////////////////////
template <class T> ref class AVArray;
//////////////////////////////////////////////////////
template <class T> 
private ref class AVEnumerator : public System::Collections::IEnumerator, public System::Collections::Generic::IEnumerator<T>
{
protected:
	int			m_nIndex;
	AVArray<T>^ m_pArray;
public:
	AVEnumerator(AVArray<T>^ _array): m_pArray(_array),m_nIndex(-1) {}
	~AVEnumerator() {}
public:
	// IEnumerator
	virtual bool MoveNext() { m_nIndex++; return (m_nIndex >= 0 && m_nIndex < m_pArray->Count); }
	virtual property T Current { T get() { if (m_nIndex == -1) { MoveNext(); } return m_pArray[m_nIndex]; } }
	virtual void Reset() { m_nIndex = -1; }
	virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get { return (Object^)Current::get(); } }
};
//////////////////////////////////////////////////////
template <class T> 
public ref class AVArray : public AVArrayBase
	, public System::Collections::IEnumerable
	, public System::Collections::Generic::IEnumerable<T>
{
internal:
	AVArray(void * _pointer,AVBase^ _parent,int nItemSize,int nCount) : AVArrayBase(_pointer,_parent,nItemSize,nCount,true) { 
		if (m_nItemSize == 0) m_nItemSize = sizeof(T); 
	}
	AVArray(void * _pointer,AVBase^ _parent,int nItemSize,int nCount, bool bValidate) : AVArrayBase(_pointer,_parent,nItemSize,nCount,bValidate) { 
		if (m_nItemSize == 0) m_nItemSize = sizeof(T); 
	}
public:
	AVArray(IntPtr p, int count) : AVArrayBase(p.ToPointer(),nullptr,sizeof(T),count) {	}
	AVArray(IntPtr p, int count, bool bValidate) : AVArrayBase(p.ToPointer(),nullptr,sizeof(T),count,bValidate) {	}
public:
	property T default[int] 
	{ 
		T get(int index)
		{
			T* p = (T*)GetValue(index);
			return *p;
		}
		void set(int index,T value)
		{
			SetValue(index,(void*)&value);
		}
	}
public:
	// IEnumerable
	virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { 
		return gcnew AVEnumerator<T>(this); 
	}
public:
	// IEnumerable<T>
	virtual System::Collections::Generic::IEnumerator<T>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<T>::GetEnumerator { 
		return gcnew AVEnumerator<T>(this); 
	}
public:
	array<T>^ ToArray()
	{
		List<T>^ _list = gcnew List<T>();
		for (int i = 0; i < m_nCount; i++)
		{
			_list->Add(this[i]);
		}
		return _list->ToArray();
	}
};
//////////////////////////////////////////////////////
template <> 
public ref class AVArray< IntPtr > : public AVArrayBase
{
internal:
	AVArray(void * _pointer,AVBase^ _parent,int nCount) : AVArrayBase(_pointer,_parent,sizeof(void*),nCount) {}
public:
	AVArray(IntPtr p, int count) : AVArrayBase(p.ToPointer(),nullptr,sizeof(void*),count) {	}
public:
	property IntPtr default[int]
	{ 
		IntPtr get(int index)
		{
			void ** p = (void **)GetValue(index);
			return (IntPtr)*p;
		}
		void set(int index,IntPtr value)
		{
			const void * p = value.ToPointer();
			SetValue(index,&p);
		}
	}
public:
	array<IntPtr>^ ToArray()
	{
		List<IntPtr>^ _list = gcnew List<IntPtr>();
		for (int i = 0; i < m_nCount; i++)
		{
			_list->Add(this[i]);
		}
		return _list->ToArray();
	}
};
//////////////////////////////////////////////////////
// AVColor
public ref class AVColor
{
private:
	// RGB Tables Initialization Flag
	static bool s_bTableInitialized = false;
internal:
	uint8_t	*	m_pPointer;
	int			m_nSize;
	uint32_t	m_nFourCC;
internal:
	AVColor(void * p, uint32_t fcc, int size);
public:
	property int Size { int get(); }
	property byte default[int] {byte get(int); void set(int, byte); }
public:
	virtual property System::Drawing::Color Color { System::Drawing::Color get(); void set(System::Drawing::Color); };
public:
	static operator IntPtr(AVColor^);
public:
	static explicit operator byte(AVColor^);
	static explicit operator short(AVColor^);
	static explicit operator int(AVColor^);
	static explicit operator unsigned short(AVColor^);
	static explicit operator unsigned int(AVColor^);
public:
	static bool operator == (AVColor^,AVColor^);
	static bool operator != (AVColor^,AVColor^);
public:
	void operator = (int);
	void operator = (AVColor^);
	void operator = (System::Drawing::Color);
public:
	array<byte>^ ToArray();
public:
	virtual String^ ToString() override;
protected:
	// Initialize Tables
	static void InitializeTables();
public:
	// Setup Specified Convesion matrix
	static void SetColorspaceMatrices(array<double,2>^ rgb,array<double,2>^ yuv);
	// Conversion RGB <-> YUV 
	static void RGBToYUV(int r, int g, int b, int % y, int % u, int % v);
	static void YUVToRGB(int y, int u, int v, int % r, int % g, int % b);
};
//////////////////////////////////////////////////////
#ifdef _WINGDI_
#pragma push_macro("RGB")
#undef RGB
#endif
//////////////////////////////////////////////////////
public ref class RGB : public AVColor
{
internal:
	RGB(void * p) : AVColor(p,'3BGR',3) {}
public:
	property int R { int get() { return m_pPointer[2]; } void set(int value) { m_pPointer[2] = (byte)value; } }
	property int G { int get() { return m_pPointer[1]; } void set(int value) { m_pPointer[1] = (byte)value; } }
	property int B { int get() { return m_pPointer[0]; } void set(int value) { m_pPointer[0] = (byte)value; } }
public:
	property System::Drawing::Color Color { System::Drawing::Color get() override; void set(System::Drawing::Color) override; };
};
//////////////////////////////////////////////////////
template <> 
public ref class AVArray< RGB^ > : public AVArrayBase
		, public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<RGB^>
{
internal:
	AVArray(void * _pointer,AVBase^ _parent,int nCount) : AVArrayBase(_pointer,_parent,3,nCount) {}
public:
	AVArray(IntPtr p, int count,bool validate) : AVArrayBase(p.ToPointer(),nullptr,3,count,validate) {	}
public:
	property RGB^ default[int]
	{ 
		RGB^ get(int index)
		{
			void * p = (void *)GetValue(index);
			return gcnew RGB(p);
		}
		void set(int index,RGB^ value)
		{
			const void * p = value->m_pPointer;
			SetValue(index,&p);
		}
	}
public:
	array<RGB^>^ ToArray()
	{
		List<RGB^>^ _list = gcnew List<RGB^>();
		for (int i = 0; i < m_nCount; i++)
		{
			_list->Add(this[i]);
		}
		return _list->ToArray();
	}
public:
	// IEnumerable
	virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { 
		return gcnew AVEnumerator<RGB^>(this); 
	}
public:
	// IEnumerable<T>
	virtual System::Collections::Generic::IEnumerator<RGB^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<RGB^>::GetEnumerator { 
		return gcnew AVEnumerator<RGB^>(this); 
	}
};
//////////////////////////////////////////////////////
#ifdef _WINGDI_
#pragma pop_macro("RGB")
#endif
//////////////////////////////////////////////////////
public ref class RGBA : public AVColor
{
internal:
	RGBA(void * p) : AVColor(p,'ABGR',4) {}
public:
	property int R { int get() { return m_pPointer[2]; } void set(int value) { m_pPointer[2] = (byte)value; } }
	property int G { int get() { return m_pPointer[1]; } void set(int value) { m_pPointer[1] = (byte)value; } }
	property int B { int get() { return m_pPointer[0]; } void set(int value) { m_pPointer[0] = (byte)value; } }
	property int A { int get() { return m_pPointer[3]; } void set(int value) { m_pPointer[3] = (byte)value; } }
};
//////////////////////////////////////////////////////
template <> 
public ref class AVArray< RGBA^ > : public AVArrayBase
	, public System::Collections::IEnumerable
	, public System::Collections::Generic::IEnumerable<RGBA^>
{
internal:
	AVArray(void * _pointer,AVBase^ _parent,int nCount) : AVArrayBase(_pointer,_parent,4,nCount) {}
public:
	AVArray(IntPtr p, int count,bool validate) : AVArrayBase(p.ToPointer(),nullptr,4,count,validate) {	}
public:
	property RGBA^ default[int]
	{ 
		RGBA^ get(int index)
		{
			void * p = (void *)GetValue(index);
			return gcnew RGBA(p);
		}
		void set(int index,RGBA^ value)
		{
			const void * p = value->m_pPointer;
			SetValue(index,&p);
		}
	}
public:
	array<RGBA^>^ ToArray()
	{
		List<RGBA^>^ _list = gcnew List<RGBA^>();
		for (int i = 0; i < m_nCount; i++)
		{
			_list->Add(this[i]);
		}
		return _list->ToArray();
	}
public:
	// IEnumerable
	virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { 
		return gcnew AVEnumerator<RGBA^>(this); 
	}
public:
	// IEnumerable<T>
	virtual System::Collections::Generic::IEnumerator<RGBA^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<RGBA^>::GetEnumerator { 
		return gcnew AVEnumerator<RGBA^>(this); 
	}
};
//////////////////////////////////////////////////////
public ref class AYUV : public AVColor
{
internal:
	AYUV(void * p) : AVColor(p,'VUYA',4) {}
public:
	property int Y { int get() { return m_pPointer[2]; } void set(int value) { m_pPointer[2] = (byte)value; } }
	property int U { int get() { return m_pPointer[1]; } void set(int value) { m_pPointer[1] = (byte)value; } }
	property int V { int get() { return m_pPointer[0]; } void set(int value) { m_pPointer[0] = (byte)value; } }
	property int A { int get() { return m_pPointer[3]; } void set(int value) { m_pPointer[3] = (byte)value; } }
public:
	property System::Drawing::Color Color { System::Drawing::Color get() override; void set(System::Drawing::Color) override; };
};
//////////////////////////////////////////////////////
template <> 
public ref class AVArray< AYUV^ > : public AVArrayBase
	, public System::Collections::IEnumerable
	, public System::Collections::Generic::IEnumerable<AYUV^>
{
internal:
	AVArray(void * _pointer,AVBase^ _parent,int nCount) : AVArrayBase(_pointer,_parent,4,nCount) {}
public:
	AVArray(IntPtr p, int count,bool validate) : AVArrayBase(p.ToPointer(),nullptr,4,count,validate) {	}
public:
	property AYUV^ default[int]
	{ 
		AYUV^ get(int index)
		{
			void * p = (void *)GetValue(index);
			return gcnew AYUV(p);
		}
		void set(int index,AYUV^ value)
		{
			const void * p = value->m_pPointer;
			SetValue(index,&p);
		}
	}
public:
	array<AYUV^>^ ToArray()
	{
		List<AYUV^>^ _list = gcnew List<AYUV^>();
		for (int i = 0; i < m_nCount; i++)
		{
			_list->Add(this[i]);
		}
		return _list->ToArray();
	}
public:
	// IEnumerable
	virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { 
		return gcnew AVEnumerator<AYUV^>(this); 
	}
public:
	// IEnumerable<T>
	virtual System::Collections::Generic::IEnumerator<AYUV^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AYUV^>::GetEnumerator { 
		return gcnew AVEnumerator<AYUV^>(this); 
	}
};
//////////////////////////////////////////////////////
public ref class YUY2 : public AVColor
{
internal:
	YUY2(void * p) : AVColor(p,'VYUY',4) {}
public:
	property int Y0 { int get() { return m_pPointer[0]; } void set(int value) { m_pPointer[0] = (byte)value; } }
	property int U { int get() { return m_pPointer[1]; } void set(int value) { m_pPointer[1] = (byte)value; } }
	property int Y1 { int get() { return m_pPointer[2]; } void set(int value) { m_pPointer[2] = (byte)value; } }
	property int V { int get() { return m_pPointer[3]; } void set(int value) { m_pPointer[3] = (byte)value; } }
public:
	property System::Drawing::Color Color { System::Drawing::Color get() override; void set(System::Drawing::Color) override; };
};
//////////////////////////////////////////////////////
template <> 
public ref class AVArray< YUY2^ > : public AVArrayBase
	, public System::Collections::IEnumerable
	, public System::Collections::Generic::IEnumerable<YUY2^>
{
internal:
	AVArray(void * _pointer,AVBase^ _parent,int nCount) : AVArrayBase(_pointer,_parent,4,nCount) {}
public:
	AVArray(IntPtr p, int count,bool validate) : AVArrayBase(p.ToPointer(),nullptr,4,count,validate) {	}
public:
	property YUY2^ default[int]
	{ 
		YUY2^ get(int index)
		{
			void * p = (void *)GetValue(index);
			return gcnew YUY2(p);
		}
		void set(int index,YUY2^ value)
		{
			const void * p = value->m_pPointer;
			SetValue(index,&p);
		}
	}
public:
	array<YUY2^>^ ToArray()
	{
		List<YUY2^>^ _list = gcnew List<YUY2^>();
		for (int i = 0; i < m_nCount; i++)
		{
			_list->Add(this[i]);
		}
		return _list->ToArray();
	}
public:
	// IEnumerable
	virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { 
		return gcnew AVEnumerator<YUY2^>(this); 
	}
public:
	// IEnumerable<T>
	virtual System::Collections::Generic::IEnumerator<YUY2^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<YUY2^>::GetEnumerator { 
		return gcnew AVEnumerator<YUY2^>(this); 
	}
};
//////////////////////////////////////////////////////
public ref class UYVY : public AVColor
{
internal:
	UYVY(void * p) : AVColor(p,'YVYU',4) {}
public:
	property int Y0 { int get() { return m_pPointer[1]; } void set(int value) { m_pPointer[1] = (byte)value; } }
	property int U { int get() { return m_pPointer[0]; } void set(int value) { m_pPointer[0] = (byte)value; } }
	property int Y1 { int get() { return m_pPointer[3]; } void set(int value) { m_pPointer[3] = (byte)value; } }
	property int V { int get() { return m_pPointer[2]; } void set(int value) { m_pPointer[2] = (byte)value; } }
public:
	property System::Drawing::Color Color { System::Drawing::Color get() override; void set(System::Drawing::Color) override; };
};
//////////////////////////////////////////////////////
template <> 
public ref class AVArray< UYVY^ > : public AVArrayBase
	, public System::Collections::IEnumerable
	, public System::Collections::Generic::IEnumerable<UYVY^>
{
internal:
	AVArray(void * _pointer,AVBase^ _parent,int nCount) : AVArrayBase(_pointer,_parent,4,nCount) {}
public:
	AVArray(IntPtr p, int count,bool validate) : AVArrayBase(p.ToPointer(),nullptr,4,count,validate) {	}
public:
	property UYVY^ default[int]
	{ 
		UYVY^ get(int index)
		{
			void * p = (void *)GetValue(index);
			return gcnew UYVY(p);
		}
		void set(int index,UYVY^ value)
		{
			const void * p = value->m_pPointer;
			SetValue(index,&p);
		}
	}
public:
	array<UYVY^>^ ToArray()
	{
		List<UYVY^>^ _list = gcnew List<UYVY^>();
		for (int i = 0; i < m_nCount; i++)
		{
			_list->Add(this[i]);
		}
		return _list->ToArray();
	}
public:
	// IEnumerable
	virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { 
		return gcnew AVEnumerator<UYVY^>(this); 
	}
public:
	// IEnumerable<T>
	virtual System::Collections::Generic::IEnumerator<UYVY^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<UYVY^>::GetEnumerator { 
		return gcnew AVEnumerator<UYVY^>(this); 
	}
};
//////////////////////////////////////////////////////
// AVMemPtr
public ref class AVMemPtr : AVMemory
{
internal:
	typedef int TGetSizeFN(void * p,void * opaque);
	void *			m_pPointer;
	AVMemPtr^		m_pRoot;
	volatile long	m_nRef;
	TGetSizeFN *	m_pGetSize;
	void *			m_pOpaque;
protected:
	int				m_nPosition;
	int				m_nSize;
	bool			m_bAllocated;
	bool			m_bDisposed;
internal:
	AVMemPtr(AVMemPtr^ root,int position);
	AVMemPtr(IntPtr p,TGetSizeFN * pGetSize, void * opaque);
	long AddRef();
	long Release();
	void Free();
public:
	AVMemPtr();
	AVMemPtr(int size,int align);
	AVMemPtr(int size);
	AVMemPtr(IntPtr p);
	AVMemPtr(IntPtr p,int size);
	virtual ~AVMemPtr();
	!AVMemPtr();
public:
	property bool allocated { bool get(); }
	property int size { int get(); }
	property byte default[int] { byte get(int); void set(int, byte); }
	property bool is_empty { bool get() { return m_pPointer == nullptr; } }
public:
	IntPtr Detach();
	void Attach(IntPtr p);
public:
	static operator IntPtr(AVMemPtr^);
	static operator AVMemPtr^(IntPtr);
public:
	static operator bool(AVMemPtr^);
public:
	static explicit operator byte(AVMemPtr^);
	static explicit operator short(AVMemPtr^);
	static explicit operator int(AVMemPtr^);
	static explicit operator __int64(AVMemPtr^);
	static explicit operator float(AVMemPtr^);
	static explicit operator double(AVMemPtr^);
	static explicit operator unsigned short(AVMemPtr^);
	static explicit operator unsigned int(AVMemPtr^);
public:
	static bool operator !(AVMemPtr^);
public:
	static bool operator == (AVMemPtr^,AVMemPtr^);
	static bool operator != (AVMemPtr^,AVMemPtr^);
	static bool operator == (AVMemPtr^,IntPtr);
	static bool operator != (AVMemPtr^,IntPtr);
	static bool operator == (AVMemPtr^,int);
	static bool operator != (AVMemPtr^,int);
	static bool operator == (AVMemPtr^,Int64 );
	static bool operator != (AVMemPtr^,Int64 );
public:
	static AVMemPtr^ operator + (AVMemPtr^,int);
	static AVMemPtr^ operator - (AVMemPtr^,int);
	static AVMemPtr^ operator + (AVMemPtr^,Int64 );
	static AVMemPtr^ operator - (AVMemPtr^,Int64 );
public:
	AVMemPtr^ operator += (int);
	AVMemPtr^ operator -= (int);
	AVMemPtr^ operator += (Int64 );
	AVMemPtr^ operator -= (Int64 );
	AVMemPtr^ operator ++ ();
	AVMemPtr^ operator ++ (int);
	AVMemPtr^ operator -- ();
	AVMemPtr^ operator -- (int);
public:
	property AVArray<byte>^   bytes				{ AVArray<byte>^ get();   }
	property AVArray<short>^  shorts			{ AVArray<short>^ get();  }
	property AVArray<int>^	  integers			{ AVArray<int>^ get();    }
	property AVArray<float>^  floats			{ AVArray<float>^ get();  }
	property AVArray<double>^ doubles			{ AVArray<double>^ get();  }
	property AVArray<IntPtr>^ pointers			{ AVArray<IntPtr>^ get(); }
	property AVArray<unsigned int>^	uints		{ AVArray<unsigned int>^ get(); }
	property AVArray<unsigned short>^ ushorts	{ AVArray<unsigned short>^ get();  }
	property AVArray<RGB^>^   rgb				{ AVArray<RGB^>^ get();    }
	property AVArray<RGBA^>^  rgba				{ AVArray<RGBA^>^ get();   }
	property AVArray<AYUV^>^  ayuv				{ AVArray<AYUV^>^ get();   }
	property AVArray<YUY2^>^  yuy2				{ AVArray<YUY2^>^ get();   }
	property AVArray<UYVY^>^  uyvy				{ AVArray<UYVY^>^ get();   }
public:
	array<byte>^ ToArray();
public:
	bool Dump(String^ sFileName, bool bAppend);
	bool Dump(String^ sFileName);
	bool Dump(Stream^ s);
public:
	virtual String^ ToString() override;
};
//////////////////////////////////////////////////////
template <> 
public ref class AVArray< AVMemPtr^ > : public AVArrayBase
{
internal:
	AVMemPtr::TGetSizeFN *	m_pGetSize = nullptr;
	void *					m_pOpaque = nullptr;
internal:
	AVArray(void * _pointer,AVBase^ _parent,int nCount) : AVArrayBase(_pointer,_parent,sizeof(void*),nCount) {}
	AVArray(void * _pointer,AVBase^ _parent,int nCount,AVMemPtr::TGetSizeFN * pGetSize,void * opaque) 
		: AVArrayBase(_pointer,_parent,sizeof(void*),nCount), m_pGetSize(pGetSize), m_pOpaque(opaque) { }
public:
	AVArray(IntPtr p, int count) : AVArrayBase(p.ToPointer(),nullptr,sizeof(void*),count) {	}
public:
	property AVMemPtr^ default[int]
	{ 
		AVMemPtr^ get(int index)
		{
			void ** p = (void **)GetValue(index);
			return gcnew AVMemPtr((IntPtr)*p,m_pGetSize,m_pOpaque);
		}
		void set(int index,AVMemPtr^ value)
		{
			const void * p = (const void *)value->m_pPointer;
			SetValue(index,&p);
		}
	}
public:
	array<AVMemPtr^>^ ToArray()
	{
		List<AVMemPtr^>^ _list = gcnew List<AVMemPtr^>();
		for (int i = 0; i < m_nCount; i++)
		{
			_list->Add(this[i]);
		}
		return _list->ToArray();
	}
	array<IntPtr>^ ToPtrArray()
	{
		List<IntPtr>^ _list = gcnew List<IntPtr>();
		for (int i = 0; i < m_nCount; i++)
		{
			_list->Add(this[i]);
		}
		return _list->ToArray();
	}
};
//////////////////////////////////////////////////////
// LibAVUtil
public ref class LibAVUtil
{
private:
	LibAVUtil();
public:
	static property UInt32 Version { UInt32 get(); }
	static property String^ Configuration { String^ get(); }
	static property String^ License { String^ get(); }
};
//////////////////////////////////////////////////////
// AVSampleFmt
public ref class AVSampleFmt
{
protected:
	AVSampleFmt() {}
public:
	/// Return the name of sample_fmt, or NULL if sample_fmt is not
	/// recognized.
	static String^ get_sample_fmt_name(AVSampleFormat sample_fmt);

	/// Return a sample format corresponding to name, or AV_SAMPLE_FMT_NONE
	/// on error.
	static AVSampleFormat get_sample_fmt(String^ name);

	/// Return the planar<->packed alternative form of the given sample format, or
	/// AV_SAMPLE_FMT_NONE on error. If the passed sample_fmt is already in the
	/// requested planar/packed format, the format returned is the same as the
	/// input.
	static AVSampleFormat get_alt_sample_fmt(AVSampleFormat sample_fmt, bool planar);
	/// Get the packed alternative form of the given sample format.
	///
	/// If the passed sample_fmt is already in packed format, the format returned is
	/// the same as the input.
	///
	/// @return  the packed alternative form of the given sample format or
	/// AV_SAMPLE_FMT_NONE on error.
	static AVSampleFormat get_packed_sample_fmt(AVSampleFormat sample_fmt);

	/// Get the planar alternative form of the given sample format.
	///
	/// If the passed sample_fmt is already in planar format, the format returned is
	/// the same as the input.
	///
	/// @return  the planar alternative form of the given sample format or
	/// AV_SAMPLE_FMT_NONE on error.
	static AVSampleFormat get_planar_sample_fmt(AVSampleFormat sample_fmt);

	/// Return number of bytes per sample.
	///
	/// @param sample_fmt the sample format
	/// @return number of bytes per sample or zero if unknown for the given
	/// sample format
	static int get_bytes_per_sample(AVSampleFormat sample_fmt);

	/// Check if the sample format is planar.
	///
	/// @param sample_fmt the sample format to inspect
	/// @return 1 if the sample format is planar, 0 if it is interleaved
	static bool sample_fmt_is_planar(AVSampleFormat sample_fmt);
};
//////////////////////////////////////////////////////
// AVChannels
public ref class AVChannels : public AVSampleFmt
{
protected:
	AVChannels() {}
public:

	/// Return a channel layout id that matches name, or 0 if no match is found.
	/// 
	/// name can be one or several of the following notations,
	/// separated by '+' or '|':
	/// - the name of an usual channel layout (mono, stereo, 4.0, quad, 5.0,
	///   5.0(side), 5.1, 5.1(side), 7.1, 7.1(wide), downmix);
	/// - the name of a single channel (FL, FR, FC, LFE, BL, BR, FLC, FRC, BC,
	///   SL, SR, TC, TFL, TFC, TFR, TBL, TBC, TBR, DL, DR);
	/// - a number of channels, in decimal, optionally followed by 'c', yielding
	///   the default channel layout for that number of channels (@see
	///   av_get_default_channel_layout);
	/// - a channel layout mask, in hexadecimal starting with "0x" (see the
	///   AV_CH_* macros).
	/// 
	/// @warning Starting from the next major bump the trailing character
	/// 'c' to specify a number of channels will be required, while a
	/// channel layout mask could also be specified as a decimal number
	/// (if and only if not followed by "c").
	/// 
	/// Example: "stereo+FC" = "2c+FC" = "2c+1c" = "0x7"
	static AVChannelLayout get_channel_layout(String^ name);

	/// Return the number of channels in the channel layout.
	static int get_nb_channels(AVChannelLayout channel_layout);

	/// Return default channel layout for a given number of channels.
	static AVChannelLayout get_default_channel_layout(int nb_channels);

	/// Get the index of a channel in channel_layout.
	/// 
	/// @param channel a channel layout describing exactly one channel which must be
	///                present in channel_layout.
	/// 
	/// @return index of channel in channel_layout on success, a negative AVERROR
	///         on error.
	static int get_channel_index(AVChannelLayout channel_layout,AVChannelLayout channel);

	/// Get the channel with the given index in channel_layout.
	static AVChannelLayout extract_channel(AVChannelLayout channel_layout, int index);
	
	/// Get the name of a given channel.
	/// 
	/// @return channel name on success, NULL on error.
	static String^ get_channel_name(AVChannelLayout channel);

	/// Return a description of a channel layout.
	/// If nb_channels is <= 0, it is guessed from the channel_layout.
	static String^ get_channel_layout_string(AVChannelLayout layout);
	static String^ get_channel_layout_string(int nb_channels, AVChannelLayout layout);

	/// Get the description of a given channel.
	/// 
	/// @param channel  a channel layout with a single channel
	/// @return  channel description on success, NULL on error
	static String^ get_channel_description(AVChannelLayout channel);

	/// Get the value and name of a standard channel layout.
	/// 
	/// @param[in]  index   index in an internal list, starting at 0
	/// @param[out] layout  channel layout mask
	/// @param[out] name    name of the layout
	/// @return  0  if the layout exists,
	///          <0 if index is beyond the limits
	static bool get_standard_channel_layout(UInt32 index, AVChannelLayout % layout,String^ % name);
};
//////////////////////////////////////////////////////
// AVSamples
public ref class AVSamples
{
protected:
	AVSamples() {}
public:
	/// Get the required buffer size for the given audio parameters.
	///
	/// @param[out] linesize calculated linesize, may be NULL
	/// @param nb_channels   the number of channels
	/// @param nb_samples    the number of samples in a single channel
	/// @param sample_fmt    the sample format
	/// @param align         buffer size alignment (0 = default, 1 = no alignment)
	/// @return              required buffer size, or negative error code on failure
	static int get_buffer_size(int nb_channels, int nb_samples,AVSampleFormat sample_fmt);
	static int get_buffer_size([Out] int % linesize, int nb_channels, int nb_samples,AVSampleFormat sample_fmt);
	static int get_buffer_size(int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align);
	static int get_buffer_size([Out] int % linesize, int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align);
public:
	// Fill plane data pointers and linesize for samples with sample
	// format sample_fmt.
	// 
	// The audio_data array is filled with the pointers to the samples data planes:
	// for planar, set the start point of each channel's data within the buffer,
	// for packed, set the start point of the entire buffer only.
	// 
	// The value pointed to by linesize is set to the aligned size of each
	// channel's data buffer for planar layout, or to the aligned size of the
	// buffer for all channels for packed layout.
	// 
	// The buffer in buf must be big enough to contain all the samples
	// (use av_samples_get_buffer_size() to compute its minimum size),
	// otherwise the audio_data pointers will point to invalid data.
	// 
	// @see enum AVSampleFormat
	// The documentation for AVSampleFormat describes the data layout.
	// 
	// @param[out] audio_data  array to be filled with the pointer for each channel
	// @param[out] linesize    calculated linesize, may be NULL
	// @param buf              the pointer to a buffer containing the samples
	// @param nb_channels      the number of channels
	// @param nb_samples       the number of samples in a single channel
	// @param sample_fmt       the sample format
	// @param align            buffer size alignment (0 = default, 1 = no alignment)
	// @return                 >=0 on success or a negative error code on failure
	// @todo return minimum size in bytes required for the buffer in case
	// of success at the next bump
	static int fill_arrays(array<IntPtr>^ % audio_data,IntPtr buf,
		int nb_channels, int nb_samples,AVSampleFormat sample_fmt);

	static int fill_arrays(AVArray<IntPtr>^ audio_data,IntPtr buf,
		int nb_channels, int nb_samples,AVSampleFormat sample_fmt);

	static int fill_arrays(array<IntPtr>^ % audio_data,[Out] int % linesize,IntPtr buf,
		int nb_channels, int nb_samples,AVSampleFormat sample_fmt);

	static int fill_arrays(AVArray<IntPtr>^ audio_data,[Out] int % linesize,IntPtr buf,
		int nb_channels, int nb_samples,AVSampleFormat sample_fmt);

	static int fill_arrays(array<IntPtr>^ % audio_data,IntPtr buf,
		int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align);

	static int fill_arrays(AVArray<IntPtr>^ audio_data,IntPtr buf,
		int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align);

	static int fill_arrays(array<IntPtr>^ % audio_data,[Out] int % linesize,IntPtr buf,
                           int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align);

	static int fill_arrays(AVArray<IntPtr>^ audio_data,[Out] int % linesize,IntPtr buf,
                           int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align);


	static int fill_arrays(AVPicture^ _frame,IntPtr buf, int nb_channels, int nb_samples,AVSampleFormat sample_fmt);
	static int fill_arrays(AVFrame^ _frame,IntPtr buf,int nb_samples);
	static int fill_arrays(AVPicture^ _frame,IntPtr buf,
                           int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align);
	static int fill_arrays(AVFrame^ _frame,IntPtr buf,int nb_samples,bool align);

	//  Copy samples from src to dst.
	// 
	//  @param dst destination array of pointers to data planes
	//  @param src source array of pointers to data planes
	//  @param dst_offset offset in samples at which the data will be written to dst
	//  @param src_offset offset in samples at which the data will be read from src
	//  @param nb_samples number of samples to be copied
	//  @param nb_channels number of audio channels
	//  @param sample_fmt audio sample format
	static int copy(array<IntPtr>^ dst, array<IntPtr>^ src, int dst_offset,
                    int src_offset, int nb_samples, int nb_channels,
                    AVSampleFormat sample_fmt);
	static int copy(AVArray<IntPtr>^ dst, AVArray<IntPtr>^ src, int dst_offset,
                    int src_offset, int nb_samples, int nb_channels,
                    AVSampleFormat sample_fmt);
	static int copy(AVPicture^ dst, AVPicture^ src, int dst_offset,
                    int src_offset, int nb_samples, int nb_channels,
                    AVSampleFormat sample_fmt);
	static int copy(AVPicture^ dst, AVFrame^ src, int dst_offset,
                    int src_offset, int nb_samples);
	static int copy(array<IntPtr>^ dst, AVFrame^ src, int dst_offset,
					int src_offset, int nb_samples);
	static int copy(AVPicture^ dst, array<IntPtr>^ src, int dst_offset,
					int src_offset, int nb_samples, int nb_channels,
					AVSampleFormat sample_fmt);

	//  Allocate a samples buffer for nb_samples samples, and fill data pointers and
	//  linesize accordingly.
	//  The allocated samples buffer can be freed by using av_freep(&audio_data[0])
	//  Allocated data will be initialized to silence.
	// 
	//  @see enum AVSampleFormat
	//  The documentation for AVSampleFormat describes the data layout.
	// 
	//  @param[out] audio_data  array to be filled with the pointer for each channel
	//  @param[out] linesize    aligned size for audio buffer(s), may be NULL
	//  @param nb_channels      number of audio channels
	//  @param nb_samples       number of samples per channel
	//  @param align            buffer size alignment (0 = default, 1 = no alignment)
	//  @return                 >=0 on success or a negative error code on failure
	//  @todo return the size of the allocated buffer in case of success at the next bump
	//  @see av_samples_fill_arrays()
	//  @see av_samples_alloc_array_and_samples()
	static int alloc([Out] array<IntPtr>^ % audio_data,[Out] int % linesize,
                           int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align);

	static int alloc(AVArray<IntPtr>^ audio_data, int % linesize,
                           int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align);

	static int alloc(AVPicture^ frame,int nb_channels, int nb_samples,AVSampleFormat sample_fmt, bool align);

	static int free(array<IntPtr>^ audio_data);
	static int free(AVArray<IntPtr>^ audio_data);
	static int free(AVPicture^ frame);

	//  Fill an audio buffer with silence.
	// 
	//  @param audio_data  array of pointers to data planes
	//  @param offset      offset in samples at which to start filling
	//  @param nb_samples  number of samples to fill
	//  @param nb_channels number of audio channels
	//  @param sample_fmt  audio sample format
	static int set_silence(AVArray<IntPtr>^ audio_data, int offset, int nb_samples,
                           int nb_channels, AVSampleFormat sample_fmt);
	static int set_silence(array<IntPtr>^ audio_data, int offset, int nb_samples,
                           int nb_channels, AVSampleFormat sample_fmt);
	static int set_silence(AVPicture^ dst, int offset, int nb_samples,
                           int nb_channels, AVSampleFormat sample_fmt);

};
//////////////////////////////////////////////////////
// AVMath
public ref class AVMath
{
protected:
	AVMath() {}
public:
	///  Internal time base represented as fractional value
	static property AVRational^ TIME_BASE_Q { AVRational^ get(); }
public:
	///  Return the greatest common divisor of a and b.
	///  If both a and b are 0 or either or both are <0 then behavior is
	///  undefined.
	static Int64 gcd(Int64 a, Int64 b);

	/// Rescale a 64-bit integer with rounding to nearest.
	/// A simple a*b/c isn't possible as it can overflow.
	static Int64 rescale(Int64 a, Int64 b, Int64 c);

	/// Rescale a 64-bit integer with specified rounding.
	/// A simple a*b/c isn't possible as it can overflow.
	/// 
	/// @return rescaled value a, or if ROUND_PASS_MINMAX is set and a is
	///         INT64_MIN or INT64_MAX then a is passed through unchanged.
		
	static Int64 rescale_rnd(Int64 a, Int64 b, Int64 c, AVRounding r);
	/// Rescale a 64-bit integer by 2 rational numbers.
		
	static Int64 rescale_q(Int64 a, AVRational^ bq, AVRational^ cq);
	/// Rescale a 64-bit integer by 2 rational numbers with specified rounding.
	/// 
	/// @return rescaled value a, or if ROUND_PASS_MINMAX is set and a is
	///         INT64_MIN or INT64_MAX then a is passed through unchanged.
		
	static Int64 rescale_q_rnd(Int64 a, AVRational^ bq, AVRational^ cq,AVRounding r);

	/// Compare 2 timestamps each in its own timebases.
	/// The result of the function is undefined if one of the timestamps
	/// is outside the Int64 range when represented in the others timebase.
	/// @return -1 if ts_a is before ts_b, 1 if ts_a is after ts_b or 0 if they represent the same position
		
	static int compare_ts(Int64 ts_a, AVRational^ tb_a, Int64 ts_b, AVRational^ tb_b);
	/// Compare 2 integers modulo mod.
	/// That is we compare integers a and b for which only the least
	/// significant log2(mod) bits are known.
	/// 
	/// @param mod must be a power of 2
	/// @return a negative value if a is smaller than b
	///         a positive value if a is greater than b
	///         0                if a equals          b
		
	static Int64 compare_mod(UInt64 a, UInt64 b, UInt64 mod);

	/// Rescale a timestamp while preserving known durations.
	/// 
	/// @param in_ts Input timestamp
	/// @param in_tb Input timebase
	/// @param fs_tb Duration and *last timebase
	/// @param duration duration till the next call
	/// @param out_tb Output timebase
	static Int64 rescale_delta(AVRational^ in_tb, Int64 in_ts,  AVRational^ fs_tb, int duration, Int64 % last, AVRational^ out_tb);


	 /// Add a value to a timestamp.
	 /// 
	 /// This function guarantees that when the same value is repeatly added that
	 /// no accumulation of rounding errors occurs.
	 /// 
	 /// @param ts Input timestamp
	 /// @param ts_tb Input timestamp timebase
	 /// @param inc value to add to ts
	 /// @param inc_tb inc timebase 
	static Int64 add_stable(AVRational^ ts_tb, Int64 ts, AVRational^ inc_tb, Int64 inc);
};
//////////////////////////////////////////////////////
// AVUtil
public ref class AVUtil : public AVSamples
{
public:
	/// Usually reported by demuxer that work on containers that do not provide
	/// either pts or dts.
	static const Int64 NOPTS_VALUE = AV_NOPTS_VALUE;
	/// Internal time base represented as integer
	static const int TIME_BASE = 1000000;
	static const int INPUT_BUFFER_PADDING_SIZE = AV_INPUT_BUFFER_PADDING_SIZE;
	static const int INPUT_BUFFER_MIN_SIZE = AV_INPUT_BUFFER_MIN_SIZE;
protected:
	AVUtil() {}
public:
	///  Return a single letter to describe the given picture type
	///  pict_type.
	/// 
	///  @param[in] pict_type the picture type @return a single character
	///  representing the picture type, '?' if pict_type is unknown
	static char get_picture_type_char(AVPictureType pict_type);

	///  Return a string describing the media_type enum, NULL if media_type
	///  is unknown.
	static String^ get_media_type_string(AVMediaType media_type);

	/// Return the flags which specify extensions supported by the CPU.
	/// The returned value is affected by av_force_cpu_flags() if that was used
	/// before. So av_get_cpu_flags() can easily be used in a application to
	/// detect the enabled cpu flags.
	static AV_CPU_FLAG get_cpu_flags();

	/// Disables cpu detection and forces the specified flags.
	/// -1 is a special case that disables forcing of specific flags.
	static void force_cpu_flags(AV_CPU_FLAG flags);

	/// @return the number of logical CPU cores present.
	static int cpu_count();

	/// Put a description of the AVERROR code errnum in errbuf.
	/// In case of failure the global variable errno is set to indicate the
	/// error. Even in case of failure av_strerror() will print a generic
	/// error message indicating the errnum provided to errbuf.
	static String^ strerror(int errnum);

	/// Get the current time in microseconds.
	static Int64 gettime();

	/// Get the current time in microseconds since some unspecified starting point.
	/// On platforms that support it, the time comes from a monotonic clock
	/// This property makes this time source ideal for measuring relative time.
	/// The returned values may not be monotonic on platforms where a monotonic
	/// clock is not available.
	static Int64 gettime_relative();

	/// Indicates with a boolean result if the av_gettime_relative() time source
	/// is monotonic.
	static bool gettime_relative_is_monotonic();

	/// Sleep for a period of time.  Although the duration is expressed in
    /// microseconds, the actual delay may be rounded to the precision of the
	/// system timer.
	/// 
	/// @param  usec Number of microseconds to sleep.
	/// @return zero on success or (negative) error code.
	static int usleep(UInt32 usec);

	/// Get a seed to use in conjunction with random functions.
	/// This function tries to provide a good seed at a best effort bases.
	/// Its possible to call this function multiple times if more bits are needed.
	/// It can be quite slow, which is why it should only be used as seed for a faster
	/// PRNG. The quality of the seed depends on the platform.
	static UInt32 get_random_seed();

	/// Fill the provided buffer with a string containing a timestamp time
	/// representation.
	///
	/// @param ts the timestamp to represent
	/// @param tb the timebase of the timestamp
	static String^ ts2timestr(Int64 ts, AVRational^ tb);

	/// Fill the provided buffer with a string containing a timestamp time
	/// representation.
	///
	static String^ ts2timestr(Int64 ts);
};
//////////////////////////////////////////////////////
///
/// A single allowed range of values, or a single allowed value.
/// 
public ref class AVOptionRange : public AVBase
{
internal:
	AVOptionRange(void * _pointer,AVBase^ _parent);
public:
	property int _StructureSize { virtual int get() override; }
public:
	property IntPtr str { IntPtr get(); }
	/// Value range.
	/// For string ranges this represents the min/max length.
	/// For dimensions this represents the min/max pixel count or width/height in multi-component case.
	property double value_min { double  get(); }
	property double value_max { double  get(); }
	/// Value's component range.
	/// For string this represents the unicode range for chars, 0-127 limits to ASCII.
	property double component_min { double  get(); }
	property double component_max { double  get(); }
	/// Range flag. If set to "true" the struct encodes a range, if set to "false" a single value.
	property bool is_range { bool get(); }
};
//////////////////////////////////////////////////////
///
/// List of AVOptionRange structs.
///
public ref class AVOptionRanges : public AVBase
								, public System::Collections::IEnumerable
								, public System::Collections::Generic::IEnumerable<AVOptionRange^>
{
private:
	ref class AVOptionRangesEnumerator : public System::Collections::IEnumerator
		, public System::Collections::Generic::IEnumerator<AVOptionRange^>
	{
	protected:
		int				m_nIndex;
		AVOptionRanges^ m_pRanges;
	public:
		AVOptionRangesEnumerator(AVOptionRanges^ _range);
		~AVOptionRangesEnumerator();
	public:
		// IEnumerator
		virtual bool MoveNext();
		virtual property AVOptionRange^ Current { AVOptionRange^ get (); }
		virtual void Reset();
		virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
	};
internal:
	AVOptionRanges(void * _pointer,AVBase^ _parent);
public:
	property int _StructureSize { virtual int get() override; }
public:
	// Array of option ranges.
	property array<AVOptionRange^>^ range { array<AVOptionRange^>^ get(); }
	// Number of ranges per component.
	property int nb_ranges { int get(); }
	// Number of components.
	property int nb_components { int get(); }
public:
	property AVOptionRange^ default[int,int] { AVOptionRange^ get(int _component,int _range); }
public:
	// IEnumerable
	virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator;
public:
	// IEnumerable<AVOptionRange>
	virtual System::Collections::Generic::IEnumerator<AVOptionRange^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVOptionRange^>::GetEnumerator;
};
//////////////////////////////////////////////////////
// AVOption
public ref class AVOption : public AVBase
{
internal:
	AVOption(void * _pointer,AVBase^ parent);
public:
	property int _StructureSize { virtual int get() override; }
public:
	property String^ name { String^ get(); }
	/// short English help text
	property String^ help { String^ get(); }
	/// The offset relative to the context structure where the option
	/// value is stored. It should be 0 for named constants.
	property int offset { int get(); }
	property AVOptionType type { AVOptionType get(); }
	/// the default value for scalar options
	property IntPtr default_val { IntPtr get(); }
	///< minimum valid value for the option
	property double min { double get(); }
	///< maximum valid value for the option
	property double max { double get(); }
	property AVOptFlag flags{ AVOptFlag get(); }
	property String^ unit { String^ get(); }
public:
	virtual String^ ToString() override;
};
//////////////////////////////////////////////////////
// AVOptions
public ref class AVOptions : public AVBase
	, public System::Collections::IEnumerable
	, public System::Collections::Generic::IEnumerable<AVOption^>
{
private:
	ref class Enumerator
		: public System::Collections::IEnumerator
		, public System::Collections::Generic::IEnumerator<AVOption^>
	{
	protected:
		IntPtr		m_pOption;
		AVOptions^	m_pParent;
	public:
		Enumerator(AVOptions^ parent);
		~Enumerator();
	public:
		// IEnumerator
		virtual bool MoveNext();
		virtual property AVOption^ Current { AVOption^ get (); }
		virtual void Reset();
		virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
	};
internal:
	AVOptions(void * _pointer,AVBase^ parent);
public:
	AVOptions(IntPtr obj);
	AVOptions(AVBase^ obj);
public:
	// Retrieve Number Of Options
	property int Count { int get(); }
	// Access Option by index
	property AVOption^ default[int] { AVOption^ get(int); }
	property AVOption^ default[String^] { AVOption^ get(String^); }
public:
	bool Contains(String^ name);
	int IndexOf(String^ name);
	int IndexOf(AVOption^ value);
public:
	virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { return gcnew Enumerator(this); }
	virtual System::Collections::Generic::IEnumerator<AVOption^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVOption^>::GetEnumerator { return gcnew Enumerator(this); }
public:
	/// Set the values of all AVOption fields to their default values.
	///
	/// @param s an AVOption-enabled struct (its first member must be a pointer to AVClass)
	void set_defaults();
	/// Parse the key/value pairs list in opts. For each key/value pair
	/// found, stores the value in the field in ctx that is named like the
	/// key. ctx must be an AVClass context, storing is done using
	/// AVOptions.
	///
	/// @param opts options string to parse, may be NULL
	/// @param key_val_sep a 0-terminated list of characters used to
	/// separate key from value
	/// @param pairs_sep a 0-terminated list of characters used to separate
	/// two pairs from each other
	/// @return the number of successfully set key/value pairs, or a negative
	/// value corresponding to an AVERROR code in case of error:
	/// AVERROR(EINVAL) if opts cannot be parsed,
	/// the error code issued by av_opt_set() if a key/value pair
	/// cannot be set
	AVRESULT set_options_string(String^ opts,String^ key_val_sep, String^ pairs_sep);
	/// Free all allocated objects in obj.
	void opt_free();
	/// Check whether a particular flag is set in a flags field.
	///
	/// @param field_name the name of the flag field option
	/// @param flag_name the name of the flag to check
	/// @return non-zero if the flag is set, zero if the flag isn't set,
	///         isn't of the right type, or the flags field doesn't exist.
	bool opt_flag_is_set(String^ field_name, String^ flag_name);
	/// Set all the options from a given dictionary on an object.
	///
	/// @param obj a struct whose first element is a pointer to AVClass
	/// @param options options to process. This dictionary will be freed and replaced
	///                by a new one containing all options not found in obj.
	///                Of course this new dictionary needs to be freed by caller
	///                with av_dict_free().
	///
	/// @return 0 on success, a negative AVERROR if some option was found in obj,
	///         but could not be set.
	///
	/// @see av_dict_copy()
	AVRESULT set_dict(AVDictionary^ options);
	/// Set all the options from a given dictionary on an object.
	///
	/// @param obj a struct whose first element is a pointer to AVClass
	/// @param options options to process. This dictionary will be freed and replaced
	///                by a new one containing all options not found in obj.
	///                Of course this new dictionary needs to be freed by caller
	///                with av_dict_free().
	/// @param search_flags A combination of AVOptSearch.
	///
	/// @return 0 on success, a negative AVERROR if some option was found in obj,
	///         but could not be set.
	///
	/// @see av_dict_copy()
	AVRESULT set_dict(AVDictionary^ options,AVOptSearch search_flags);
	/// @defgroup opt_eval_funcs Evaluating option strings
	/// @{
	/// This group of functions can be used to evaluate option strings
	/// and get numbers out of them. They do the same thing as av_opt_set(),
	/// except the result is written into the caller-supplied pointer.
	///
	/// @param obj a struct whose first element is a pointer to AVClass.
	/// @param o an option for which the string is to be evaluated.
	/// @param val string to be evaluated.
	/// @param *_out value of the string will be written here.
	///
	/// @return 0 on success, a negative number on failure.
	AVRESULT eval_flags (AVOption^ o, String^ val, [Out] int		 % flags_out);
	AVRESULT eval_int   (AVOption^ o, String^ val, [Out] int		 % int_out);
	AVRESULT eval_int64 (AVOption^ o, String^ val, [Out] Int64		 % int64_out);
	AVRESULT eval_float (AVOption^ o, String^ val, [Out] float		 % float_out);
	AVRESULT eval_double(AVOption^ o, String^ val, [Out] double		 % double_out);
	AVRESULT eval_q     (AVOption^ o, String^ val, [Out] AVRational^ % q_out);

	/// Look for an option in an object. Consider only options which
	/// have all the specified flags set.
	///
	/// @param[in] obj A pointer to a struct whose first element is a
	///                pointer to an AVClass.
	///                Alternatively a double pointer to an AVClass, if
	///                AV_OPT_SEARCH_FAKE_OBJ search flag is set.
	/// @param[in] name The name of the option to look for.
	/// @param[in] unit When searching for named constants, name of the unit
	///                 it belongs to.
	/// @param opt_flags Find only options with all the specified flags set (AV_OPT_FLAG).
	/// @param search_flags A combination of AV_OPT_SEARCH_*.
	///
	/// @return A pointer to the option found, or NULL if no option
	///         was found.
	///
	/// @note Options found with AV_OPT_SEARCH_CHILDREN flag may not be settable
	/// directly with av_opt_set(). Use special calls which take an options
	/// AVDictionary (e.g. avformat_open_input()) to set options found with this
	/// flag.
	AVOption^ Find(String^ name,String^ unit,AVOptFlag opt_flags, AVOptSearch search_flags);
	/// Look for an option in an object. Consider only options which
	/// have all the specified flags set.
	///
	/// @param[in] obj A pointer to a struct whose first element is a
	///                pointer to an AVClass.
	///                Alternatively a double pointer to an AVClass, if
	///                AV_OPT_SEARCH_FAKE_OBJ search flag is set.
	/// @param[in] name The name of the option to look for.
	/// @param[in] unit When searching for named constants, name of the unit
	///                 it belongs to.
	/// @param opt_flags Find only options with all the specified flags set (AV_OPT_FLAG).
	/// @param search_flags A combination of AV_OPT_SEARCH_*.
	/// @param[out] target_obj if non-NULL, an object to which the option belongs will be
	/// written here. It may be different from obj if AV_OPT_SEARCH_CHILDREN is present
	/// in search_flags. This parameter is ignored if search_flags contain
	/// AV_OPT_SEARCH_FAKE_OBJ.
	///
	/// @return A pointer to the option found, or NULL if no option
	///         was found.
	AVOption^ Find(String^ name,String^ unit,AVOptFlag opt_flags, AVOptSearch search_flags,AVBase^ % target_obj);
	/// Iterate over all AVOptions belonging to obj.
	///
	/// @param obj an AVOptions-enabled struct or a double pointer to an
	///            AVClass describing it.
	/// @param prev result of the previous call to av_opt_next() on this object
	///             or NULL
	/// @return next AVOption or NULL
	AVOption^ Next(AVOption^ prev);
	/// Iterate over AVOptions-enabled children of obj.
	///
	/// @param prev result of a previous call to this function or NULL
	/// @return next AVOptions-enabled child or NULL
	AVBase^ child_next(AVBase^ prev);
	/// @defgroup opt_set_funcs Option setting functions
	/// @{
	/// Those functions set the field of obj with the given name to value.
	///
	/// @param[in] obj A struct whose first element is a pointer to an AVClass.
	/// @param[in] name the name of the field to set
	/// @param[in] val The value to set. In case of av_opt_set() if the field is not
	/// of a string type, then the given string is parsed.
	/// SI postfixes and some named scalars are supported.
	/// If the field is of a numeric type, it has to be a numeric or named
	/// scalar. Behavior with more than one scalar and +- infix operators
	/// is undefined.
	/// If the field is of a flags type, it has to be a sequence of numeric
	/// scalars or named flags separated by '+' or '-'. Prefixing a flag
	/// with '+' causes it to be set without affecting the other flags;
	/// similarly, '-' unsets a flag.
	/// @param search_flags flags passed to av_opt_find2. I.e. if AV_OPT_SEARCH_CHILDREN
	/// is passed here, then the option may be set on a child of obj.
	///
	/// @return 0 if the value has been set, or an AVERROR code in case of
	/// error:
	/// AVERROR_OPTION_NOT_FOUND if no matching option exists
	/// AVERROR(ERANGE) if the value is out of range
	/// AVERROR(EINVAL) if the value is not valid
	AVRESULT set			(String^ name, String^ val, AVOptSearch search_flags);
	AVRESULT set_int		(String^ name, Int64     val, AVOptSearch search_flags);
	AVRESULT set_double		(String^ name, double      val, AVOptSearch search_flags);
	AVRESULT set_q			(String^ name, AVRational^  val, AVOptSearch search_flags);
	AVRESULT set_bin		(String^ name, IntPtr val, int size, AVOptSearch search_flags);
	AVRESULT set_image_size	(String^ name, int w, int h, AVOptSearch search_flags);
	AVRESULT set_pixel_fmt	(String^ name, AVPixelFormat fmt, AVOptSearch search_flags);
	AVRESULT set_sample_fmt	(String^ name, AVSampleFormat fmt, AVOptSearch search_flags);
	AVRESULT set_video_rate	(String^ name, AVRational^ val, AVOptSearch search_flags);
	AVRESULT set_channel_layout(String^ name, AVChannelLayout ch_layout, AVOptSearch search_flags);
	AVRESULT set_int_list(String^ name, array<int>^ val, AVOptSearch search_flags);
	AVRESULT set_int_list(String^ name, array<Int64>^ val, AVOptSearch search_flags);

	/// @note Any old dictionary present is discarded and replaced with a copy of the new one. The
	/// caller still owns val is and responsible for freeing it.
	AVRESULT set_dict_val(String^ name, AVDictionary^ val, AVOptSearch search_flags);

	/// Those functions get a value of the option with the given name from an object.
	///
	/// @param[in] obj a struct whose first element is a pointer to an AVClass.
	/// @param[in] name name of the option to get.
	/// @param[in] search_flags flags passed to av_opt_find2. I.e. if AV_OPT_SEARCH_CHILDREN
	/// is passed here, then the option may be found in a child of obj.
	/// @param[out] out_val value of the option will be written here
	/// @return >=0 on success, a negative error code otherwise
	AVRESULT get			(String^ name, AVOptSearch search_flags, [Out] String^  % out_val);
	AVRESULT get_int		(String^ name, AVOptSearch search_flags, [Out] Int64    % out_val);
	AVRESULT get_double		(String^ name, AVOptSearch search_flags, [Out] double    % out_val);
	AVRESULT get_q			(String^ name, AVOptSearch search_flags, [Out] AVRational^ % out_val);
	AVRESULT get_image_size	(String^ name, AVOptSearch search_flags, [Out] int % w_out, [Out] int % h_out);
	AVRESULT get_pixel_fmt	(String^ name, AVOptSearch search_flags, [Out] AVPixelFormat % out_fmt);
	AVRESULT get_sample_fmt	(String^ name, AVOptSearch search_flags, [Out] AVSampleFormat % out_fmt);
	AVRESULT get_video_rate	(String^ name, AVOptSearch search_flags, [Out] AVRational^ % out_val);
	AVRESULT get_channel_layout(String^ name, AVOptSearch search_flags, [Out] AVChannelLayout % ch_layout);

	/// @param[out] out_val The returned dictionary is a copy of the actual value and must
	/// be freed with av_dict_free() by the caller
	AVRESULT get_dict_val(String^ name, AVOptSearch search_flags, [Out] AVDictionary^ % out_val);

	/// Gets a pointer to the requested field in a struct.
	/// This function allows accessing a struct even when its fields are moved or
	/// renamed since the application making the access has been compiled,
	///
	/// @returns a pointer to the field, it can be cast to the correct type and read
	///          or written to.
	IntPtr ptr(AVClass^ avclass, String^ name);

	/// Get a list of allowed ranges for the given option.
	///
	/// The returned list may depend on other fields in obj like for example profile.
	///
	/// @param flags is a bitmask of flags, undefined flags should not be set and should be ignored
	///              AV_OPT_SEARCH_FAKE_OBJ indicates that the obj is a double pointer to a AVClass instead of a full instance
	///              AV_OPT_MULTI_COMPONENT_RANGE indicates that function may return more than one component, @see AVOptionRanges
	///
	/// The result must be freed with av_opt_freep_ranges.
	///
	/// @return number of compontents returned on success, a negative errro code otherwise
	int query_ranges([Out] AVOptionRanges^ % ranges, String^ key, AVOptFlag flags);

	/// Copy options from src object into dest object.
	///
	/// Options that require memory allocation (e.g. string or binary) are malloc'ed in dest object.
	/// Original memory allocated for such options is freed unless both src and dest options points to the same memory.
	///
	/// @param dest Object to copy from
	/// @param src  Object to copy into
	/// @return 0 on success, negative on error
	AVRESULT CopyTo(AVBase^ dest);

	/// Get a default list of allowed ranges for the given option.
	///
	/// This list is constructed without using the AVClass.query_ranges() callback
	/// and can be used as fallback from within the callback.
	///
	/// @param flags is a bitmask of flags, undefined flags should not be set and should be ignored
	///              AV_OPT_SEARCH_FAKE_OBJ indicates that the obj is a double pointer to a AVClass instead of a full instance
	///              AV_OPT_MULTI_COMPONENT_RANGE indicates that function may return more than one component, @see AVOptionRanges
	///
	/// The result must be freed with av_opt_free_ranges.
	///
	/// @return number of compontents returned on success, a negative errro code otherwise
	int query_ranges_default([Out] AVOptionRanges^ % ranges, String^ key, AVOptFlag flags);

	/// Check if given option is set to its default value.
	///
	/// Options o must belong to the obj. This function must not be called to check child's options state.
	/// @see av_opt_is_set_to_default_by_name().
	///
	/// @param obj  AVClass object to check option on
	/// @param o    option to be checked
	/// @return     >0 when option is set to its default,
	///              0 when option is not set its default,
	///             <0 on error
	int is_set_to_default(AVOption^ o);

	/// Check if given option is set to its default value.
	///
	/// @param obj          AVClass object to check option on
	/// @param name         option name
	/// @param search_flags combination of AV_OPT_SEARCH_*
	/// @return             >0 when option is set to its default,
	///                     0 when option is not set its default,
	///                     <0 on error
	int is_set_to_default(String^ name, AVOptSearch search_flags);

	/// Serialize object's options.
	///
	/// Create a string containing object's serialized options.
	/// Such string may be passed back to av_opt_set_from_string() in order to restore option values.
	/// A key/value or pairs separator occurring in the serialized value or
	/// name string are escaped through the av_escape() function.
	///
	/// @param[in]  obj           AVClass object to serialize
	/// @param[in]  opt_flags     serialize options with all the specified flags set (AV_OPT_FLAG)
	/// @param[in]  flags         combination of AV_OPT_SERIALIZE_/// flags
	/// @param[out] buffer        Pointer to buffer that will be allocated with string containg serialized options.
	///                           Buffer must be freed by the caller when is no longer needed.
	/// @param[in]  key_val_sep   character used to separate key from value
	/// @param[in]  pairs_sep     character used to separate two pairs from each other
	/// @return                   >= 0 on success, negative on error
	/// @warning Separators cannot be neither '\\' nor '\0'. They also cannot be the same.
	AVRESULT Serialize(AVOptFlag opt_flags, AVOptSerializeFlags flags, [Out] String^ % buffer, Char key_val_sep, Char pairs_sep);
};
//////////////////////////////////////////////////////
// AVClass
public ref class AVClass : public AVBase
{
private:
	ref class Enumerator
		: public System::Collections::IEnumerator
		, public System::Collections::Generic::IEnumerator<AVClass^>
	{
	protected:
		IntPtr		m_pOpaque;
		AVClass^	m_pParent;
		AVClass^	m_pCurrent;
	public:
		Enumerator(AVClass^ parent);
		~Enumerator();
	public:
		// IEnumerator
		virtual bool MoveNext();
		virtual property AVClass^ Current { AVClass^ get (); }
		virtual void Reset();
		virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
	};
public:
	/// Iterate over potential AVOptions-enabled children of parent.
	///
	/// @param prev result of a previous call to this function or NULL
	/// @return AVClass corresponding to next potential child or NULL
	ref class AVClasses
		: public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<AVClass^>
	{
	protected:
		AVClass^ m_pParent;
	public:
		AVClasses(AVClass^ parent) { m_pParent = parent; }
		~AVClasses() {}
	public:
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { return gcnew Enumerator(m_pParent); }
		virtual System::Collections::Generic::IEnumerator<AVClass^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVClass^>::GetEnumerator { return gcnew Enumerator(m_pParent); }
	};
internal:
	AVClass(void * _pointer,AVBase^ _parent);
public:
	property String^ class_name { String^ get(); }
	property array<AVOption^>^ option { array<AVOption^>^ get(); }
	property int version { int get(); }
	property int log_level_offset_offset { int get(); }
	property int parent_log_context_offset { int get(); }
	property AVClassCategory category { AVClassCategory get(); }
public:
	virtual String^ ToString() override;
public:
	property int _StructureSize { virtual int get() override; }
public:
	property AVClasses^ Children { AVClasses^ get() { return gcnew AVClasses(this); } }
public:
	static AVClass^ GetCodecClass();
	static AVClass^ GetFrameClass();
	static AVClass^ GetSubtitleRectClass();
	static AVClass^ GetFormatClass();
	static AVClass^ GetSwsClass();
	static AVClass^ GetSwrClass();
};
//////////////////////////////////////////////////////
// AVProfile
public ref class AVProfile : public AVBase 
{ 
internal:
	AVProfile(void * _pointer,AVBase^ _parent);
public:
	property Profile profile { Profile get(); }
	property String^ name { String^ get(); }
public:
	virtual String^ ToString() override;
public:
	property int _StructureSize { virtual int get() override; }
};
//////////////////////////////////////////////////////
// AVCodecDescriptor
/// This struct describes the properties of a single codec described by an AVCodecID.
public ref class AVCodecDescriptor : public AVBase
{
public:
	ref class AVMimeTypes
		: public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<String^>
	{
	private:
		ref class AVMimeTypesEnumerator
			: public System::Collections::IEnumerator
			, public System::Collections::Generic::IEnumerator<String^>
		{
		internal:
			// Target Index
			int				m_nIndex;
			// Formats Access
			AVCodecDescriptor^	m_pDescriptor;
		public:
			AVMimeTypesEnumerator(AVCodecDescriptor^ _descriptor);
			~AVMimeTypesEnumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property String^ Current { String^ get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
		};
	private:
		AVCodecDescriptor^ m_Parent;
	internal:
		AVMimeTypes(AVCodecDescriptor^ _parent): m_Parent(_parent) {}
	public:
		~AVMimeTypes() {}
	public:
		property String^ default[int] { String^ get(int index) { return m_Parent->GetMimeType(index); } }
		property int Count { int get(); }
	public:
		array<String^>^ ToArray();
	public:
		// IEnumerable
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator;
	public:
		// IEnumerable<String^>
		virtual System::Collections::Generic::IEnumerator<String^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<String^>::GetEnumerator;
	};
public:
	ref class AVProfiles
		: public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<AVProfile^>
	{
	private:
		ref class AVProfilesEnumerator
			: public System::Collections::IEnumerator
			, public System::Collections::Generic::IEnumerator<AVProfile^>
		{
		internal:
			// Target Index
			int					m_nIndex;
			// Formats Access
			AVCodecDescriptor^	m_pDescriptor;
		public:
			AVProfilesEnumerator(AVCodecDescriptor^ _descriptor);
			~AVProfilesEnumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVProfile^ Current { AVProfile^ get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
		};
	private:
		AVCodecDescriptor^ m_Parent;
	internal:
		AVProfiles(AVCodecDescriptor^ _parent): m_Parent(_parent) {}
	public:
		~AVProfiles() {}
	public:
		property AVProfile^ default[int] { AVProfile^ get(int index) { return m_Parent->GetProfile(index); } }
		property int Count { int get(); }
	public:
		array<AVProfile^>^ ToArray();
	public:
		// IEnumerable
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator;
	public:
		// IEnumerable<AVProfile^>
		virtual System::Collections::Generic::IEnumerator<AVProfile^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVProfile^>::GetEnumerator;
	};
public:
	ref class AVCodecDescriptors
		: public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<AVCodecDescriptor^>
	{
		ref class Enumerator
			: public System::Collections::IEnumerator
			, public System::Collections::Generic::IEnumerator<AVCodecDescriptor^>
		{
		protected:
			AVCodecDescriptor^	m_pCurrent;
		public:
			Enumerator() { Reset(); }
			~Enumerator() {}
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVCodecDescriptor^ Current { AVCodecDescriptor^ get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
		};
	public:
		AVCodecDescriptors() {}
		~AVCodecDescriptors() {}
	public:
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { return gcnew Enumerator(); }
		virtual System::Collections::Generic::IEnumerator<AVCodecDescriptor^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVCodecDescriptor^>::GetEnumerator { return gcnew Enumerator(); }
	};
internal:
	AVCodecDescriptor(void * _pointer,AVBase^ _parent);
public:
	property AVCodecID id { AVCodecID get(); }
	property AVMediaType type{ AVMediaType get(); }
	/// Name of the codec described by this descriptor. It is non-empty and
	/// unique for each codec descriptor. It should contain alphanumeric
	/// characters and '_' only.
	property String^ name { String^ get(); }
	// A more descriptive name for this codec. May be NULL.
	property String^ long_name { String^ get(); }
	// Codec properties, a combination of AV_CODEC_PROP_* flags.
	property AVCodecProp props{ AVCodecProp get(); }
	/// MIME type(s) associated with the codec.
	/// May be NULL; if not, a NULL-terminated array of MIME types.
	/// The first item is always non-NULL and is the preferred MIME type.
	property AVMimeTypes^ mime_types{ AVMimeTypes^ get(); }
	/// If non-NULL, an array of profiles recognized for this codec.
	/// Terminated with FF_PROFILE_UNKNOWN.
	property AVProfiles^ profiles{ AVProfiles^ get(); }
public:
	AVCodecDescriptor^ Next();
public:
	virtual String^ ToString() override;
public:
	property int _StructureSize { virtual int get() override; }
public:
	static property AVCodecDescriptors^ Descriptors { AVCodecDescriptors^ get() { return gcnew AVCodecDescriptors(); } }
public:
	static AVCodecDescriptor^ Next(AVCodecDescriptor^ _prev);
	static AVCodecDescriptor^ Find(String^ _name);
	static AVCodecDescriptor^ Find(AVCodecID _id);
private:
	String^ GetMimeType(int n);
	AVProfile^ GetProfile(int n);
};
//////////////////////////////////////////////////////
// AVRational
public ref class AVRational : public AVBase
{
internal:
	AVRational(void * _pointer,AVBase^ _parent);
internal:
	AVRational(const ::AVRational & r);
public:
	AVRational(Int64 a);
	AVRational(int _num, int _den);
public:
	/// <summary>
	/// numerator
	/// </summary>
	property int num { int get(); void set(int); }

	/// <summary>
	/// denominator
	/// </summary>
	property int den { int get(); void set(int); }
public:
	virtual String^ ToString() override;
	virtual int GetHashCode() override;
	virtual bool Equals(Object^ obj) override;
public:
	property int _StructureSize { virtual int get() override; }
public:
	/// <summary>
	/// Convert rational to double.
	/// </summary>
	/// <returns>(double) a</returns>
	double ToDouble();
	
	/// <summary>
	/// Convert rational to long.
	/// </summary>
	/// <returns>(long) a</returns>
	Int64 ToLong();

	/// <summary>
	/// Converts a AVRational to a IEEE 32bit float.
	/// </summary>
	/// <returns>The float is returned in a uint32_t and its value is platform independant.</returns>
	int ToIntFloat();

	/// <summary>
	/// Invert a rational.
	/// </summary>
	/// <returns>1 / q</returns>
	AVRational^ inv_q();

	/// <summary>
	/// Find the nearest value in _list to q.
	/// </summary>
	/// <param name="_list">an array of rationals</param>
	/// <returns>the index of the nearest value found in the array</returns>
	int find_nearest_q_idx(array<AVRational^>^ _list);

	/// <summary>
	/// Check nearest rational
	/// </summary>
	/// <param name="q1">first rational</param>
	/// <param name="q2">second rational</param>
	/// <returns>1 if q1 is nearer to q than q2, -1 if q2 is nearer than q1, 0 if they have the same distance.</returns>
	int nearer_q(AVRational^ q1, AVRational^ q2);

	/// <summary>
	/// Reduce a fraction.
	/// This is useful for framerate calculations.
	/// </summary>
	/// <param name="_num">source numerator</param>
	/// <param name="_den">source denominator</param>
	/// <param name="_max">the maximum allowed for output</param>
	/// <returns>true if exact, false otherwise</returns>
	bool reduce(Int64 _num,Int64 _den,Int64 _max);
public:
	static explicit operator double(AVRational^ );
	static explicit operator Int64(AVRational^ );
internal:
	static explicit operator ::AVRational(AVRational^ );
	static explicit operator ::AVRational*(AVRational^ );
public:
	/// <summary>
	/// Multiply two rationals.
	/// </summary>
	/// <param name="b">first rational</param>
	/// <param name="c">second rational</param>
	/// <returns>b*c</returns>
	static AVRational^ operator * (AVRational^, AVRational^ );
	
	/// <summary>
	/// Divide one rational by another.
	/// </summary>
	/// <param name="b">first rational</param>
	/// <param name="c">second rational</param>
	/// <returns>b/c</returns>
	static AVRational^ operator / (AVRational^, AVRational^ );
	
	/// <summary>
	/// Subtract one rational from another.
	/// </summary>
	/// <param name="b">first rational</param>
	/// <param name="c">second rational</param>
	/// <returns>b-c</returns>
	static AVRational^ operator - (AVRational^, AVRational^ );
	
	/// <summary>
	/// Add two rationals.
	/// </summary>
	/// <param name="b">first rational</param>
	/// <param name="c">second rational</param>
	/// <returns>b+c</returns>
	static AVRational^ operator + (AVRational^, AVRational^ );
public:
	AVRational^ operator += (AVRational^);
	AVRational^ operator -= (AVRational^);
	AVRational^ operator *= (AVRational^);
	AVRational^ operator /= (AVRational^);
public:
	static bool operator == (AVRational^,AVRational^);
	static bool operator != (AVRational^,AVRational^);
public:
	/// <summary>   
	/// Internal time base represented as fractional value
	/// </summary>
	static property AVRational^ AVTimeBaseQ { AVRational^ get(); }
public:
	/// <summary>
	/// Return the fractional representation of the internal time base.
	/// </summary>
	/// <returns>rational value</returns>
	static AVRational^ get_time_base_q();

	/// <summary>
	/// Convert a double precision floating point number to a rational.
	/// inf is expressed as {1,0} or {-1,0} depending on the sign.
	/// </summary>
	/// <param name="d">double to convert</param>
	/// <param name="max">the maximum allowed numerator and denominator</param>
	/// <returns>rational value</returns>
	static AVRational^ d2q(double d, int max);

	/// <summary>
	/// Compare two rationals.
	/// </summary>
	/// <param name="a">First rational</param>
	/// <param name="b">Second rational</param>
	/// <returns>One of the following values:
	///        - 0 if `a == b`
	///        - 1 if `a > b`
	///        - -1 if `a < b`
	///        - `INT_MIN` if one of the values is of the form `0 / 0`
	/// </returns>
	static int cmp_q(AVRational^ a, AVRational^ b);
};
//////////////////////////////////////////////////////
// AVDictionaryEntry
public ref class AVDictionaryEntry : public AVBase
{
internal:
	AVDictionaryEntry(void * _pointer);
	AVDictionaryEntry(void * _pointer,AVBase^ parent);
public:
	property String^ key { String^ get(); }
	property String^ value { String^ get(); }
public:
	virtual String^ ToString() override;
};
//////////////////////////////////////////////////////
///AVDictionary
///
///Dictionaries are used for storing key:value pairs. To create
///an AVDictionary, simply pass an address of a NULL pointer to
///av_dict_set(). NULL can be used as an empty dictionary wherever
///a pointer to an AVDictionary is required.
///Use av_dict_get() to retrieve an entry or iterate over all
///entries and finally av_dict_free() to free the dictionary
///and all its contents.
///
public ref class AVDictionary : public AVBase
							  , public System::ICloneable
							  , public System::Collections::IEnumerable
							  , public System::Collections::Generic::IEnumerable<AVDictionaryEntry^>
{
private:
	ref class AVDictionaryEnumerator : public System::Collections::IEnumerator
									 , public System::Collections::Generic::IEnumerator<AVDictionaryEntry^>
	{
	protected:
		void * m_pEntry;
		AVDictionary^ m_pDictionary;
	public:
		AVDictionaryEnumerator(AVDictionary^ _dictionary);
		~AVDictionaryEnumerator();
	public:
		// IEnumerator
		virtual bool MoveNext();
		virtual property AVDictionaryEntry^ Current { AVDictionaryEntry^ get (); }
		virtual void Reset();
		virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
	};
public:
	ref class AVCollection
		: public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<String^>
	{
	private:
		ref class AVCollectionEnumerator : public System::Collections::IEnumerator
										 , public System::Collections::Generic::IEnumerator<String^>
		{
		protected:
			int				m_nIndex;
			AVCollection^	m_pCollection;
		public:
			AVCollectionEnumerator(AVCollection^ _collection): m_pCollection(_collection),m_nIndex(-1) {}
			~AVCollectionEnumerator(){}
		public:
			// IEnumerator
			virtual bool MoveNext() { m_nIndex++; return (m_nIndex >= 0 && m_nIndex < m_pCollection->m_pDictionary->Count); }
			virtual property String^ Current { String^ get () { if (m_nIndex == -1) MoveNext(); return (m_nIndex >= 0 && m_nIndex < m_pCollection->m_pDictionary->Count) ? m_pCollection->GetItem(m_nIndex) : nullptr; } }
			virtual void Reset() { m_nIndex = -1; }
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get { return (Object^)Current::get(); } }
		};
	protected:
		AVDictionary^ m_pDictionary;
	internal:
		AVCollection(AVDictionary^ _dictionary) : m_pDictionary(_dictionary) {}
	protected:
		virtual String^ GetItem(int _index) { return nullptr; }
	public:
		property String^ default[int] { String^ get(int index) { return GetItem(index); } }
		property int Count { int get() { return m_pDictionary->Count; } }
	public:
		// IEnumerable
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { return gcnew AVCollectionEnumerator(this); }
	public:
		// IEnumerable<String>
		virtual System::Collections::Generic::IEnumerator<String^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<String^>::GetEnumerator { return gcnew AVCollectionEnumerator(this); }
	};
public:
	ref class AVKeys : public AVCollection
	{
	internal:
		AVKeys(AVDictionary^ _dictionary) : AVCollection(_dictionary) {}
	protected:
		virtual String^ GetItem(int _index) override { return m_pDictionary->GetKey(_index); }
	};
public:
	ref class AVValues : public AVCollection
	{
	internal:
		AVValues(AVDictionary^ _dictionary) : AVCollection(_dictionary) {}
	protected:
		virtual String^ GetItem(int _index) override { return m_pDictionary->GetValue(_index); }
	public:
		property String^ default[int] { String^ get(int index) { return m_pDictionary->GetValue(index); } void set(int index,String^ value) { m_pDictionary->SetValue(m_pDictionary->GetKey(index),value); } }
		property String^ default[String^] { String^ get(String^ key) { return m_pDictionary->GetValue(key); }  void set(String^ key,String^ value) { m_pDictionary->SetValue(key,value); } }
	};
internal:
	AVDictionary(void * _pointer,AVBase^ _parent);
public:
	AVDictionary();
	AVDictionary(AVDictionary^ _dictionary,AVDictFlags _flags);
public:
	property int Count { int get(); }
	property String^ default[int] { String^ get(int); void set(int,String^); }
	property String^ default[String^] { String^ get(String^); void set(String^,String^); }
	property AVKeys^ Keys { AVKeys^ get(); }
	property AVValues^ Values { AVValues^ get(); }
public:
	int GetCount();
	String^ GetKey(int _index);
	String^ GetValue(int _index);
	String^ GetValue(String^ _key);
	void SetValue(String^ _key,String^ _value);
	void SetValue(String^ _key,String^ _value, AVDictFlags _flags);
	int IndexOf(String^ _key);
	bool IsExists(String^ _key);
	AVDictionaryEntry^ Find(String^ _key);
	AVDictionaryEntry^ Find(String^ _key, AVDictFlags _flags);
public:
	// IEnumerable
	virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator;
public:
	// IEnumerable<AVDictionaryEntry>
	virtual System::Collections::Generic::IEnumerator<AVDictionaryEntry^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVDictionaryEntry^>::GetEnumerator;
public:
	// ICloneable
	virtual Object^ Clone();
};
//////////////////////////////////////////////////////
public delegate void AVLogCallback(AVBase^ avcl, int level, String^ fmt,IntPtr vl);
//////////////////////////////////////////////////////
// AVLog
public ref class AVLog
{
private:
	static Object^ s_csCallbackLock = gcnew Object();
	static AVLogCallback^ s_pCallback = nullptr;
private:
	AVLog() {};
public:
	//
	 //Sets additional colors for extended debugging sessions.
	 //@code
	 // av_log(ctx, AV_LOG_DEBUG|AV_LOG_C(134), "Message in purple\n");
	 // @endcode
	 //Requires 256color terminal support. Uses outside debugging is not
	 //recommended.
	 //
	static AVLogLevel LOG_C(int x) { return (AVLogLevel)((x) << 8); } 

	 // Send the specified message to the log if the level is less than or equal
	 // to the current av_log_level. By default, all logging messages are sent to
	 // stderr. This behavior can be altered by setting a different logging callback
	 // function.
	 // @see av_log_set_callback
	 //
	 // @param avcl A pointer to an arbitrary struct of which the first field is a
	 //        pointer to an AVClass struct or NULL if general log.
	 // @param level The importance level of the message expressed using a @ref
	 //        lavu_log_constants "Logging Constant".
	 // @param fmt The format string (printf-compatible) that specifies how
	 //        subsequent arguments are converted to output.
	static void log(AVLogLevel level, String^ text);
	static void log(AVBase^ avcl,AVLogLevel level, String^ text);
	static void log(IntPtr avcl,AVLogLevel level, String^ text);

	static String^ default_item_name(IntPtr avcl);
	static String^ default_item_name(AVBase^ avcl);
	static AVClassCategory default_get_category(IntPtr ptr);
	static AVClassCategory default_get_category(AVBase^ ptr);

	// Get / Set the current log level
	static property AVLogLevel Level { AVLogLevel get(); void set(AVLogLevel);  }
	static property AVLogFlags Flags { AVLogFlags get(); void set(AVLogFlags);  }
public:
	// Log callback
	static property AVLogCallback^ Callback { AVLogCallback^ get(); void set(AVLogCallback^);  }
public:
	/// Send a nice hexadecimal dump of a buffer to the log.
	///
	/// @param avcl A pointer to an arbitrary struct of which the first field is a
	/// pointer to an AVClass struct.
	/// @param level The importance level of the message, lower values signifying
	/// higher importance.
	/// @param buf buffer
	/// @param size buffer size
	///
	/// @see av_hex_dump, av_pkt_dump2, av_pkt_dump_log2
	static void hex_dump_log(AVBase^ avcl, int level, IntPtr buf, int size);

	/// Send a nice dump of a packet to the log.
	///
	/// @param avcl A pointer to an arbitrary struct of which the first field is a
	/// pointer to an AVClass struct.
	/// @param level The importance level of the message, lower values signifying
	/// higher importance.
	/// @param pkt packet to dump
	/// @param dump_payload True if the payload must be displayed, too.
	/// @param st AVStream that the packet belongs to
	static void pkt_dump_log(AVBase^ avcl, int level, AVPacket^ pkt, int dump_payload,AVStream^ st);
};
//////////////////////////////////////////////////////
// AVMotionVector
public ref class AVMotionVector : public AVBase
{
internal:
	AVMotionVector(void * _pointer,AVBase^ _parent);
public:
	AVMotionVector();
public:
	property int _StructureSize { virtual int get() override; }
public:
	/// Where the current macroblock comes from; negative value when it comes
    /// from the past, positive value when it comes from the future.
    /// XXX: set exact relative ref frame reference instead of a +/- 1 "direction".
	property Int32 source { Int32 get(); void set(Int32); }
    /// Width and height of the block.
    property Byte w { Byte get(); void set(Byte); }
	property Byte h { Byte get(); void set(Byte); }
    /// Absolute source position. Can be outside the frame area.
    property Int16 src_x { Int16 get(); void set(Int16); }
	property Int16 src_y { Int16 get(); void set(Int16); }
    /// Absolute destination position. Can be outside the frame area.
	property Int16 dst_x { Int16 get(); void set(Int16); }
	property Int16 dst_y { Int16 get(); void set(Int16); }
    /// Extra flag information.
    /// Currently unused.
    property UInt64 flags { UInt64 get(); void set(UInt64); }
};
//////////////////////////////////////////////////////
// AVCRC
public ref class AVCRC : public AVBase
{
public:
	AVCRC(AVCRCId id);
	AVCRC(IntPtr ctx,bool le, int bits, UInt32 poly, int ctx_size);
public:
	UInt32 CRC(UInt32 crc,IntPtr buffer, int length);
	UInt32 CRC(IntPtr buffer, int length);
	UInt32 CRC(UInt32 crc,array<byte>^ buffer);
	UInt32 CRC(UInt32 crc,array<byte>^ buffer,int count);
	UInt32 CRC(UInt32 crc,array<byte>^ buffer,int index, int count);
	UInt32 CRC(array<byte>^ buffer);
	UInt32 CRC(array<byte>^ buffer,int count);
	UInt32 CRC(array<byte>^ buffer,int index, int count);
public: 
	static UInt32 CRC(AVCRCId crc_id, UInt32 crc,IntPtr buffer, int length);
	static UInt32 CRC(AVCRCId crc_id, UInt32 crc,array<byte>^ buffer, int index, int count);
};
//////////////////////////////////////////////////////
// AVAudioFifo
public ref class AVAudioFifo : public AVBase
{
public:
	/// Allocate an AVAudioFifo.
	/// 
	/// @param sample_fmt  sample format
	/// @param channels    number of channels
	/// @param nb_samples  initial allocation size, in samples
	AVAudioFifo(AVSampleFormat sample_fmt, int channels,int nb_samples);
public:
	/// Get the current number of samples in the AVAudioFifo available for reading.
	/// @return    number of samples available for reading
	property int Size { int get(); }
	/// Get the current number of samples in the AVAudioFifo available for writing.
	/// @return    number of samples available for writing
	property int Space { int get(); }
public:
	/// Reallocate an AVAudioFifo.
	/// 
	/// @param af          AVAudioFifo to reallocate
	/// @param nb_samples  new allocation size, in samples
	/// @return            0 if OK, or negative AVERROR code on failure
	AVRESULT Realloc(int nb_samples);
	/// Reset the AVAudioFifo buffer.
	/// 
	/// This empties all data in the buffer.
	void Reset();
	/// Drain data from an AVAudioFifo.
	/// 
	/// Removes the data without reading it.
	/// 
	/// @param nb_samples  number of samples to drain
	/// @return            0 if OK, or negative AVERROR code on failure
	AVRESULT Drain(int nb_samples);
public:
	/// Write data to an AVAudioFifo.
	///
	/// The AVAudioFifo will be reallocated automatically if the available space
	/// is less than nb_samples.
	///
	/// @see enum AVSampleFormat
	/// The documentation for AVSampleFormat describes the data layout.
	///
	/// @param data        audio data plane pointers
	/// @param nb_samples  number of samples to write
	/// @return            number of samples actually written, or negative AVERROR
	///                    code on failure. If successful, the number of samples
	///                    actually written will always be nb_samples.
	int Write(array<IntPtr>^ data, int nb_samples);
	int Write(IntPtr data, int nb_samples);
	int Write(AVArray<IntPtr>^ data, int nb_samples);
	/// Read data from an AVAudioFifo.
	///
	/// @see enum AVSampleFormat
	/// The documentation for AVSampleFormat describes the data layout.
	///
	/// @param af          AVAudioFifo to read from
	/// @param data        audio data plane pointers
	/// @param nb_samples  number of samples to read
	/// @return            number of samples actually read, or negative AVERROR code
	///                    on failure. The number of samples actually read will not
	///                    be greater than nb_samples, and will only be less than
	///                    nb_samples if av_audio_fifo_size is less than nb_samples.
	int Read(array<IntPtr>^ data, int nb_samples);
	int Read(IntPtr data, int nb_samples);
	int Read(AVArray<IntPtr>^ data, int nb_samples);
};
//////////////////////////////////////////////////////
public delegate void AVFifoReadDelegate(IntPtr, IntPtr, int);
public delegate int AVFifoWriteDelegate(IntPtr, IntPtr, int);
//////////////////////////////////////////////////////
// AVFifoBuffer
public ref class AVFifoBuffer : public AVBase
{
public:
	/// Initialize an AVFifoBuffer.
	/// @param size of FIFO
	AVFifoBuffer(int _size);
	/// Initialize an AVFifoBuffer.
	/// @param nmemb number of elements
	/// @param size  size of the single element
	AVFifoBuffer(int nmemb, int size);
public:
	/// Return the amount of data in bytes in the AVFifoBuffer, that is the
	/// amount of data you can read from it.
	/// @return size
	property int Size { int get(); }
	/// Return the amount of space in bytes in the AVFifoBuffer, that is the
	/// amount of data you can write into it.
	/// @return size
	property int Space { int get(); }
public:
	/// Reset the AVFifoBuffer to the state right after av_fifo_alloc, in particular it is emptied.
	void Reset();
	/// Resize an AVFifoBuffer.
	/// In case of reallocation failure, the old FIFO is kept unchanged.
	///
	/// @param size new AVFifoBuffer size in bytes
	/// @return <0 for failure, >=0 otherwise
	int Realloc(int size);
	/// Enlarge an AVFifoBuffer.
	/// In case of reallocation failure, the old FIFO is kept unchanged.
	/// The new fifo size may be larger than the requested size.
	///
	/// @param additional_space the amount of space in bytes to allocate in addition to av_fifo_size()
	/// @return <0 for failure, >=0 otherwise
	int Grow(int additional_space);
	/// Read and discard the specified amount of data from an AVFifoBuffer.
	/// @param size amount of data to read in bytes
	void Drain(int size);
	/// Return a pointer to the data stored in a FIFO buffer at a certain offset.
	/// The FIFO buffer is not modified.
	///
	/// @param offs an offset in bytes, its absolute value must be less
	///             than the used buffer size or the returned pointer will
	///             point outside to the buffer data.
	///             The used buffer size can be checked with av_fifo_size().
	IntPtr Peek(int offs);
public:
	/// Feed data from an AVFifoBuffer to a user-supplied callback.
	/// @param f AVFifoBuffer to read from
	/// @param buf_size number of bytes to read
	/// @param func generic read function
	/// @param dest data destination
	int Read(IntPtr dest, int buf_size, AVFifoReadDelegate^ func);

	/// Feed data from a user-supplied callback to an AVFifoBuffer.
	/// @param f AVFifoBuffer to write to
	/// @param src data source; non-const since it may be used as a
	/// modifiable context by the function defined in func
	/// @param size number of bytes to write
	/// @param func generic write function; the first parameter is src,
	/// the second is dest_buf, the third is dest_buf_size.
	/// func must return the number of bytes written to dest_buf, or <= 0 to
	/// indicate no more data available to write.
	/// If func is NULL, src is interpreted as a simple byte array for source data.
	/// @return the number of bytes written to the FIFO
	int Write(IntPtr src, int size, AVFifoWriteDelegate^ func);
};
//////////////////////////////////////////////////////
// AVStereo3D
public ref class AVStereo3D : public AVBase
{
internal:
	AVStereo3D(void * _pointer,AVBase^ _parent);
public:
	AVStereo3D();
public:
	property int _StructureSize { virtual int get() override; }
public:
	// How views are packed within the video.
	property AVStereo3DType type { AVStereo3DType get(); void set(AVStereo3DType); }
	 // Additional information about the frame packing.
    property AVStereo3DFlags flags { AVStereo3DFlags get(); void set(AVStereo3DFlags); }
public:
	/// Allocate a complete AVFrameSideData and add it to the frame.
	///
	/// @param frame The frame which side data is added to.
	///
	/// @return The AVStereo3D structure to be filled by caller.
	static AVStereo3D^ CreateSideData(AVFrame^ frame);
};
//////////////////////////////////////////////////////
// AVMD5
public ref class AVMD5 : public AVBase
{
public:
	///<summary>
	/// Allocate an AVMD5 context.
	///</summary>
	AVMD5();
public:
	/// Initialize MD5 hashing.
	void Reset();
	// Update hash value.
	void Update(IntPtr src, int length);
	void Update(array<byte>^ src);
	void Update(array<byte>^ src, int length);
	void Update(array<byte>^ src, int start, int length);
	// Finish hashing and output digest value.
	void Final(IntPtr dst);
	void Final([Out] array<byte>^ % dst);
public:
	///<summary>
	/// Hash an array of data.
	///
	/// @param dst The output buffer to write the digest into
	/// @param src The data to hash
	/// @param len The length of the data, in bytes
	///</summary>
	static void sum(IntPtr dst, IntPtr src, int len);
	static void sum(IntPtr dst, array<byte>^ src, int start, int len);
	static void sum(IntPtr dst, array<byte>^ src, int len);
	static void sum(IntPtr dst, array<byte>^ src);
	static void sum([Out] array<byte>^ % dst, array<byte>^ src, int start, int len);
	static void sum([Out] array<byte>^ % dst, array<byte>^ src, int len);
	static void sum([Out] array<byte>^ % dst, array<byte>^ src);
};
//////////////////////////////////////////////////////
// AVHashContext
public ref class AVHashContext : public AVBase
{
public:
	const int MAX_SIZE = AV_HASH_MAX_SIZE;
public:
	AVHashContext(String^ name);
public:
	property String^ name { String^ get(); }
	property int size { int get(); }
public:
	virtual String^ ToString() override;
public:
	void Reset();
	// Update hash value.
	void Update(IntPtr src, int length);
	void Update(array<byte>^ src);
	void Update(array<byte>^ src, int length);
	void Update(array<byte>^ src, int start, int length);
	void Update(AVMemPtr^ ptr, int length);
	void Update(AVMemPtr^ ptr);
	// Finish hashing and output digest value.
	void Final(IntPtr dst);
	void Final([Out] array<byte>^ % dst);
	void Final([Out] AVMemPtr^ % dst);
public:
	void FinalBin(IntPtr dst,int size);
	void FinalHex([Out] String^ % dst);
	void FinalB64([Out] String^ % dst);
public:
	static property array<String^>^ Names { array<String^>^ get(); }
};
//////////////////////////////////////////////////////
public value class FFTSample
{
private:
	float m_fValue;
public:
	FFTSample(float val) : m_fValue(val) {}
public:
	static bool operator == (FFTSample a,FFTSample b)  { return a.m_fValue == b.m_fValue; }
	static bool operator != (FFTSample a, FFTSample b) { return a.m_fValue != b.m_fValue; }
public:
	static operator float(FFTSample sample) { return sample.m_fValue; }
public:
	virtual String^ ToString() override { return m_fValue.ToString(); }
};
//////////////////////////////////////////////////////
// RDFTContext
public ref class RDFTContext : public AVBase
{
public:
	RDFTContext(int nbits,RDFTransformType trans);
public:
	void calc(array<FFTSample>^ data);
	void calc(array<FFTSample>^ data,int start_idx,int length);
	void calc(IntPtr data);
	void calc(IntPtr data,int size);
	void calc(IntPtr data,int start_idx,int length);
};
//////////////////////////////////////////////////////
// DCTContext
public ref class DCTContext : public AVBase
{
public:
	DCTContext(int nbits,DCTTransformType trans);
public:
	void calc(array<FFTSample>^ data);
	void calc(array<FFTSample>^ data,int start_idx,int length);
	void calc(IntPtr data);
	void calc(IntPtr data,int size);
	void calc(IntPtr data,int start_idx,int length);
};
//////////////////////////////////////////////////////
#pragma endregion
//////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////
