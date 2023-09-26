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
// libavformat 
// I/O and Muxing/Demuxing Library.
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
[Flags]
public enum class AVfmt : int
{
	/// Demuxer will use avio_open, no opened file should be provided by the caller.
	NOFILE = 0x0001,
	NEEDNUMBER = 0x0002, ///<summary>< Needs '%d' in filename. ///</summary>
	SHOW_IDS = 0x0008, ///<summary>< Show format stream IDs numbers. ///</summary>
	///<summary>< Format wants AVPicture structure for raw picture data. ///</summary>
	RAWPICTURE = 0x0020, 
	GLOBALHEADER = 0x0040, ///<summary>< Format wants global header. ///</summary>
	NOTIMESTAMPS = 0x0080, ///<summary>< Format does not need / have any timestamps. ///</summary>
	GENERIC_INDEX = 0x0100, ///<summary>< Use generic index building code. ///</summary>
	TS_DISCONT = 0x0200, ///<summary>< Format allows timestamp discontinuities. Note, muxers always require valid (monotone) timestamps ///</summary>
	VARIABLE_FPS = 0x0400, ///<summary>< Format allows variable fps. ///</summary>
	NODIMENSIONS = 0x0800, ///<summary>< Format does not need width/height ///</summary>
	NOSTREAMS = 0x1000, ///<summary>< Format does not require any streams ///</summary>
	NOBINSEARCH = 0x2000, ///<summary>< Format does not allow to fallback to binary search via read_timestamp ///</summary>
	NOGENSEARCH = 0x4000, ///<summary>< Format does not allow to fallback to generic search ///</summary>
	NO_BYTE_SEEK = 0x8000, ///<summary>< Format does not allow seeking by bytes ///</summary>
	ALLOW_FLUSH = 0x10000, ///<summary>< Format allows flushing. If not set, the muxer will not receive a NULL packet in the write_packet function. ///</summary>
	TS_NONSTRICT = 0x20000, ///<summary>< Format does not require strictly increasing timestamps, but they must still be monotonic ///</summary>
	TS_NEGATIVE  = 0x40000, ///<summary>< Format allows muxing negative timestamps. If not set the timestamp will be shifted in av_write_frame and av_interleaved_write_frame so they start from 0. The user or muxer can override this through	AVFormatContext.avoid_negative_ts ///</summary>
	SEEK_TO_PTS = 0x4000000, ///<summary>< Seeking is based on PTS ///</summary>
};
//////////////////////////////////////////////////////
[Flags]
public enum class AvioFlag : int
{
	None = 0,
	///<summary>
	/// @name URL open modes
	/// The flags argument to avio_open must be one of the following
	/// constants, optionally ORed with other flags.
	/// @{
	///</summary>
	READ = 1,                                      ///<summary>< read-only ///</summary>
	WRITE = 2,                                      ///<summary>< write-only ///</summary>
	READ_WRITE = (READ | WRITE),  ///<summary>< read-write pseudo flag ///</summary>
	///<summary>
	/// @}
	///</summary>

	///<summary>
	/// Use non-blocking mode.
	/// If this flag is set, operations on the context will return
	/// AVERROR(EAGAIN) if they can not be performed immediately.
	/// If this flag is not set, operations on the context will never return
	/// AVERROR(EAGAIN).
	/// Note that this flag does not affect the opening/connecting of the
	/// context. Connecting a protocol will always block if necessary (e.g. on
	/// network protocols) but never hang (e.g. on busy devices).
	/// Warning: non-blocking protocols is work-in-progress, this flag may be
	/// silently ignored.
	///</summary>
	NONBLOCK = 8,

