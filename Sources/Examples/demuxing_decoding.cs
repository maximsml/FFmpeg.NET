//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// Demuxing and decoding example.
// Show how to use the libavformat and libavcodec API to demux and
// decode audio and video data.
//////////////////////////////////////////////////////
using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("demuxing_decoding")]
[assembly: AssemblyDescription("C# Demuxing and decoding example."
    + "Show how to use the libavformat and libavcodec API to demux and"
    + "decode audio and video data.")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class demuxing_decoding
    {
        static AVFormatContext fmt_ctx = null;
        static AVCodecContext video_dec_ctx = null, audio_dec_ctx;
        static int width, height;
        static AVPixelFormat pix_fmt;
        static AVStream video_stream = null, audio_stream = null;
        static string src_filename = null;
        static string video_dst_filename = null;
        static string audio_dst_filename = null;
        static Stream video_dst_file = null;
        static Stream audio_dst_file = null;

        static AVPicture video_dst_data = null;
        static int video_dst_bufsize;
        static byte[] video_data = null;
        static byte[] audio_data = null;

        static int video_stream_idx = -1, audio_stream_idx = -1;
        static AVFrame frame = null;
        static AVPacket pkt = null;
        static int video_frame_count = 0;
        static int audio_frame_count = 0;

        /* Enable or disable frame reference counting. You are not supposed to support
         * both paths in your application but pick the one most appropriate to your
         * needs. Look for the use of refcount in this example to see what are the
         * differences of API usage between them. */
        static int refcount = 0;

        static int decode_packet(out bool got_frame, int cached)
        {
            AVRESULT ret = 0;
            int decoded = pkt.size;

            got_frame = false;

            if (pkt.stream_index == video_stream_idx) {
                /* decode video frame */
                ret = video_dec_ctx.DecodeVideo(frame, ref got_frame, pkt);
                if (ret < 0) {
                    Console.WriteLine("Error decoding video frame ({0})", ret);
                    return ret;
                }

                if (got_frame) {

                    if (frame.width != width || frame.height != height ||
                        frame.format != (int)pix_fmt) {
                        /* To handle this change, one could call av_image_alloc again and
                         * decode the following frames into another rawvideo file. */
                        Console.WriteLine("Error: Width, height and pixel format have to be "
                                + "constant in a rawvideo file, but the width, height or "
                                + "pixel format of the input video changed:\n"
                                + "old: width = %d, height = %d, format = %s\n"
                                + "new: width = %d, height = %d, format = %s\n",
                                width, height, AVPixel.get_pix_fmt_name(pix_fmt),
                                frame.width, frame.height,
                                AVPixel.get_pix_fmt_name((AVPixelFormat)frame.format));
                        return -1;
                    }

                    Console.WriteLine("video_frame{0} n:{1} coded_n:{2}",
                           cached != 0 ? "(cached)" : "",
                           video_frame_count++, frame.coded_picture_number);

                    /* copy decoded frame to destination buffer:
                     * this is required since rawvideo expects non aligned data */
                    frame.CopyTo(video_dst_data, pix_fmt, width, height);

                    /* write to rawvideo file */
                    if (video_data == null) video_data = new byte[video_dst_bufsize];
                    Marshal.Copy(video_dst_data.data[0], video_data, 0, video_dst_bufsize);
                    video_dst_file.Write(video_data, 0, video_dst_bufsize);
                }
            } else if (pkt.stream_index == audio_stream_idx) {
                /* decode audio frame */
                ret = audio_dec_ctx.DecodeAudio(frame, ref got_frame, pkt);
                if (ret < 0) {
                    Console.WriteLine("Error decoding audio frame ({0})", ret);
                    return ret;
                }
                /* Some audio decoders decode only part of the packet, and have to be
                 * called again with the remainder of the packet data.
                 * Sample: fate-suite/lossless-audio/luckynight-partial.shn
                 * Also, some decoders might over-read the packet. */
                decoded = ret < pkt.size ? (int)ret : pkt.size;

                if (got_frame) {
                    int unpadded_linesize = frame.nb_samples * ((AVSampleFormat)frame.format).bytes_per_sample;
                    Console.WriteLine("audio_frame{0} n:{1} nb_samples:{2} pts:{3}",
                           cached != 0 ? "(cached)" : "",
                           audio_frame_count++, frame.nb_samples,
                           AVUtil.ts2timestr(frame.pts, audio_dec_ctx.time_base));

                    /* Write the raw audio data samples of the first plane. This works
                     * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
                     * most audio decoders output planar audio, which uses a separate
                     * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
                     * In other words, this code will write only the first audio channel
                     * in these cases.
                     * You should use libswresample or libavfilter to convert the frame
                     * to packed data. */
                    if (audio_data == null || audio_data.Length < unpadded_linesize) audio_data = new byte[unpadded_linesize];
                    Marshal.Copy(Marshal.ReadIntPtr(frame.extended_data), audio_data, 0, unpadded_linesize);
                    audio_dst_file.Write(audio_data, 0, unpadded_linesize);
                }
            }

            /* If we use frame reference counting, we own the data and need
             * to de-reference it when we don't use it anymore */
            if (got_frame && refcount > 0)
                frame.Free();

            return decoded;
        }

        static int open_codec_context(out int stream_idx,
                              out AVCodecContext dec_ctx, AVFormatContext fmt_ctx, AVMediaType type)
        {
            int ret, stream_index;
            AVStream st;
            AVCodec dec = null;
            AVDictionary opts = null;
            stream_idx = -1;
            dec_ctx = null;

            ret = fmt_ctx.FindBestStream(type);
            if (ret < 0) {
                Console.WriteLine("Could not find {0} stream in input file '{1}'\n",
                    AVUtil.get_media_type_string(type), src_filename);
                return ret;
            } else {
                stream_index = ret;
                st = fmt_ctx.streams[stream_index];

                /* find decoder for the stream */
                dec = AVCodec.FindDecoder(st.codecpar.codec_id);
                if (dec == null) {
                    Console.WriteLine("Failed to find %s codec\n", AVUtil.get_media_type_string(type));
                    return AVRESULT.EINVAL;
                }

                /* Allocate a codec context for the decoder */
                dec_ctx = new AVCodecContext(dec);
                if (dec_ctx == null) {
                    Console.WriteLine("Failed to allocate the {0} codec context\n",
                        AVUtil.get_media_type_string(type));
                    return AVRESULT.ENOMEM;
                }

                /* Copy codec parameters from input stream to output codec context */
                if ((ret = st.codecpar.CopyTo(dec_ctx)) < 0) {
                    Console.WriteLine("Failed to copy {0} codec parameters to decoder context\n",
                            AVUtil.get_media_type_string(type));
                    return ret;
                }

                /* Init the decoders, with or without reference counting */
                opts = new AVDictionary();
                opts.SetValue("refcounted_frames", refcount != 0 ? "1" : "0");
                if ((ret = dec_ctx.Open(dec, opts)) < 0) {
                    Console.WriteLine("Failed to open {0} codec\n",
                            AVUtil.get_media_type_string(type));
                    opts.Dispose();
                    return ret;
                }
                opts.Dispose();
                stream_idx = stream_index;
            }

            return 0;
        }

        static int get_format_from_sample_fmt(out string fmt, AVSampleFormat sample_fmt, bool be)
        {
            fmt = "";
            AVSampleFormat[] fmts = new[] { AVSampleFormat.U8, AVSampleFormat.S16, AVSampleFormat.S32, AVSampleFormat.FLT, AVSampleFormat.DBL };
            string[] sbe = new[] { "u8", "s16be", "s32be", "f32be", "f64be" };
            string[] sle = new[] { "u8", "s16le", "s32le", "f32le", "f64le" };

            for (int i = 0; i < fmts.Length; i++)
            {
                if (fmts[i] == sample_fmt)
                {
                    fmt = be ? sbe[i] : sle[i];
                    return 0;
                }
            }
            Console.WriteLine("sample format {0} is not supported as output format\n",
                    sample_fmt.name);
            return -1;
        }

        static void Main(string[] args)
        {
            int ret = 0;
            bool got_frame = false;
            if (args.Length != 3 && args.Length != 4)
            {
                Console.WriteLine("usage: {0} [-refcount] input_file video_output_file audio_output_file\n"
                    + "API example program to show how to read frames from an input file.\n" 
                    + "This program reads frames from a file, decodes them, and writes decoded\n"
                    + "video frames to a rawvideo file named video_output_file, and decoded\n"
                    + "audio frames to a rawaudio file named audio_output_file.\n\n"
                    + "If the -refcount option is specified, the program use the\n"
                    + "reference counting frame system which allows keeping a copy of\n"
                    + "the data for longer than one decode call.\n"
                    + "\n", Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }
            
            int argv = -1;
            if (args.Length == 5 && args[0].ToLower() == "-refcount")
            {
                refcount = 1;
                argv++;
            }

            src_filename = args[argv + 1];
            video_dst_filename = args[argv + 2];
            audio_dst_filename = args[argv + 3];

            /* open input file, and allocate format context */
            if (AVFormatContext.OpenInput(out fmt_ctx, src_filename) < 0) {
                Console.WriteLine("Could not open source file {0}\n", src_filename);
                Environment.Exit(1);
            }

            /* retrieve stream information */
            if (fmt_ctx.FindStreamInfo() < 0) {
                Console.WriteLine("Could not find stream information\n");
                Environment.Exit(1);
            }

            if (open_codec_context(out video_stream_idx, out video_dec_ctx, fmt_ctx, AVMediaType.VIDEO) >= 0)
            {
                video_stream = fmt_ctx.streams[video_stream_idx];

                video_dst_file = File.OpenWrite(video_dst_filename);
                if (video_dst_file == null)
                {
                    Console.WriteLine("Could not open destination file {0}\n", video_dst_filename);
                    ret = 1;
                    goto end;
                }

                /* allocate image where the decoded image will be put */
                width = video_dec_ctx.width;
                height = video_dec_ctx.height;
                pix_fmt = video_dec_ctx.pix_fmt;
                video_dst_data = new AVPicture(pix_fmt, width, height);
                ret = video_dst_data.AllocatedSize;

                if (video_dst_data._IsValid && ret <= 0) {
                    Console.WriteLine("Could not allocate raw video buffer\n");
                    goto end;
                }
                
                video_dst_bufsize = ret;
            }

            if (open_codec_context(out audio_stream_idx, out audio_dec_ctx, fmt_ctx, AVMediaType.AUDIO) >= 0) {
                audio_stream = fmt_ctx.streams[audio_stream_idx];
                audio_dst_file = File.OpenWrite(audio_dst_filename);
                if (audio_dst_file == null) {
                    Console.WriteLine("Could not open destination file {0}\n", audio_dst_filename);
                    ret = 1;
                    goto end;
                }
            }

            /* dump input information to stderr */
            fmt_ctx.DumpFormat(0, src_filename, false);
            if (audio_stream == null || video_stream == null) {
                Console.WriteLine("Could not find audio or video stream in the input, aborting\n");
                ret = 1;
                goto end;
            }

            frame = new AVFrame();
            if (!frame._IsValid)
            {
                Console.WriteLine("Could not allocate frame\n");
                ret = AVRESULT.ENOMEM;
                goto end;
            }

            /* initialize packet, set data to NULL, let the demuxer fill it */
            pkt = new AVPacket();
            pkt.data = null;
            pkt.size = 0;

            if (video_stream != null)
                Console.WriteLine("Demuxing video from file '{0}' into '{1}'", src_filename, video_dst_filename);
            if (audio_stream != null)
                Console.WriteLine("Demuxing audio from file '{0}' into '{1}'", src_filename, audio_dst_filename);


            /* read frames from the file */
            while (fmt_ctx.ReadFrame(pkt) >= 0) {
                AVPacket orig_pkt = pkt;
                do {
                    ret = decode_packet(out got_frame, 0);
                    if (ret < 0)
                        break;
                    pkt.data += ret;
                    pkt.size -= ret;
                } while (pkt.size > 0);
                orig_pkt.Free();
            }

            /* flush cached frames */
            pkt.data = null;
            pkt.size = 0;
            do {
                decode_packet(out got_frame, 1);
            } while (got_frame);

            Console.WriteLine("Demuxing succeeded.\n");

            if (video_stream != null) {
                Console.WriteLine("Play the output video file with the command:\n"
                       +"ffplay -f rawvideo -pix_fmt {0} -video_size {1}x{2} {3}\n",
                       AVPixel.get_pix_fmt_name(pix_fmt), width, height,
                       video_dst_filename);
            }

            if (audio_stream != null) {
                AVSampleFormat sfmt = audio_dec_ctx.sample_fmt;
                int n_channels = audio_dec_ctx.channels;
                string fmt;

                if (sfmt.is_planar) {
                    string packed = sfmt.name;
                    Console.WriteLine("Warning: the sample format the decoder produced is planar "
                           + "({0}). This example will output the first channel only.\n",
                           !string.IsNullOrEmpty(packed) ? packed : "?");
                    sfmt = sfmt.get_packed_sample_fmt();
                    n_channels = 1;
                }

                if ((ret = get_format_from_sample_fmt(out fmt, sfmt, false)) < 0)
                    goto end;

                Console.WriteLine("Play the output audio file with the command:\n"
                       + "ffplay -f {0} -ac {1} -ar {2} {3}",
                       fmt, n_channels, audio_dec_ctx.sample_rate,
                       audio_dst_filename);
            }

            end:

            if (video_dec_ctx != null) video_dec_ctx.Dispose();
            if (audio_dec_ctx != null) audio_dec_ctx.Dispose();

            if (fmt_ctx != null) fmt_ctx.Dispose();

            if (video_dst_file != null)
                video_dst_file.Dispose();
            if (audio_dst_file != null)
                audio_dst_file.Dispose();

            if (frame != null) frame.Dispose();
            if (pkt != null) pkt.Dispose();
            if (video_dst_data != null) video_dst_data.Dispose();
        }
    }
}
