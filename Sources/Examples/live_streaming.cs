//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2023 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// Example of reading media file with libavformat 
// decoding audio and video with libavcodec 
// encode resulted audio and video muxing it and perform live output
//////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("live_streaming")]
[assembly: AssemblyDescription("Example of reading media file with libavformat" + 
    " decoding audio and video with libavcodec encode resulted audio and video muxing it and perform live output")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class live_streaming
    {
        delegate AVRESULT ProcessPacket(StreamBase st, AVPacket pkt, long pts);

        class QueuedPacket
        {
            // Packet
            public AVPacket packet = null;
            // timestamp in time base q
            public long ts = AVUtil.NOPTS_VALUE;
        };

        class StreamBase
        {
            // Output Thread
            public Thread thread = null;
            // Packets queue
            public List<QueuedPacket> packets = new List<QueuedPacket>();
            // Notification Event for new packet added
            public EventWaitHandle ev_signal = new AutoResetEvent(false);
            // Notification Event for packet queue updated
            public EventWaitHandle ev_free = new ManualResetEvent(true);
            // Queue start ts in AVTimeBaseQ
            public long queue_start_ts = AVUtil.NOPTS_VALUE;
            // Queue end ts in AVTimeBaseQ
            public long queue_end_ts = AVUtil.NOPTS_VALUE;
            // maximum queue size
            public static long queue_max_size = AVMath.rescale_q(4000,new AVRational(1,1000), AVRational.AVTimeBaseQ);
            // Thread Quit flag
            public bool exit = false;
            // Process packet handlet 
            public ProcessPacket process = null;
        }

        // Output Target Context
        class LiveOutput : StreamBase
        {
            // Output Format context
            public AVFormatContext oc = null;
        }

        // Stream Definition class
        class OutputStream : StreamBase
        {
            // Encoder
            public AVCodecContext encoder = null;
            // Decoder
            public AVCodecContext decoder = null;
            // Output Stream Defition
            public AVStream stream = null;
            // Scaling Context
            public SwsContext sws_ctx = null;
            // Resampling Context
            public SwrContext swr_ctx = null;
            // Shortcut to media type
            public bool is_video = false;
            // Output 
            public LiveOutput oc = null;
            // frames for decoding
            public AVFrame temporal_frame = null;
            public AVFrame target_frame = null;
            public AVPacket output_pkt = null;
            public long start_pts = AVUtil.NOPTS_VALUE;
            public long nb_frames = 0;
            // Audio FIFO
            public AVAudioFifo fifo = null;
        }

        static AVRESULT ProcessMuxerPacket(StreamBase sb, AVPacket pkt, long pts)
        {
            AVRESULT ret = 0;
            LiveOutput st = (LiveOutput)sb;
            lock (st.oc)
            {
                ret = st.oc.InterleavedWriteFrame(pkt);
            }
            return ret;
        }

        static AVRESULT EncodeFrame(OutputStream st, AVFrame frame, long pts)
        {
            AVRESULT ret = 0;
            ret = st.encoder.SendFrame(frame);

            while (ret >= 0)
            {
                // Process all encoded packets 
                ret = st.encoder.ReceivePacket(st.output_pkt);
                if (ret == AVRESULT.EAGAIN || ret == AVRESULT.EOF)
                {
                    break;
                }
                if (ret >= 0)
                {
                    // And write them into output
                    st.output_pkt.RescaleTS(st.encoder.time_base, st.stream.time_base);
                    st.output_pkt.stream_index = st.stream.index;
                    put_packet(st.oc, st.output_pkt, pts);
                    st.output_pkt.Free();
                }
                else
                {
                    Console.WriteLine("{0} Encoding Error {1}", st.is_video ? "Video" : "Audio", ret);
                    break;
                }
            }
            return ret;
        }

        static AVRESULT ProcessDecodingPacket(StreamBase sb, AVPacket pkt, long pts)
        {
            AVRESULT ret = 0;
            OutputStream st = (OutputStream)sb;
            AVFrame output_frame = null;
            if (st.start_pts == AVUtil.NOPTS_VALUE)
            {
                if (pts == AVUtil.NOPTS_VALUE)
                {
                    st.start_pts = 0;
                }
                else
                {
                    if (st.encoder != null)
                    {
                        st.start_pts = AVMath.rescale_q(pts, AVRational.AVTimeBaseQ, st.encoder.time_base);
                    }
                }
            }
            // If We have decoder
            if (st.decoder != null)
            {
                // Process packet
                ret = st.decoder.SendPacket(pkt);

                while (ret >= 0)
                {
                    ret = st.decoder.ReceiveFrame(st.target_frame);
                    if (ret == AVRESULT.EAGAIN || ret == AVRESULT.EOF)
                    {
                        break;
                    }
                    if (ret >= 0)
                    {
                        // In Case of video
                        if (st.is_video)
                        {
                            // We need to check if the coorspace compatible
                            if (st.sws_ctx == null && st.encoder.pix_fmt != st.target_frame.format)
                            {
                                // We create and allocate temporal frame
                                st.temporal_frame = new AVFrame();
                                st.temporal_frame.width = st.encoder.width;
                                st.temporal_frame.height = st.encoder.height;
                                st.temporal_frame.format = (int)st.encoder.pix_fmt;
                                st.temporal_frame.AllocateBuffers(16);
                                st.temporal_frame.MakeWritable();
                                // Create scaling context if it required
                                st.sws_ctx = new SwsContext(st.target_frame, st.temporal_frame, SwsFlags.FastBilinear);
                            }
                            // If have scaling context then apply it
                            if (st.sws_ctx != null)
                            {
                                ret = st.sws_ctx.Scale(st.target_frame, 0, st.target_frame.height, st.temporal_frame);
                                output_frame = st.temporal_frame;
                                output_frame.pts = st.target_frame.pts;
                                output_frame.pkt_dts = st.target_frame.pkt_dts;
                            }
                            else
                            {
                                // Otherwise just set output as the target frame
                                output_frame = st.target_frame;
                            }
                        }
                        else // Audio
                        {
                            // Allocate Fifo and the temporal frame
                            if (st.fifo == null)
                            {
                                st.fifo = new AVAudioFifo(st.encoder.sample_fmt, st.encoder.channels, st.encoder.sample_rate);
                                // We create and allocate temporal frame
                                st.temporal_frame = new AVFrame();
                                st.temporal_frame.format = (int)st.encoder.sample_fmt;
                                st.temporal_frame.channels = st.encoder.channels;
                                st.temporal_frame.channel_layout = st.encoder.channel_layout;
                                st.temporal_frame.sample_rate = st.encoder.sample_rate;
                                st.temporal_frame.nb_samples = st.encoder.sample_rate;
                                st.temporal_frame.AllocateBuffers(0);
                                st.temporal_frame.MakeWritable();
                            }
                            // We need to check if the format is compatible 
                            if (st.swr_ctx == null && (st.encoder.sample_fmt != st.target_frame.format 
                                || st.encoder.channels != st.target_frame.channels
                                || st.encoder.sample_rate != st.target_frame.sample_rate
                                ))
                            {
                                // Otherwise allocate convesion context
                                st.swr_ctx = new SwrContext(
                                                    st.temporal_frame.channel_layout,
                                                    st.temporal_frame.format, st.temporal_frame.sample_rate,
                                                    st.target_frame.channel_layout,
                                                    st.target_frame.format, st.target_frame.sample_rate);

                                ret = st.swr_ctx.Init();
                                System.Diagnostics.Debug.Assert(ret.Succeeded);
                            }
                            // Resample if needed
                            if (st.swr_ctx != null)
                            {
                                st.temporal_frame.nb_samples = st.encoder.sample_rate;
                                int _count = st.swr_ctx.Convert(st.temporal_frame, st.target_frame);
                                if (_count >= 0)
                                {
                                    // We use the FIFO if the encoder requres that
                                    if (st.encoder.frame_size > 0)
                                    {
                                        st.fifo.Write(st.temporal_frame, _count);
                                    }
                                    else
                                    {
                                        output_frame = st.temporal_frame;
                                        st.temporal_frame.nb_samples = _count;
                                    }
                                }
                            }
                            else
                            {
                                // Otherwise just write data into fifo
                                if (st.encoder.frame_size > 0)
                                {
                                    st.fifo.Write(st.target_frame);
                                }
                                else
                                {
                                    output_frame = st.target_frame;
                                }
                            }
                        }
                        if (st.is_video)
                        {
                            // Send Frame to encoder
                            output_frame.key_frame = true;
                            output_frame.pts = st.start_pts + st.nb_frames++;
                            ret = EncodeFrame(st, output_frame,pts);
                        }
                        else
                        {
                            if (st.encoder.frame_size > 0)
                            {
                                // While we have enougth audio data to process
                                while (st.fifo.Size >= st.encoder.frame_size)
                                {
                                    ret = st.fifo.Read(st.temporal_frame, st.encoder.frame_size);
                                    if (ret > 0)
                                    {
                                        st.temporal_frame.nb_samples = ret;
                                        st.temporal_frame.pts = st.nb_frames;
                                        st.nb_frames += st.temporal_frame.nb_samples;
                                        ret = EncodeFrame(st, st.temporal_frame, pts);
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                output_frame.pts = st.start_pts + st.nb_frames;
                                st.nb_frames += output_frame.nb_samples;
                                ret = EncodeFrame(st, output_frame, pts);
                            }
                        }

                        // We only free buffer of the target frame as the output can be allocated temporal frame
                        st.target_frame.Free();
                    }
                    else
                    {
                        Console.WriteLine("{0} Decoding Error {1}", st.is_video ? "Video" : "Audio", ret);
                        break;
                    }
                }
            }
            else
            {
                // We are do not not performing re-encoding
                pkt.stream_index = st.stream.index;
                put_packet(st.oc, st.output_pkt, pts);
            }
            return ret;
        }

        // Output Stream Thread function
        static void OutputStreamThread(object o)
        {
            StreamBase st = (StreamBase)o;
            var handles = new WaitHandle[] { st.ev_signal, g_evQuit };
            bool exit = false;
            // Loop until quit signaled
            while (!exit)
            {
                // Wait for any signal
                if (0 != WaitHandle.WaitAny(handles))
                {
                    // In case of Quit - mark thread to quit also for proper flush
                    lock (st)
                    {
                        st.exit = true;
                    }
                }
                // Process all pending packets
                while (true)
                {
                    AVPacket pkt = null;
                    long pts = AVUtil.NOPTS_VALUE;
                    lock (st)
                    {
                        exit = st.exit;
                        // Extract packets if we have
                        if (st.packets.Count > 0)
                        {
                            pkt = st.packets[0].packet;
                            st.packets.RemoveAt(0);
                            if (st.packets.Count > 0)
                            {
                                pts = st.packets[0].ts;
                                if (pts != AVUtil.NOPTS_VALUE)
                                {
                                    st.queue_start_ts = pts;
                                    if (st.queue_end_ts - st.queue_start_ts < StreamBase.queue_max_size)
                                    {
                                        st.ev_free.Set();
                                    }
                                }
                            }
                            else
                            {
                                st.queue_start_ts = AVUtil.NOPTS_VALUE;
                                st.queue_end_ts = AVUtil.NOPTS_VALUE;
                                st.ev_free.Set();
                            }
                        }
                    }
                    if (pkt != null)
                    {
                        try
                        {
                            AVRESULT ret = st.process(st,pkt,pts);
                        }
                        finally
                        {
                            pkt.Dispose();
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            // Just in case...
            lock (st)
            {
                st.exit = true;
            }
        }

        static void StartStreamThread(StreamBase st, ProcessPacket process)
        {
            st.thread = new Thread(OutputStreamThread);
            st.process = process;
            st.thread.Start(st);
        }

        // Create Output Stream
        static AVRESULT create_output(out OutputStream st, LiveOutput live, AVStream istream)
        {
            AVRESULT ret = 0;
            st = new OutputStream();
            st.oc = live;
            AVFormatContext oc = live.oc;
            AVCodec codec = AVCodec.FindDecoder(istream.codecpar.codec_id);

            if (codec != null)
            {
                st.decoder = new AVCodecContext(codec);
                istream.codecpar.CopyTo(st.decoder);
                ret = st.decoder.Open(codec);
                if (ret.Failed)
                {
                    st.decoder.Dispose();
                    st.decoder = null;
                }
            }

            st.stream = oc.AddStream();
            st.stream.id = oc.nb_streams - 1;
            st.stream.time_base = istream.time_base;
            st.is_video = istream.codecpar.codec_type == AVMediaType.VIDEO;
            st.target_frame = new AVFrame();
            st.output_pkt = new AVPacket();

            if (st.decoder != null)
            {
                AVCodecID[] codecs = null;
                if (st.is_video)
                {
                    codecs = new AVCodecID[] { oc.oformat.video_codec, AVCodecID.H264 };
                }
                else
                {
                    codecs = new AVCodecID[] { oc.oformat.audio_codec, AVCodecID.AAC };
                }
                if (codecs != null)
                {
                    for (int i = 0; i < codecs.Length; i++)
                    {
                        codec = AVCodec.FindEncoder(codecs[i]);
                        if (codec != null) break;
                    }
                }
                if (codec != null)
                {
                    st.encoder = new AVCodecContext(codec);
                    st.encoder.codec_id = codec.id;

                    if (st.is_video)
                    {
                        // Configure Video Encoding
                        st.encoder.width = istream.codecpar.width;
                        st.encoder.height = istream.codecpar.height;
                        st.encoder.pix_fmt = AVPixelFormat.YUV420P;
                        st.encoder.max_b_frames = 2;
                        st.encoder.gop_size = 50;
                        st.encoder.bit_rate = (st.encoder.width * st.encoder.height) << 1;

                        st.encoder.time_base.num = 1;
                        if (st.stream.time_base.num != 0 && st.stream.time_base.den != 0)
                        {
                            st.encoder.time_base.den = (int)((float)st.stream.time_base.den / (float)st.stream.time_base.num);
                        }
                        else
                        {
                            st.encoder.time_base.den = 30;
                            st.stream.time_base = st.encoder.time_base;
                        }
                        if (codec.pix_fmts != null && codec.pix_fmts.Length > 0)
                        {
                            bool found = false;
                            for (int i = 0; i < codec.pix_fmts.Length && !found; i++)
                            {
                                found = codec.pix_fmts[i] == st.encoder.pix_fmt;
                            }
                            if (!found)
                            {
                                st.encoder.pix_fmt = codec.pix_fmts[0];
                            }
                        }
                    }
                    else
                    {
                        // Configure Audio Encoding
                        st.encoder.sample_rate = istream.codecpar.sample_rate;
                        st.encoder.sample_fmt = AVSampleFormat.FLTP;
                        st.encoder.channels = 2;
                        st.encoder.channel_layout = AVChannels.get_default_channel_layout(st.encoder.channels);
                        st.encoder.time_base = new AVRational(1, istream.codecpar.sample_rate);
                        if (codec.sample_fmts != null && codec.sample_fmts.Length > 0)
                        {
                            bool found = false;
                            for (int i = 0; i < codec.sample_fmts.Length && !found; i++)
                            {
                                found = codec.sample_fmts[i] == st.encoder.sample_fmt;
                            }
                            if (!found)
                            {
                                st.encoder.sample_fmt = codec.sample_fmts[0];
                            }
                        }
                    }
                }
                if (codec != null)
                {
                    if ((oc.oformat.flags & AVfmt.GLOBALHEADER) != 0)
                    {
                        st.encoder.flags |= AVCodecFlag.GLOBAL_HEADER;
                    }

                    ret = st.encoder.Open(codec);
                    if (ret >= 0)
                    {
                        ret = st.stream.codecpar.FromContext(st.encoder);
                    }
                }
                else
                {
                    ret = AVRESULT.ENCODER_NOT_FOUND;
                }
            }
            else
            {
                ret = istream.codecpar.CopyTo(st.stream.codecpar);
            }
            if (ret >= 0)
            {
                StartStreamThread(st, ProcessDecodingPacket);
            }
            return ret;
        }

        // Free Base Stream
        static void free_stream(StreamBase st)
        {
            if (st != null)
            {
                if (st.thread != null)
                {

                    lock (st)
                    {
                        st.exit = true;
                        st.ev_signal.Set();
                    }
                    st.thread.Join();
                    lock (st)
                    {
                        st.thread = null;
                    }
                }
                lock (st)
                {
                    while (st.packets.Count > 0)
                    {
                        var pkt = st.packets[0];
                        st.packets.Remove(pkt);
                        if (pkt.packet != null)
                        {
                            pkt.packet.Dispose();
                        }
                    }
                    st.queue_start_ts = AVUtil.NOPTS_VALUE;
                    st.queue_end_ts = AVUtil.NOPTS_VALUE;
                    st.ev_free.Set();
                }
            }
        }

        // Free Output Stream
        static void free_output_stream(OutputStream st)
        {
            if (st != null)
            {
                free_stream(st);
                if (st.encoder != null)
                {
                    st.encoder.Dispose();
                    st.encoder = null;
                }
                if (st.decoder != null)
                {
                    st.decoder.Dispose();
                    st.decoder = null;
                }
                // Free all objects
                if (st.fifo != null)
                {
                    st.fifo.Dispose();
                    st.fifo = null;
                }
                if (st.temporal_frame != null)
                {
                    st.temporal_frame.Dispose();
                    st.temporal_frame = null;
                }
                if (st.target_frame != null)
                {
                    st.target_frame.Dispose();
                    st.target_frame = null;
                }
                if (st.output_pkt != null)
                {
                    st.output_pkt.Dispose();
                    st.output_pkt = null;
                }
            }
        }

        // Free Live Output Stream
        static void free_live_output(LiveOutput st)
        {
            if (st != null)
            {
                if (st.oc != null && (int)(st.oc.oformat.flags & AVfmt.NOFILE) == 0)
                {
                    if (st.oc.pb != null)
                    {
                        st.oc.pb.Dispose();
                    }
                }
                free_stream(st);
                if (st.oc != null)
                {
                    st.oc.Dispose();
                    st.oc = null;
                }
            }
        }

        // Put packet into stream queue
        static void put_packet(StreamBase st, AVPacket pkt, long pts)
        {
            if (st != null && pkt != null && pkt.size > 0)
            {
                if (pts != AVUtil.NOPTS_VALUE)
                {
                    if (0 != WaitHandle.WaitAny(new WaitHandle[] { st.ev_free, g_evQuit }))
                    {
                        return;
                    }
                }
                AVPacket output_pkt = new AVPacket();
                pkt.MoveTo(output_pkt);
                lock (st)
                {
                    if (st.thread != null && !st.exit)
                    {
                        var qp = new QueuedPacket();
                        qp.packet = output_pkt;
                        qp.ts = pts;
                        if (pts != AVUtil.NOPTS_VALUE)
                        {
                            st.queue_end_ts = pts;
                            if (st.queue_start_ts == AVUtil.NOPTS_VALUE || st.queue_start_ts < pts)
                            {
                                st.queue_start_ts = pts;
                            }
                            if (pts - st.queue_start_ts >= StreamBase.queue_max_size)
                            {
                                st.ev_free.Reset();
                            }
                        }
                        st.packets.Add(qp);
                        st.ev_signal.Set();
                    }
                    else
                    {
                        output_pkt.Dispose();
                    }
                }
            }
        }

        delegate bool HandlerRoutine(int dwCtrlType);

        [DllImport("Kernel32")]
        static extern bool SetConsoleCtrlHandler(HandlerRoutine Handler, bool Add);

        [DllImport("Kernel32.dll")]
        static extern uint GetTickCount();

        [DllImport("kernel32.dll", EntryPoint = "OutputDebugStringW", CharSet = CharSet.Unicode)]
        static extern void OutputDebugString(string _text);

        static EventWaitHandle g_evQuit = new EventWaitHandle(false, EventResetMode.ManualReset);
        static EventWaitHandle g_evReady = new EventWaitHandle(false, EventResetMode.AutoReset);

        static bool ConsoleHandlerRoutine(int dwCtrlType)
        {
            if (dwCtrlType == 2 || dwCtrlType == 5 || dwCtrlType == 6)
            {
                g_evQuit.Set();
                g_evReady.WaitOne();
            }
            return true;
        }

        static bool AVIOInterrupt(object opaque)
        {
            return g_evQuit.WaitOne(0);
        }

        static void Main(string[] args)
        {
            HandlerRoutine handler = new HandlerRoutine(ConsoleHandlerRoutine);
            SetConsoleCtrlHandler(handler, true);

            if (args.Length <= 1)
            {
                Console.WriteLine("Usage: {0}  <input file> <url>\n",
                    Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Console.WriteLine("Example: {0} temp.mp4 rtsp://127.0.0.1:8554/stream\n",
                    Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            string filename = args[0];
            string url = args[1];

            if (url.StartsWith("\"") || url.StartsWith("'"))
            {
                url = url.Substring(1);
            }
            if (url.EndsWith("\"") || url.EndsWith("'"))
            {
                url = url.Substring(0, url.Length - 1);
            }
            if (filename.StartsWith("\"") || filename.StartsWith("'"))
            {
                filename = filename.Substring(1);
            }
            if (filename.EndsWith("\"") || filename.EndsWith("'"))
            {
                filename = filename.Substring(0, filename.Length - 1);
            }
            AVRESULT ret = 0;
            AVFormatContext ic = null;

            AVDictionary opt = new AVDictionary();
            ret = AVFormatContext.OpenInput(out ic, null, filename, opt);
            opt.Dispose();
            opt = null;

            if (ret != 0)
            {
                Console.WriteLine("Could not open input stream '{0}' {1}", filename, ret);
                Environment.Exit(1);
            }
            ret = ic.FindStreamInfo();

            if (ret != 0)
            {
                ic.Dispose();
                Console.WriteLine("Failed to find stream info '{0}' {1}", filename, ret);
                Environment.Exit(1);
            }

            LiveOutput live_output = new LiveOutput();
            string format_name = "";
            AVOutputFormat fmt = AVOutputFormat.GuessFormat(null,url,null);
            if (fmt == null)
            {
                int idx = 0;
                while (url.Length >= 3 && url.Substring(idx, 3) != "://") idx++;
                format_name = url.Substring(0,idx);
                fmt = AVOutputFormat.GuessFormat(format_name, url, null);
                if (fmt == null && format_name.ToLower() == "udp")
                {
                    fmt = AVOutputFormat.GuessFormat("mpegts", url, null);
                    opt = new AVDictionary();
                    opt.SetValue("mpegts_transport_stream_id", "102");
                    opt.SetValue("mpegts_flags", "1");
                }
            }
            // Open Output
            live_output.oc = AVFormatContext.OpenOutput(fmt, format_name, url);

            var oc = live_output.oc;
            if (oc == null)
            {
                ic.Dispose();
                Console.WriteLine("Could not open output file '{0}'", url);
                Environment.Exit(1);
            }
            oc.interrupt_callback = new AVIOInterruptDesc(AVIOInterrupt, null);

            OutputStream video_stream = null;
            OutputStream audio_stream = null;
            
            int video_stream_idx = -1;
            int audio_stream_idx = -1;

            for (int i = 0; i < ic.nb_streams; i++)
            {
                // Setup Video Stream Output
                if (ic.streams[i].codecpar.codec_type == AVMediaType.VIDEO && video_stream_idx == -1)
                {
                    ret = create_output(out video_stream, live_output, ic.streams[i]);
                    if (ret == 0)
                    {
                        video_stream_idx = i;
                    }
                    else
                    {
                        Console.WriteLine("Could not create output video stream '{0}'", url);
                        break;
                    }
                }
                // Setup Audio Stream Output
                if (ic.streams[i].codecpar.codec_type == AVMediaType.AUDIO && audio_stream_idx == -1)
                {
                    ret = create_output(out audio_stream, live_output, ic.streams[i]);
                    if (ret == 0)
                    {
                        audio_stream_idx = i;
                    }
                    else
                    {
                        Console.WriteLine("Could not create output audio stream '{0}'", url);
                        break;
                    }
                }
            }
            // We should have audio or video for the output file
            if (audio_stream_idx == -1 && video_stream_idx == -1)
            {
                ic.Dispose();
                oc.Dispose();
                Console.WriteLine("Could not configure audio or video stream '{0}'", url);
                Environment.Exit(1);
            }
            oc.DumpFormat(0, url, true);

            if ((int)(oc.oformat.flags & AVfmt.NOFILE) == 0)
            {
                // Open actual file context
                oc.pb = new AVIOContext(url, AvioFlag.WRITE);
                if (!oc.pb._IsValid)
                {
                    Console.WriteLine("Could not open output file '{0}'", url);
                    ret = AVRESULT.EFAULT;
                }
            }
            // Start file output
            ret = oc.WriteHeader(opt);
            if (opt != null)
            {
                opt.Dispose();
                opt = null;
            }
            if (ret < 0)
            {
                Console.WriteLine("Error occurred when opening output file: {0}\n", (AVRESULT)ret);
            }
            if (ret == 0)
            {
                // Start Muxer queue thread
                StartStreamThread(live_output, ProcessMuxerPacket);

                int time_out = (int)(AVMath.rescale_q(StreamBase.queue_max_size, AVRational.AVTimeBaseQ, new AVRational(1, 1000)) >> 1);
                // While not key pressed
                AVPacket pkt = new AVPacket();
                long start_pts = AVUtil.NOPTS_VALUE;
                long start_time = 0;
                while (true)
                {
                    if (Console.KeyAvailable)
                    {
                        var ki = Console.ReadKey();
                        if (ki.Key == ConsoleKey.Escape)
                        {
                            g_evQuit.Set();
                            break;
                        }
                    }
                    if (g_evQuit.WaitOne(0)) {
                        break;
                    }
                    ret = ic.ReadFrame(pkt);
                    if (ret == AVRESULT.EOF)
                    {
                        break;
                    }
                    if (ret >= 0)
                    {
                        long pts = AVUtil.NOPTS_VALUE;
                        if (pkt.pts == AVUtil.NOPTS_VALUE)
                        {
                            if (pkt.dts != AVUtil.NOPTS_VALUE)
                            {
                                pts = AVMath.rescale_q(pkt.dts, ic.streams[pkt.stream_index].time_base, AVRational.AVTimeBaseQ);
                            }
                        }
                        else
                        {
                            pts = AVMath.rescale_q(pkt.pts, ic.streams[pkt.stream_index].time_base, AVRational.AVTimeBaseQ);
                        }

                        if (pkt.stream_index == video_stream_idx)
                        {
                            put_packet(video_stream, pkt, pts);
                        }
                        if (pkt.stream_index == audio_stream_idx)
                        {
                            put_packet(audio_stream, pkt, pts);
                        }
                        pkt.Free();
                        if (pts != AVUtil.NOPTS_VALUE) {
                            if (start_time == 0)
                            {
                                start_time = GetTickCount();
                            }
                            if (start_pts == AVUtil.NOPTS_VALUE)
                            {
                                start_pts = pts;
                            }
                            long queue_pts = pts;
                            if (video_stream_idx != -1 && queue_pts != AVUtil.NOPTS_VALUE)
                            {
                                lock (video_stream)
                                {
                                    if (video_stream.queue_end_ts != AVUtil.NOPTS_VALUE)
                                    {
                                        if (queue_pts > video_stream.queue_end_ts)
                                        {
                                            queue_pts = video_stream.queue_end_ts;
                                        }
                                    }
                                    else
                                    {
                                        if (video_stream.packets.Count < 20)
                                        {
                                            queue_pts = AVUtil.NOPTS_VALUE;
                                        }
                                    }
                                }
                            }
                            
                            if (audio_stream_idx != -1 && queue_pts != AVUtil.NOPTS_VALUE)
                            {
                                lock (audio_stream)
                                {
                                    if (audio_stream.queue_end_ts != AVUtil.NOPTS_VALUE)
                                    {
                                        if (queue_pts > audio_stream.queue_end_ts)
                                        {
                                            queue_pts = audio_stream.queue_end_ts;
                                        }
                                    }
                                    else
                                    {
                                        if (audio_stream.packets.Count < 40)
                                        {
                                            queue_pts = AVUtil.NOPTS_VALUE;
                                        }
                                    }
                                }
                            }
                            if (queue_pts != AVUtil.NOPTS_VALUE && queue_pts > StreamBase.queue_max_size)
                            {
                                queue_pts -= start_pts;
                                queue_pts -= StreamBase.queue_max_size / 2;
                                var playback_time = AVMath.rescale_q(queue_pts, AVRational.AVTimeBaseQ, new AVRational(1, 1000));
                                var current = GetTickCount() - start_time;
                                OutputDebugString(string.Format("PTS: {0} Elapsed: {1}\n",playback_time, current));
                                if (current < playback_time)
                                {
                                    g_evQuit.WaitOne((int)(playback_time - current));
                                }
                            }
                        }
                    }
                    else
                    {
                        Console.WriteLine("Could read data '{0}' {1}", filename, ret);
                        break;
                    }
                }
            }

            // Flush and close video stream
            free_output_stream(video_stream);
            // Flush and close audio stream
            free_output_stream(audio_stream);

            // Signal to quit
            g_evQuit.Set();

            // Finalizing Output 
            oc.WriteTrailer();
            // Close Output
            free_live_output(live_output);

            // free the stream 
            if (oc != null) oc.Dispose();
            if (ic != null) ic.Dispose();

            // We are done
            g_evReady.Set();
            SetConsoleCtrlHandler(handler, false);
        }
    }
}
