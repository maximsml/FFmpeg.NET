//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// API example for audio decoding and filtering
//////////////////////////////////////////////////////
using System;
using System.Reflection;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("filtering_audio")]
[assembly: AssemblyDescription("C# API example for audio decoding and filtering")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class filtering_audio
    {
        const string filter_descr = "aresample=8000,aformat=sample_fmts=s16:channel_layouts=mono";
        const string player       = "ffplay -f s16le -ar 8000 -ac 1 -";

        static AVFormatContext fmt_ctx = null;
        static AVCodecContext dec_ctx = null;
        static AVFilterContext buffersink_ctx = null;
        static AVFilterContext buffersrc_ctx = null;
        static AVFilterGraph filter_graph = null;
        static int audio_stream_index = -1;

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

            /* select the audio stream */
            ret = fmt_ctx.FindBestStream(AVMediaType.AUDIO,out dec);
            if (ret < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot find a audio stream in the input file\n");
                return ret;
            }
            audio_stream_index = ret;

            /* create decoding context */
            dec_ctx = new AVCodecContext(dec);
            if (dec_ctx == null)
                return AVRESULT.ENOMEM;
            dec_ctx.SetParameters(fmt_ctx.streams[audio_stream_index].codecpar);

            /* init the audio decoder */
            if ((ret = dec_ctx.Open(dec)) < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot open audio decoder\n");
                return ret;
            }
            return 0;
        }

        static int init_filters(string filters_descr)
        {
            int ret = 0;
            AVFilter abuffersrc  = AVFilter.GetByName("abuffer");
            AVFilter abuffersink = AVFilter.GetByName("abuffersink");
            AVFilterInOut outputs = new AVFilterInOut();
            AVFilterInOut inputs  = new AVFilterInOut();

            AVRational time_base = fmt_ctx.streams[audio_stream_index].time_base;
            int[] out_sample_fmts = new[]{ (int)AVSampleFormat.S16 };
            Int64[] out_channel_layouts = new[]{ (Int64)AVChannelLayout.LAYOUT_MONO };
            int[] out_sample_rates = new[]{ 8000 };

            filter_graph = new AVFilterGraph();
            if (!outputs._IsValid || !inputs._IsValid || !filter_graph._IsValid) {
                ret = AVRESULT.ENOMEM;
                goto end;
            }

            /* buffer audio source: the decoded frames from the decoder will be inserted here. */
            if (dec_ctx.channel_layout == AVChannelLayout.UNKNOWN)
                dec_ctx.channel_layout = AVChannelLayout.get_default_channel_layout(dec_ctx.channels);

            string args = string.Format(
                    "time_base={0}/{1}:sample_rate={2}:sample_fmt={3}:channel_layout=0x{4}",
                    time_base.num, time_base.den, dec_ctx.sample_rate,
                    dec_ctx.sample_fmt.name, (int)dec_ctx.channel_layout);

            ret = filter_graph.CreateFilter(out buffersrc_ctx, abuffersrc, "in", args);
            if (ret < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot create audio buffer source\n");
                goto end;
            }

            /* buffer audio sink: to terminate the filter chain. */
            ret = filter_graph.CreateFilter(out buffersink_ctx, abuffersink, "out");
            if (ret < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot create audio buffer sink\n");
                goto end;
            }
            var options = new AVOptions(buffersink_ctx);
            ret = options.set_int_list("sample_fmts", out_sample_fmts, AVOptSearch.CHILDREN);
            if (ret < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot set output sample rate\n");
                goto end;
            }
            ret = options.set_int_list("channel_layouts", out_channel_layouts, AVOptSearch.CHILDREN);
            if (ret < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot set output sample rate\n");
                goto end;
            }
            ret = options.set_int_list("sample_rates", out_sample_rates, AVOptSearch.CHILDREN);
            if (ret < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot set output sample rate\n");
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

            /* Print summary of the sink buffer
             * Note: args buffer is reused to store channel layout string */
            AVFilterLink outlink = buffersink_ctx.inputs[0];
            var s = ((AVSampleFormat)outlink.format).name;
            AVLog.log(AVLogLevel.Info, string.Format("Output: srate:{0}Hz fmt:{1} chlayout:{2}\n",
                   (int)outlink.sample_rate,
                   string.IsNullOrEmpty(s) ? "?" : s,
                   outlink.channel_layout.ToString()));
        end:
            if (inputs != null)  inputs.Dispose();
            if (outputs != null) outputs.Dispose();

            return ret;
        }

        static void print_frame(AVFrame frame)
        {
            int n = frame.nb_samples * frame.channel_layout.channels;
            var p = frame.data[0].shorts;
            for (int i = 0; i < n; i++)
            {
                Console.Write((char)(p[i] & 0xff));
                Console.Write((char)((p[i] >> 8) & 0xff));
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
                Console.WriteLine("usage: {0} file | {1}\n",
                                Assembly.GetExecutingAssembly().GetName().Name + ".exe", player);
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

                if (packet.stream_index == audio_stream_index) {
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

                        if (ret >= 0)
                        {
                            /* push the audio data from decoded frame into the filtergraph */
                            if (source.add_frame(frame, AVBufferSrcFlags.KeepRef) < 0)
                            {
                                AVLog.log(AVLogLevel.Error, "Error while feeding the audio filtergraph\n");
                                break;
                            }
                            /* pull filtered audio from the filtergraph */
                            while (true)
                            {
                                ret = sink.get_frame(filt_frame);
                                if (ret == AVRESULT.EAGAIN || ret == AVRESULT.EOF)
                                    break;
                                if (ret < 0)
                                    goto end;
                                print_frame(filt_frame);
                                filt_frame.Free();
                            }
                            frame.Free();
                        }
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
