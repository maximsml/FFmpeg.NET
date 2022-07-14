//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// API example for decoding and filtering
//////////////////////////////////////////////////////
using System;
using System.Reflection;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("filtering_video")]
[assembly: AssemblyDescription("C# API example for decoding and filtering")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class filtering_video
    {
        const int _XOPEN_SOURCE = 600;

        const string filter_descr = "scale=78:24,transpose=cclock";
        /* other way:
           scale=78:24 [scl]; [scl] transpose=cclock // assumes "[in]" and "[out]" to be input output pads respectively
         */

        static AVFormatContext fmt_ctx = null;
        static AVCodecContext dec_ctx = null;
        static AVFilterContext buffersink_ctx = null;
        static AVFilterContext buffersrc_ctx = null;
        static AVFilterGraph filter_graph = null;
        static int video_stream_index = -1;
        static Int64 last_pts = AVUtil.NOPTS_VALUE;

        static int open_input_file(string filename)
        {
            int ret = 0;
            AVCodec dec;

            if ((ret = AVFormatContext.OpenInput(out fmt_ctx, filename)) < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot open input file\n");
                return ret;
            }

            if ((ret = fmt_ctx.FindStreamInfo()) < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot find stream information\n");
                return ret;
            }

            /* select the video stream */
            ret = fmt_ctx.FindBestStream(AVMediaType.VIDEO,out dec);
            if (ret < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot find a video stream in the input file\n");
                return ret;
            }
            video_stream_index = ret;

            /* create decoding context */
            dec_ctx = new AVCodecContext(dec);
            if (dec_ctx == null)
                return AVRESULT.ENOMEM;
            dec_ctx.SetParameters(fmt_ctx.streams[video_stream_index].codecpar);

            /* init the video decoder */
            if ((ret = dec_ctx.Open(dec)) < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot open video decoder\n");
                return ret;
            }
            return 0;
        }

        static int init_filters(string filters_descr)
        {
            int ret = 0;
            AVFilter buffersrc  = AVFilter.GetByName("buffer");
            AVFilter buffersink = AVFilter.GetByName("buffersink");
            AVFilterInOut outputs = new AVFilterInOut();
            AVFilterInOut inputs  = new AVFilterInOut();
            AVRational time_base = fmt_ctx.streams[video_stream_index].time_base;
            int[] pix_fmts = new[] { (int)AVPixelFormat.GRAY8 };

            filter_graph = new AVFilterGraph();
            if (!outputs._IsValid || !inputs._IsValid || !filter_graph._IsValid) {
                ret = AVRESULT.ENOMEM;
                goto end;
            }

            /* buffer video source: the decoded frames from the decoder will be inserted here. */
            string args = string.Format(
                    "video_size={0}x{1}:pix_fmt={2}:time_base={3}/{4}:pixel_aspect={5}/{6}",
                    dec_ctx.width, dec_ctx.height, (int)dec_ctx.pix_fmt,
                    time_base.num, time_base.den,
                    dec_ctx.sample_aspect_ratio.num, dec_ctx.sample_aspect_ratio.den);

            ret = filter_graph.CreateFilter(out buffersrc_ctx, buffersrc, "in", args);
            if (ret < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot create buffer source\n");
                goto end;
            }

            /* buffer video sink: to terminate the filter chain. */
            ret = filter_graph.CreateFilter(out buffersink_ctx, buffersink, "out");
            if (ret < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot create buffer sink\n");
                goto end;
            }
            var options = new AVOptions(buffersink_ctx);
            ret = options.set_int_list("pix_fmts", pix_fmts, AVOptSearch.CHILDREN);
            if (ret < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot set output pixel format\n");
                goto end;
            }

            /*
             * Set the endpoints for the filter graph. The filter_graph will
             * be linked to the graph described by filters_descr.
             */

            /*
             * The buffer source output must be connected to the input pad of
             * the first filter described by filters_descr; since the first
             * filter input label is not specified, it is set to "in" by
             * default.
             */
            outputs.name       = "in";
            outputs.filter_ctx = buffersrc_ctx;
            outputs.pad_idx    = 0;
            outputs.next       = null;

            /*
             * The buffer sink input must be connected to the output pad of
             * the last filter described by filters_descr; since the last
             * filter output label is not specified, it is set to "out" by
             * default.
             */
            inputs.name       = "out";
            inputs.filter_ctx = buffersink_ctx;
            inputs.pad_idx    = 0;
            inputs.next       = null;

            if ((ret = filter_graph.ParsePtr(filters_descr,inputs, outputs)) < 0)
                goto end;

            if ((ret = filter_graph.Config()) < 0)
                goto end;

        end:
            if (buffersrc != null)  buffersrc.Dispose();
            if (buffersink != null) buffersink.Dispose();

            if (inputs != null)  inputs.Dispose();
            if (outputs != null) outputs.Dispose();

            return ret;
        }

        static void display_frame(AVFrame frame, AVRational time_base)
        {
            int x, y;
            Int64 delay;

            if (frame.pts != AVUtil.NOPTS_VALUE) {
                if (last_pts != AVUtil.NOPTS_VALUE) {
                    /* sleep roughly the right amount of time;
                     * usleep is in microseconds, just like AV_TIME_BASE. */
                    delay = AVMath.rescale_q(frame.pts - last_pts,
                                         time_base, AVMath.TIME_BASE_Q);
                    if (delay > 0 && delay < 1000000)
                        AVUtil.usleep((uint)delay);
                }
                last_pts = frame.pts;
            }

            /* Trivial ASCII grayscale display. */
            var p0 = frame.data[0];
            Console.Write("\033c");
            for (y = 0; y < frame.height; y++) {
                var p = p0;
                string s = " .-+#";
                for (x = 0; x < frame.width; x++)
                    Console.Write(s[p[x] / 52]);
                Console.Write('\n');
                p0 += frame.linesize[0];
            }
            Console.Out.Flush();
            
        }

        static void Main(string[] args)
        {
            int ret;
            AVPacket packet = new AVPacket();
            AVFrame frame = new AVFrame();
            AVFrame filt_frame = new AVFrame();

            if (args.Length != 1)
            {
                Console.WriteLine("usage: {0} file\n",
                                Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            Console.CancelKeyPress += Console_CancelKeyPress;

            if ((ret = open_input_file(args[0])) < 0)
                goto end;
            if ((ret = init_filters(filter_descr)) < 0)
                goto end;

            AVBufferSink sink = new AVBufferSink(buffersink_ctx);
            AVBufferSrc source = new AVBufferSrc(buffersrc_ctx);

            /* read all packets */
            while (true) {
                if ((ret = fmt_ctx.ReadFrame(packet)) < 0)
                    break;

                if (packet.stream_index == video_stream_index) {
                    ret = dec_ctx.SendPacket(packet);
                    if (ret < 0) {
                        AVLog.log(AVLogLevel.Error, "Error while sending a packet to the decoder\n");
                        break;
                    }

                    while (ret >= 0) {
                        ret = dec_ctx.ReceiveFrame(frame);
                        if (ret == AVRESULT.EAGAIN || ret == AVRESULT.EOF) {
                            break;
                        } else if (ret < 0) {
                            AVLog.log(AVLogLevel.Error, "Error while receiving a frame from the decoder\n");
                            goto end;
                        }

                        frame.pts = frame.best_effort_timestamp;

                        /* push the decoded frame into the filtergraph */
                        if (source.add_frame(frame, AVBufferSrcFlags.KeepRef) < 0) {
                            AVLog.log(AVLogLevel.Error, "Error while feeding the filtergraph\n");
                            break;
                        }

                        /* pull filtered frames from the filtergraph */
                        while (true) {
                            ret = sink.get_frame(filt_frame);
                            if (ret == AVRESULT.EAGAIN || ret == AVRESULT.EOF)
                                break;
                            if (ret < 0)
                                goto end;
                            display_frame(filt_frame, buffersink_ctx.inputs[0].time_base);
                            filt_frame.Free();
                        }
                        frame.Free();
                    }
                }
                packet.Free();
                /* Check For Quit */
                if (Console.KeyAvailable)
                {
                    var ki = Console.ReadKey();
                    if (ki.Key == ConsoleKey.Escape)
                    {
                        break;
                    }
                }
                if (s_bQuit)
                {
                    break;
                }
            }

            end:

            if (filter_graph != null) filter_graph.Dispose();
            if (dec_ctx != null) dec_ctx.Dispose();
            if (fmt_ctx != null) fmt_ctx.Dispose();

            if (packet != null) packet.Dispose();
            if (frame != null) frame.Dispose();
            if (filt_frame != null) filt_frame.Dispose();

            if (ret < 0 && ret != AVRESULT.EOF)
            {
                Console.WriteLine("Error occurred: {0}\n", (AVRESULT)ret);
                Environment.Exit(1);
            }

            Environment.Exit(0);
        }

        static bool s_bQuit = false;

        private static void Console_CancelKeyPress(object sender, ConsoleCancelEventArgs e)
        {
            s_bQuit = true;
        }
    }
}
