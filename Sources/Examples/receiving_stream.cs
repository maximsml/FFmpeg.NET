//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2023 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// Receiving data from specified url decode audio and video and remux it 
// into another file with re-encoding
//////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("receiving_stream")]
[assembly: AssemblyDescription("Receiving data from specified url decode audio and video and remux it" +
"into another file with re-encoding")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class receiving_stream
    {
        // Stream Definition class
        class OutputStream
        {
            // Packets queue
            public List<AVPacket> packets = new List<AVPacket>();
            // Notification Event for new packet added
            public EventWaitHandle signal = new AutoResetEvent(false);
            // Output Thread
            public Thread thread = null;
            // Output Format context
            public AVFormatContext oc = null;
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
            // Thread Quit flag
            public bool exit = false;
            // Shortcut to media type
            public bool is_video = false;
        }

        // Output Stream Thread function
        static void OutputStreamThread(object o)
        {
            OutputStream st = (OutputStream)o;
            AVFrame temporal_frame = null;

            AVFrame target_frame = new AVFrame();
            AVPacket output_pkt = new AVPacket();
            var handles = new WaitHandle[] { st.signal, g_evQuit };
            bool exit = false;
            int index = 0;
            long stream_start_pts = AVUtil.NOPTS_VALUE;
            // Loop until quit signaled
            while (!exit) {

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
                    lock (st)
                    {
                        exit = st.exit;
                        // Extract packets if we have
                        if (st.packets.Count > 0)
                        {
                            pkt = st.packets[0];
                            st.packets.RemoveAt(0);
                        }
                    }
                    if (pkt != null)
                    {
                        try
                        {
                            AVRESULT ret = 0;
                            AVFrame output_frame = null;
                            // If We have decoder
                            if (st.decoder != null)
                            {
                                // Process packet
                                ret = st.decoder.SendPacket(pkt);

                                while (ret >= 0)
                                {
                                    ret = st.decoder.ReceiveFrame(target_frame);
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
                                            if (st.sws_ctx == null && st.encoder.pix_fmt != target_frame.format)
                                            {
                                                // We create and allocate temporal frame
                                                temporal_frame = new AVFrame();
                                                temporal_frame.width = st.encoder.width;
                                                temporal_frame.height = st.encoder.height;
                                                temporal_frame.format = (int)st.encoder.pix_fmt;
                                                temporal_frame.AllocateBuffers(16);
                                                temporal_frame.MakeWritable();
                                                // Create scaling context if it required
                                                st.sws_ctx = new SwsContext(target_frame, temporal_frame, SwsFlags.FastBilinear);
                                            }
                                            // If have scaling context then apply it
                                            if (st.sws_ctx != null)
                                            {
                                                ret = st.sws_ctx.Scale(target_frame, 0, target_frame.height, temporal_frame);
                                                output_frame = temporal_frame;
                                                output_frame.pts = target_frame.pts;
                                            }
                                            else
                                            {
                                                // Otherwise just set output as the target frame
                                                output_frame = target_frame;
                                            }
                                        }
                                        else // Audio
                                        {
                                            // We need to check if the format is compatible
                                            if (st.swr_ctx == null && (st.encoder.sample_fmt != target_frame.format || st.encoder.channel_layout != target_frame.channel_layout))
                                            {
                                                // We create and allocate temporal frame
                                                temporal_frame = new AVFrame();

                                                temporal_frame.format = (int)st.encoder.sample_fmt;
                                                temporal_frame.channels = st.encoder.channels;
                                                temporal_frame.channel_layout = st.encoder.channel_layout;
                                                temporal_frame.AllocateBuffers(16);
                                                temporal_frame.MakeWritable();

                                                st.swr_ctx = new SwrContext(
                                                                    temporal_frame.channel_layout,
                                                                    temporal_frame.format, temporal_frame.sample_rate,
                                                                    target_frame.channel_layout,
                                                                    target_frame.format, target_frame.sample_rate);

                                                ret = st.swr_ctx.Init();

                                            }
                                            if (st.swr_ctx != null)
                                            {
                                                int _count = st.swr_ctx.Convert(temporal_frame,target_frame);

                                                output_frame = temporal_frame;
                                            }
                                            else
                                            {
                                                // Otherwise just set output as the target frame
                                                output_frame = target_frame;
                                            }
                                        }
                                        // Send Frame to encoder
                                        if (stream_start_pts == AVUtil.NOPTS_VALUE)
                                        {
                                            stream_start_pts = output_frame.pts;
                                        }

                                        output_frame.pts -= stream_start_pts;
                                        output_frame.display_picture_number = index++;
                                        output_frame.key_frame = true;

                                        ret = st.encoder.SendFrame(output_frame);
                                        // We only free buffer of the target frame as the output can be allocated temporal frame
                                        target_frame.Free();

                                        while (ret >= 0)
                                        {
                                            // Process all encoded packets 
                                            ret = st.encoder.ReceivePacket(output_pkt);
                                            if (ret == AVRESULT.EAGAIN || ret == AVRESULT.EOF)
                                            {
                                                break;
                                            }
                                            if (ret >= 0)
                                            {
                                                // And write them into output
                                                output_pkt.RescaleTS(st.encoder.time_base, st.stream.time_base);
                                                output_pkt.stream_index = st.stream.index;
                                                lock (st.oc)
                                                {
                                                    ret = st.oc.InterleavedWriteFrame(output_pkt);
                                                }
                                                output_pkt.Free();
                                                if (ret < 0)
                                                {
                                                    Console.WriteLine("{0} Write Output Error {1}", st.is_video ? "Video" : "Audio", ret);
                                                    break;
                                                }
                                            }
                                            else
                                            {
                                                Console.WriteLine("{0} Encoding Error {1}", st.is_video ? "Video" : "Audio", ret);
                                                break;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        Console.WriteLine("{0} Decoding Error {1}", st.is_video ? "Video" : "Audio", ret);
                                        exit = true;
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                // We are do not not performing re-encoding
                                pkt.stream_index = st.stream.index;
                                lock (st.oc)
                                {
                                    ret = st.oc.InterleavedWriteFrame(pkt);
                                }
                                if (ret < 0)
                                {
                                    Console.WriteLine("{0} Write Output Error {1}", st.is_video ? "Video" : "Audio", ret);
                                }
                            }
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
            lock (st) {
                st.exit = true;
            }
            // Free all objects
            if (temporal_frame != null) temporal_frame.Dispose();
            if (target_frame != null) target_frame.Dispose();
            if (output_pkt != null) output_pkt.Dispose();
        }

        // Create Output Stream
        static AVRESULT create_output(out OutputStream st, AVFormatContext oc, AVStream istream)
        {
            AVRESULT ret = 0;
            st = new OutputStream();

            st.oc = oc;
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
                        st.encoder.flags |= AVCodecFlag.CLOSED_GOP;

                        st.encoder.bit_rate = (st.encoder.width * st.encoder.height) << 1;

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
                        st.encoder.channels = istream.codecpar.channels;
                        st.encoder.channel_layout = istream.codecpar.channel_layout;
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
                    st.encoder.time_base = st.stream.time_base;

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
            if (ret >= 0) {
                st.thread = new Thread(OutputStreamThread);
                st.thread.Start(st);
            }
            return ret;
        }

        // Free Output Stream
        static void free_output(OutputStream st)
        {
            if (st != null)
            {
                if (st.thread != null) {

                    lock (st)
                    {
                        st.exit = true;
                        st.signal.Set();
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
                        pkt.Dispose();
                    }
                }
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
            }
        }

        // Put packet into queue
        static void put_packet(OutputStream st, AVPacket pkt)
        {
            if (st != null && pkt != null && pkt.size > 0)
            {
                AVPacket output_pkt = new AVPacket();
                pkt.MoveTo(output_pkt);

                lock (st)
                {
                    if (st.thread != null && !st.exit)
                    {
                        st.packets.Add(output_pkt);
                        st.signal.Set();
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

        static void Main(string[] args)
        {
            HandlerRoutine handler = new HandlerRoutine(ConsoleHandlerRoutine);
            SetConsoleCtrlHandler(handler, true);

            if (args.Length <= 1)
            {
                Console.WriteLine("Usage: {0} <url> <output file>\n",
                    Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Console.WriteLine("Example: {0} rtsp://192.168.0.50:554/live/main temp.mp4\n",
                    Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            string url = args[0];
            string filename = args[1];

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
            AVInputFormat fmt = null;
            AVRESULT ret = 0;
            AVFormatContext ic = null;

            AVDictionary opt = new AVDictionary();

            ret = AVFormatContext.OpenInput(out ic, fmt, url, opt);
            opt.Dispose();

            if (ret != 0)
            {
                Console.WriteLine("Could not open input stream '{0}' {1}", url, ret);
                Environment.Exit(1);
            }
            if (fmt == null)
            {
                fmt = ic.iformat;
            }
            ret = ic.FindStreamInfo();

            if (ret != 0)
            {
                ic.Dispose();
                Console.WriteLine("Failed to find stream info '{0}' {1}", url, ret);
                Environment.Exit(1);
            }
            // Open Output
            AVFormatContext oc = AVFormatContext.OpenOutput(filename);

            if (oc == null)
            {
                ic.Dispose();
                Console.WriteLine("Could not open output file '{0}'", filename);
                Environment.Exit(1);
            }
            OutputStream video_stream = null;
            OutputStream audio_stream = null;

            int video_stream_idx = -1;
            int audio_stream_idx = -1;

            for (int i = 0; i < ic.nb_streams; i++)
            {
                // Setup Video Stream Output
                if (ic.streams[i].codecpar.codec_type == AVMediaType.VIDEO && video_stream_idx == -1)
                {
                    ret = create_output(out video_stream,oc, ic.streams[i]);
                    if (ret == 0)
                    {
                        video_stream_idx = i;
                    }
                    else
                    {
                        Console.WriteLine("Could not create output video stream '{0}'", filename);
                        break;
                    }
                }
                // Setup Audio Stream Output
                if (ic.streams[i].codecpar.codec_type == AVMediaType.AUDIO && audio_stream_idx == -1)
                {
                    ret = create_output(out audio_stream, oc, ic.streams[i]);
                    if (ret == 0)
                    {
                        audio_stream_idx = i;
                    }
                    else
                    {
                        Console.WriteLine("Could not create output audio stream '{0}'", filename);
                        break;
                    }
                }
            }
            // We should have audio or video for the output file
            if (audio_stream_idx == -1 && video_stream_idx == -1)
            {
                ic.Dispose();
                oc.Dispose();
                Console.WriteLine("Could not configure audio or video stream '{0}'", filename);
                Environment.Exit(1);
            }
            oc.DumpFormat(0, filename, true);

            if ((int)(oc.oformat.flags & AVfmt.NOFILE) == 0)
            {
                // Open actual file context
                oc.pb = new AVIOContext(filename, AvioFlag.WRITE);
                if (!oc.pb._IsValid)
                {
                    Console.WriteLine("Could not open output file '{0}'", filename);
                    ret = AVRESULT.EFAULT;
                }
            }
            // Start file output
            ret = oc.WriteHeader();
            if (ret < 0)
            {
                Console.WriteLine("Error occurred when opening output file: {0}\n", (AVRESULT)ret);
            }
            if (ret == 0)
            {
                // While not key pressed
                AVPacket pkt = new AVPacket();
                while (true)
                {
                    if (Console.KeyAvailable)
                    {
                        var ki = Console.ReadKey();
                        if (ki.Key == ConsoleKey.Escape)
                        {
                            break;
                        }
                    }
                    ret = ic.ReadFrame(pkt);
                    if (ret == AVRESULT.EOF)
                    {
                        break;
                    }
                    if (ret >= 0)
                    {
                        if (pkt.stream_index == video_stream_idx)
                        {
                            put_packet(video_stream, pkt);
                        }
                        if (pkt.stream_index == audio_stream_idx)
                        {
                            put_packet(audio_stream, pkt);
                        }
                        pkt.Free();
                    }
                    else
                    {
                        Console.WriteLine("Could read data '{0}' {1}", url,ret);
                        break;
                    }
                }
            }
            // Signal to quit
            g_evQuit.Set();

            // Flush and close video stream
            free_output(video_stream);
            // Flush and close audio stream
            free_output(audio_stream);

            // Finalizing Output 
            oc.WriteTrailer();
            if (oc != null && (int)(oc.oformat.flags & AVfmt.NOFILE) == 0)
            {
                if (oc.pb != null)
                {
                    oc.pb.Dispose();
                }
            }

            // free the stream 
            if (oc != null) oc.Dispose();
            if (ic != null) ic.Dispose();

            // We are done
            g_evReady.Set();
            SetConsoleCtrlHandler(handler, false);
        }
    }
}
