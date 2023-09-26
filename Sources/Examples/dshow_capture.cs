//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2023 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// DirectShow 
// Video capture and encoding with libavformat and libavcodec API example.
//////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("dshow_capture")]
[assembly: AssemblyDescription("C# DirectShow Video capture and encoding with libavformat and libavcodec API example.")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class dshow_capture
    {
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

            if (args.Length <= 3)
            {
                Console.WriteLine("Usage: {0} <video device> <Width> <Height> <output file>\n",
                    Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Console.WriteLine("Example: {0} 'Logitech BRIO' 640 480 temp.mp4\n",
                    Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            List<string> _args = new List<string>();

            for (int i = 0; i < args.Length; i++)
            {
                string s = args[i];
                while (s.StartsWith("\"") || s.StartsWith("'"))
                {
                    s = s.Substring(1);
                }
                while (s.EndsWith("\"") || s.EndsWith("'"))
                {
                    s = s.Substring(0, s.Length - 1);
                }
                _args.Add(s);
            }
            string device = _args[0];
            string filename = _args[3];

            int framerate = -1;
            string pixel_format = "";
            Size Resolution = new Size(1920, 1080);
            int temp = 0;
            if (int.TryParse(_args[1], out temp) && temp > 0)
            {
                Resolution.Width = temp;
            }
            if (int.TryParse(_args[2], out temp) && temp > 0)
            {
                Resolution.Height = temp;
            }
            var fmt = AVInputFormat.FindInputFormat("dshow");
            if (fmt == null)
            {
                Console.WriteLine("DirectShow not supporte by this version of libavdevice");
                Environment.Exit(1);
            }
            AVDictionary opt = new AVDictionary();
            opt.SetValue("video_size", Resolution.Width + "x" + Resolution.Height);
            if (framerate > 0)
            {
                opt.SetValue("framerate", framerate.ToString());
            }
            if (!string.IsNullOrEmpty(pixel_format)) {
                opt.SetValue("pixel_format", pixel_format);
            }
            AVRESULT ret = 0;
            AVFormatContext ic = null;

            ret = AVFormatContext.OpenInput(out ic, fmt, "video=" + device, opt);
            opt.Dispose();

            if (ret != 0)
            {
                Console.WriteLine("Could not open output capture device '{0}' {1}", device,ret);
                Environment.Exit(1);
            }

            ret = ic.FindStreamInfo();

            if (ret != 0)
            {
                ic.Dispose();
                Console.WriteLine("Failed to find stream info '{0}' {1}", device, ret);
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

            int video_stream_idx = -1;
            AVCodecContext video_encoder = null;
            AVCodecContext video_decoder = null;
            AVStream video_stream = null;

            for (int i = 0; i < ic.nb_streams; i++)
            {
                if (ic.streams[i].codecpar.codec_type == AVMediaType.VIDEO && video_stream_idx == -1)
                {
                    AVCodec codec = AVCodec.FindDecoder(ic.streams[i].codecpar.codec_id);

                    if (codec != null)
                    {
                        video_decoder = new AVCodecContext(codec);
                        ic.streams[i].codecpar.CopyTo(video_decoder);
                        ret = video_decoder.Open(codec);
                        if (ret.Failed)
                        {
                            video_decoder.Dispose();
                            video_decoder = null;
                        }
                    }
                    var codecs = new AVCodecID[] { oc.oformat.video_codec, AVCodecID.H264 };
                    AVCodecContext encoder = null;
                    if (video_stream == null)
                    {
                        video_stream = oc.AddStream();
                        video_stream.id = oc.nb_streams - 1;
                        video_stream.time_base = ic.streams[i].time_base;
                    }

                    for (int j = 0; j < codecs.Length && encoder == null; j++)
                    {
                        codec = AVCodec.FindEncoder(codecs[i]);
                        if (codec == null) continue;
                        encoder = new AVCodecContext(codec);
                        encoder.codec_id = codec.id;

                        encoder.width = ic.streams[i].codecpar.width;
                        encoder.height = ic.streams[i].codecpar.height;
                        encoder.time_base = video_stream.time_base;
                        if (codec.pix_fmts != null && codec.pix_fmts.Length > 0)
                        {
                            encoder.pix_fmt = codec.pix_fmts[0];
                        }
                        else
                        {
                            encoder.pix_fmt = AVPixelFormat.YUV420P;
                        }
                        encoder.max_b_frames = 0;
                        encoder.gop_size = 0;
                        encoder.flags |= AVCodecFlag.CLOSED_GOP | AVCodecFlag.LOW_DELAY;
                        encoder.flags2 |= AVCodecFlag2.FAST;

                        encoder.bit_rate = (encoder.width * encoder.height) << 1;
                        if ((oc.oformat.flags & AVfmt.GLOBALHEADER) != 0)
                        {
                            encoder.flags |= AVCodecFlag.GLOBAL_HEADER;
                        }

                        ret = encoder.Open(codec);
                        if (ret >= 0)
                        {
                            ret = video_stream.codecpar.FromContext(encoder);
                        }
                        if (ret < 0)
                        {
                            encoder.Dispose();
                            encoder = null;
                            continue;
                        }
                    }
                    if (encoder != null)
                    {
                        video_encoder = encoder;
                        video_stream_idx = i;
                    }
                    break;
                }
            }

            oc.DumpFormat(0, filename, true);

            if ((int)(oc.oformat.flags & AVfmt.NOFILE) == 0)
            {
                oc.pb = new AVIOContext(filename, AvioFlag.WRITE);
                if (!oc.pb._IsValid)
                {
                    Console.WriteLine("Could not open output file '{0}'", filename);
                }
            }
            ret = oc.WriteHeader();
            if (ret < 0)
            {
                Console.WriteLine("Error occurred when opening output file: {0}\n", (AVRESULT)ret);
            }
            if (ret >= 0)
            {
                SwsContext sws_ctx = null;
                AVPacket pkt = new AVPacket();
                AVPacket output_pkt = new AVPacket();
                AVFrame video_frame = new AVFrame();
                AVFrame temporal_frame = null;
                long video_start_pts = AVUtil.NOPTS_VALUE;
                int index = 0;
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
                    bool bGotPicture = false;
                    if (pkt.stream_index == video_stream_idx)
                    {
                        AVFrame output_frame = null;
                        if (video_decoder != null)
                        {
                            ret = video_decoder.DecodeVideo(video_frame, ref bGotPicture, pkt);
                        }
                        else
                        {
                            bGotPicture = true;
                            var cp = ic.streams[pkt.stream_index].codecpar;
                            video_frame.pts = pkt.pts;
                            video_frame.Fill(pkt.data, cp.format, cp.width, cp.height);
                        }
                        if (bGotPicture)
                        {
                            if (sws_ctx == null && video_encoder.pix_fmt != video_frame.format)
                            {
                                temporal_frame = new AVFrame();
                                temporal_frame.width = video_encoder.width;
                                temporal_frame.height = video_encoder.height;
                                temporal_frame.format = (int)video_encoder.pix_fmt;
                                temporal_frame.AllocateBuffers(16);
                                temporal_frame.MakeWritable();
                                sws_ctx = new SwsContext(video_frame, temporal_frame, SwsFlags.FastBilinear);
                            }
                            if (sws_ctx != null)
                            {
                                ret = sws_ctx.Scale(video_frame, 0, video_frame.height, temporal_frame);
                                output_frame = temporal_frame;
                                output_frame.pts = video_frame.pts;
                            }
                            else
                            {
                                output_frame = video_frame;
                            }
                            if (video_start_pts == AVUtil.NOPTS_VALUE)
                            {
                                video_start_pts = output_frame.pts;
                            }
                            output_frame.pts -= video_start_pts;
                            output_frame.key_frame = true;
                            output_frame.display_picture_number = index++;
                            bGotPicture = false;
                            ret = video_encoder.EncodeVideo(output_pkt, output_frame, ref bGotPicture);
                            if (bGotPicture)
                            {
                                output_pkt.RescaleTS(video_encoder.time_base, video_stream.time_base);
                                output_pkt.stream_index = video_stream.index;
                                oc.InterleavedWriteFrame(output_pkt);
                                output_pkt.Free();
                            }
                            video_frame.Free();
                        }
                    }
                    pkt.Free();
                }

                if (temporal_frame != null) temporal_frame.Dispose();
                if (video_frame != null) video_frame.Dispose();
                if (output_pkt != null) output_pkt.Dispose();
                if (sws_ctx != null) sws_ctx.Dispose();
            }
            // Signal to quit
            g_evQuit.Set();

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

            if (video_decoder != null) video_decoder.Dispose();
            if (video_encoder != null) video_encoder.Dispose();

            // We are done
            g_evReady.Set();
            SetConsoleCtrlHandler(handler, false);
        }
    }
}
