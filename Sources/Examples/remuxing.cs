//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// libavformat/libavcodec demuxing and muxing API example.
// Remux streams from one container format to another.
//////////////////////////////////////////////////////
using System;
using System.Reflection;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("remuxing")]
[assembly: AssemblyDescription("C# libavformat/libavcodec demuxing and muxing API example. "
            +"Remux streams from one container format to another.")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class remuxing
    {
        static void log_packet(AVFormatContext fmt_ctx, AVPacket pkt, string tag)
        {
            AVRational time_base = fmt_ctx.streams[pkt.stream_index].time_base;

            Console.WriteLine("{0}: pts:{1} pts_time:{2} dts:{3} dts_time:{4} duration:{5} duration_time:{6} stream_index:{7}",
                   tag,
                   AVUtil.ts2timestr(pkt.pts), AVUtil.ts2timestr(pkt.pts, time_base),
                   AVUtil.ts2timestr(pkt.dts), AVUtil.ts2timestr(pkt.dts, time_base),
                   AVUtil.ts2timestr(pkt.duration), AVUtil.ts2timestr(pkt.duration, time_base),
                   pkt.stream_index);
        }


        static void Main(string[] args)
        {
            AVFormatContext ifmt_ctx = null;
            AVFormatContext ofmt_ctx = null;
            AVOutputFormat ofmt = null;
            AVPacket pkt = null;
            int stream_index = 0;

            if (args.Length < 2)
            {
                Console.WriteLine("usage: {0} input output\n"
                                + "API example program to remux a media file with libavformat and libavcodec.\n"
                                + "The output format is guessed according to the file extension.\n",
                                Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }
            string in_filename  = args[0];
            string out_filename = args[1];
            int ret = 0;

            if ((ret = AVFormatContext.OpenInput(out ifmt_ctx, in_filename)) < 0) {
                Console.WriteLine("Could not open input file {0}", in_filename);
                goto end;
            }

            if ((ret = ifmt_ctx.FindStreamInfo()) < 0) {
                Console.WriteLine("Failed to retrieve input stream information");
                goto end;
            }

            ifmt_ctx.DumpFormat(0, in_filename, false);

            ofmt_ctx = AVFormatContext.OpenOutput(out_filename);
            if (ofmt_ctx == null) {
                Console.WriteLine("Could not create output context");
                ret = AVRESULT.UNKNOWN;
                goto end;
            }

            int stream_mapping_size = ifmt_ctx.nb_streams;
            int[] stream_mapping = new int[stream_mapping_size];
            if (stream_mapping == null)
            {
                ret = AVRESULT.ENOMEM;
                goto end;
            }

            ofmt = ofmt_ctx.oformat;

            for (int i = 0; i < ifmt_ctx.nb_streams; i++) {
                AVStream out_stream;
                AVStream in_stream = ifmt_ctx.streams[i];
                AVCodecParameters in_codecpar = in_stream.codecpar;

                if (in_codecpar.codec_type != AVMediaType.AUDIO &&
                    in_codecpar.codec_type != AVMediaType.VIDEO &&
                    in_codecpar.codec_type != AVMediaType.SUBTITLE) {
                    stream_mapping[i] = -1;
                    continue;
                }

                stream_mapping[i] = stream_index++;

                out_stream = ofmt_ctx.AddStream();
                if (out_stream == null) {
                    Console.WriteLine("Failed allocating output stream\n");
                    ret = AVRESULT.UNKNOWN;
                    goto end;
                }

                ret = in_codecpar.CopyTo(out_stream.codecpar);
                if (ret < 0) {
                    Console.WriteLine("Failed to copy codec parameters\n");
                    goto end;
                }
                out_stream.codecpar.codec_tag = 0;
            }
            ofmt_ctx.DumpFormat(0, out_filename, true);

            if ((int)(ofmt.flags & AVfmt.NOFILE) == 0) {
                ofmt_ctx.pb = new AVIOContext(out_filename,AvioFlag.WRITE);
                if (!ofmt_ctx.pb._IsValid) {
                    Console.WriteLine("Could not open output file '{0}'", out_filename);
                    goto end;
                }
            }

            ret = ofmt_ctx.WriteHeader();
            if (ret < 0) {
                Console.WriteLine("Error occurred when opening output file\n");
                goto end;
            }

            pkt = new AVPacket();

            while (true) {
                AVStream in_stream, out_stream;

                ret = ifmt_ctx.ReadFrame(pkt);
                if (ret < 0)
                    break;

                in_stream  = ifmt_ctx.streams[pkt.stream_index];
                if (pkt.stream_index >= stream_mapping_size ||
                    stream_mapping[pkt.stream_index] < 0) {
                    pkt.Free();
                    continue;
                }

                pkt.stream_index = stream_mapping[pkt.stream_index];
                out_stream = ofmt_ctx.streams[pkt.stream_index];
                log_packet(ifmt_ctx, pkt, "in");

                
                /* copy packet */
                pkt.pts = AVMath.rescale_q_rnd(pkt.pts, in_stream.time_base, out_stream.time_base, AVRounding.NEAR_INF | AVRounding.PASS_MINMAX);
                pkt.dts = AVMath.rescale_q_rnd(pkt.dts, in_stream.time_base, out_stream.time_base, AVRounding.NEAR_INF | AVRounding.PASS_MINMAX);
                pkt.duration = AVMath.rescale_q(pkt.duration, in_stream.time_base, out_stream.time_base);
                pkt.pos = -1;
                log_packet(ofmt_ctx, pkt, "out");

                ret = ofmt_ctx.InterleavedWriteFrame(pkt);
                if (ret < 0) {
                    Console.WriteLine("Error muxing packet");
                    break;
                }
                pkt.Free();
            }

            ofmt_ctx.WriteTrailer();

            end:
            if (pkt != null) pkt.Dispose();
            if (ifmt_ctx != null) ifmt_ctx.Dispose();

            if (ofmt_ctx != null && (int)(ofmt.flags & AVfmt.NOFILE) == 0) {
                if (ofmt_ctx.pb != null)
                {
                    ofmt_ctx.pb.Dispose();
                }
            }
            if (ofmt_ctx != null) ofmt_ctx.Dispose();

            if (ret < 0 && ret != AVRESULT.EOF)
            {
                Console.WriteLine("Error occurred: {0}\n", (AVRESULT)ret);
                Environment.Exit(1);
            }

            Environment.Exit(0);
        }
    }
}
