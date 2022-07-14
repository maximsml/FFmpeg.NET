//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
 // libavfilter API usage example.
 //
 // @example filter_audio.c
 // This example will generate a sine wave audio,
 // pass it through a simple filter chain, and then compute the MD5 checksum of
 // the output data.
 //
 // The filter chain it uses is:
 // (input) -> abuffer -> volume -> aformat -> abuffersink -> (output)
 //
 // abuffer: This provides the endpoint where you can feed the decoded samples.
 // volume: In this example we hardcode it to 0.90.
 // aformat: This converts the samples to the samplefreq, channel layout,
 //          and sample format required by the audio device.
 // abuffersink: This provides the endpoint where you can read the samples after
 //              they have passed through the filter chain.
//////////////////////////////////////////////////////
using System;
using System.Reflection;
using System.Globalization;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("filter_audio")]
[assembly: AssemblyDescription("C# libavfilter API usage example."
            +"This example will generate a sine wave audio,"
            +"pass it through a simple filter chain, and then compute the MD5 checksum of"
            +"the output data.")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class filter_audio
    {
        const int INPUT_SAMPLERATE = 48000;
        static AVSampleFormat INPUT_FORMAT = AVSampleFormat.FLTP;
        static AVChannelLayout INPUT_CHANNEL_LAYOUT = AVChannelLayout.LAYOUT_5POINT0;
        const int FRAME_SIZE = 1024;
        const float VOLUME_VAL = 0.90f;

        static int init_filter_graph(out AVFilterGraph filter_graph, out AVFilterContext abuffer_ctx,
                             out AVFilterContext abuffersink_ctx)
        {
            filter_graph = null;
            abuffer_ctx = null;
            abuffersink_ctx = null;

            AVFilter abuffer = null;
            AVFilterContext volume_ctx = null;
            AVFilter volume = null;
            AVFilterContext aformat_ctx = null;
            AVFilter aformat = null;
            AVFilter abuffersink = null;

            int err;

            /* Create a new filtergraph, which will contain all the filters. */
            filter_graph = new AVFilterGraph();
            if (!filter_graph._IsValid) {
                Console.Write("Unable to create filter graph.\n");
                return AVRESULT.ENOMEM;
            }

            /* Create the abuffer filter;
             * it will be used for feeding the data into the graph. */
            abuffer = AVFilter.GetByName("abuffer");
            if (abuffer == null) {
                Console.Write("Could not find the abuffer filter.\n");
                return AVRESULT.FILTER_NOT_FOUND;
            }

            abuffer_ctx = filter_graph.AllocFilter(abuffer, "src");
            if (abuffer_ctx == null) {
                Console.Write("Could not allocate the abuffer instance.\n");
                return AVRESULT.ENOMEM;
            }

            /* Set the filter options through the AVOptions API. */
            AVOptions options = new AVOptions(abuffer_ctx);
            options.set("channel_layout", INPUT_CHANNEL_LAYOUT.ToString(), AVOptSearch.CHILDREN);
            options.set("sample_fmt", INPUT_FORMAT.name, AVOptSearch.CHILDREN);
            options.set_q("time_base", new AVRational( 1, INPUT_SAMPLERATE ),  AVOptSearch.CHILDREN);
            options.set_int("sample_rate", INPUT_SAMPLERATE, AVOptSearch.CHILDREN);

            /* Now initialize the filter; we pass NULL options, since we have already
             * set all the options above. */
            err = abuffer_ctx.Init();
            if (err < 0) {
                Console.Write("Could not initialize the abuffer filter.\n");
                return err;
            }

            /* Create volume filter. */
            volume = AVFilter.GetByName("volume");
            if (volume == null) {
                Console.Write("Could not find the volume filter.\n");
                return AVRESULT.FILTER_NOT_FOUND;
            }

            volume_ctx = filter_graph.AllocFilter(volume, "volume");
            if (volume_ctx == null) {
                Console.Write("Could not allocate the volume instance.\n");
                return AVRESULT.ENOMEM;
            }

            /* A different way of passing the options is as key/value pairs in a
             * dictionary. */
            AVDictionary options_dict = new AVDictionary();
            options_dict.SetValue("volume",VOLUME_VAL.ToString("F1",CultureInfo.InvariantCulture),AVDictFlags.NONE);
            err = volume_ctx.Init(options_dict);
            options_dict.Dispose();
            if (err < 0) {
                Console.Write("Could not initialize the volume filter.\n");
                return err;
            }

            /* Create the aformat filter;
             * it ensures that the output is of the format we want. */
            aformat = AVFilter.GetByName("aformat");
            if (aformat == null) {
                Console.Write("Could not find the aformat filter.\n");
                return AVRESULT.FILTER_NOT_FOUND;
            }

            aformat_ctx = filter_graph.AllocFilter(aformat, "aformat");
            if (aformat_ctx == null) {
                Console.Write("Could not allocate the aformat instance.\n");
                return AVRESULT.ENOMEM;
            }

            /* A third way of passing the options is in a string of the form
             * key1=value1:key2=value2.... */
            err = aformat_ctx.Init(string.Format("sample_fmts={0}:sample_rates={1}:channel_layouts=0x{2}",
                     AVSampleFormat.S16.name, 44100,(long)AVChannelLayout.LAYOUT_STEREO));
            if (err < 0) {
                AVLog.log(AVLogLevel.Error, "Could not initialize the aformat filter.\n");
                return err;
            }

            /* Finally create the abuffersink filter;
             * it will be used to get the filtered data out of the graph. */
            abuffersink = AVFilter.GetByName("abuffersink");
            if (abuffersink == null) {
                Console.Write("Could not find the abuffersink filter.\n");
                return AVRESULT.FILTER_NOT_FOUND;
            }

            abuffersink_ctx = filter_graph.AllocFilter(abuffersink, "sink");
            if (abuffersink_ctx == null) {
                Console.Write("Could not allocate the abuffersink instance.\n");
                return AVRESULT.ENOMEM;
            }

            /* This filter takes no options. */
            err = abuffersink_ctx.Init();
            if (err < 0) {
                Console.Write("Could not initialize the abuffersink instance.\n");
                return err;
            }

            /* Connect the filters;
             * in this simple case the filters just form a linear chain. */
            err = abuffer_ctx.Link(volume_ctx);
            if (err >= 0)
                err = volume_ctx.Link(aformat_ctx);
            if (err >= 0)
                err = aformat_ctx.Link(abuffersink_ctx);
            if (err < 0) {
                Console.Write("Error connecting filters\n");
                return err;
            }

            /* Configure the graph. */
            err = filter_graph.Config();
            if (err < 0) {
                AVLog.log(AVLogLevel.Error, "Error configuring the filter graph\n");
                return err;
            }
            return 0;
        }

        /* Do something useful with the filtered data: this simple
        * example just prints the MD5 checksum of each plane to stdout. */
        static int process_output(AVMD5 md5, AVFrame frame)
        {
            bool planar    = ((AVSampleFormat)frame.format).is_planar;
            int channels   = frame.channel_layout.channels;
            int planes     = planar ? channels : 1;
            int bps        = ((AVSampleFormat)frame.format).bytes_per_sample;
            int plane_size = bps * frame.nb_samples * (planar ? 1 : channels);

            for (int i = 0; i < planes; i++) {
                byte[] checksum = null;

                md5.Reset();
                md5.Update(frame.data[i], plane_size);
                md5.Final(out checksum);
                
                Console.Write("plane {0}: 0x", i);
                for (int j = 0; j < checksum.Length; j++)
                    Console.Write("{0:X2}", checksum[j]);
                Console.Write("\n");
            }
            Console.Write("\n");
            return 0;
        }

        /* Construct a frame of audio data to be filtered;
        * this simple example just synthesizes a sine wave. */
        static int get_input(AVFrame frame, int frame_num)
        {
            /* Set up the frame properties and allocate the buffer for the data. */
            frame.sample_rate    = INPUT_SAMPLERATE;
            frame.format         = (int)INPUT_FORMAT;
            frame.channel_layout = INPUT_CHANNEL_LAYOUT;
            frame.nb_samples     = FRAME_SIZE;
            frame.pts            = frame_num * FRAME_SIZE;

            int err = frame.AllocateBuffers();
            if (err < 0)
                return err;

            /* Fill the data for each channel. */
            for (int i = 0; i < 5; i++) {
                var data = frame.data[i].floats;
                for (int j = 0; j < frame.nb_samples; j++)
                    data[j] = (float)(Math.Sin(2 * Math.PI * (frame_num + j) * (i + 1) / FRAME_SIZE));
            }

            return 0;
        }

        static void Main(string[] args)
        {
            AVFilterGraph graph = null;
            AVFilterContext src = null;
            AVFilterContext sink = null;
            int err = 0;
            AVMD5 md5 = null;
            AVFrame frame = null;

            if (args.Length < 1)
            {
                Console.WriteLine("usage: {0} <duration>\n"
                                ,Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }
            Console.CancelKeyPress += Console_CancelKeyPress;

            float duration;
            float.TryParse(args[0], out duration);
            int nb_frames = (int)(duration * INPUT_SAMPLERATE / FRAME_SIZE);
            if (nb_frames <= 0) {
                Console.WriteLine("Invalid duration: %s\n", args[0]);
                Environment.Exit(1);
            }

            /* Allocate the frame we will be using to store the data. */
            frame = new AVFrame();
            if (!frame._IsValid) {
                Console.WriteLine("Error allocating the frame\n");
                Environment.Exit(1);
            }

            md5 = new AVMD5();
            if (!md5._IsValid) {
                Console.WriteLine("Error allocating the MD5 context\n");
                Environment.Exit(1);
            }

            /* Set up the filtergraph. */
            err = init_filter_graph(out graph, out src, out sink);
            if (err < 0) {
                Console.Write("Unable to init filter graph:");
                goto fail;
            }

            AVBufferSrc _src = new AVBufferSrc(src);
            AVBufferSink _sink = new AVBufferSink(sink);
            /* the main filtering loop */
            for (int i = 0; i < nb_frames; i++) {
                /* get an input frame to be filtered */
                err = get_input(frame, i);
                if (err < 0) {
                    Console.Write("Error generating input frame:");
                    goto fail;
                }

                /* Send the frame to the input of the filtergraph. */
                err = _src.add_frame(frame);
                if (err < 0) {
                    frame.Free();
                    Console.Write("Error submitting the frame to the filtergraph:");
                    goto fail;
                }

                /* Get all the filtered output that is available. */
                while ((err = _sink.get_frame(frame)) >= 0) {
                    /* now do something with our filtered frame */
                    err = process_output(md5, frame);
                    if (err < 0) {
                        Console.Write("Error processing the filtered frame:");
                        goto fail;
                    }
                    frame.Free();
                }

                if (err == AVRESULT.EAGAIN) {
                    /* Need to feed more frames in. */
                    continue;
                } else if (err == AVRESULT.EOF) {
                    /* Nothing more to do, finish. */
                    break;
                } else if (err < 0) {
                    /* An error occurred. */
                    Console.Write("Error filtering the data:");
                    goto fail;
                }

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

            if (graph != null) graph.Dispose();
            if (md5 != null) md5.Dispose();
            if (frame != null) frame.Dispose();

            Environment.Exit(0);

            fail:
            
            Console.WriteLine("{0}", (AVRESULT)err);

            Environment.Exit(1);
        }

        static bool s_bQuit = false;

        private static void Console_CancelKeyPress(object sender, ConsoleCancelEventArgs e)
        {
            s_bQuit = true;
        }
    }
}
