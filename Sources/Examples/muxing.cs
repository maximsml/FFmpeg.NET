//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// libavformat API example.
// Output a media file in any supported libavformat format. The default
// codecs are used.
//////////////////////////////////////////////////////
using System;
using System.Reflection;
using System.Diagnostics;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("muxing")]
[assembly: AssemblyDescription("C# libavformat API example."
                    + " Output a media file in any supported libavformat format.")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class muxing
    {
        const long STREAM_DURATION = 10;
        const int STREAM_FRAME_RATE = 25; /* 25 images/s */
        static AVPixelFormat STREAM_PIX_FMT = AVPixelFormat.YUV420P; /* default pix_fmt */

        // a wrapper around a single output AVStream
        public class OutputStream
        {
            public AVStream st;
            public AVCodecContext enc;

            /* pts of the next frame that will be generated */
            public long next_pts;
            public int samples_count;

            public AVFrame frame;
            public AVFrame tmp_frame;

            public float t, tincr, tincr2;

            public SwsContext sws_ctx;
            public SwrContext swr_ctx;
        }

        static void log_packet(AVFormatContext fmt_ctx, AVPacket pkt)
        {
            AVRational time_base = fmt_ctx.streams[pkt.stream_index].time_base;

            Console.WriteLine("pts:{0} pts_time:{1} dts:{2} dts_time:{3} duration:{4} duration_time:{5} stream_index:{6}",
                   AVUtil.ts2timestr(pkt.pts), AVUtil.ts2timestr(pkt.pts, time_base),
                   AVUtil.ts2timestr(pkt.dts), AVUtil.ts2timestr(pkt.dts, time_base),
                   AVUtil.ts2timestr(pkt.duration), AVUtil.ts2timestr(pkt.duration, time_base),
                   pkt.stream_index);
        }

        static AVRESULT write_frame(AVFormatContext fmt_ctx, AVRational time_base, AVStream st, AVPacket pkt)
        {
            /* rescale output packet timestamp values from codec to stream timebase */
            pkt.RescaleTS(time_base, st.time_base);
            
            pkt.stream_index = st.index;

            /* Write the compressed frame to the media file. */
            log_packet(fmt_ctx, pkt);
            return fmt_ctx.InterleavedWriteFrame(pkt);
        }

        /* Add an output stream. */
        static void add_stream(OutputStream ost, AVFormatContext oc, 
            out AVCodec codec, 
            AVCodecID codec_id)
        {
            AVCodecContext c;
            int i;

            /* find the encoder */
            codec = AVCodec.FindEncoder(codec_id);
            if (codec == null) {
                Console.WriteLine("Could not find encoder for '{0}'\n",
                    AVCodec.GetCodecName(codec_id));
                Environment.Exit(1);
            }

            ost.st = oc.AddStream();
            if (ost.st == null) {
                Console.WriteLine("Could not allocate stream\n");
                Environment.Exit(1);
            }
            ost.st.id = oc.nb_streams-1;

            c = new AVCodecContext(codec);
            if (!c._IsValid) {
                Console.WriteLine("Could not alloc an encoding context\n");
                Environment.Exit(1);
            }
            ost.enc = c;

            switch (codec.type) {
            case AVMediaType.AUDIO:
                c.sample_fmt = codec.sample_fmts != null && codec.sample_fmts.Length > 0?
                    codec.sample_fmts[0] : AVSampleFormat.FLTP;
                c.bit_rate = 64000;
                c.sample_rate = 44100;
                if (codec.supported_samplerates != null && codec.supported_samplerates.Length > 0) {
                    c.sample_rate = codec.supported_samplerates[0];
                    for (i = 0; i < codec.supported_samplerates.Length; i++) {
                        if (codec.supported_samplerates[i] == 44100)
                            c.sample_rate = 44100;
                    }
                }
                c.channels = c.channel_layout.channels;
                c.channel_layout = AVChannelLayout.LAYOUT_STEREO;
                if (codec.channel_layouts != null && codec.channel_layouts.Length > 0) {
                    c.channel_layout = codec.channel_layouts[0];
                    for (i = 0; i < codec.channel_layouts.Length; i++) {
                        if (codec.channel_layouts[i] == AVChannelLayout.LAYOUT_STEREO)
                            c.channel_layout = AVChannelLayout.LAYOUT_STEREO;
                    }
                }
                c.channels = c.channel_layout.channels;
                ost.st.time_base = new AVRational( 1, c.sample_rate );
                break;
            case AVMediaType.VIDEO:
                c.codec_id = codec_id;

                c.bit_rate = 400000;
                /* Resolution must be a multiple of two. */
                c.width    = 352;
                c.height   = 288;
                /* timebase: This is the fundamental unit of time (in seconds) in terms
                 * of which frame timestamps are represented. For fixed-fps content,
                 * timebase should be 1/framerate and timestamp increments should be
                 * identical to 1. */
                ost.st.time_base  = new AVRational( 1, STREAM_FRAME_RATE );
                c.time_base       = ost.st.time_base;

                c.gop_size      = 12; /* emit one intra frame every twelve frames at most */
                c.pix_fmt       = STREAM_PIX_FMT;
                if (c.codec_id == AVCodecID.MPEG2VIDEO) {
                    /* just for testing, we also add B-frames */
                    c.max_b_frames = 2;
                }
                if (c.codec_id == AVCodecID.MPEG1VIDEO) {
                    /* Needed to avoid using macroblocks in which some coeffs overflow.
                     * This does not happen with normal video, it just happens here as
                     * the motion of the chroma plane does not match the luma plane. */
                    c.mb_decision = (FFMbDecision)2;
                }
                break;
            default:
                break;
            }

            /* Some formats want stream headers to be separate. */
            if ((oc.oformat.flags & AVfmt.GLOBALHEADER) != 0)
                c.flags |= AVCodecFlag.GLOBAL_HEADER;
        }

        /**************************************************************/
        /* audio output */

        static AVFrame alloc_audio_frame(AVSampleFormat sample_fmt,
                                          AVChannelLayout channel_layout,
                                          int sample_rate, int nb_samples)
        {
            AVFrame frame = new AVFrame();
            int ret;

            if (!frame._IsValid) {
                Console.WriteLine("Error allocating an audio frame\n");
                Environment.Exit(1);
            }

            frame.format = (int)sample_fmt;
            frame.channel_layout = channel_layout;
            frame.sample_rate = sample_rate;
            frame.nb_samples = nb_samples;

            if (nb_samples > 0) {
                ret = frame.AllocateBuffers(0);
                if (ret < 0) {
                    Console.WriteLine("Error allocating an audio buffer\n");
                    Environment.Exit(1);
                }
            }

            return frame;
        }

        static void open_audio(AVFormatContext oc, AVCodec codec, OutputStream ost, AVDictionary opt_arg)
        {
            int nb_samples;
            int ret;
            AVCodecContext c = ost.enc;
            AVDictionary opt = opt_arg != null ? new AVDictionary(opt_arg,AVDictFlags.NONE) : null;

            /* open it */
            ret = c.Open(codec, opt);
            if (opt != null) opt.Dispose();
            if (ret < 0) {
                Console.WriteLine("Could not open audio codec: {0}\n", (AVRESULT)(ret));
                Environment.Exit(1);
            }

            /* init signal generator */
            ost.t     = 0;
            ost.tincr = (float)(2 * Math.PI * 110.0 / c.sample_rate);
            /* increment frequency by 110 Hz per second */
            ost.tincr2 = (float)(2 * Math.PI * 110.0 / c.sample_rate / c.sample_rate);

            if ((c.codec.capabilities & AVCodecCap.VARIABLE_FRAME_SIZE) != 0)
                nb_samples = 10000;
            else
                nb_samples = c.frame_size;

            ost.frame     = alloc_audio_frame(c.sample_fmt, c.channel_layout,
                                               c.sample_rate, nb_samples);
            ost.tmp_frame = alloc_audio_frame( AVSampleFormat.S16, c.channel_layout,
                                               c.sample_rate, nb_samples);

            /* copy the stream parameters to the muxer */
            ret = ost.st.codecpar.FromContext(c);
            if (ret < 0) {
                Console.WriteLine("Could not copy the stream parameters\n");
                Environment.Exit(1);
            }

            /* create resampler context */
            ost.swr_ctx = new SwrContext();
            if (!ost.swr_ctx._IsValid) {
                Console.WriteLine("Could not allocate resampler context\n");
                Environment.Exit(1);
            }

            /* set options */
            var options = new AVOptions(ost.swr_ctx);
            options.set_int       ("in_channel_count",   c.channels,        0);
            options.set_int       ("in_sample_rate",     c.sample_rate,     0);
            options.set_sample_fmt("in_sample_fmt",      AVSampleFormat.S16,0);
            options.set_int       ("out_channel_count",  c.channels,        0);
            options.set_int       ("out_sample_rate",    c.sample_rate,     0);
            options.set_sample_fmt("out_sample_fmt",     c.sample_fmt,      0);

            /* initialize the resampling context */
            if ((ret = ost.swr_ctx.Init()) < 0) {
                Console.WriteLine("Failed to initialize the resampling context\n");
                Environment.Exit(1);
            }
        }

        /* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
         * 'nb_channels' channels. */
        static AVFrame get_audio_frame(OutputStream ost)
        {
            AVFrame frame = ost.tmp_frame;
            int j, i, v;
            var q = frame.data[0].shorts;
            int idx = 0;

            /* check if we want to generate more frames */
            if (AVMath.compare_ts(ost.next_pts, ost.enc.time_base,
                              STREAM_DURATION, new AVRational(1, 1)) >= 0)
                return null;

            for (j = 0; j <frame.nb_samples; j++) {
                v = (int)(Math.Sin(ost.t) * 10000);
                for (i = 0; i < ost.enc.channels; i++)
                    q[idx++] = (short)v;
                ost.t     += ost.tincr;
                ost.tincr += ost.tincr2;
            }

            frame.pts = ost.next_pts;
            ost.next_pts  += frame.nb_samples;

            return frame;
        }

        /*
         * encode one audio frame and send it to the muxer
         * return 1 when encoding is finished, 0 otherwise
         */
        static int write_audio_frame(AVFormatContext oc, OutputStream ost)
        {
            AVPacket pkt = new AVPacket();
            int ret;
            bool got_packet = false;
            int dst_nb_samples;

            AVCodecContext c = ost.enc;
            AVFrame frame = get_audio_frame(ost);

            if (frame != null) {
                /* convert samples from native format to destination codec format, using the resampler */
                /* compute destination number of samples */
                dst_nb_samples = (int)AVMath.rescale_rnd(ost.swr_ctx.GetDelay(c.sample_rate) + frame.nb_samples,
                                                    c.sample_rate, c.sample_rate, AVRounding.UP);
                Debug.Assert(dst_nb_samples == frame.nb_samples);

                /* when we pass a frame to the encoder, it may keep a reference to it
                 * internally;
                 * make sure we do not overwrite it here
                 */
                if (!ost.frame.MakeWritable())
                    Environment.Exit(1);

                /* convert to destination format */
                ret = ost.swr_ctx.Convert(ost.frame.data, dst_nb_samples,
                                  frame.data, frame.nb_samples);
                if (ret < 0) {
                    Console.WriteLine("Error while converting\n");
                    Environment.Exit(1);
                }
                frame = ost.frame;

                frame.pts = AVMath.rescale_q(ost.samples_count, new AVRational(1, c.sample_rate), c.time_base);
                ost.samples_count += dst_nb_samples;
            }

            ret = c.EncodeAudio(pkt, frame, ref got_packet);
            if (ret < 0) {
                Console.WriteLine("Error encoding audio frame: {0}\n", (AVRESULT)(ret));
                Environment.Exit(1);
            }

            if (got_packet) {
                ret = write_frame(oc, c.time_base, ost.st, pkt);
                if (ret < 0) {
                    Console.WriteLine("Error while writing audio frame: {0}\n",
                            (AVRESULT)(ret));
                    Environment.Exit(1);
                }
            }

            return (frame != null || got_packet) ? 0 : 1;
        }

        /**************************************************************/
        /* video output */

        static AVFrame alloc_picture(AVPixelFormat pix_fmt, int width, int height)
        {
            AVFrame picture = new AVFrame();
            if (picture == null || !picture._IsValid)
                return null;

            picture.format = (int)pix_fmt;
            picture.width  = width;
            picture.height = height;

            /* allocate the buffers for the frame data */
            int ret = picture.AllocateBuffers(32);
            if (ret < 0) {
                Console.WriteLine("Could not allocate frame data.\n");
                Environment.Exit(1);
            }

            return picture;
        }

        static void open_video(AVFormatContext oc, AVCodec codec, OutputStream ost, AVDictionary opt_arg)
        {
            int ret;
            AVCodecContext c = ost.enc;
            AVDictionary opt = opt_arg != null ? new AVDictionary(opt_arg,AVDictFlags.NONE) : null;

            /* open the codec */
            ret = c.Open(codec, opt);
            if (opt != null) opt.Dispose();
            if (ret < 0) {
                Console.WriteLine("Could not open video codec: {0}\n", (AVRESULT)(ret));
                Environment.Exit(1);
            }

            /* allocate and init a re-usable frame */
            ost.frame = alloc_picture(c.pix_fmt, c.width, c.height);
            if (!ost.frame._IsValid) {
                Console.WriteLine("Could not allocate video frame\n");
                Environment.Exit(1);
            }

            /* If the output format is not YUV420P, then a temporary YUV420P
             * picture is needed too. It is then converted to the required
             * output format. */
            ost.tmp_frame = null;
            if (c.pix_fmt != AVPixelFormat.YUV420P) {
                ost.tmp_frame = alloc_picture(AVPixelFormat.YUV420P, c.width, c.height);
                if (!ost.tmp_frame._IsValid) {
                    Console.WriteLine("Could not allocate temporary picture\n");
                    Environment.Exit(1);
                }
            }

            /* copy the stream parameters to the muxer */
            //ret = ost.st.codecpar.CopyTo(c);
            ret = ost.st.codecpar.FromContext(c);
            if (ret < 0) {
                Console.WriteLine("Could not copy the stream parameters\n");
                Environment.Exit(1);
            }
        }

        /* Prepare a dummy image. */
        static void fill_yuv_image(AVFrame pict, int frame_index,
                                   int width, int height)
        {
            int x, y, i;

            i = frame_index;

            /* Y */
            for (y = 0; y < height; y++)
                for (x = 0; x < width; x++)
                    pict.data[0][y * pict.linesize[0] + x] = (byte)(x + y + i * 3);

            /* Cb and Cr */
            for (y = 0; y < height / 2; y++) {
                for (x = 0; x < width / 2; x++) {
                    pict.data[1][y * pict.linesize[1] + x] = (byte)(128 + y + i * 2);
                    pict.data[2][y * pict.linesize[2] + x] = (byte)(64 + x + i * 5);
                }
            }
        }

        static AVFrame get_video_frame(OutputStream ost)
        {
            AVCodecContext c = ost.enc;

            /* check if we want to generate more frames */
            if (AVMath.compare_ts(ost.next_pts, c.time_base,
                              STREAM_DURATION, new AVRational( 1, 1 )) >= 0)
                return null;

            /* when we pass a frame to the encoder, it may keep a reference to it
             * internally; make sure we do not overwrite it here */
            if (!ost.frame.MakeWritable())
                Environment.Exit(1);

            if (c.pix_fmt != AVPixelFormat.YUV420P) {
                /* as we only generate a YUV420P picture, we must convert it
                 * to the codec pixel format if needed */
                if (ost.sws_ctx == null) {
                    ost.sws_ctx = new SwsContext(c.width, c.height,
                                                  AVPixelFormat.YUV420P,
                                                  c.width, c.height,
                                                  c.pix_fmt,
                                                  SwsFlags.Bicubic);
                    if (ost.sws_ctx == null) {
                        Console.WriteLine(
                                "Could not initialize the conversion context\n");
                        Environment.Exit(1);
                    }
                }
                fill_yuv_image(ost.tmp_frame, (int)ost.next_pts, c.width, c.height);
                ost.sws_ctx.Scale(ost.tmp_frame, 0, c.height, ost.frame);
            } else {
                fill_yuv_image(ost.frame, (int)ost.next_pts, c.width, c.height);
            }

            ost.frame.pts = ost.next_pts++;

            return ost.frame;
        }

        /*
        * encode one video frame and send it to the muxer
        * return 1 when encoding is finished, 0 otherwise
        */
        static int write_video_frame(AVFormatContext oc, OutputStream ost)
        {
            int ret;
            bool got_packet = false;
            AVPacket pkt = new AVPacket();
            AVCodecContext c = ost.enc;
            AVFrame frame = get_video_frame(ost);

            /* encode the image */
            ret = c.EncodeVideo(pkt, frame, ref got_packet);
            if (ret < 0) {
                Console.WriteLine("Error encoding video frame: {0}\n", (AVRESULT)(ret));
                Environment.Exit(1);
            }

            if (got_packet) {
                ret = write_frame(oc, c.time_base, ost.st, pkt);
            } else {
                ret = 0;
            }

            if (ret < 0) {
                Console.WriteLine("Error while writing video frame: {0}\n", (AVRESULT)(ret));
                Environment.Exit(1);
            }
            pkt.Dispose();
            return (frame != null || got_packet) ? 0 : 1;
        }

        static void close_stream(AVFormatContext oc, OutputStream ost)
        {
            if (ost.enc != null) ost.enc.Dispose();
            if (ost.frame != null) ost.frame.Dispose();
            if (ost.tmp_frame != null) ost.tmp_frame.Dispose();
            if (ost.sws_ctx != null) ost.sws_ctx.Dispose();
            if (ost.swr_ctx != null) ost.swr_ctx.Dispose();
        }

        /**************************************************************/
        /* media file output */

        static void Main(string[] args)
        {
            OutputStream video_st = new OutputStream();
            OutputStream audio_st = new OutputStream();
            string filename;
            AVOutputFormat fmt = null;
            AVFormatContext oc = null;
            AVCodec audio_codec = null, video_codec = null;
            bool have_video = false, have_audio = false;
            bool encode_video = false, encode_audio = false;
            AVDictionary opt = new AVDictionary();

            if (args.Length < 1)
            {
                Console.WriteLine("usage: {0} output_file\n"
                                + "API example program to output a media file with libavformat.\n"
                                + "This program generates a synthetic audio and video stream, encodes and\n"
                                + "muxes them into a file named output_file.\n"
                                + "The output format is automatically guessed according to the file extension.\n"
                                + "Raw images can also be output by using '%%d' in the filename.\n"
                                , Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }
            filename  = args[0];

            for (int i = 1; i+1 < args.Length; i+=2) {
                if ((args[i].ToLower() == "-flags") || (args[i].ToLower() == "-fflags"))
                {
                    opt.SetValue(args[i] + 1, args[i + 1]);
                }
            }

            /* allocate the output media context */
            oc = AVFormatContext.OpenOutput(filename);
            if (oc == null) {
                Console.WriteLine("Could not deduce output format from file extension: using MPEG.\n");
                oc = AVFormatContext.OpenOutput("mpeg",filename);
            }
            if (oc == null) {
                Environment.Exit(1);
            }

            fmt = oc.oformat;

            /* Add the audio and video streams using the default format codecs
            * and initialize the codecs. */
            if (fmt.video_codec != AVCodecID.NONE) {
                add_stream(video_st, oc,out video_codec, fmt.video_codec);
                have_video = true;
                encode_video = true;
            }
            if (fmt.audio_codec != AVCodecID.NONE) {
                add_stream(audio_st, oc, out audio_codec, fmt.audio_codec);
                have_audio = true;
                encode_audio = true;
            }
            
            /* Now that all the parameters are set, we can open the audio and
             * video codecs and allocate the necessary encode buffers. */
            if (have_video)
                open_video(oc, video_codec, video_st, opt);

            if (have_audio)
                open_audio(oc, audio_codec, audio_st, opt);

            oc.DumpFormat(0, filename, true);

            if ((int)(fmt.flags & AVfmt.NOFILE) == 0) {
                oc.pb = new AVIOContext(filename,AvioFlag.WRITE);
                if (!oc.pb._IsValid) {
                    Console.WriteLine("Could not open output file '{0}'", filename);
                    Environment.Exit(1);
                }
            }

            /* Write the stream header, if any. */
            int ret = oc.WriteHeader(opt);
            if (ret < 0) {
                Console.WriteLine("Error occurred when opening output file: {0}\n",(AVRESULT)ret);
                Environment.Exit(1);
            }
            
            while (encode_video || encode_audio) {
                /* select the stream to encode */
                if (encode_video &&
                    (!encode_audio || AVMath.compare_ts(video_st.next_pts, video_st.enc.time_base,
                                                    audio_st.next_pts, audio_st.enc.time_base) <= 0)) {
                    encode_video = (0 == write_video_frame(oc, video_st));
                } else {
                    encode_audio = (0 == write_audio_frame(oc, audio_st));
                }
            }

            /* Write the trailer, if any. The trailer must be written before you
             * close the CodecContexts open when you wrote the header; otherwise
             * av_write_trailer() may try to use memory that was freed on
             * av_codec_close(). */
            oc.WriteTrailer();

            /* Close each codec. */
            if (have_video)
                close_stream(oc, video_st);
            if (have_audio)
                close_stream(oc, audio_st);

            /* Close the output file. */
            if (oc != null && (int)(fmt.flags & AVfmt.NOFILE) == 0) {
              if (oc.pb != null)
                {
                    oc.pb.Dispose();
                }
            }
            /* free the stream */
            if (oc != null) oc.Dispose();

            Environment.Exit(0);
        }
    }
}