	///<summary>
	/// Use direct mode.
	/// avio_read and avio_write should if possible be satisfied directly
	/// instead of going through a buffer, and avio_seek will always
	/// call the underlying seek function directly.
	///</summary>
	DIRECT = 0x8000,
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVSeek : int
{
	CUR = 1,
	END = 2,
	SET = 0,
	///<summary>
	/// Passing this as the "whence" parameter to a seek function causes it to
	/// return the filesize without seeking anywhere. Supporting this is optional.
	/// If it is not supported then the seek function will return <0.
	///</summary>
	SIZE = 0x10000,

	///<summary>
	/// Oring this flag as into the "whence" parameter to a seek function causes it to
	/// seek by any means (like reopening and linear reading) or other normally unreasonble
	/// means that can be extreemly slow.
	/// This may be ignored by the seek code.
	///</summary>
	FORCE = 0x20000,

	///<summary>
	/// Bytestream IO Context.
	/// New fields can be added to the end with minor version bumps.
	/// Removal, reordering and changes to existing fields require a major
	/// version bump.
	/// sizeof(AVIOContext) must not be used outside libav///.
	///
	/// @note None of the function pointers in AVIOContext should be called
	///       directly, they should only be set by the client application
	///       when implementing custom I/O. Normally these are set to the
	///       function pointers specified in avio_alloc_context()
	///</summary>
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVSeekFlag : int
{
	NONE = 0,
	BACKWARD = 1, ///< seek backward
	BYTE     = 2, ///< seeking based on position in bytes
	ANY      = 4, ///< seek to any frame, even non-keyframes
	FRAME    = 8, ///< seeking based on frame number
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVDisposition : int
{
    DEFAULT = 0x0001,
    DUB = 0x0002,
    ORIGINAL = 0x0004,
    COMMENT = 0x0008,
    LYRICS = 0x0010,
    KARAOKE = 0x0020,

	/// Track should be used during playback by default.
	/// Useful for subtitle track that should be displayed
	/// even when user did not explicitly ask for subtitles.
    FORCED = 0x0040,
    HEARING_IMPAIRED = 0x0080,  ///< stream for hearing impaired audiences 
    VISUAL_IMPAIRED = 0x0100,  ///< stream for visual impaired audiences 
    CLEAN_EFFECTS = 0x0200,  ///< stream without voice 
    /// The stream is stored in the file as an attached picture/"cover art" (e.g.
    /// APIC frame in ID3v2). The single packet associated with it will be returned
    /// among the first few packets read from the file unless seeking takes place.
    /// It can also be accessed at any time in AVStream.attached_pic.
    ATTACHED_PIC = 0x0400,
	// To specify text track kind (different from subtitles default).
	CAPTIONS     = 0x10000,
	DESCRIPTIONS = 0x20000,
	METADATA     = 0x40000,
	///< dependent audio stream (mix_type=0 in mpegts)
	DEPENDENT    = 0x80000, 
	///< still images in video stream	(still_picture_flag=1 in mpegts)
	STILL_IMAGE = 0x100000, 
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVStreamEventFlag : int
{
	None = 0,
	///< The call resulted in updated metadata.
	METADATA_UPDATED = 0x0001,
};
//////////////////////////////////////////////////////
public enum class AVStreamParseType : UInt32
{
    NONE,
    FULL,       ///< full parsing and repack
    HEADERS,    ///< Only parse headers, do not repack. 
    TIMESTAMPS, ///< full parsing and interpolation of timestamps for frames not starting on a packet boundary 
    FULL_ONCE,  ///< full parsing and repack of the first frame only, only implemented for H.264 currently 
    FULL_RAW=MKTAG(0,'R','A','W'),  ///< full parsing and repack with timestamp and position generation by parser for raw
				/// this assumes that each packet in the file contains no demuxer level headers and
				/// just codec level data, otherwise position generation would fail
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVfmtFlag : int
{
	GENPTS = 0x0001, ///< Generate missing pts even if it requires parsing future frames.
	IGNIDX = 0x0002, ///< Ignore index.
	NONBLOCK = 0x0004, ///< Do not block when reading packets from input.
	IGNDTS = 0x0008, ///< Ignore DTS on frames that contain both DTS & PTS
	NOFILLIN = 0x0010, ///< Do not infer any values from other values, just return what is stored in the container
	NOPARSE = 0x0020, ///< Do not use AVParsers, you also must set AVFMT_FLAG_NOFILLIN as the fillin code works on frames and no parsing -> no frames. Also seeking to frames can not work if parsing to find frame boundaries has been disabled
	NOBUFFER = 0x0040, ///< Do not buffer frames when possible
	CUSTOM_IO = 0x0080, ///< The caller has supplied a custom AVIOContext, don't avio_close() it.
	DISCARD_CORRUPT = 0x0100, ///< Discard frames marked corrupted
	FLUSH_PACKETS = 0x0200, ///< Flush the AVIOContext every packet.
	/// When muxing, try to avoid writing any random/volatile data to the output.
	/// This includes any random IDs, real-time timestamps/dates, muxer version, etc.
	/// This flag is mainly intended for testing.
	BITEXACT = 0x0400, 
	MP4A_LATM = 0x8000, ///< Enable RTP MP4A-LATM payload
	SORT_DTS = 0x10000, ///< try to interleave outputted packets by dts (using this flag can slow demuxing down)
	PRIV_OPT = 0x20000, ///< Enable use of private options by delaying codec open (this could be made default once all code is converted)
	KEEP_SIDE_DATA = 0x40000, ///< Don't merge side data but keep it separate.
	FAST_SEEK = 0x80000, ///< Enable fast, but inaccurate seeks for some formats
	SHORTEST = 0x100000, ///< Stop muxing when the shortest stream stops.
	AUTO_BSF = 0x200000, ///< Add bitstream filters as requested by the muxer
};
//////////////////////////////////////////////////////
/// The duration of a video can be estimated through various ways, and this enum can be used
/// to know how the duration was estimated.
public enum class AVDurationEstimationMethod 
{
	FROM_PTS,    ///< Duration accurately estimated from PTSes
	FROM_STREAM, ///< Duration estimated from a stream with a known duration
	FROM_BITRATE ///< Duration estimated from bitrate (less accurate)
};
//////////////////////////////////////////////////////
public enum class AVPtsWrap : int
{
	Ignore = 0,   ///< ignore the wrap
	AddOffset = 1,   ///< add the format specific offset on wrap detection
	SubOffset = -1,  ///< subtract the format specific offset on wrap detection
};
//////////////////////////////////////////////////////
public enum class AVProgramFlags : int
{
	None = 0,
	Running = 1,
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVFMTCTX : int
{
	None = 0x0000,
	NOHEADER = 0x0001,///< signal that no header is present (streams are added dynamically)
	UNSEEKABLE = 0x0002 /// < signal that the stream is definitely not seekable, and attempts to call the
						/// seek function will fail. For some
						/// network protocols (e.g. HLS), this can
						/// change dynamically at runtime.
};
//////////////////////////////////////////////////////
[Flags]
public enum class FDebugFlags
{
	None = 0,
	TS = 0x0001,
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVFmtEventFlags
{
	None = 0,
	///< The call resulted in updated metadata.
	METADATA_UPDATED = 0x0001,
};
//////////////////////////////////////////////////////
public enum class AVFmtAvoidNegTS
{
	Default = 0,
	///< Enabled when required by target format
	Auto = -1,
	 ///< Shift timestamps so they are non negative
	MakeNonNegative = 1,
	 ///< Shift timestamps so that they start at 0
	MakeZero = 2,
};
//////////////////////////////////////////////////////
/// Directory entry types.
public enum class AVIODirEntryType 
{
    UNKNOWN,
    BLOCK_DEVICE,
    CHARACTER_DEVICE,
    DIRECTORY,
    NAMED_PIPE,
    SYMBOLIC_LINK,
    SOCKET,
    FILE,
    SERVER,
    SHARE,
    WORKGROUP,
};
//////////////////////////////////////////////////////
/// Different data types that can be returned via the AVIO
/// write_data_type callback.
public enum class AVIODataMarkerType
{
	///
	/// Header data; this needs to be present for the stream to be decodeable.
	///
	HEADER,
	///
	/// A point in the output bytestream where a decoder can start decoding
	/// (i.e. a keyframe). A demuxer/decoder given the data flagged with
	/// HEADER, followed by any SYNC_POINT,
	/// should give decodeable results.
	///
	SYNC_POINT,
	///
	/// A point in the output bytestream where a demuxer can start parsing
	/// (for non self synchronizing bytestream formats). That is, any
	/// non-keyframe packet start point.
	///
	BOUNDARY_POINT,
	///
	/// This is any, unlabelled data. It can either be a muxer not marking
	/// any positions at all, it can be an actual boundary/sync point
	/// that the muxer chooses not to mark, or a later part of a packet/fragment
	/// that is cut into multiple write callbacks due to limited IO buffer size.
	///
	UNKNOWN,
	///
	/// Trailer data, which doesn't contain actual content, but only for
	/// finalizing the output file.
	///
	TRAILER,
	///
	/// A point in the output bytestream where the underlying AVIOContext might
	/// flush the buffer depending on latency or buffering requirements. Typically
	/// means the end of a packet.
	///
	FLUSH_POINT,
};
//////////////////////////////////////////////////////
public enum class UrlProtocolFlags
{
	None = 0,
	///< The protocol name can be the first part of a nested protocol scheme
	NESTED_SCHEME = 1,
	///< The protocol uses network
	NETWORK       = 2
};
//////////////////////////////////////////////////////
public enum class AVTimebaseSource 
{
	AUTO = -1,
	DECODER,
	DEMUXER,
	FRAMERATE,
};
//////////////////////////////////////////////////////
#pragma endregion 
//////////////////////////////////////////////////////
#pragma region Classes
//////////////////////////////////////////////////////
ref class AVFormatContext;
ref class AVInputFormat;
ref class AVOutputFormat;
//////////////////////////////////////////////////////
// LibAVFormat
public ref class LibAVFormat
{
private:
	static bool m_bRegistered = false;
private:
	LibAVFormat();
public:
	/// Return the LIBAVFORMAT_VERSION_INT constant.
	static property UInt32 Version { UInt32 get(); }
	/// Return the libavformat build-time configuration.
	static property String^  Configuration { String^ get(); }
	// Return the libavformat license.
	static property String^  License { String^ get(); }
public:
	/// Initialize libavformat and register all the muxers, demuxers and
	/// protocols. If you do not call this function, then you can select
	/// exactly which formats you want to support.
	static void RegisterAll();
	static void RegisterInputFormat(AVInputFormat^ format);
	static void RegisterOutputFormat(AVOutputFormat^ format);
	/// Do global initialization of network components. This is optional,
	/// but recommended, since it avoids the overhead of implicitly
	/// doing the setup for each session.
	///
	/// Calling this function will become mandatory if using network
	/// protocols at some major version bump.
	static int NetworkInit();
	/// Undo the initialization done by avformat_network_init.
	static int NetworkDeinit();
};
//////////////////////////////////////////////////////
// AVIOProtocolEnum
public ref class AVIOProtocolEnum
	: public System::Collections::IEnumerable
	, public System::Collections::Generic::IEnumerable<String^>
{
private:
	ref class AVIOProtocolEnumerator : public System::Collections::IEnumerator
		, public System::Collections::Generic::IEnumerator<String^>
	{
	protected:
		void *	m_pOpaque;
		void *	m_pCurrent;
		bool	m_bOutput;
	public:
		AVIOProtocolEnumerator(bool _output);
		~AVIOProtocolEnumerator();
	public:
		// IEnumerator
		virtual bool MoveNext();
		virtual property String^ Current { String^ get (); }
		virtual void Reset();
		virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
	};
private:
	bool	m_bOutput;
internal:
	AVIOProtocolEnum(bool _output);
public:
	// IEnumerable
	virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator;
public:
	// IEnumerable<String>
	virtual System::Collections::Generic::IEnumerator<String^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<String^>::GetEnumerator;
};
//////////////////////////////////////////////////////
public delegate int AVIOPacketCB(Object^ opaque, IntPtr buf, int buf_size);
public delegate int AVIOSeekCB(Object^ opaque, Int64 offset, AVSeek whence);
public delegate bool AVIOInterruptCB(Object^ opaque);
//////////////////////////////////////////////////////
// AVIOInterruptDesc
public ref class AVIOInterruptDesc
{
private:
	AVIOInterruptCB^	m_pCallback;
	Object^				m_pOpaque;
public:
	property AVIOInterruptCB^ callback { AVIOInterruptCB^ get(); }
	property Object^ opaque { Object^ get(); }
public:
	AVIOInterruptDesc(AVIOInterruptCB^ _callback,Object^ _opaque);
};
//////////////////////////////////////////////////////
// AVIOContext
/// Bytestream IO Context.
/// New fields can be added to the end with minor version bumps.
/// Removal, reordering and changes to existing fields require a major
/// version bump.
/// sizeof(AVIOContext) must not be used outside libav///.
///
/// @note None of the function pointers in AVIOContext should be called
///       directly, they should only be set by the client application
///       when implementing custom I/O. Normally these are set to the
///       function pointers specified in avio_alloc_context()
public ref class AVIOContext : public AVBase
{
protected:
	Object^				m_pOpaque;
	AVIOInterruptDesc^	m_pInterruptCB;
	AVIOSeekCB^			m_pSeek;
	AVIOPacketCB^		m_pRead;
	AVIOPacketCB^		m_pWrite;
	AVMemPtr^			m_pBuffer;
internal:
	AVIOContext(void * _pointer,AVBase^ _parent);
public:
	AVIOContext(String^ _url, AvioFlag _flags);
	AVIOContext(String^ _url, AvioFlag _flags, AVIOInterruptCB^ int_cb,Object^ opaque);
	AVIOContext(String^ _url, AvioFlag _flags, AVIOInterruptCB^ int_cb,Object^ opaque,AVDictionary^ options);
	AVIOContext(AVMemPtr^ buf, int write_flag, Object^ opaque, AVIOPacketCB^ read_packet, AVIOPacketCB^ write_packet, AVIOSeekCB^ seek);
internal:
	virtual void _FreeObjects() override;
public:
	 /// A class for private options.
     ///
     /// If this AVIOContext is created by avio_open2(), av_class is set and
     /// passes the options down to protocols.
     ///
     /// If this AVIOContext is manually allocated, then av_class may be set by
     /// the caller.
     ///
     /// warning -- this field can be NULL, be sure to not pass this AVIOContext
     /// to any av_opt_/// functions in that case.
	property AVClass^ av_class { AVClass^ get(); }
	/// Start of the buffer.
	property IntPtr buffer { IntPtr get(); }
	/// Maximum buffer size
	property int buffer_size { int get(); }
	/// Current position in the buffer
	property IntPtr buf_ptr { IntPtr get(); }
	/// End of the data, may be less than buffer+buffer_size if the read function returned
	/// less data than requested, e.g. for streams where no more data has been received yet.
	property IntPtr buf_end { IntPtr get(); }

	property Int64 pos { Int64 get(); }
	property bool must_flush { bool get(); }
	property bool eof_reached { bool get(); void set(bool ); }
	property bool write_flag { bool get(); }
	property int max_packet_size { int get(); }
	property UInt32 checksum { UInt32 get(); }
	property IntPtr checksum_ptr{ IntPtr get(); }
	property int error { int get(); }
	property bool seekable { bool get(); }
	property Int64 maxsize { Int64 get(); }
	property bool direct { bool get(); }
	property Int64 bytes_read { Int64 get(); }
	property Int64 bytes_written { Int64 get(); }
	property int seek_count { int get(); }
	property int writeout_count { int get(); }
	property int orig_buffer_size { int get(); }
public:
	void w8(int b);
	void write(IntPtr buf, int size);
	void wl64(UInt64 val);
	void wb64(UInt64 val);
	void wl32(UInt32 val);
	void wb32(UInt32 val);
	void wl24(UInt32 val);
	void wb24(UInt32 val);
	void wl16(UInt32 val);
	void wb16(UInt32 val);

	/// Write a NULL-terminated string.
	/// @return number of bytes written.
	int put_str(String^ str);

	/// Convert an UTF-8 string to UTF-16LE and write it.
	/// @return number of bytes written.
	int put_str16le(String^ str);

	/// Convert an UTF-8 string to UTF-16BE and write it.
	/// @return number of bytes written.
	int put_str16be(String^ str);

	/**
	* Mark the written bytestream as a specific type.
	*
	* Zero-length ranges are omitted from the output.
	*
	* @param time the stream time the current bytestream pos corresponds to
	*             (in AV_TIME_BASE units), or AV_NOPTS_VALUE if unknown or not
	*             applicable
	* @param type the kind of data written starting at the current pos
	*/
	void write_marker(Int64 time, AVIODataMarkerType type);
public:
	/// fseek() equivalent for AVIOContext.
	/// @return new position or AVERROR.
	Int64 seek(Int64 offset, AVSeek whence);

	/// Skip given number of bytes forward
	/// @return new position or AVERROR.
	Int64 skip(Int64 offset);

	/// ftell() equivalent for AVIOContext.
	/// @return position or AVERROR.
	Int64 tell();

	/// Get the filesize.
	/// @return filesize or AVERROR
	Int64 size();

	/// feof() equivalent for AVIOContext.
	/// @return non zero if and only if end of file
	bool feof();

	///Force flushing of buffered data.
	///
	///For write streams, force the buffered data to be immediately written to the output,
	///without to wait to fill the internal buffer.
	///
	///For read streams, discard all currently buffered data, and advance the
	///reported file position to that of the underlying stream. This does not
	///read new data, and does not perform any seeks.
	void flush();
public:
	///Read size bytes from AVIOContext into buf.
	///@return number of bytes read or AVERROR
	int read(IntPtr buf, int size);
	
	/**
	* Read size bytes from AVIOContext into buf. Unlike avio_read(), this is allowed
	* to read fewer bytes than requested. The missing bytes can be read in the next
	* call. This always tries to read at least 1 byte.
	* Useful to reduce latency in certain cases.
	* @return number of bytes read or AVERROR
	*/
	int read_partial(IntPtr buf, int size);

	/// @name Functions for reading from AVIOContext
	/// return 0 if EOF, so you cannot use it if EOF handling is necessary
	int    r8  ();
	UInt32 rl16();
	UInt32 rl24();
	UInt32 rl32();
	UInt64 rl64();
	UInt32 rb16();
	UInt32 rb24();
	UInt32 rb32();
	UInt64 rb64();

	/// Read a string from pb into buf. The reading will terminate when either
	/// a NULL character was encountered, maxlen bytes have been read, or nothing
	/// more can be read from pb. The result is guaranteed to be NULL-terminated, it
	/// will be truncated if buf is too small.
	/// Note that the string is not interpreted or validated in any way, it
	/// might get truncated in the middle of a sequence for multi-byte encodings.
	///
	/// @return number of bytes read (is always <= maxlen).
	/// If reading ends on EOF or error, the return value will be one more than
	/// bytes actually read.
 
	int get_str(int maxlen, String^ % str);

	/// Read a UTF-16 string from pb and convert it to UTF-8.
	/// The reading will terminate when either a null or invalid character was
	/// encountered or maxlen bytes have been read.
	/// @return number of bytes read (is always <= maxlen)
	int get_str16le(int maxlen, String^ % str);
	int get_str16be(int maxlen, String^ % str);

	/// Pause and resume playing - only meaningful if using a network streaming
	/// protocol (e.g. MMS).
	int pause(bool _pause);

	/// Seek to a given timestamp relative to some component stream.
	/// Only meaningful if using a network streaming protocol (e.g. MMS.).
	///
	/// @param h IO context from which to call the seek function pointers
	/// @param stream_index The stream index that the timestamp is relative to.
	///        If stream_index is (-1) the timestamp should be in AV_TIME_BASE
	///        units from the beginning of the presentation.
	///        If a stream_index >= 0 is used and the protocol does not support
	///        seeking based on component streams, the call will fail.
	/// @param timestamp timestamp in AVStream.time_base units
	///        or if there is no stream specified then in AV_TIME_BASE units.
	/// @param flags Optional combination of AVSEEK_FLAG_BACKWARD, AVSEEK_FLAG_BYTE
	///        and AVSEEK_FLAG_ANY. The protocol may silently ignore
	///        AVSEEK_FLAG_BACKWARD and AVSEEK_FLAG_ANY, but AVSEEK_FLAG_BYTE will
	///        fail if used and not supported.
	/// @return >= 0 on success
	/// @see AVInputFormat::read_seek
	Int64 seek_time(int stream_index,Int64 timestamp, AVSeekFlag flags);
public:
	/// Allocate and read the payload of a packet and initialize its
	/// fields with default values.
	///
	/// @param s    associated IO context
	/// @param pkt packet
	/// @param size desired payload size
	/// @return >0 (read size) if OK, AVERROR_xxx otherwise
	int get_packet(AVPacket^ pkt, int size);

	/// Read data and append it to the current content of the AVPacket.
	/// If pkt->size is 0 this is identical to av_get_packet.
	/// Note that this uses av_grow_packet and thus involves a realloc
	/// which is inefficient. Thus this function should only be used
	/// when there is no reasonable way to know (an upper bound of)
	/// the final size.
	///
	/// @param s    associated IO context
	/// @param pkt packet
	/// @param size amount of data to read
	/// @return >0 (read size) if OK, AVERROR_xxx otherwise, previous data
	///         will not be lost even if an error occurs.
	int append_packet(AVPacket^ pkt, int size);
public:
	///<summary>
	/// Accept and allocate a client context on a server context.
	/// @param  s the server context
	/// @param  c the client context, must be unallocated
	/// @return   >= 0 on success or a negative value corresponding
	///           to an AVERROR on failure
	///</summary>
	int accept([Out] AVIOContext^ % c);

	///<summary>
	/// Perform one step of the protocol handshake to accept a new client.
	/// This function must be called on a client returned by avio_accept() before
	/// using it as a read/write context.
	/// It is separate from avio_accept() because it may block.
	/// A step of the handshake is defined by places where the application may
	/// decide to change the proceedings.
	/// For example, on a protocol with a request header and a reply header, each
	/// one can constitute a step because the application may use the parameters
	/// from the request to change parameters in the reply; or each individual
	/// chunk of the request can constitute a step.
	/// If the handshake is already finished, avio_handshake() does nothing and
	/// returns 0 immediately.
	///
	/// @param  c the client context to perform the handshake on
	/// @return   0   on a complete and successful handshake
	///           > 0 if the handshake progressed, but is not complete
	///           < 0 for an AVERROR code
	///</summary>
	int handshake();

protected:
	bool Open(String^ _url, AvioFlag _flags, AVIOInterruptDesc^ int_cb,AVDictionary^ options);
internal:
	int ReadCB(IntPtr buf, int buf_size);
	int WriteCB(IntPtr buf, int buf_size);
	int SeekCB(Int64 offset, int whence);
public:
	///<summary>
	/// Return AVIO_FLAG_/// access flags corresponding to the access permissions
	/// of the resource in url, or a negative value corresponding to an
	/// AVERROR code in case of failure. The returned access flags are
	/// masked by the value in flags.
	///
	/// @note This function is intrinsically unsafe, in the sense that the
	/// checked resource may change its existence or permission status from
	/// one call to another. Thus you should not trust the returned value,
	/// unless you are sure that no other processes are accessing the
	/// checked resource.
	///</summary>
	static AvioFlag Check(String^ _url);
	static AvioFlag Check(String^ _url, AvioFlag _flags);

	///<summary>
	/// Return the name of the protocol that will handle the passed URL.
	///
	/// NULL is returned if no protocol could be found for the given URL.
	///
	/// @return Name of the protocol or NULL.
	///</summary>
	static String^ FindProtocolName(String^ _url);

	///<summary>
	/// Iterate through names of available protocols.
	///
	/// @param opaque A private pointer representing current protocol.
	///        It must be a pointer to NULL on first iteration and will
	///        be updated by successive calls to avio_enum_protocols.
	/// @param output If set to 1, iterate over output protocols,
	///               otherwise over input protocols.
	///
	/// @return A static string containing the name of current protocol or NULL
	///</summary>
	static AVIOProtocolEnum^ EnumProtocols(bool bOutput);
};
//////////////////////////////////////////////////////
// AVCodecTag
public ref class AVCodecTag : public AVBase
{
internal:
	AVCodecTag(void * _pointer,AVBase^ _parent);
public:
	property int _StructureSize { virtual int get() override; }
public:
	virtual String^ ToString() override;
public:
	property AVCodecID id { AVCodecID get(); }
	property UInt32 tag { UInt32 get(); }
	property String^ name { String^ get(); }
public:
	/// Get the AVCodecID for the given codec tag tag.
	/// If no codec id is found returns AV_CODEC_ID_NONE.
	///
	/// @param tags list of supported codec_id-codec_tag pairs, as stored
	/// in AVInputFormat.codec_tag and AVOutputFormat.codec_tag
	/// @param tag  codec tag to match to a codec ID
	static AVCodecID GetCodecID(array<AVCodecTag^>^ tags, int tag);

	/// Get the codec tag for the given codec id id.
	/// If no codec tag is found returns 0.
	///
	/// @param tags list of supported codec_id-codec_tag pairs, as stored
	/// in AVInputFormat.codec_tag and AVOutputFormat.codec_tag
	/// @param id   codec ID to match to a codec tag
	static int GetCodecTag(array<AVCodecTag^>^ tags, AVCodecID id);
	
	/// @return the table mapping RIFF FourCCs for video to libavcodec AVCodecID.
	static array<AVCodecTag^>^ GetRiffVideoTags();

	/// @return the table mapping RIFF FourCCs for audio to AVCodecID.
	static array<AVCodecTag^>^ GetRiffAudioTags();

	/// @return the table mapping MOV FourCCs for video to libavcodec AVCodecID.
	static array<AVCodecTag^>^ GetMovVideoTags();

	// @return the table mapping MOV FourCCs for audio to AVCodecID.
	static array<AVCodecTag^>^ GetMovAudioTags();
};
//////////////////////////////////////////////////////
// AVOutputFormat
public ref class AVOutputFormat : public AVBase
{
public:
	ref class AVOutputFormats : public System::Collections::IEnumerable
			, public System::Collections::Generic::IEnumerable<AVOutputFormat^>
	{
	private:
		ref class Enumerator : public System::Collections::IEnumerator
			, public System::Collections::Generic::IEnumerator<AVOutputFormat^>
		{
		protected:
			IntPtr			m_pOpaque;
			AVOutputFormat^	m_pCurrent;
		public:
			Enumerator();
			~Enumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVOutputFormat^ Current { AVOutputFormat^ get(); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get() sealed = IEnumerator::Current::get; }
		};
	public:
		AVOutputFormats();
	public:
		property AVOutputFormat^ default[int] { AVOutputFormat^ get(int); }
		property int Count { int get(); }
	public:
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { return gcnew Enumerator(); }
		virtual System::Collections::Generic::IEnumerator<AVOutputFormat^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVOutputFormat^>::GetEnumerator { return gcnew Enumerator(); }
	};
internal:
	AVOutputFormat(void * _pointer,AVBase^ _parent);
public:
	virtual String^ ToString() override;
public:
	property int _StructureSize { virtual int get() override; }
public:
	property String^ name { String^ get(); }
	/// Descriptive name for the format, meant to be more human-readable
    /// than name. You should use the NULL_IF_CONFIG_SMALL() macro
    /// to define it.
	property String^ long_name { String^ get(); }
	property String^ mime_type { String^ get(); }
	/// comma-separated filename extensions
	property String^ extensions { String^ get(); }
	/// default audio codec
	property AVCodecID audio_codec { AVCodecID get(); }
	/// default video codec
	property AVCodecID video_codec { AVCodecID get(); }
	/// default subtitle codec
	property AVCodecID subtitle_codec { AVCodecID get(); }
	/// can use flags: AVFMT_NOFILE, AVFMT_NEEDNUMBER,
    /// AVFMT_GLOBALHEADER, AVFMT_NOTIMESTAMPS, AVFMT_VARIABLE_FPS,
    /// AVFMT_NODIMENSIONS, AVFMT_NOSTREAMS, AVFMT_ALLOW_FLUSH,
    /// AVFMT_TS_NONSTRICT, AVFMT_TS_NEGATIVE
	property AVfmt flags { AVfmt get(); }
	/// List of supported codec_id-codec_tag pairs, ordered by "better
	/// choice first". The arrays are all terminated by AV_CODEC_ID_NONE.
	property array<AVCodecTag^>^ codec_tag { array<AVCodecTag^>^ get(); }
	/// AVClass for the private context
	property AVClass^ priv_class { AVClass^ get(); }
private:
	/// default data codec
	property AVCodecID data_codec { AVCodecID get(); }	
public:
	AVOutputFormat^ Next();
public:
	AVFormatContext^ OpenContext();
	AVCodecID GuessCodec(String^ short_name, String^ filename, String^ mime_type, AVMediaType type);

	/// Test if the given container can store a codec.
	///
	/// @param ofmt           container to check for compatibility
	/// @param codec_id       codec to potentially store in container
	/// @param std_compliance standards compliance level, one of FF_COMPLIANCE_*
	///
	/// @return 1 if codec with ID codec_id can be stored in ofmt, 0 if it cannot.
	///         A negative number if this information is not available.
	bool QueryCodec(AVCodecID codec_id,FFCompliance std_compliance);
public:
	/// Transfer internal timing information from one stream to another.
	///
	/// This function is useful when doing stream copy.
	///
	/// @param ofmt     target output format for ost
	/// @param ost      output stream which needs timings copy and adjustments
	/// @param ist      reference input stream to copy timings from
	/// @param copy_tb  define from where the stream codec timebase needs to be imported
	int transfer_internal_stream_timing_info(AVStream^ ost, AVStream^ ist, AVTimebaseSource copy_tb);
public:
	static AVOutputFormat^ Next(AVOutputFormat^ f);
	static AVOutputFormat^ GuessFormat(String^ short_name, String^ filename, String^ mime_type);
public:
	static property AVOutputFormats^ Formats { AVOutputFormats^ get(); }
};
//////////////////////////////////////////////////////
// AVProbeData
// This structure contains the data a format has to probe a file.
public ref class AVProbeData : public AVBase
{
public:
	///< maximum score, half of that is used for file-extension-based detection
	static const int SCORE_MAX = 100;
	static const int SCORE_RETRY = (SCORE_MAX/4);
	static const int SCORE_STREAM_RETRY = (SCORE_MAX/4-1);
	///< score for file extension
	static const int SCORE_EXTENSION = 50;
	///< score for file mime type
	static const int SCORE_MIME = 75;
	///< extra allocated bytes at the end of the probe buffer
	static const int PADDING_SIZE = 32;
internal:
	AVProbeData(void * _pointer,AVBase^ _parent);
public:
	AVProbeData();
public:
	property int _StructureSize { virtual int get() override; }
public:
	property String^ filename { String^ get(); void set(String^); }
	/// Buffer must have PADDING_SIZE of extra allocated bytes filled with zero.
	property IntPtr buf { IntPtr get(); void set(IntPtr); }
	/// Size of buf except extra allocated bytes
	property int buf_size { int get(); void set(int); }
	/// mime_type, when known.
	property String^ mime_type { String^ get(); void set(String^); }
};
//////////////////////////////////////////////////////
// AVInputFormat
public ref class AVInputFormat : public AVBase
{
public:
	// Formats Iterator
	ref class AVInputFormats
		: public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<AVInputFormat^>
	{
	private:
		ref class AVInputFormatEnumerator 
			: public System::Collections::IEnumerator
			, public System::Collections::Generic::IEnumerator<AVInputFormat^>
		{
		protected:
			IntPtr			m_pOpaque;
			AVInputFormat^	m_pCurrent;
		public:
			AVInputFormatEnumerator();
			~AVInputFormatEnumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVInputFormat^ Current { AVInputFormat^ get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
		};
	public:
		AVInputFormats();
	public:
		property AVInputFormat^ default[int] { AVInputFormat^ get(int); }
		property int Count { int get(); }
	public:
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { return gcnew AVInputFormatEnumerator(); }
		virtual System::Collections::Generic::IEnumerator<AVInputFormat^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVInputFormat^>::GetEnumerator { return gcnew AVInputFormatEnumerator(); }
	};
internal:
	AVInputFormat(void * _pointer,AVBase^ _parent);
public:
	virtual String^ ToString() override;
public:
	property int _StructureSize { virtual int get() override; }
public:
	/// A comma separated list of short names for the format. New names
	/// may be appended with a minor bump.
	property String^ name { String^ get(); }
	/// Descriptive name for the format, meant to be more human-readable
	/// than name. You should use the NULL_IF_CONFIG_SMALL() macro
	/// to define it.
	property String^ long_name { String^ get(); }
	/// Can use flags: AVFMT_NOFILE, AVFMT_NEEDNUMBER, AVFMT_SHOW_IDS,
    /// AVFMT_NOTIMESTAMPS, AVFMT_GENERIC_INDEX, AVFMT_TS_DISCONT, AVFMT_NOBINSEARCH,
    /// AVFMT_NOGENSEARCH, AVFMT_NO_BYTE_SEEK, AVFMT_SEEK_TO_PTS.
	property AVfmt flags { AVfmt get(); }
	/// If extensions are defined, then no probe is done. You should
	/// usually not use extension format guessing because it is not
	/// reliable enough
	property String^ extensions { String^ get(); }

	property array<AVCodecTag^>^ codec_tag { array<AVCodecTag^>^ get(); }
	/// AVClass for the private context
	property AVClass^ priv_class { AVClass^ get(); }
	/// Comma-separated list of mime types.
	/// It is used check for matching mime types while probing.
	/// @see av_probe_input_format2
	property String^ mime_type { String^ get(); }

	property bool can_seek { bool get(); }
private:
	/// Raw demuxers store their codec ID here.
	property int raw_codec_id { int get(); }
public:
	AVInputFormat^ Next();
public:
	static property AVInputFormats^ Formats { AVInputFormats^ get();}
public:
	static AVInputFormat^ Next(AVInputFormat^ f);
	static AVInputFormat^ FindInputFormat(String^ short_name);
	static AVInputFormat^ ProbeInputFormat(AVProbeData^ pd, bool is_opened);
	static AVInputFormat^ ProbeInputFormat2(AVProbeData^ pd, bool is_opened, int % score_max);
	static AVInputFormat^ ProbeInputFormat3(AVProbeData^ pd, bool is_opened, int % score_ret);
	static AVInputFormat^ ProbeInputBuffer(AVIOContext^ pb, String^ url, IntPtr logctx, UInt32 offset, UInt32 max_probe_size);
};
//////////////////////////////////////////////////////
/// Stream structure.
/// New fields can be added to the end with minor version bumps.
/// Removal, reordering and changes to existing fields require a major
/// version bump.
public ref class AVStream : public AVBase
{
internal:
	// Context
	AVCodecContext^ m_pContext;
internal:
	AVStream(void * _pointer,AVBase^ _parent);
public:
	/// < stream index in AVFormatContext
	property int index { int get(); }
	
	/// Format-specific stream ID.
	/// decoding: set by libavformat
	/// encoding: set by the user, replaced by libavformat if left unset
	property int id { int get(); void set(int); }
	
	/// Codec context associated with this stream. Allocated and freed by
	/// libavformat.
	///
	/// - decoding: The demuxer exports codec information stored in the headers
	///             here.
	/// - encoding: The user sets codec information, the muxer writes it to the
	///             output. Mandatory fields as specified in AVCodecContext
	///             documentation must be set even if this AVCodecContext is
	///             not actually used for encoding.
	property AVCodecContext^ codec { AVCodecContext^ get(); void set(AVCodecContext^); }
	property IntPtr priv_data { IntPtr get(); void set(IntPtr); }
	
	/// This is the fundamental unit of time (in seconds) in terms
	/// of which frame timestamps are represented.
	///
	/// decoding: set by libavformat
	/// encoding: May be set by the caller before avformat_write_header() to
	///           provide a hint to the muxer about the desired timebase. In
	///           avformat_write_header(), the muxer will overwrite this field
	///           with the timebase that will actually be used for the timestamps
	///           written into the file (which may or may not be related to the
	///           user-provided one, depending on the format).
	property AVRational^ time_base { AVRational^ get(); void set(AVRational^); }
	
	/// Decoding: pts of the first frame of the stream in presentation order, in stream time base.
	/// Only set this if you are absolutely 100% sure that the value you set
	/// it to really is the pts of the first frame.
	/// This may be undefined (AV_NOPTS_VALUE).
	/// @note The ASF header does NOT contain a correct start_time the ASF
	/// demuxer must NOT set this.
	property Int64 start_time { Int64 get(); void set(Int64); }

	/// Decoding: duration of the stream, in stream time base.
	/// If a source file does not specify a duration, but does specify
	/// a bitrate, this value will be estimated from bitrate and file size.
	property Int64 duration { Int64 get(); }

	///< number of frames in this stream if known or 0
	property Int64 nb_frames { Int64 get();  void set(Int64); }
	property AVDisposition disposition { AVDisposition get();  void set(AVDisposition); }
	///< Selects which packets can be discarded at will and do not need to be demuxed.
	property AVDiscard discard { AVDiscard get();  void set(AVDiscard); }

	/// sample aspect ratio (0 if unknown)
	/// - encoding: Set by user.
	/// - decoding: Set by libavformat.
	property AVRational^ sample_aspect_ratio { AVRational^ get();  void set(AVRational^); }

	property AVDictionary^ metadata { AVDictionary^ get(); void set(AVDictionary^); }

	/// Average framerate
	///
	/// - demuxing: May be set by libavformat when creating the stream or in
	///             avformat_find_stream_info().
	/// - muxing: May be set by the caller before avformat_write_header().
	property AVRational^ avg_frame_rate { AVRational^ get();  void set(AVRational^); }

	/// For streams with AV_DISPOSITION_ATTACHED_PIC disposition, this packet
	/// will contain the attached picture.
	///
	/// decoding: set by libavformat, must not be modified by the caller.
	/// encoding: unused
	property AVPacket^ attached_pic { AVPacket^ get(); }

	/// An array of side data that applies to the whole stream (i.e. the
	/// container does not allow it to change between packets).
	///
	/// There may be no overlap between the side data in this array and side data
	/// in the packets. I.e. a given side data is either exported by the muxer
	/// (demuxing) / set by the caller (muxing) in this array, then it never
	/// appears in the packets, or the side data is exported / sent through
	/// the packets (always in the first packet where the value becomes known or
	/// changes), then it does not appear in this array.
	///
	/// - demuxing: Set by libavformat when the stream is created.
	/// - muxing: May be set by the caller before avformat_write_header().
	///
	/// Freed by libavformat in avformat_free_context().
	///
	/// @see av_format_inject_global_side_data()
	property array<AVPacketSideData^>^ side_data { array<AVPacketSideData^>^ get(); }

	/// Flags for the user to detect events happening on the stream. Flags must
	/// be cleared by the user once the event has been handled.
	/// A combination of AVStreamEventFlag.
	property AVStreamEventFlag event_flags { AVStreamEventFlag get();  void set(AVStreamEventFlag); }
public:
	/// Real base framerate of the stream.
	/// This is the lowest framerate with which all timestamps can be
	/// represented accurately (it is the least common multiple of all
	/// framerates in the stream). Note, this value is just a guess!
	/// For example, if the time base is 1/90000 and all frames have either
	/// approximately 3600 or 1800 timer ticks, then r_frame_rate will be 50/1.
	///
	/// Code outside avformat should access this field using:
	/// av_stream_get/set_r_frame_rate(stream)
	property AVRational^ r_frame_rate { AVRational^ get(); void set(AVRational^); }

	/// String containing paris of key and values describing recommended encoder configuration.
	/// Paris are separated by ','.
	/// Keys are separated from values by '='.
	property String^ recommended_encoder_configuration { String^ get(); void set(String^); }

	/// Codec parameters associated with this stream. Allocated and freed by
    /// libavformat in avformat_new_stream() and avformat_free_context()
    /// respectively.
    ///
    /// - demuxing: filled by libavformat on stream creation or in
    ///             avformat_find_stream_info()
    /// - muxing: filled by the caller before avformat_write_header()
	property AVCodecParameters^ codecpar { AVCodecParameters^ get(); }
public:

	/// Returns the pts of the last muxed packet + its duration
	/// 
	/// the retuned value is undefined when used with a demuxer.
	property Int64 end_pts { Int64 get(); }
	
	/// Get the internal codec timebase from a stream.
	property AVRational^ codec_timebase { AVRational^ get(); }

public:
	AVCodecParserContext^ GetParser();

	/// Wrap an existing array as stream side data.
	///
	/// @param st stream
	/// @param type side information type
	/// @param data the side data array. It must be allocated with the av_malloc()
	///             family of functions. The ownership of the data is transferred to
	///             st.
	/// @param size side information size
	/// @return zero on success, a negative AVERROR code on failure. On failure,
	///         the stream is unchanged and the data remains owned by the caller.
	AVRESULT AddSizeData(AVPacketSideDataType type, IntPtr data, int size);

	/// Get side information from stream.
	///
	/// @param stream stream
	/// @param type desired side information type
	/// @param size pointer for side information size to store (optional)
	/// @return pointer to data if present or NULL otherwise
	IntPtr GetSideData(AVPacketSideDataType type, Int64 % size);

	/// Allocate new information from stream.
	///
	/// @param stream stream
	/// @param type desired side information type
	/// @param size side information size
	/// @return pointer to fresh allocated data or NULL otherwise
	IntPtr NewSideData(AVPacketSideDataType type, int size);

	/// Get the index for a specific timestamp.
	///
	/// @param st        stream that the timestamp belongs to
	/// @param timestamp timestamp to retrieve the index for
	/// @param flags if AVSEEK_FLAG_BACKWARD then the returned index will correspond
	///                 to the timestamp which is <= the requested one, if backward
	///                 is 0, then it will be >=
	///              if AVSEEK_FLAG_ANY seek to any frame, only keyframes otherwise
	/// @return < 0 if no such timestamp could be found
	int IndexSearchTimestamp(Int64 timestamp, AVSeekFlag flags);

	/// Add an index entry into a sorted list. Update the entry if the list
	/// already contains it.
	///
	/// @param timestamp timestamp in the time base of the given stream
	int AddIndexEntry(Int64 pos, Int64 timestamp,int size, int distance, AVIndexFlags flags);
};
//////////////////////////////////////////////////////
// AVIndexEntry
public ref class AVIndexEntry : public AVBase
{
internal:
	AVIndexEntry(void * _pointer,AVBase^ _parent);
public:
	AVIndexEntry();
public:
	property int _StructureSize { virtual int get() override; }
public:
    property Int64 pos { Int64 get(); void set(Int64); }
	/// Timestamp in AVStream.time_base units, preferably the time from which on correctly decoded frames are available
	/// when seeking to this entry. That means preferable PTS on keyframe based formats.
	/// But demuxers can choose to store a different timestamp, if it is more convenient for the implementation or nothing better
	/// is known
    property Int64 timestamp { Int64 get(); void set(Int64); }
    property AVIndexFlags flags { AVIndexFlags get(); void set(AVIndexFlags); }
	//Yeah, trying to keep the size of this small to reduce memory requirements (it is 24 vs. 32 bytes due to possible 8-byte alignment).
    property int size { int get(); void set(int); }
	///< Minimum distance between this and the previous keyframe, used to avoid unneeded searching.
    property int min_distance { int get(); void set(int); }
};
//////////////////////////////////////////////////////
// AVProgram
/// New fields can be added to the end with minor version bumps.
/// Removal, reordering and changes to existing fields require a major
/// version bump.
/// sizeof(AVProgram) must not be used outside libav///.
public ref class AVProgram : public AVBase
{
internal:
	AVProgram(void * _pointer,AVBase^ _parent);
public:
	property int id { int get(); void set(int); }
	property AVProgramFlags flags { AVProgramFlags get(); void set(AVProgramFlags); }
	///< selects which program to discard and which to feed to the caller
	property AVDiscard discard { AVDiscard get(); void set(AVDiscard); }
	property array<UInt32>^ stream_index { array<UInt32>^ get(); }
	property AVDictionary^ metadata { AVDictionary^ get(); }
	property int program_num { int get(); void set(int); }
	property int pmt_pid { int get(); void set(int); }
	property int pcr_pid { int get(); void set(int); }
	property int pmt_version { int get(); void set(int); }
public:
	void AddStreamIndex(UInt32 idx);
};
//////////////////////////////////////////////////////
// AVChapter
public ref class AVChapter : public AVBase
{
internal:
	AVChapter(void * _pointer,AVBase^ _parent);
public:
	AVChapter();
public:
	property int _StructureSize { virtual int get() override; }
public:
	///< unique ID to identify the chapter
	property Int64 id { Int64 get(); void set(Int64); }
	///< time base in which the start/end timestamps are specified
	property AVRational^ time_base { AVRational^ get(); void set(AVRational^); }
	///< chapter start/end time in time_base units
	property Int64 start { Int64 get(); void set(Int64); }
	property Int64 end { Int64 get(); void set(Int64); }

	property AVDictionary^ metadata { AVDictionary^ get(); void set(AVDictionary^); }
};
//////////////////////////////////////////////////////
// AVFormatContext
/// 
/// Format I/O context.
/// New fields can be added to the end with minor version bumps.
/// Removal, reordering and changes to existing fields require a major
/// version bump.
/// sizeof(AVFormatContext) must not be used outside libav*, use
/// avformat_alloc_context() to create an AVFormatContext.
/// 
public ref class AVFormatContext : public AVBase
{
public:
	/// Get the AVClass for AVFormatContext. It can be used in combination with
	/// AV_OPT_SEARCH_FAKE_OBJ for examining options.
	static property AVClass^ Class { AVClass^ get(); }
public:
	ref class AVStreams
		: public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<AVStream^>
	{
	private:
		ref class AVStreamsEnumerator
			: public System::Collections::IEnumerator
			, public System::Collections::Generic::IEnumerator<AVStream^>
		{
		protected:
			AVStreams^	m_pParent;
			int			m_nIndex;
		public:
			AVStreamsEnumerator(AVStreams^ streams);
			~AVStreamsEnumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVStream^ Current { AVStream^ get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get() sealed = IEnumerator::Current::get; }
		};
	protected:
		AVFormatContext^ m_pParent;
	internal:
		AVStreams(AVFormatContext^ _parent) : m_pParent(_parent) {}
	public:
		property AVStream^ default[int] { AVStream^ get(int index) { return m_pParent->GetStream(index); } }
		property int Count { int get() { return m_pParent->nb_streams; } }
	public:
		// IEnumerable
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { return gcnew AVStreamsEnumerator(this); }
	public:
		// IEnumerable<AVStream^>
		virtual System::Collections::Generic::IEnumerator<AVStream^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVStream^>::GetEnumerator 
		{
			return gcnew AVStreamsEnumerator(this);
		}
	};
protected:
	// Callback
	AVIOInterruptDesc^	m_pInterruptCB;
internal:
	AVFormatContext(void * _pointer,AVBase^ _parent);
public:
	AVFormatContext();
public:
	/// A class for logging and @ref avoptions. Set by avformat_alloc_context().
	/// Exports (de)muxer private options if they exist.
	property AVClass^ av_class { AVClass^ get(); }
	/// The input container format.
	/// Demuxing only, set by avformat_open_input().
	property AVInputFormat^ iformat { AVInputFormat^ get(); void set(AVInputFormat^); }
	/// The output container format.
	/// Muxing only, must be set by the caller before avformat_write_header().
	property AVOutputFormat^ oformat { AVOutputFormat^ get(); void set(AVOutputFormat^); }
	/// Format private data. This is an AVOptions-enabled struct
	/// if and only if iformat/oformat.priv_class is not NULL.
	///
	/// - muxing: set by avformat_write_header()
	/// - demuxing: set by avformat_open_input()
	property IntPtr priv_data { IntPtr get(); }

	/// I/O context.
	///
	/// - demuxing: either set by the user before avformat_open_input() (then
	///             the user must close it manually) or set by avformat_open_input().
	/// - muxing: set by the user before avformat_write_header(). The caller must
	///           take care of closing / freeing the IO context.
	///
	/// Do NOT set this field if AVFMT_NOFILE flag is set in
	/// iformat/oformat.flags. In such a case, the (de)muxer will handle
	/// I/O in some other way and this field will be NULL.
	property AVIOContext^ pb  { AVIOContext^ get(); void set(AVIOContext^); }

	/// Flags signalling stream properties. A combination of AVFMTCTX_
	/// Set by libavformat.
	property AVFMTCTX ctx_flags { AVFMTCTX get(); }

	/// Number of elements in AVFormatContext.streams.
	///
	/// Set by avformat_new_stream(), must not be modified by any other code.
	property int nb_streams { int get(); }

	/// A list of all streams in the file. New streams are created with
	/// avformat_new_stream().
	///
	/// - demuxing: streams are created by libavformat in avformat_open_input().
	///             If AVFMTCTX_NOHEADER is set in ctx_flags, then new streams may also
	///             appear in av_read_frame().
	/// - muxing: streams are created by the user before avformat_write_header().
	///
	/// Freed by libavformat in avformat_free_context().
	property AVStreams^ streams { AVStreams^ get(); };

	/// input or output filename
	///
	/// - demuxing: set by avformat_open_input()
	/// - muxing: may be set by the caller before avformat_write_header()
	property String^ filename { String^ get(); void set(String^); }

	/// input or output URL. Unlike the old filename field, this field has no
    /// length restriction.
    ///
    /// - demuxing: set by avformat_open_input(), initialized to an empty
    ///             string if url parameter was NULL in avformat_open_input().
    /// - muxing: may be set by the caller before calling avformat_write_header()
    ///           (or avformat_init_output() if that is called first) to a string
    ///           which is freeable by av_free(). Set to an empty string if it
    ///           was NULL in avformat_init_output().
    ///
    /// Freed by libavformat in avformat_free_context().
	property String^ url { String^ get(); void set(String^); }

	/// Position of the first frame of the component, in
	/// AV_TIME_BASE fractional seconds. NEVER set this value directly:
	/// It is deduced from the AVStream values.
	///
	/// Demuxing only, set by libavformat.
	property Int64 start_time { Int64 get(); }

	/// Duration of the stream, in AV_TIME_BASE fractional
    /// seconds. Only set this value if you know none of the individual stream
    /// durations and also do not set any of them. This is deduced from the
    /// AVStream values if not set.
    ///
    /// Demuxing only, set by libavformat.
	property Int64 duration { Int64 get(); }

	/// Total stream bitrate in bit/s, 0 if not
	/// available. Never set it directly if the file_size and the
	/// duration are known as FFmpeg can compute it automatically.
	property Int64 bit_rate { Int64 get(); void set(Int64 ); }

	property UInt32 packet_size { UInt32 get(); void set(UInt32); }
	property int max_delay { int get(); void set(int); }

	/// Flags modifying the (de)muxer behaviour. A combination of AVFMT_FLAG_*.
	/// Set by the user before avformat_open_input() / avformat_write_header().
	property AVfmtFlag flags { AVfmtFlag get(); void set(AVfmtFlag); }

	/// Maximum size of the data read from input for determining
	/// the input container format.
	/// Demuxing only, set by the caller before avformat_open_input()
	/// via AVOptions (NO direct access).
	property Int64 probesize { Int64 get(); void set(Int64); }

	/// Maximum duration (in AV_TIME_BASE units) of the data read
	/// from input in avformat_find_stream_info().
	/// Demuxing only, set by the caller before avformat_find_stream_info()
	/// via AVOptions (NO direct access).
	/// Can be set to 0 to let avformat choose using a heuristic.
	property Int64 max_analyze_duration { Int64 get(); void set(Int64); }

	property array<byte>^ key { array<byte>^ get(); void set(array<byte>^); }

	property UInt32 nb_programs { UInt32 get(); }
	property AVProgram^ programs [int] { AVProgram^ get(int); };

	/// Forced video codec_id.
	/// Demuxing: Set by user.
	property AVCodecID video_codec_id { AVCodecID get(); void set(AVCodecID); }
	
	/// Forced audio codec_id.
	/// Demuxing: Set by user.
	property AVCodecID audio_codec_id { AVCodecID get(); void set(AVCodecID); }

	/// Forced subtitle codec_id.
	/// Demuxing: Set by user.
	property AVCodecID subtitle_codec_id { AVCodecID get(); void set(AVCodecID); }

	/// Maximum amount of memory in bytes to use for the index of each stream.
	/// If the index exceeds this size, entries will be discarded as
	/// needed to maintain a smaller size. This can lead to slower or less
	/// accurate seeking (depends on demuxer).
	/// Demuxers for which a full in-memory index is mandatory will ignore
	/// this.
	/// - muxing: unused
	/// - demuxing: set by user
	property UInt32 max_index_size { UInt32 get(); void set(UInt32); }

	/// Maximum amount of memory in bytes to use for buffering frames
	/// obtained from realtime capture devices.
	property UInt32 max_picture_buffer { UInt32 get(); void set(UInt32); }

	/// Number of chapters in AVChapter array.
	/// When muxing, chapters are normally written in the file header,
	/// so nb_chapters should normally be initialized before write_header
	/// is called. Some muxers (e.g. mov and mkv) can also write chapters
	/// in the trailer.  To write chapters in the trailer, nb_chapters
	/// must be zero when write_header is called and non-zero when
	/// write_trailer is called.
	/// - muxing: set by user
	/// - demuxing: set by libavformat
	property array<AVChapter^>^ chapters { array<AVChapter^>^ get(); void set(array<AVChapter^>^); };

	/// Metadata that applies to the whole file.
	///
	/// - demuxing: set by libavformat in avformat_open_input()
	/// - muxing: may be set by the caller before avformat_write_header()
	///
	/// Freed by libavformat in avformat_free_context().
	property AVDictionary^ metadata { AVDictionary^ get(); void set(AVDictionary^); }

	/// Start time of the stream in real world time, in microseconds
	/// since the Unix epoch (00:00 1st January 1970). That is, pts=0 in the
	/// stream was captured at this real world time.
	/// - muxing: Set by the caller before avformat_write_header(). If set to
	///           either 0 or AV_NOPTS_VALUE, then the current wall-time will
	///           be used.
	/// - demuxing: Set by libavformat. AV_NOPTS_VALUE if unknown. Note that
	///             the value may become known after some number of frames
	///             have been received.
	property Int64 start_time_realtime  { Int64 get(); void set(Int64); }

	/// The number of frames used for determining the framerate in
	/// avformat_find_stream_info().
	/// Demuxing only, set by the caller before avformat_find_stream_info().
	property int fps_probe_size { int get(); void set(int); }

	/// Error recognition; higher values will detect more errors but may
	/// misdetect some more or less valid parts as errors.
	/// Demuxing only, set by the caller before avformat_open_input().
	property int error_recognition { int get(); void set(int); }

	/// Custom interrupt callbacks for the I/O layer.
	///
	/// demuxing: set by the user before avformat_open_input().
	/// muxing: set by the user before avformat_write_header()
	/// (mainly useful for AVFMT_NOFILE formats). The callback
	/// should also be passed to avio_open2() if it's used to
	/// open the file.
	property AVIOInterruptDesc^ interrupt_callback { AVIOInterruptDesc^ get(); void set(AVIOInterruptDesc^); }

	/// Flags to enable debugging.
	property FDebugFlags debug { FDebugFlags get(); void set(FDebugFlags); }

	/// Maximum buffering duration for interleaving.
	///
	/// To ensure all the streams are interleaved correctly,
	/// av_interleaved_write_frame() will wait until it has at least one packet
	/// for each stream before actually writing any packets to the output file.
	/// When some streams are "sparse" (i.e. there are large gaps between
	/// successive packets), this can result in excessive buffering.
	///
	/// This field specifies the maximum difference between the timestamps of the
	/// first and the last packet in the muxing queue, above which libavformat
	/// will output a packet regardless of whether it has queued a packet for all
	/// the streams.
	///
	/// Muxing only, set by the caller before avformat_write_header().
	property Int64 max_interleave_delta { Int64 get(); void set(Int64); }

	/// Allow non-standard and experimental extension
	/// @see AVCodecContext.strict_std_compliance
	property FFCompliance strict_std_compliance { FFCompliance get(); void set(FFCompliance); }

	/// Flags for the user to detect events happening on the file. Flags must
	/// be cleared by the user once the event has been handled.
	/// A combination of AVFmtEventFlags.
	property AVFmtEventFlags event_flags { AVFmtEventFlags get(); void set(AVFmtEventFlags); }

	/// Maximum number of packets to read while waiting for the first timestamp.
	/// Decoding only.
	property int max_ts_probe { int get(); void set(int); }

	/// Avoid negative timestamps during muxing.
	/// Any value of the AVFMT_AVOID_NEG_TS_/// constants.
	/// Note, this only works when using av_interleaved_write_frame. (interleave_packet_per_dts is in use)
	/// - muxing: Set by user
	/// - demuxing: unused
	property AVFmtAvoidNegTS avoid_negative_ts { AVFmtAvoidNegTS get(); void set(AVFmtAvoidNegTS); }

	/// Transport stream id.
	/// This will be moved into demuxer private options. Thus no API/ABI compatibility
	property int ts_id { int get(); void set(int); }

	/// Audio preload in microseconds.
	/// Note, not all formats support this and unpredictable things may happen if it is used when not supported.
	/// - encoding: Set by user via AVOptions (NO direct access)
	/// - decoding: unused
	property int audio_preload { int get(); void set(int); }

	/// Max chunk time in microseconds.
	/// Note, not all formats support this and unpredictable things may happen if it is used when not supported.
	/// - encoding: Set by user via AVOptions (NO direct access)
	/// - decoding: unused
	property int max_chunk_duration { int get(); void set(int); }

	/// Max chunk size in bytes
	/// Note, not all formats support this and unpredictable things may happen if it is used when not supported.
	/// - encoding: Set by user via AVOptions (NO direct access)
	/// - decoding: unused
	property int max_chunk_size { int get(); void set(int); }

	/// forces the use of wallclock timestamps as pts/dts of packets
    /// This has undefined results in the presence of B frames.
    /// - encoding: unused
    /// - decoding: Set by user via AVOptions (NO direct access)
	property int use_wallclock_as_timestamps { int get(); void set(int); }

	/// avio flags, used to force AVIO_FLAG_DIRECT.
	/// - encoding: unused
	/// - decoding: Set by user via AVOptions (NO direct access)
	property AvioFlag avio_flags { AvioFlag get(); void set(AvioFlag); }

	/// The duration field can be estimated through various ways, and this field can be used
	/// to know how the duration was estimated.
	/// - encoding: unused
	/// - decoding: Read by user via AVOptions (NO direct access)
	property AVDurationEstimationMethod duration_estimation_method { AVDurationEstimationMethod get(); }

	/// Skip initial bytes when opening stream
	/// - encoding: unused
	/// - decoding: Set by user via AVOptions (NO direct access)
	property Int64 skip_initial_bytes { Int64 get(); void set(Int64); }

	/// Correct single timestamp overflows
	/// - encoding: unused
	/// - decoding: Set by user via AVOptions (NO direct access)
	property bool correct_ts_overflow { bool get(); void set(bool); }

	/// Force seeking to any (also non key) frames.
	/// - encoding: unused
	/// - decoding: Set by user via AVOptions (NO direct access)
	property bool seek2any { bool get(); void set(bool); }

	/// Flush the I/O context after each packet.
	/// - encoding: Set by user via AVOptions (NO direct access)
	/// - decoding: unused
	property bool flush_packets { bool get(); void set(bool); }

	/// format probing score.
	/// The maximal score is AVPROBE_SCORE_MAX, its set when the demuxer probes
	/// the format.
	/// - encoding: unused
	/// - decoding: set by avformat, read by user via av_format_get_probe_score() (NO direct access)
	property int probe_score { int get(); }

	/// number of bytes to read maximally to identify format.
	/// - encoding: unused
	/// - decoding: set by user through AVOPtions (NO direct access)
	property int format_probesize { int get(); void set(int); }

	/// ',' separated list of allowed decoders.
	/// If NULL then all are allowed
	/// - encoding: unused
	/// - decoding: set by user through AVOptions (NO direct access)
	property String^ codec_whitelist { String^ get(); void set(String^); }

	/// ',' separated list of allowed demuxers.
	/// If NULL then all are allowed
	/// - encoding: unused
	/// - decoding: set by user through AVOptions (NO direct access)
	property String^ format_whitelist { String^ get(); void set(String^); }

	/// IO repositioned flag.
	/// This is set by avformat when the underlaying IO context read pointer
	/// is repositioned, for example when doing byte based seeking.
	/// Demuxers can use the flag to detect such changes.
	property int io_repositioned { int get(); }

	/// Forced video codec.
	/// This allows forcing a specific decoder, even when there are multiple with
	/// the same codec_id.
	/// Demuxing: Set by user via av_format_set_video_codec (NO direct access).
	property AVCodec^ video_codec { AVCodec^ get(); void set(AVCodec^); }

	/// Forced audio codec.
	/// This allows forcing a specific decoder, even when there are multiple with
	/// the same codec_id.
	/// Demuxing: Set by user via av_format_set_audio_codec (NO direct access).
	property AVCodec^ audio_codec { AVCodec^ get(); void set(AVCodec^); }

	/// Forced subtitle codec.
	/// This allows forcing a specific decoder, even when there are multiple with
	/// the same codec_id.
	/// Demuxing: Set by user via av_format_set_subtitle_codec (NO direct access).
	property AVCodec^ subtitle_codec { AVCodec^ get(); void set(AVCodec^); }

	/// Forced data codec.
	/// This allows forcing a specific decoder, even when there are multiple with
	/// the same codec_id.
	/// Demuxing: Set by user via av_format_set_data_codec (NO direct access).
	property AVCodec^ data_codec { AVCodec^ get(); void set(AVCodec^); }

	/// Number of bytes to be written as padding in a metadata header.
	/// Demuxing: Unused.
	/// Muxing: Set by user via av_format_set_metadata_header_padding.
	property int metadata_header_padding { int get(); void set(int); }

	/// User data.
	/// This is a place for some private data of the user.
	/// Mostly usable with control_message_cb or any future callbacks in device's context.
	property IntPtr opaque { IntPtr get(); void set(IntPtr); }

	/// Callback used by devices to communicate with application.
	//av_format_control_message control_message_cb;

	/// Output timestamp offset, in microseconds.
	/// Muxing: set by user via AVOptions (NO direct access)
	property Int64 output_ts_offset { Int64 get(); void set(Int64); }

	/// dump format separator.
	/// can be ", " or "\n      " or anything else
	/// - muxing: Set by user.
	/// - demuxing: Set by user.
	property String^ dump_separator { String^ get(); void set(String^); }

	/// Forced Data codec_id.
	/// Demuxing: Set by user.
	property AVCodecID data_codec_id  { AVCodecID get(); void set(AVCodecID); }

	//int (*open_cb)(struct AVFormatContext *s, AVIOContext **p, const char
	//	*url, int flags, const AVIOInterruptCB *int_cb, AVDictionary **options);

	/// ',' separated list of allowed protocols.
    /// - encoding: unused
    /// - decoding: set by user
	property String^ protocol_whitelist { String^ get(); void set(String^); }

	//int (*io_open)(struct AVFormatContext *s, AVIOContext **pb, const char *url, int flags, AVDictionary **options);
	//void (*io_close)(struct AVFormatContext *s, AVIOContext *pb);

	/// ',' separated list of disallowed protocols.
    /// - encoding: unused
    /// - decoding: set by user
	property String^ protocol_blacklist { String^ get(); void set(String^); }

	/// The maximum number of streams.
	/// - encoding: unused
	/// - decoding: set by user
	property int max_streams { int get(); void set(int); }
private:
	AVStream^ GetStream(int index);
public:
	/// Add a new stream to a media file.
	/// When demuxing, it is called by the demuxer in read_header(). If the
	/// flag AVFMTCTX_NOHEADER is set in s.ctx_flags, then it may also
	/// be called in read_packet().
	///
	/// When muxing, should be called by the user before avformat_write_header().
	///
	/// User is required to call avcodec_close() and avformat_free_context() to
	/// clean up the allocation by avformat_new_stream().
	///
	/// @param s media file handle
	/// @param c If non-NULL, the AVCodecContext corresponding to the new stream
	/// will be initialized to use this codec. This is needed for e.g. codec-specific
	/// defaults to be set, so codec should be provided if it is known.
	///
	/// @return newly created stream or NULL on error.
	AVStream^ AddStream();
	AVStream^ AddStream(AVCodec^ c);

	AVProgram^ AddProgram(int id);
public:
	property int default_stream_index{ int get(); }
public:
	/// Print detailed information about the input or output format, such as
	/// duration, bitrate, streams, container, programs, metadata, side data,
	/// codec and time base.
	///
	/// @param ic        the context to analyze
	/// @param index     index of the stream to dump information about
	/// @param url       the URL to print, such as source or destination file
	/// @param is_output Select whether the specified context is an input(0) or output(1)
	void DumpFormat(int index, String^ url, bool is_output);
public:
	///Open an input stream and read the header. The codecs are not opened.
	///The stream must be closed with avformat_close_input().
	///
	///@param ps Pointer to user-supplied AVFormatContext (allocated by avformat_alloc_context).
	///          May be a pointer to NULL, in which case an AVFormatContext is allocated by this
	///          function and written into ps.
	///          Note that a user-supplied AVFormatContext will be freed on failure.
	///@param url URL of the stream to open.
	///@param fmt If non-NULL, this parameter forces a specific input format.
	///           Otherwise the format is autodetected.
	///@param options  A dictionary filled with AVFormatContext and demuxer-private options.
	///                On return this parameter will be destroyed and replaced with a dict containing
	///                options that were not found. May be NULL.
	///
	///@return 0 on success, a negative AVERROR on failure.
	///
	///@note If you want to use custom IO, preallocate the format context and set its pb field.
	AVRESULT OpenInput();
	AVRESULT OpenInput(AVDictionary^ options);
	AVRESULT OpenInput(String^ filename);
	AVRESULT OpenInput(AVInputFormat^ format, String^ filename);
	AVRESULT OpenInput(AVInputFormat^ format, String^ filename, AVDictionary^ options);
	// Open with allocation of context
	static AVRESULT OpenInput([Out] AVFormatContext^ % context);
	static AVRESULT OpenInput([Out] AVFormatContext^ % context,AVDictionary^ options);
	static AVRESULT OpenInput([Out] AVFormatContext^ % context,String^ filename);
	static AVRESULT OpenInput([Out] AVFormatContext^ % context,AVInputFormat^ format, String^ filename);
	static AVRESULT OpenInput([Out] AVFormatContext^ % context,AVInputFormat^ format, String^ filename, AVDictionary^ options);
public:
	/// Read packets of a media file to get stream information. This
	/// is useful for file formats with no headers such as MPEG. This
	/// function also computes the real framerate in case of MPEG-2 repeat
	/// frame mode.
	/// The logical file position is not changed by this function;
	/// examined packets may be buffered for later processing.
	///
	/// @param options  If non-NULL, an ic.nb_streams long array of pointers to
	///                 dictionaries, where i-th member contains options for
	///                 codec corresponding to i-th stream.
	///                 On return each dictionary will be filled with options that were not found.
	/// @return >=0 if OK, AVERROR_xxx on error
	///
	/// @note this function isn't guaranteed to open all the codecs, so
	///       options being non-empty at return is a perfectly normal behavior.
	///
	/// @todo Let the user decide somehow what information is needed so that
	///       we do not waste time getting stuff the user does not need.
	AVRESULT FindStreamInfo();
	AVRESULT FindStreamInfo(array<AVDictionary^>^ options);

	/// Find the programs which belong to a given stream.
	///
	/// @param last  the last found program, the search will start after this
	///              program, or from the beginning if it is NULL
	/// @param s     stream index
	/// @return the next program which belongs to s, NULL if no program is found or
	///         the last program is not among the programs of ic.
	AVProgram^ FindProgramFromStream(AVProgram^ last, int s);

	//void av_program_add_stream_index(AVFormatContext *ac, int progid, unsigned int idx);

	/// Find the "best" stream in the file.
	/// The best stream is determined according to various heuristics as the most
	/// likely to be what the user expects.
	/// If the decoder parameter is non-NULL, av_find_best_stream will find the
	/// default decoder for the stream's codec; streams for which no decoder can
	/// be found are ignored.
	///
	/// @param type              stream type: video, audio, subtitles, etc.
	/// @param wanted_stream_nb  user-requested stream number,
	///                          or -1 for automatic selection
	/// @param related_stream    try to find a stream related (eg. in the same
	///                          program) to this one, or -1 if none
	/// @param decoder_ret       if non-NULL, returns the decoder for the
	///                          selected stream
	/// @param flags             flags; none are currently defined
	/// @return  the non-negative stream number in case of success,
	///          AVERROR_STREAM_NOT_FOUND if no stream with the requested type
	///          could be found,
	///          AVERROR_DECODER_NOT_FOUND if streams were found but no decoder
	/// @note  If av_find_best_stream returns successfully and decoder_ret is not
	///        NULL, then ///decoder_ret is guaranteed to be set to a valid AVCodec.
	int FindBestStream(AVMediaType type);
	int FindBestStream(AVMediaType type, int wanted_stream_nb, int related_stream);
	int FindBestStream(AVMediaType type, [Out] AVCodec^ % decoder_ret);
	int FindBestStream(AVMediaType type, int wanted_stream_nb, int related_stream, [Out] AVCodec^ % decoder_ret);

	/// Return the next frame of a stream.
	/// This function returns what is stored in the file, and does not validate
	/// that what is there are valid frames for the decoder. It will split what is
	/// stored in the file into frames and return one for each call. It will not
	/// omit invalid data between valid frames so as to give the decoder the maximum
	/// information possible for decoding.
	///
	/// If pkt->buf is NULL, then the packet is valid until the next
	/// av_read_frame() or until avformat_close_input(). Otherwise the packet
	/// is valid indefinitely. In both cases the packet must be freed with
	/// av_free_packet when it is no longer needed. For video, the packet contains
	/// exactly one frame. For audio, it contains an integer number of frames if each
	/// frame has a known fixed size (e.g. PCM or ADPCM data). If the audio frames
	/// have a variable size (e.g. MPEG audio), then it contains one frame.
	///
	/// pkt->pts, pkt->dts and pkt->duration are always set to correct
	/// values in AVStream.time_base units (and guessed if the format cannot
	/// provide them). pkt->pts can be AV_NOPTS_VALUE if the video format
	/// has B-frames, so it is better to rely on pkt->dts if you do not
	/// decompress the payload.
	///
	/// @return 0 if OK, < 0 on error or end of file
	AVRESULT ReadFrame(AVPacket^ pkt);

	/// Seek to the keyframe at timestamp.
	/// 'timestamp' in 'stream_index'.
	///
	/// @param stream_index If stream_index is (-1), a default
	/// stream is selected, and timestamp is automatically converted
	/// from AV_TIME_BASE units to the stream specific time_base.
	/// @param timestamp Timestamp in AVStream.time_base units
	///        or, if no stream is specified, in AV_TIME_BASE units.
	/// @param flags flags which select direction and seeking mode
	/// @return >= 0 on success
	AVRESULT SeekFrame(int stream_index, __int64 timestamp, AVSeekFlag flags);

	/// Seek to timestamp ts.
	/// Seeking will be done so that the point from which all active streams
	/// can be presented successfully will be closest to ts and within min/max_ts.
	/// Active streams are all streams that have AVStream.discard < AVDISCARD_ALL.
	///
	/// If flags contain AVSEEK_FLAG_BYTE, then all timestamps are in bytes and
	/// are the file position (this may not be supported by all demuxers).
	/// If flags contain AVSEEK_FLAG_FRAME, then all timestamps are in frames
	/// in the stream with stream_index (this may not be supported by all demuxers).
	/// Otherwise all timestamps are in units of the stream selected by stream_index
	/// or if stream_index is -1, in AV_TIME_BASE units.
	/// If flags contain AVSEEK_FLAG_ANY, then non-keyframes are treated as
	/// keyframes (this may not be supported by all demuxers).
	/// If flags contain AVSEEK_FLAG_BACKWARD, it is ignored.
	///
	/// @param stream_index index of the stream which is used as time base reference
	/// @param min_ts smallest acceptable timestamp
	/// @param ts target timestamp
	/// @param max_ts largest acceptable timestamp
	/// @param flags flags
	/// @return >=0 on success, error code otherwise
	///
	/// @note This is part of the new seek API which is still under construction.
	///       Thus do not use this yet. It may change at any time, do not expect
	///       ABI compatibility yet!
	AVRESULT SeekFile(int stream_index, __int64 min_ts, __int64 ts, __int64 max_ts, AVSeekFlag flags);

	/// Discard all internally buffered data. This can be useful when dealing with
	/// discontinuities in the byte stream. Generally works only with formats that
	/// can resync. This includes headerless formats like MPEG-TS/TS but should also
	/// work with NUT, Ogg and in a limited way AVI for example.
	///
	/// The set of streams, the detected duration, stream parameters and codecs do
	/// not change when calling this function. If you want a complete reset, it's
	/// better to open a new AVFormatContext.
	///
	/// This does not flush the AVIOContext (s->pb). If necessary, call
	/// avio_flush(s->pb) before calling this function.
	///
	/// @return >=0 on success, error code otherwise
	AVRESULT Flush();

	/// Start playing a network-based stream (e.g. RTSP stream) at the
	/// current position.
	AVRESULT ReadPlay();

	/// Pause a network-based stream (e.g. RTSP stream).
	///
	/// Use av_read_play() to resume it.
	AVRESULT ReadPause();

	/// Allocate the stream private data and write the stream header to
	/// an output media file.
	///
	/// @param s Media file handle, must be allocated with avformat_alloc_context().
	///          Its oformat field must be set to the desired output format;
	///          Its pb field must be set to an already opened AVIOContext.
	/// @param options  An AVDictionary filled with AVFormatContext and muxer-private options.
	///                 On return this parameter will be destroyed and replaced with a dict containing
	///                 options that were not found. May be NULL.
	///
	/// @return 0 on success, negative AVERROR on failure.
	///
	/// @see av_opt_find, av_dict_set, avio_open, av_oformat_next.
	AVRESULT WriteHeader();
	AVRESULT WriteHeader(AVDictionary^ options);

	 /// Allocate the stream private data and initialize the codec, but do not write the header.
	 /// May optionally be used before avformat_write_header to initialize stream parameters
	 /// before actually writing the header.
	 /// If using this function, do not pass the same options to avformat_write_header.
	 ///
	 /// @param s Media file handle, must be allocated with avformat_alloc_context().
	 ///          Its oformat field must be set to the desired output format;
	 ///          Its pb field must be set to an already opened AVIOContext.
	 /// @param options  An AVDictionary filled with AVFormatContext and muxer-private options.
	 ///                 On return this parameter will be destroyed and replaced with a dict containing
	 ///                 options that were not found. May be NULL.
	 ///
	 /// @return AVSTREAM_INIT_IN_WRITE_HEADER on success if the codec requires avformat_write_header to fully initialize,
	 ///         AVSTREAM_INIT_IN_INIT_OUTPUT  on success if the codec has been fully initialized,
	 ///         negative AVERROR on failure.
	AVRESULT InitOutput(AVDictionary^ options);

	/// Write a packet to an output media file.
	///
	/// This function passes the packet directly to the muxer, without any buffering
	/// or reordering. The caller is responsible for correctly interleaving the
	/// packets if the format requires it. Callers that want libavformat to handle
	/// the interleaving should call av_interleaved_write_frame() instead of this
	/// function.
	///
	/// @param s media file handle
	/// @param pkt The packet containing the data to be written. Note that unlike
	///            av_interleaved_write_frame(), this function does not take
	///            ownership of the packet passed to it (though some muxers may make
	///            an internal reference to the input packet).
	///            <br>
	///            This parameter can be NULL (at any time, not just at the end), in
	///            order to immediately flush data buffered within the muxer, for
	///            muxers that buffer up data internally before writing it to the
	///            output.
	///            <br>
	///            Packet's @ref AVPacket.stream_index "stream_index" field must be
	///            set to the index of the corresponding stream in @ref
	///            AVFormatContext.streams "s->streams". It is very strongly
	///            recommended that timing information (@ref AVPacket.pts "pts", @ref
	///            AVPacket.dts "dts", @ref AVPacket.duration "duration") is set to
	///            correct values.
	/// @return < 0 on error, = 0 if OK, 1 if flushed and there is no more data to flush
	///
	/// @see av_interleaved_write_frame()
	AVRESULT WriteFrame(AVPacket^ pkt);

	/// Write a packet to an output media file ensuring correct interleaving.
	///
	/// This function will buffer the packets internally as needed to make sure the
	/// packets in the output file are properly interleaved in the order of
	/// increasing dts. Callers doing their own interleaving should call
	/// av_write_frame() instead of this function.
	///
	/// @param pkt The packet containing the data to be written.
	///            <br>
	///            If the packet is reference-counted, this function will take
	///            ownership of this reference and unreference it later when it sees
	///            fit.
	///            The caller must not access the data through this reference after
	///            this function returns. If the packet is not reference-counted,
	///            libavformat will make a copy.
	///            <br>
	///            This parameter can be NULL (at any time, not just at the end), to
	///            flush the interleaving queues.
	///            <br>
	///            Packet's @ref AVPacket.stream_index "stream_index" field must be
	///            set to the index of the corresponding stream in @ref
	///            AVFormatContext.streams "s->streams". It is very strongly
	///            recommended that timing information (@ref AVPacket.pts "pts", @ref
	///            AVPacket.dts "dts", @ref AVPacket.duration "duration") is set to
	///            correct values.
	///
	/// @return 0 on success, a negative AVERROR on error. Libavformat will always
	///         take care of freeing the packet, even if this function fails.
	///
	/// @see av_write_frame(), AVFormatContext.max_interleave_delta
	AVRESULT InterleavedWriteFrame(AVPacket^ pkt);

	/// Write a uncoded frame to an output media file.
	///
	/// The frame must be correctly interleaved according to the container
	/// specification; if not, then av_interleaved_write_frame() must be used.
	///
	/// See av_interleaved_write_frame() for details.
	AVRESULT WriteUncodedFrame(int stream_index,AVFrame^ frame);

	/// Write a uncoded frame to an output media file.
	///
	/// If the muxer supports it, this function allows to write an AVFrame
	/// structure directly, without encoding it into a packet.
	/// It is mostly useful for devices and similar special muxers that use raw
	/// video or PCM data and will not serialize it into a byte stream.
	///
	/// To test whether it is possible to use it with a given muxer and stream,
	/// use av_write_uncoded_frame_query().
	///
	/// The caller gives up ownership of the frame and must not access it
	/// afterwards.
	///
	/// @return  >=0 for success, a negative code on error
	AVRESULT InterleavedWriteUncodedFrame(int stream_index,AVFrame^ frame);

	/// Test whether a muxer supports uncoded frame.
	///
	/// @return  >=0 if an uncoded frame can be written to that muxer and stream,
	///          <0 if not
	AVRESULT WriteUncodedFrameQuery(int stream_index);

	/// Write the stream trailer to an output media file and free the
	/// file private data.
	///
	/// May only be called after a successful call to avformat_write_header.
	///
	/// @param s media file handle
	/// @return 0 if OK, AVERROR_xxx on error
	AVRESULT WriteTrailer();

	/// Get timing information for the data currently output.
	/// The exact meaning of "currently output" depends on the format.
	/// It is mostly relevant for devices that have an internal buffer and/or
	/// work in real time.
	/// @param s          media file handle
	/// @param stream     stream in the media file
	/// @param[out] dts   DTS of the last packet output for the stream, in stream
	///                   time_base units
	/// @param[out] wall  absolute time when that packet whas output,
	///                   in microsecond
	/// @return  0 if OK, AVERROR(ENOSYS) if the format does not support it
	/// Note: some formats or devices may not allow to measure dts and wall
	/// atomically.
	AVRESULT GetOutputTimestamp(int stream,Int64 % dts,Int64 % wall);

	/// This function will cause global side data to be injected in the next packet
	/// of each stream as well as after any subsequent seek.
	void inject_global_side_data();

	/// Guess the sample aspect ratio of a frame, based on both the stream and the
	/// frame aspect ratio.
	///
	/// Since the frame aspect ratio is set by the codec but the stream aspect ratio
	/// is set by the demuxer, these two may not be equal. This function tries to
	/// return the value that you should use if you would like to display the frame.
	///
	/// Basic logic is to use the stream aspect ratio if it is set to something sane
	/// otherwise use the frame aspect ratio. This way a container setting, which is
	/// usually easy to modify can override the coded value in the frames.
	///
	/// @param stream the stream which the frame is part of
	/// @param frame the frame with the aspect ratio to be determined
	/// @return the guessed (valid) sample_aspect_ratio, 0/1 if no idea
	AVRational^ GuessSampleAspectRatio(AVStream^ stream, AVFrame^ frame);

	/// Guess the frame rate, based on both the container and codec information.
	///
	/// @param stream the stream which the frame is part of
	/// @param frame the frame for which the frame rate should be determined, may be NULL
	/// @return the guessed (valid) frame rate, 0/1 if no idea
	AVRational^ GuessFrameRate(AVStream^ stream, AVFrame^ frame);
	AVRational^ GuessFrameRate(AVStream^ stream);

	/// Check if the stream st contained in s is matched by the stream specifier
	/// spec.
	///
	/// See the "stream specifiers" chapter in the documentation for the syntax
	/// of spec.
	///
	/// @return  >0 if st is matched by spec;
	///          0  if st is not matched by spec;
	///          AVERROR code if spec is invalid
	///
	/// @note  A stream specifier can match several streams in the format.
	int match_stream_specifier(AVStream^ st,String^ spec);

	AVRESULT queue_attached_pictures();
public:
	static AVFormatContext^ OpenOutput(String^ format_name,String^ filename);
	static AVFormatContext^ OpenOutput(AVOutputFormat^ format,String^ filename);
	static AVFormatContext^ OpenOutput(String^ filename);
	static AVFormatContext^ OpenOutput(AVOutputFormat^ format);
	static AVFormatContext^ OpenOutput(AVOutputFormat^ format, String^ format_name, String^ filename);
	static AVFormatContext^ OpenInputFile(String^ filename);
	static AVFormatContext^ OpenInputFile(AVInputFormat^ format, String^ filename);
	static AVFormatContext^ OpenInputFile(AVInputFormat^ format, String^ filename, AVDictionary^ options);
public:
	static String^ SDP_Create(array<AVFormatContext^>^ ac);
	
	/// Generate an SDP for an RTP session.
	///
	/// Note, this overwrites the id values of AVStreams in the muxer contexts
	/// for getting unique dynamic payload types.
	///
	/// @param ac array of AVFormatContexts describing the RTP streams. If the
	///           array is composed by only one context, such context can contain
	///           multiple AVStreams (one AVStream per RTP stream). Otherwise,
	///           all the contexts in the array (an AVCodecContext per RTP stream)
	///           must contain only one AVStream.
	/// @param n_files number of AVCodecContexts contained in ac
	/// @param buf buffer where the SDP will be stored (must be allocated by
	///            the caller)
	/// @param size the size of the buffer
	/// @return 0 if OK, AVERROR_xxx on error
	static AVRESULT SDP_Create(array<AVFormatContext^>^ ac,IntPtr buf, int size);
	static AVRESULT SDP_Create(array<AVFormatContext^>^ ac,Text::StringBuilder^ sb);

	String^ SDP_Create();
};
//////////////////////////////////////////////////////
// URLProtocol
public ref class URLProtocol : public AVBase
{
internal:
	URLProtocol(void * _pointer,AVBase^ _parent);
public:
	property String^ name { String^ get(); }
	property AVClass^ priv_data_class{ AVClass^ get(); }
    property UrlProtocolFlags flags { UrlProtocolFlags get(); }
public:
	virtual String^ ToString() override;
};
//////////////////////////////////////////////////////
// URLContext
public ref class URLContext : public AVBase
{
internal:
	URLContext(void * _pointer,AVBase^ _parent);
public:
	///< information for av_log(). Set by url_open().
	property AVClass^ av_class { AVClass^ get(); }
    property URLProtocol^ prot { URLProtocol^ get(); }
	property IntPtr priv_data { IntPtr get(); void set(IntPtr); }
	///< specified URL
    property String^ filename { String^ get(); }
    property AvioFlag flags { AvioFlag get(); }
	///< if non zero, the stream is packetized with this max packet size
    property int max_packet_size { int get(); void set(int); }
	/// < true if streamed (no seek possible), default = false
    property bool is_streamed { bool get(); }
    property bool is_connected { bool get(); }
    property AVIOInterruptDesc^ interrupt_callback { AVIOInterruptDesc^ get(); }
	///< maximum time to wait for (network) read/write operation completion, in mcs
    property Int64 rw_timeout { Int64 get(); void set(Int64); }
public:
	virtual String^ ToString() override;
};
//////////////////////////////////////////////////////
// AVIODirEntry
/// Describes single entry of the directory.
///
/// Only name and type fields are guaranteed be set.
/// Rest of fields are protocol or/and platform dependent and might be unknown.
public ref class AVIODirEntry : public AVBase
{
private:
	AVIODirEntry();
internal:
	AVIODirEntry(void * _pointer,AVBase^ _parent);
public:
	/// < Filename
	property String^ name { String^ get(); }
	///< Type of the entry
    property AVIODirEntryType type { AVIODirEntryType get(); }
	/// < Set to 1 when name is encoded with UTF-8, 0 otherwise.
	/// Name can be encoded with UTF-8 eventhough 0 is set.
    property bool utf8 { bool get(); }
	///< File size in bytes, -1 if unknown.
    property Int64 size { Int64 get(); }
	///< Time of last modification in microseconds since unix epoch, -1 if unknown.
    property Int64 modification_timestamp { Int64 get(); }
	/// < Time of last access in microseconds since unix epoch, -1 if unknown.
    property Int64 access_timestamp { Int64 get(); }
	/// < Time of last status change in microseconds since unix
	///	epoch, -1 if unknown. 
    property Int64 status_change_timestamp { Int64 get(); }
	///< User ID of owner, -1 if unknown.
    property Int64 user_id { Int64 get(); }
	///< Group ID of owner, -1 if unknown.
    property Int64 group_id { Int64 get(); }
	///< Unix file mode, -1 if unknown.
    property Int64 filemode { Int64 get(); }
public:
	property int _StructureSize { virtual int get() override; }
public:
	virtual String^ ToString() override;
};
//////////////////////////////////////////////////////
// AVIODirContext
public ref class AVIODirContext : public AVBase
	, public System::Collections::IEnumerable
	, public System::Collections::Generic::IEnumerable<AVIODirEntry^>
{
private:
	ref class Enumerator
		: public System::Collections::IEnumerator
		, public System::Collections::Generic::IEnumerator<AVIODirEntry^>
	{
	protected:
		AVIODirEntry^	m_pCurrent;
		AVIODirContext^	m_pContext;
	public:
		Enumerator(AVIODirContext^ context);
		~Enumerator();
	public:
		// IEnumerator
		virtual bool MoveNext();
		virtual property AVIODirEntry^ Current { AVIODirEntry^ get (); }
		virtual void Reset();
		virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
	};
internal:
	AVIODirContext(void * _pointer,AVBase^ _parent);
public:
	property URLContext^ url_context { URLContext^ get(); }
public:
	/// Get next directory entry.
	///
	/// Returned entry must be freed with avio_free_directory_entry(). In particular
	/// it may outlive AVIODirContext.
	///
	/// @param s         directory read context.
	/// @param[out] next next entry or NULL when no more entries.
	/// @return >=0 on success or negative on error. End of list is not considered an
	///             error.
	AVIODirEntry^ Read();
public:
	virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { return gcnew Enumerator(this); }
	virtual System::Collections::Generic::IEnumerator<AVIODirEntry^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVIODirEntry^>::GetEnumerator { return gcnew Enumerator(this); }
public:
	/// Open directory for reading.
	///
	/// @param s       directory read context. Pointer to a NULL pointer must be passed.
	/// @param url     directory to be listed.
	/// @param options A dictionary filled with protocol-private options. On return
	///                this parameter will be destroyed and replaced with a dictionary
	///                containing options that were not found. May be NULL.
	/// @return >=0 on success or negative on error.
	static AVIODirContext^ Open(String^ url);
	static AVIODirContext^ Open(String^ url, AVDictionary^ options);
};
//////////////////////////////////////////////////////
#pragma endregion 
//////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////