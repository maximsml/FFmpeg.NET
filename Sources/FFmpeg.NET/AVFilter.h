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
// libavfilter
// Graph-based frame editing library.
//////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;  
//////////////////////////////////////////////////////
#include "AVCodec.h"
//////////////////////////////////////////////////////
namespace FFmpeg {
//////////////////////////////////////////////////////
#pragma region Enums
//////////////////////////////////////////////////////
[Flags]
public enum class AVFilterFlags : int
{
	/// The number of the filter inputs is not determined just by AVFilter.inputs.
	/// The filter might add additional inputs during initialization depending on the
	/// options supplied to it.
	DYNAMIC_INPUTS        = (1 << 0),
	/// The number of the filter outputs is not determined just by AVFilter.outputs.
	/// The filter might add additional outputs during initialization depending on
	/// the options supplied to it.
	DYNAMIC_OUTPUTS       = (1 << 1),
	/// The filter supports multithreading by splitting frames into multiple parts
	/// and processing them concurrently.
	SLICE_THREADS         = (1 << 2),
	/// Some filters support a generic "enable" expression option that can be used
	/// to enable or disable a filter in the timeline. Filters supporting this
	/// option have this flag set. When the enable expression is false, the default
	/// no-op filter_frame() function is called in place of the filter_frame()
	/// callback defined on each input pad, thus the frame is passed unchanged to
	/// the next filters.
	SUPPORT_TIMELINE_GENERIC  = (1 << 16),
	/// Same as AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC, except that the filter will
	/// have its filter_frame() callback(s) called as usual even when the enable
	/// expression is false. The filter will disable filtering within the
	/// filter_frame() callback(s) itself, for example executing code depending on
	/// the AVFilterContext->is_disabled value.
	SUPPORT_TIMELINE_INTERNAL = (1 << 17),
	/// Handy mask to test whether the filter supports or no the timeline feature
	/// (internally or generically).
	SUPPORT_TIMELINE = (SUPPORT_TIMELINE_GENERIC | SUPPORT_TIMELINE_INTERNAL),
};
//////////////////////////////////////////////////////
public enum class AVFilterThreadType
{
	DEFAULT = 0,
	SLICE = (1 << 0),
};
//////////////////////////////////////////////////////
public enum class AVFilterAutoConvert : int
{
	///< all automatic conversions enabled
    ALL  =  0,
	///< all automatic conversions disabled
    NONE = -1, 
};
//////////////////////////////////////////////////////
public enum class AVFilterCmdFlags
{
	NONE = 0,
	///< Stop once a filter understood the command (for target=all for example), fast filters are favored automatically
	ONE   = 1,
	///< Only execute command when its fast (like a video out that supports contrast adjustment in hw)
	FAST  = 2,
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVBufferSinkFlags : int
{
	None = 0,
	/// Tell av_buffersink_get_buffer_ref() to read video/samples buffer
	/// reference, but not remove it from the buffer. This is useful if you
	/// need only to read a video/samples buffer, without to fetch it.
	Peek = 1,
	/// Tell av_buffersink_get_buffer_ref() not to request a frame from its input.
	/// If a frame is already buffered, it is read (and removed from the buffer),
	/// but if no frame is present, return AVERROR(EAGAIN).
	NoRequest = 2,
};
//////////////////////////////////////////////////////
[Flags]
public enum class AVBufferSrcFlags : int
{
	None = 0,
	/// Do not check for format changes.
	NoCheckFormat = 1,
	/// Immediately push the frame to the output.
	Push = 2,
	/// Keep a reference to the frame.
	/// If the frame if reference-counted, create a new reference; otherwise
	/// copy the frame data.
	KeepRef = 8,
};
//////////////////////////////////////////////////////
#pragma endregion
//////////////////////////////////////////////////////
#pragma region Classes
//////////////////////////////////////////////////////
ref class AVCodec;
ref class AVFilter;
ref class AVFilterPad;
ref class AVFilterLink;
ref class AVFilterGraph;
ref class AVFilterContext;
//////////////////////////////////////////////////////
// LibAVFilter
public ref class LibAVFilter
{
private:
	static bool s_bRegistered = false;
private:
	LibAVFilter();
public:
	// Return the LIBAVFILTER_VERSION_INT constant.
	static property UInt32 Version { UInt32 get(); }
	// Return the libavfilter build-time configuration.
	static property String^ Configuration { String^ get(); }
	// Return the libavfilter license.
	static property String^ License { String^ get(); }
public:
	/// Register a filter. This is only needed if you plan to use
	/// avfilter_get_by_name later to lookup the AVFilter structure by name. A
	/// filter can still by instantiated with avfilter_graph_alloc_filter even if it
	/// is not registered.
	///
	/// @param filter the filter to register
	/// @return 0 if the registration was successful, a negative value
	/// otherwise
	static void Register(AVFilter^ _filter);
	// Initialize the filter system. Register all builtin filters.
	static void RegisterAll();
};
//////////////////////////////////////////////////////
// AVFilter
/// Filter definition. This defines the pads a filter contains, and all the
/// callback functions used to interact with the filter.
public ref class AVFilter : public  AVBase
{
public:
	// Pads Collection
	ref class AVFilterPadList : public  AVBase
		, public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<AVFilterPad^>
	{
	private:
		bool m_bIsOutput;
		bool m_bUpdated;
		List<AVFilterPad^>^ m_Pads;
	internal:
		AVFilterPadList(void * _pointer,AVBase^ _parent);
	internal:
		~AVFilterPadList();
	internal:
		void SetOutputType(bool bOutput);
	public:
		array<AVFilterPad^>^ ToArray();
	private:
		void Update();
	public:
		property AVFilterPad^ default[int]  {  AVFilterPad^ get(int index) { Update(); return m_Pads[index]; } }
		property int Count { int get() { Update(); return m_Pads->Count; } }
	public:
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { Update(); return m_Pads->GetEnumerator(); }
		virtual System::Collections::Generic::IEnumerator<AVFilterPad^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVFilterPad^>::GetEnumerator { Update(); return m_Pads->GetEnumerator(); }
	};
private:
	// Filters Iterator
	ref class AVFilters
		: public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<AVFilter^>
	{
	private:
		ref class AVFilterEnumerator 
			: public System::Collections::IEnumerator
			, public System::Collections::Generic::IEnumerator<AVFilter^>
		{
		protected:
			IntPtr		m_pOpaque;
			AVFilter^	m_pCurrent;
		public:
			AVFilterEnumerator();
			~AVFilterEnumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVFilter^ Current { AVFilter^ get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
		};
	public:
		AVFilters();
	public:
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator { return gcnew AVFilterEnumerator(); }
		virtual System::Collections::Generic::IEnumerator<AVFilter^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVFilter^>::GetEnumerator { return gcnew AVFilterEnumerator(); }
	};
internal:
	AVFilter(void * _pointer,AVBase^ _parent);
public:
	/// Filter name. Must be non-NULL and unique among filters.
	property String^ name { String^ get(); }
	/// A description of the filter. May be NULL.
	///
	/// You should use the NULL_IF_CONFIG_SMALL() macro to define it.
	property String^ description { String^ get(); }
	/// List of inputs, terminated by a zeroed element.
	///
	/// NULL if there are no (static) inputs. Instances of filters with
	/// AVFILTER_FLAG_DYNAMIC_INPUTS set may have more inputs than present in
	/// this list.
	property AVFilterPadList^ inputs { AVFilterPadList^ get(); }
	/// List of outputs, terminated by a zeroed element.
	///
	/// NULL if there are no (static) outputs. Instances of filters with
	/// AVFILTER_FLAG_DYNAMIC_OUTPUTS set may have more outputs than present in
	/// this list.
	property AVFilterPadList^ outputs { AVFilterPadList^ get(); }
	/// A class for the private data, used to declare filter private AVOptions.
	/// This field is NULL for filters that do not declare any options.
	///
	/// If this field is non-NULL, the first member of the filter private data
	/// must be a pointer to AVClass, which will be set by libavfilter generic
	/// code to this class.
	property AVClass^ priv_class { AVClass^ get(); }
	/// A combination of AVFILTER_FLAG_*
	property AVFilterFlags flags { AVFilterFlags get(); }
public:
	virtual String^ ToString() override;
public:
	// Accessing Filters Collection
	/// Iterate over all registered filters.
	/// @return If prev is non-NULL, next registered filter after prev or NULL if
	/// prev is the last filter. If prev is NULL, return the first registered filter.
	static property System::Collections::Generic::IEnumerable<AVFilter^>^ Filters {
		System::Collections::Generic::IEnumerable<AVFilter^>^ get() { return gcnew AVFilters(); } 
	}
public:
	/// Get a filter definition matching the given name.
	///
	/// @param name the filter name to find
	/// @return     the filter definition, if any matching one is registered.
	///             NULL if none found.
	static AVFilter^ GetByName(String^ name);
};
//////////////////////////////////////////////////////
// AVFilterPad
public ref class AVFilterPad : public  AVBase
{
private:
	// Pad idx
	int m_nIndex;
internal:
	AVFilterPad(void * _pointer, AVBase^ _parent);
	AVFilterPad(void * _pointer,AVBase^ _parent, int index);
public:
	/// Pad name. The name is unique among inputs and among outputs, but an
	/// input may have the same name as an output. This may be NULL if this
	/// pad has no need to ever be referenced by name.
	property String^ name { String^ get(); }
	/// Get the type of an AVFilterPad.
	property AVMediaType type { AVMediaType get(); }
public:
	virtual String^ ToString() override;
};
//////////////////////////////////////////////////////
// AVFilterContext
/// An instance of a filter
public ref class AVFilterContext : public  AVBase
{
public:
	// Array Of Links
	ref class AVFilterLinkArray : public AVBase
								, public System::Collections::IEnumerable
								, public System::Collections::Generic::IEnumerable<AVFilterLink^>
	{
	private:
		ref class AVFilterLinkEnumerator : public System::Collections::IEnumerator
										 , public System::Collections::Generic::IEnumerator<AVFilterLink^>
		{
		protected:
			int					m_nIndex;
			AVFilterLinkArray^	m_pLinkArray;
		public:
			AVFilterLinkEnumerator(AVFilterLinkArray^ _array);
			~AVFilterLinkEnumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVFilterLink^ Current { AVFilterLink^ get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
		};
	internal:
		unsigned int * m_pnCount;
	internal:
		AVFilterLinkArray(void * _pointer,AVBase^ _parent);
	public:
		property AVFilterLink^ default[int] { AVFilterLink^ get(int index); }
		property int Count { int get(); }
	public:
		array<AVFilterLink^>^ ToArray();
	public:
		// IEnumerable
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator;
	public:
		// IEnumerable<AVDictionaryEntry>
		virtual System::Collections::Generic::IEnumerator<AVFilterLink^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVFilterLink^>::GetEnumerator;
	};
public:
	// Array of AVFilterPads Implementation
	ref class AVFilterPadArray : public AVBase
								, public System::Collections::IEnumerable
								, public System::Collections::Generic::IEnumerable<AVFilterPad^>
	{
	private:
		ref class AVFilterPadEnumerator : public System::Collections::IEnumerator
										 , public System::Collections::Generic::IEnumerator<AVFilterPad^>
		{
		protected:
			int					m_nIndex;
			AVFilterPadArray^	m_pArray;
		public:
			AVFilterPadEnumerator(AVFilterPadArray^ _array);
			~AVFilterPadEnumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVFilterPad^ Current { AVFilterPad^ get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
		};
	internal:
		unsigned int * m_pnCount;
	internal:
		AVFilterPadArray(void * _pointer,AVBase^ _parent);
	public:
		property AVFilterPad^ default[int] { AVFilterPad^ get(int index); }
		property int Count { int get(); }
	public:
		array<AVFilterPad^>^ ToArray();
	public:
		// IEnumerable
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator;
	public:
		// IEnumerable<AVDictionaryEntry>
		virtual System::Collections::Generic::IEnumerator<AVFilterPad^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVFilterPad^>::GetEnumerator;
	};
internal:
	AVFilterContext(void * _pointer,AVBase^ _parent);
public:
	///< needed for av_log() and filters common options
	property AVClass^ av_class { AVClass^ get(); }

	///< the AVFilter of which this is an instance
    property AVFilter^ filter { AVFilter^ get(); }

	///< name of this filter instance
    property String^ name { String^ get(); }

	///< array of input pads
    property AVFilterPadArray^  input_pads { AVFilterPadArray^ get(); }
	///< array of pointers to input links
    property AVFilterLinkArray^ inputs { AVFilterLinkArray^ get(); }

	///< array of output pads
	property AVFilterPadArray^  output_pads { AVFilterPadArray^ get(); }
	///< array of pointers to output links
    property AVFilterLinkArray^ outputs { AVFilterLinkArray^ get(); }

	///< filtergraph this filter belongs to
	property AVFilterGraph^ graph { AVFilterGraph^ get(); }

	/// Type of multithreading being allowed/used. A combination of
	/// AVFILTER_THREAD_* flags.
	///
	/// May be set by the caller before initializing the filter to forbid some
	/// or all kinds of multithreading for this filter. The default is allowing
	/// everything.
	///
	/// When the filter is initialized, this field is combined using bit AND with
	/// AVFilterGraph.thread_type to get the final mask used for determining
	/// allowed threading types. I.e. a threading type needs to be set in both
	/// to be allowed.
	///
	/// After the filter is initialized, libavfilter sets this field to the
	/// threading type that is actually used (0 for no multithreading).
	property AVFilterThreadType thread_type { AVFilterThreadType get(); void set(AVFilterThreadType); }

	///< enable expression string
	property String^ enable_str { String^ get(); void set(String^ value); }

	///< the enabled state from the last expression evaluation
	property bool is_disabled { bool get(); void set(bool value); }

	 /// For filters which will create hardware frames, sets the device the
     /// filter should create them in.  All other filters will ignore this field:
     /// in particular, a filter which consumes or processes hardware frames will
     /// instead use the hw_frames_ctx field in AVFilterLink to carry the
     /// hardware context information.
	property AVBufferRef^ hw_device_ctx { AVBufferRef^ get(); }

	 /// Max number of threads allowed in this filter instance.
     /// If <= 0, its value is ignored.
     /// Overrides global number of threads set per filter graph.
	property int nb_threads { int get(); void set(int value); }

	/// Ready status of the filter.
    /// A non-0 value means that the filter needs activating;
	property bool ready { bool get(); }

	 /// Sets the number of extra hardware frames which the filter will
     /// allocate on its output links for use in following filters or by
     /// the caller.
     ///
     /// Some hardware filters require all frames that they will use for
     /// output to be defined in advance before filtering starts.  For such
     /// filters, any hardware frame pools used for output must therefore be
     /// of fixed size.  The extra frames set here are on top of any number
     /// that the filter needs internally in order to operate normally.
     ///
     /// This field must be set before the graph containing this filter is
     /// configured.
	property int extra_hw_frames { int get(); void set(int value); }
public:
	virtual String^ ToString() override;
public:
	/// Negotiate the media format, dimensions, etc of all inputs to a filter.
	///
	/// @param filter the filter to negotiate the properties for its inputs
	/// @return       zero on successful negotiation
	AVRESULT ConfigLinks();

	/// Make the filter instance process a command.
	/// It is recommended to use avfilter_graph_send_command().
	AVRESULT ProcessCommand(String^ cmd,String^ arg, IntPtr res, int res_len, AVFilterCmdFlags flags);
	
	/// Initialize a filter with the supplied parameters.
	///
	/// @param ctx  uninitialized filter context to initialize
	/// @param args Options to initialize the filter with. This must be a
	///             ':'-separated list of options in the 'key=value' form.
	///             May be NULL if the options have been set directly using the
	///             AVOptions API or there are no options that need to be set.
	/// @return 0 on success, a negative AVERROR on failure
	AVRESULT Init(String^ args);
	AVRESULT Init();

	/// Initialize a filter with the supplied dictionary of options.
	///
	/// @param ctx     uninitialized filter context to initialize
	/// @param options An AVDictionary filled with options for this filter. On
	///                return this parameter will be destroyed and replaced with
	///                a dict containing options that were not found. This dictionary
	///                must be freed by the caller.
	///                May be NULL, then this function is equivalent to
	///                avfilter_init_str() with the second parameter set to NULL.
	/// @return 0 on success, a negative AVERROR on failure
	///
	/// @note This function and avfilter_init_str() do essentially the same thing,
	/// the difference is in manner in which the options are passed. It is up to the
	/// calling code to choose whichever is more preferable. The two functions also
	/// behave differently when some of the provided options are not declared as
	/// supported by the filter. In such a case, avfilter_init_str() will fail, but
	/// this function will leave those extra options in the options AVDictionary and
	/// continue as usual.
	AVRESULT Init(AVDictionary^ options);

	/// Link two filters together.
	///
	/// @param src    the source filter
	/// @param srcpad index of the output pad on the source filter
	/// @param dst    the destination filter
	/// @param dstpad index of the input pad on the destination filter
	/// @return       zero on success
	AVRESULT Link(AVFilterContext^ dst,int srcpad, int dstpad);
	AVRESULT Link(AVFilterContext^ dst);
public:
	/// @return AVClass for AVFilterContext.
	///
	/// @see av_opt_find().
	static AVClass^ GetClass();
};
//////////////////////////////////////////////////////
// AVFilterLink
/// A link between two filters. This contains pointers to the source and
/// destination filters between which this link exists, and the indexes of
/// the pads involved. In addition, this link also contains the parameters
/// which have been negotiated and agreed upon between the filter, such as
/// image dimensions, format, etc.
public ref class AVFilterLink : public  AVBase
{
internal:
	AVFilterLink(void * _pointer,AVBase^ _parent);
public:
	///< source filter
	property AVFilterContext^ src { AVFilterContext^ get(); }
	///< output pad on the source filter
    property AVFilterPad^ srcpad { AVFilterPad^ get(); }
	///< dest filter
    property AVFilterContext^ dst { AVFilterContext^ get(); }
	///< input pad on the dest filter
    property AVFilterPad^ dstpad { AVFilterPad^ get(); }
	///< filter media type
    property AVMediaType type { AVMediaType get(); }

    /// These parameters apply only to video
	///< agreed upon image width
    property int w { int get(); }
	///< agreed upon image height
    property int h { int get(); }
	///< agreed upon sample aspect ratio
    property AVRational^ sample_aspect_ratio { AVRational^ get(); }
    
	/// These parameters apply only to audio
	///< channel layout of current buffer (see libavutil/channel_layout.h)
    property AVChannelLayout channel_layout { AVChannelLayout get(); }
	///< samples per second
    property int sample_rate { int get(); }
	///< agreed upon media format
    property int format { int get(); }

	/// Define the time base used by the PTS of the frames/samples
	/// which will pass through this link.
	/// During the configuration stage, each filter is supposed to
	/// change only the output timebase, while the timebase of the
	/// input link is assumed to be an unchangeable property.
    property AVRational^ time_base { AVRational^ get(); }

	/// Get the number of channels of a link.
	property int channels { int get(); }
private:
	/// Set the closed field of a link.
	/// @deprecated applications are not supposed to mess with links, they should
	/// close the sinks.
	property int closed { void set(int); }
public:
	/// Insert a filter in the middle of an existing link.
	///
	/// @param link the link into which the filter should be inserted
	/// @param filt the filter to be inserted
	/// @param filt_srcpad_idx the input pad on the filter to connect
	/// @param filt_dstpad_idx the output pad on the filter to connect
	/// @return     zero on success
	AVRESULT InsertFilter(AVFilterContext^ filt,int filt_srcpad_idx, int filt_dstpad_idx);
};
//////////////////////////////////////////////////////
// AVFilterInOut
/// A linked-list of the inputs/outputs of the filter chain.
///
/// This is mainly useful for avfilter_graph_parse() / avfilter_graph_parse2(),
/// where it is used to communicate open (unlinked) inputs and outputs from and
/// to the caller.
/// This struct specifies, per each not connected pad contained in the graph, the
/// filter context and the pad index required for establishing a link.
public ref class AVFilterInOut : public  AVBase
{
internal:
	AVFilterInOut(void * _pointer,AVBase^ _parent);
public:
	AVFilterInOut();
public:
	/// unique name for this input/output in the list
    property String^ name { String^ get(); void set(String^); }
    /// filter context associated to this input/output
    property AVFilterContext^ filter_ctx { AVFilterContext^ get(); void set(AVFilterContext^); }
    /// index of the filt_ctx pad to use for linking
    property int pad_idx { int get(); void set(int); }
	/// next input/input in the list, NULL if this is the last
	property AVFilterInOut^ next { AVFilterInOut^ get(); void set(AVFilterInOut^); }
};
//////////////////////////////////////////////////////
/// A function pointer passed to the @ref AVFilterGraph.execute callback to be
/// executed multiple times, possibly in parallel.
///
/// @param ctx the filter context the job belongs to
/// @param arg an opaque parameter passed through from @ref
///            AVFilterGraph.execute
/// @param jobnr the index of the job being executed
/// @param nb_jobs the total number of jobs
///
/// @return 0 on success, a negative AVERROR on error
public delegate int (AVFilterActionDelegate)(AVFilterContext^ ctx, Object^ target, IntPtr arg, int jobnr, int nb_jobs);

/// A function executing multiple jobs, possibly in parallel.
///
/// @param ctx the filter context to which the jobs belong
/// @param func the function to be called multiple times
/// @param arg the argument to be passed to func
/// @param ret a nb_jobs-sized array to be filled with return values from each
///            invocation of func
/// @param nb_jobs the number of jobs to execute
///
/// @return 0 on success, a negative AVERROR on error
public delegate int (AVFilterExecuteDelegate)(AVFilterContext^ ctx, AVFilterActionDelegate^ func, Object^ target, IntPtr arg, int % ret, int nb_jobs);
//////////////////////////////////////////////////////
// AVFilterGraph
public ref class AVFilterGraph : public  AVBase
{
internal:
	AVFilterExecuteDelegate^	m_pExecute;
	AVFilterActionDelegate^		m_pAction;
	IntPtr						m_pOpaque;
	IntPtr						m_pExecuteNative;
	GCHandle					m_ThisPtr;
public:
	ref class AVFiltersArray : public AVBase
							 , public System::Collections::IEnumerable
							 , public System::Collections::Generic::IEnumerable<AVFilterContext^>
	{
	private:
		ref class AVFiltersEnumerator : public System::Collections::IEnumerator
							 		  , public System::Collections::Generic::IEnumerator<AVFilterContext^>
		{
		protected:
			int					m_nIndex;
			AVFiltersArray^		m_pArray;
		public:
			AVFiltersEnumerator(AVFiltersArray^ _array);
			~AVFiltersEnumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVFilterContext^ Current { AVFilterContext^ get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
		};
	internal:
		unsigned int * m_pnCount;
	internal:
		AVFiltersArray(void * _pointer,AVBase^ _parent);
	public:
		property AVFilterContext^ default[int] { AVFilterContext^ get(int index); }
		property int Count { int get(); }
	public:
		array<AVFilterContext^>^ ToArray();
	public:
		// IEnumerable
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator;
	public:
		// IEnumerable<AVDictionaryEntry>
		virtual System::Collections::Generic::IEnumerator<AVFilterContext^>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVFilterContext^>::GetEnumerator;
	};
internal:
	AVFilterGraph(void * _pointer,AVBase^ _parent);
public:
	AVFilterGraph();
public:
	property AVClass^ av_class { AVClass^ get(); }
	property AVFiltersArray^ filters { AVFiltersArray^ get(); }

	property String^ scale_sws_opts { String^ get(); void set(String^); }
	property String^ resample_lavr_opts { String^ get(); void set(String^); }

	/// Type of multithreading allowed for filters in this graph. A combination
	/// of AVFILTER_THREAD_* flags.
	///
	/// May be set by the caller at any point, the setting will apply to all
	/// filters initialized after that. The default is allowing everything.
	///
	/// When a filter in this graph is initialized, this field is combined using
	/// bit AND with AVFilterContext.thread_type to get the final mask used for
	/// determining allowed threading types. I.e. a threading type needs to be
	/// set in both to be allowed.
	property AVFilterThreadType thread_type { AVFilterThreadType get(); void set(AVFilterThreadType); }
	/// Maximum number of threads used by filters in this graph. May be set by
	/// the caller before adding any filters to the filtergraph. Zero (the
	/// default) means that the number of threads is determined automatically.
	property int nb_threads  { int get(); void set(int); }
	/// Opaque user data. May be set by the caller to an arbitrary value, e.g. to
	/// be used from callbacks like @ref AVFilterGraph.execute.
	/// Libavfilter will not touch this field in any way.
	property IntPtr opaque { IntPtr get(); void set(IntPtr); }
	/// This callback may be set by the caller immediately after allocating the
	/// graph and before adding any filters to it, to provide a custom
	/// multithreading implementation.
	///
	/// If set, filters with slice threading capability will call this callback
	/// to execute multiple jobs in parallel.
	///
	/// If this field is left unset, libavfilter will use its internal
	/// implementation, which may or may not be multithreaded depending on the
	/// platform and build options.
	property AVFilterExecuteDelegate^ execute { AVFilterExecuteDelegate^ get(); void set(AVFilterExecuteDelegate^); }
	///< swr options to use for the auto-inserted aresample filters, Access ONLY through AVOptions
	property String^ aresample_swr_opts { String^ get(); void set(String^); }

	/// Enable or disable automatic format conversion inside the graph.
	///
	/// Note that format conversion can still happen inside explicitly inserted
	/// scale and aresample filters.
	///
	/// @param flags  any of the AVFILTER_AUTO_CONVERT_/// constants
	property AVFilterAutoConvert flags { AVFilterAutoConvert get(); void set(AVFilterAutoConvert); }
public:
	/// Create a new filter instance in a filter graph.
	///
	/// @param graph graph in which the new filter will be used
	/// @param filter the filter to create an instance of
	/// @param name Name to give to the new instance (will be copied to
	///             AVFilterContext.name). This may be used by the caller to identify
	///             different filters, libavfilter itself assigns no semantics to
	///             this parameter. May be NULL.
	///
	/// @return the context of the newly created filter instance (note that it is
	///         also retrievable directly through AVFilterGraph.filters or with
	///         avfilter_graph_get_filter()) on success or NULL on failure.
	AVFilterContext^ AllocFilter(AVFilter^ filter,String^ name);

	AVFilterContext^ CreateFilter(AVFilter^ filter,String^ name);
	AVRESULT CreateFilter([Out] AVFilterContext^ % context,AVFilter^ filter,String^ name);
	AVRESULT CreateFilter([Out] AVFilterContext^ % context,AVFilter^ filter,String^ name, String^ args);
	AVRESULT CreateFilter([Out] AVFilterContext^ % context,AVFilter^ filter,String^ name, String^ args, IntPtr opaque);

	/// Get a filter instance identified by instance name from graph.
	///
	/// @param graph filter graph to search through.
	/// @param name filter instance name (should be unique in the graph).
	/// @return the pointer to the found filter instance or NULL if it
	/// cannot be found.
	AVFilterContext^ GetFilter(String^ name);

	/// Create and add a filter instance into an existing graph.
	/// The filter instance is created from the filter filt and inited
	/// with the parameters args and opaque.
	///
	/// In case of success put in ///filt_ctx the pointer to the created
	/// filter instance, otherwise set ///filt_ctx to NULL.
	///
	/// @param name the instance name to give to the created filter instance
	/// @param graph_ctx the filter graph
	/// @return a negative AVERROR error code in case of failure, a non
	/// negative value otherwise
	AVFilterContext^ CreateFilter(AVFilter^ filter,String^ name,String^ args, IntPtr opaque);

	/// Check validity and configure all the links and formats in the graph.
	///
	/// @param graphctx the filter graph
	/// @param log_ctx context used for logging
	/// @return >= 0 in case of success, a negative AVERROR code otherwise
	AVRESULT Config(IntPtr log_ctx);
	AVRESULT Config();

	/// Add a graph described by a string to a graph.
	///
	/// @note The caller must provide the lists of inputs and outputs,
	/// which therefore must be known before calling the function.
	///
	/// @note The inputs parameter describes inputs of the already existing
	/// part of the graph; i.e. from the point of view of the newly created
	/// part, they are outputs. Similarly the outputs parameter describes
	/// outputs of the already existing filters, which are provided as
	/// inputs to the parsed filters.
	///
	/// @param graph   the filter graph where to link the parsed graph context
	/// @param filters string to be parsed
	/// @param inputs  linked list to the inputs of the graph
	/// @param outputs linked list to the outputs of the graph
	/// @return zero on success, a negative AVERROR code on error
	AVRESULT Parse(String^ filters,AVFilterInOut^ inputs,AVFilterInOut^ outputs);
	AVRESULT Parse(String^ filters,AVFilterInOut^ inputs,AVFilterInOut^ outputs,IntPtr log_ctx);

	/// Add a graph described by a string to a graph.
	///
	/// In the graph filters description, if the input label of the first
	/// filter is not specified, "in" is assumed; if the output label of
	/// the last filter is not specified, "out" is assumed.
	///
	/// @param graph   the filter graph where to link the parsed graph context
	/// @param filters string to be parsed
	/// @param inputs  pointer to a linked list to the inputs of the graph, may be NULL.
	///                If non-NULL, ///inputs is updated to contain the list of open inputs
	///                after the parsing, should be freed with avfilter_inout_free().
	/// @param outputs pointer to a linked list to the outputs of the graph, may be NULL.
	///                If non-NULL, ///outputs is updated to contain the list of open outputs
	///                after the parsing, should be freed with avfilter_inout_free().
	/// @return non negative on success, a negative AVERROR code on error
	AVRESULT ParsePtr(String^ filters,AVFilterInOut^ inputs,AVFilterInOut^ outputs);
	AVRESULT ParsePtr(String^ filters,AVFilterInOut^ inputs,AVFilterInOut^ outputs,IntPtr log_ctx);
	AVRESULT ParsePtr2(String^ filters,[Out] AVFilterInOut^ % inputs,[Out] AVFilterInOut^ % outputs);
	AVRESULT ParsePtr2(String^ filters,[Out] AVFilterInOut^ % inputs,[Out] AVFilterInOut^ % outputs,IntPtr log_ctx);

	/// Add a graph described by a string to a graph.
	///
	/// @param[in]  graph   the filter graph where to link the parsed graph context
	/// @param[in]  filters string to be parsed
	/// @param[out] inputs  a linked list of all free (unlinked) inputs of the
	///                     parsed graph will be returned here. It is to be freed
	///                     by the caller using avfilter_inout_free().
	/// @param[out] outputs a linked list of all free (unlinked) outputs of the
	///                     parsed graph will be returned here. It is to be freed by the
	///                     caller using avfilter_inout_free().
	/// @return zero on success, a negative AVERROR code on error
	///
	/// @note This function returns the inputs and outputs that are left
	/// unlinked after parsing the graph and the caller then deals with
	/// them.
	/// @note This function makes no reference whatsoever to already
	/// existing parts of the graph and the inputs parameter will on return
	/// contain inputs of the newly parsed part of the graph.  Analogously
	/// the outputs parameter will contain outputs of the newly created
	/// filters.
	AVRESULT Parse2(String^ filters,[Out] AVFilterInOut^ % inputs,[Out] AVFilterInOut^ % outputs);

	/// Send a command to one or more filter instances.
	///
	/// @param graph  the filter graph
	/// @param target the filter(s) to which the command should be sent
	///               "all" sends to all filters
	///               otherwise it can be a filter or filter instance name
	///               which will send the command to all matching filters.
	/// @param cmd    the command to send, for handling simplicity all commands must be alphanumeric only
	/// @param arg    the argument for the command
	/// @param res    a buffer with size res_size where the filter(s) can return a response.
	///
	/// @returns >=0 on success otherwise an error code.
	///              AVERROR(ENOSYS) on unsupported commands
	AVRESULT SendCommand(String^ target,String^ cmd,String^ arg, IntPtr res, int res_len, AVFilterCmdFlags flags);

	/// Queue a command for one or more filter instances.
	///
	/// @param graph  the filter graph
	/// @param target the filter(s) to which the command should be sent
	///               "all" sends to all filters
	///               otherwise it can be a filter or filter instance name
	///               which will send the command to all matching filters.
	/// @param cmd    the command to sent, for handling simplicity all commands must be alphanumeric only
	/// @param arg    the argument for the command
	/// @param ts     time at which the command should be sent to the filter
	///
	/// @note As this executes commands after this function returns, no return code
	///       from the filter is provided, also AVFILTER_CMD_FLAG_ONE is not supported.
	AVRESULT QueueCommand(String^ target,String^ cmd,String^ arg, AVFilterCmdFlags flags, double ts);

	/// Dump a graph into a human-readable string representation.
	///
	/// @param graph    the graph to dump
	/// @param options  formatting options; currently ignored
	/// @return  a string, or NULL in case of memory allocation failure;
	///          the string must be freed using av_free
	String^ Dump();
	String^ Dump(String^ options);

	/// Request a frame on the oldest sink link.
	///
	/// If the request returns AVERROR_EOF, try the next.
	///
	/// Note that this function is not meant to be the sole scheduling mechanism
	/// of a filtergraph, only a convenience function to help drain a filtergraph
	/// in a balanced way under normal circumstances.
	///
	/// Also note that AVERROR_EOF does not mean that frames did not arrive on
	/// some of the sinks during the process.
	/// When there are multiple sink links, in case the requested link
	/// returns an EOF, this may cause a filter to flush pending frames
	/// which are sent to another sink link, although unrequested.
	///
	/// @return  the return value of ff_request_frame(),
	///          or AVERROR_EOF if all links returned AVERROR_EOF
	AVRESULT RequestOldest();
};
//////////////////////////////////////////////////////
// AVBufferSinkParams
public ref class AVBufferSinkParams : public AVBase 
{
public:
	ref class AVPixelFormats : public AVBase
		, public System::Collections::IEnumerable
		, public System::Collections::Generic::IEnumerable<AVPixelFormat>
	{
	private:
		// Is ReadOnly
		bool	m_bReadOnly;
		// Current Version
		int		m_nVersion;
		// Allocated Elements If Any
		int		m_nAllocated;
	private:
		ref class AVPixelFormatsEnumerator 
			: public System::Collections::IEnumerator
			, public System::Collections::Generic::IEnumerator<AVPixelFormat>
		{
		internal:
			// Enumerator Version
			int				m_nVersion;
			// Target Index
			int				m_nIndex;
			// Formats Access
			AVPixelFormats^	m_pFormats;
		public:
			AVPixelFormatsEnumerator(AVPixelFormats^ _array);
			~AVPixelFormatsEnumerator();
		public:
			// IEnumerator
			virtual bool MoveNext();
			virtual property AVPixelFormat Current { AVPixelFormat get (); }
			virtual void Reset();
			virtual property Object^ CurrentObject { virtual Object^ get () sealed = IEnumerator::Current::get; }
		};
	internal:
		AVPixelFormats(void * _pointer,AVBase^ _parent);
	public:
		property AVPixelFormat default[int] { AVPixelFormat get(int index); }
		property int Count { int get(); }
	public:
		property bool ReadOnly { bool get() { return m_bReadOnly; } }
	private:
		bool EnsureAllocatedPointer(int count);
	public:
		// Add elements to list 
		bool Add(array<AVPixelFormat>^ formats);
		// Rmove elements from list if presents
		bool Remove(array<AVPixelFormat>^ formats);
		// Add element 
		bool Add(AVPixelFormat format);
		// Remove element
		bool Remove(AVPixelFormat format);
		// Check whatever elements in a list
		bool Contains(AVPixelFormat format);
		// Remove element by specified eindex
		bool RemoveAt(int index);
		// Remove all elements
		bool RemoveAll();
	public:
		AVPixelFormats^ operator += (AVPixelFormat format);
		AVPixelFormats^ operator -= (AVPixelFormat format);
		AVPixelFormats^ operator += (array<AVPixelFormat>^ formats);
		AVPixelFormats^ operator -= (array<AVPixelFormat>^ formats);
		static AVPixelFormats^ operator + (AVPixelFormats^ target,AVPixelFormat format);
		static AVPixelFormats^ operator - (AVPixelFormats^ target,AVPixelFormat format);
		static AVPixelFormats^ operator + (AVPixelFormats^ target,array<AVPixelFormat>^ formats);
		static AVPixelFormats^ operator - (AVPixelFormats^ target,array<AVPixelFormat>^ formats);
	public:
		array<AVPixelFormat>^ ToArray();
	public:
		// IEnumerable
		virtual System::Collections::IEnumerator^ GetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator;
	public:
		// IEnumerable<AVDictionaryEntry>
		virtual System::Collections::Generic::IEnumerator<AVPixelFormat>^ GetEnumeratorGeneric() sealed = System::Collections::Generic::IEnumerable<AVPixelFormat>::GetEnumerator;
	};
protected:
	AVPixelFormats^ m_Formats;
internal:
	AVBufferSinkParams(void * _pointer,AVBase^ _parent);
public:
	AVBufferSinkParams();
public:
	property AVPixelFormats^ pixel_fmts { AVPixelFormats^ get() { return m_Formats; } }
};
//////////////////////////////////////////////////////
// AVABufferSinkParams
// Struct to use for initializing an abuffersink context.
public ref class AVABufferSinkParams : public AVBase 
{
internal:
	AVABufferSinkParams(void * _pointer,AVBase^ _parent);
public:
	AVABufferSinkParams();
public:
	///< list of allowed sample formats, terminated by AV_SAMPLE_FMT_NONE
	property array<AVSampleFormat>^ sample_fmts { array<AVSampleFormat>^ get(); void set(array<AVSampleFormat>^); }
	///< list of allowed channel layouts, terminated by -1
	property array<AVChannelLayout>^ channel_layouts { array<AVChannelLayout>^ get(); void set(array<AVChannelLayout>^); }
	///< list of allowed channel counts, terminated by -1
	property array<int>^ channel_counts { array<int>^ get(); void set(array<int>^); }
	///< if not 0, accept any channel count or layout
	property int all_channel_counts { int get(); void set(int ); }
	///< list of allowed sample rates, terminated by -1
	property array<int>^ sample_rates { array<int>^get(); void set(array<int>^); }
};
//////////////////////////////////////////////////////
// AVBufferSink
public ref class AVBufferSink
{
protected:
	// Target Context
	AVFilterContext^  m_pContext;
public:
	AVBufferSink(AVFilterContext^ ctx);
public:
	property AVMediaType type { AVMediaType get(); }
	property AVRational^ time_base { AVRational^ get(); }
	property int format { int get(); }

	property AVRational^ frame_rate { AVRational^ get(); }
	property int w { int get(); }
	property int h { int get(); }
	property AVRational^ sample_aspect_ratio { AVRational^ get(); }

	property int channels { int get(); }
	property AVChannelLayout channel_layout { AVChannelLayout get(); }
	property int sample_rate { int get(); }
	
	property AVBufferRef^ hw_frames_ctx { AVBufferRef^ get(); }
public:
	/// Get a frame with filtered data from sink and put it in frame.
	///
	/// @param ctx    pointer to a buffersink or abuffersink filter context.
	/// @param frame  pointer to an allocated frame that will be filled with data.
	///               The data must be freed using av_frame_unref() / av_frame_free()
	/// @param flags  a combination of AV_BUFFERSINK_FLAG_/// flags
	///
	/// @return  >= 0 in for success, a negative AVERROR code for failure.
	AVRESULT get_frame(AVFrame^ frame, AVBufferSinkFlags flags);

	/// Get a frame with filtered data from sink and put it in frame.
	///
	/// @param ctx pointer to a context of a buffersink or abuffersink AVFilter.
	/// @param frame pointer to an allocated frame that will be filled with data.
	///              The data must be freed using av_frame_unref() / av_frame_free()
	///
	/// @return
	///         - >= 0 if a frame was successfully returned.
	///         - AVERROR(EAGAIN) if no frames are available at this point; more
	///           input frames must be added to the filtergraph to get more output.
	///         - AVERROR_EOF if there will be no more output frames on this sink.
	///         - A different negative AVERROR code in other failure cases.
	AVRESULT get_frame(AVFrame^ frame);

	/// Set the frame size for an audio buffer sink.
	///
	/// All calls to av_buffersink_get_buffer_ref will return a buffer with
	/// exactly the specified number of samples, or AVERROR(EAGAIN) if there is
	/// not enough. The last buffer at EOF will be padded with 0.
	void set_frame_size(unsigned frame_size);

	/// Same as av_buffersink_get_frame(), but with the ability to specify the number
	/// of samples read. This function is less efficient than
	/// av_buffersink_get_frame(), because it copies the data around.
	///
	/// @param ctx pointer to a context of the abuffersink AVFilter.
	/// @param frame pointer to an allocated frame that will be filled with data.
	///              The data must be freed using av_frame_unref() / av_frame_free()
	///              frame will contain exactly nb_samples audio samples, except at
	///              the end of stream, when it can contain less than nb_samples.
	///
	/// @return The return codes have the same meaning as for
	///         av_buffersink_get_frame().
	///
	/// @warning do not mix this function with av_buffersink_get_frame(). Use only one or
	/// the other with a single sink, not both.
	AVRESULT get_samples(AVFrame^ frame, int nb_samples);
};
//////////////////////////////////////////////////////
// AVBufferSrcParameters
public ref class AVBufferSrcParameters : public AVBase 
{
internal:
	AVBufferSrcParameters(void * _pointer,AVBase^ _parent);
public:
	AVBufferSrcParameters();
public:
	///<summary>
	/// video: the pixel format, value corresponds to enum AVPixelFormat
	/// audio: the sample format, value corresponds to enum AVSampleFormat
	///</summary>
	property int format { int get(); void set(int); }

	///<summary>
	/// The timebase to be used for the timestamps on the input frames.
	///</summary>
	property AVRational^ time_base { AVRational^ get(); void set(AVRational^); }

	///<summary>
	/// Video only, the display dimensions of the input frames.
	///</summary>
	property int width { int get(); void set(int); }
	property int height { int get(); void set(int); }

	///<summary>
	/// Video only, the sample (pixel) aspect ratio.
	///</summary>
	property AVRational^ sample_aspect_ratio { AVRational^ get(); void set(AVRational^); }

	///<summary>
	/// Video only, the frame rate of the input video. This field must only be
	/// set to a non-zero value if input stream has a known constant framerate
	/// and should be left at its initial value if the framerate is variable or
	/// unknown.
	///</summary>
	property AVRational^ frame_rate { AVRational^ get(); void set(AVRational^); }

	///<summary>
	/// Video with a hwaccel pixel format only. This should be a reference to an
	/// AVHWFramesContext instance describing the input frames.
	///</summary>
	property AVBufferRef^ hw_frames_ctx { AVBufferRef^ get(); void set(AVBufferRef^); }

	///<summary>
	/// Audio only, the audio sampling rate in samples per second.
	///</summary>
	property int sample_rate { int get(); void set(int ); }

	///<summary>
	/// Audio only, the audio channel layout
	///</summary>
	property AVChannelLayout channel_layout { AVChannelLayout get(); void set(AVChannelLayout ); }
};
//////////////////////////////////////////////////////
// AVBufferSrc
public ref class AVBufferSrc
{
protected:
	// Target Context
	AVFilterContext^  m_pContext;
public:
	AVBufferSrc(AVFilterContext^ ctx);
public:
	 /// Initialize the buffersrc or abuffersrc filter with the provided parameters.
	 /// This function may be called multiple times, the later calls override the
	 /// previous ones. Some of the parameters may also be set through AVOptions, then
	 /// whatever method is used last takes precedence.
	 ///
	 /// @param ctx an instance of the buffersrc or abuffersrc filter
	 /// @param param the stream parameters. The frames later passed to this filter
	 ///              must conform to those parameters. All the allocated fields in
	 ///              param remain owned by the caller, libavfilter will make internal
	 ///              copies or references when necessary.
	 /// @return 0 on success, a negative AVERROR code on failure.
	AVRESULT parameters_set(AVBufferSrcParameters^ param);

	 /// Add a frame to the buffer source.
	 ///
	 /// @param ctx   an instance of the buffersrc filter
	 /// @param frame frame to be added. If the frame is reference counted, this
	 /// function will make a new reference to it. Otherwise the frame data will be
	 /// copied.
	 ///
	 /// @return 0 on success, a negative AVERROR on error
	 ///
	 /// This function is equivalent to av_buffersrc_add_frame_flags() with the
	 /// AV_BUFFERSRC_FLAG_KEEP_REF flag.
	AVRESULT write_frame(AVFrame^ frame);

	 /// Add a frame to the buffer source.
	 ///
	 /// @param ctx   an instance of the buffersrc filter
	 /// @param frame frame to be added. If the frame is reference counted, this
	 /// function will take ownership of the reference(s) and reset the frame.
	 /// Otherwise the frame data will be copied. If this function returns an error,
	 /// the input frame is not touched.
	 ///
	 /// @return 0 on success, a negative AVERROR on error.
	 ///
	 /// @note the difference between this function and av_buffersrc_write_frame() is
	 /// that av_buffersrc_write_frame() creates a new reference to the input frame,
	 /// while this function takes ownership of the reference passed to it.
	 ///
	 /// This function is equivalent to av_buffersrc_add_frame_flags() without the
	 /// AV_BUFFERSRC_FLAG_KEEP_REF flag.
	AVRESULT add_frame(AVFrame^ frame);

	 /// Add a frame to the buffer source.
	 ///
	 /// By default, if the frame is reference-counted, this function will take
	 /// ownership of the reference(s) and reset the frame. This can be controlled
	 /// using the flags.
	 ///
	 /// If this function returns an error, the input frame is not touched.
	 ///
	 /// @param buffer_src  pointer to a buffer source context
	 /// @param frame       a frame, or NULL to mark EOF
	 /// @param flags       a combination of AV_BUFFERSRC_FLAG_///
	 /// @return            >= 0 in case of success, a negative AVERROR code
	 ///                    in case of failure
	AVRESULT add_frame(AVFrame^ frame,AVBufferSrcFlags flags);
	 /// Close the buffer source after EOF.
	 ///
	 /// This is similar to passing NULL to av_buffersrc_add_frame_flags()
	 /// except it takes the timestamp of the EOF, i.e. the timestamp of the end
	 /// of the last frame.
	AVRESULT close(Int64 pts,AVBufferSrcFlags flags);

	///<summary>
	/// Get the number of failed requests.
	///
	/// A failed request is when the request_frame method is called while no
	/// frame is present in the buffer.
	/// The number is reset when a frame is added.
	///</summary>
	unsigned get_nb_failed_requests();
};
//////////////////////////////////////////////////////
#pragma endregion
//////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////