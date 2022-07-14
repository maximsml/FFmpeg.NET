//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// audio encoding with libavcodec API example.
//////////////////////////////////////////////////////
using System;
using System.IO;
using System.Reflection;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("encode_audio")]
[assembly: AssemblyDescription("C# audio encoding with libavcodec API example.")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class encode_audio
    {
        /* check that a given sample format is supported by the encoder */
        static bool check_sample_fmt(AVCodec codec, AVSampleFormat sample_fmt)
        {
            foreach (var p in codec.sample_fmts)
            {
                if (p == sample_fmt)
                {
                    return true;
                }
            }
            return false;
        }

        /* just pick the highest supported samplerate */
        static int select_sample_rate(AVCodec codec)
        {
            int best_samplerate = 0;

            if (codec.supported_samplerates == null || codec.supported_samplerates.Length == 0)
                return 44100;

            foreach (var p in codec.supported_samplerates)
            {
                if (best_samplerate == 0 || Math.Abs(44100 - p) < Math.Abs(44100 - best_samplerate))
                {
                    best_samplerate = p;
                }
            }
            return best_samplerate;
        }

        /* select layout with the highest channel count */
        static AVChannelLayout select_channel_layout(AVCodec codec)
        {
            AVChannelLayout best_ch_layout = (AVChannelLayout)0;
            int best_nb_channels   = 0;

            if (codec.channel_layouts == null || codec.channel_layouts.Length == 0)
                return AVChannelLayout.LAYOUT_STEREO;

            foreach (var p in codec.channel_layouts)
            {
                int nb_channels = p.channels;
                if (nb_channels > best_nb_channels) {
                    best_ch_layout    = p;
                    best_nb_channels = nb_channels;
                }
            }
            return best_ch_layout;
        }

        static void encode(AVCodecContext ctx, AVFrame frame, AVPacket pkt, Stream output)
        {
            int ret;

            /* send the frame for encoding */
            
            ret = ctx.SendFrame(frame);
            if (ret < 0) {
                Console.WriteLine("Error sending the frame to the encoder\n");
                Environment.Exit(1);
            }

            /* read all the available output packets (in general there may be any
             * number of them */
            while (ret >= 0) {
                ret = ctx.ReceivePacket(pkt);
                if (ret == AVRESULT.EAGAIN || ret == AVRESULT.EOF)
                    return;
                else if (ret < 0) {
                    Console.WriteLine("Error encoding audio frame\n");
                    Environment.Exit(1);
                }
                output.Write(pkt.ToArray(), 0, pkt.size);
                pkt.Free();
            }
        }

        static void Main(string[] args)
        {
            if (args.Length <= 0)
            {
                Console.WriteLine("Usage: {0} <output file>\n", 
                    Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            string filename = args[0];

            /* find the MP2 encoder */
            AVCodec codec = AVCodec.FindEncoder(AVCodecID.MP2);
            if (codec == null)
            {
                Console.WriteLine("Codec not found\n");
                Environment.Exit(1);
            }
            AVCodecContext c = new AVCodecContext(codec);
            if (c == null)
            {
                Console.WriteLine("Could not allocate audio codec context\n");
                Environment.Exit(1);
            }

            /* put sample parameters */
            c.bit_rate = 64000;

            /* check that the encoder supports s16 pcm input */
            c.sample_fmt = AVSampleFormat.S16;
            if (!check_sample_fmt(codec, c.sample_fmt))
            {
                Console.WriteLine("Encoder does not support sample format {0}\n", c.sample_fmt.name);
                Environment.Exit(1);
            }
    
            /* select other audio parameters supported by the encoder */
            c.sample_rate    = select_sample_rate(codec);
            c.channel_layout = select_channel_layout(codec);
            c.channels       = c.channel_layout.channels;

            /* open it */
            if ((int)c.Open(codec) < 0) {
                Console.WriteLine("Could not open codec\n");
                Environment.Exit(1);
            }

            Stream f = File.OpenWrite(filename);
            if (f == null) {
                Console.WriteLine("Could not open {0}\n", filename);
                Environment.Exit(1);
            }

            /* packet for holding encoded output */
            AVPacket pkt = new AVPacket();
            if (!pkt._IsValid) {
                Console.WriteLine("could not allocate the packet\n");
                Environment.Exit(1);
            }

            /* frame containing input raw audio */
            AVFrame frame = new AVFrame();
            if (!frame._IsValid) {
                Console.WriteLine("Could not allocate audio frame\n");
                Environment.Exit(1);
            }

            frame.nb_samples     = c.frame_size;
            frame.format         = (int)c.sample_fmt;
            frame.channel_layout = c.channel_layout;

            /* allocate the data buffers */
            AVRESULT ret = frame.AllocateBuffers(0);
            if ((int)ret < 0) {
                Console.WriteLine("Could not allocate audio data buffers\n");
                Environment.Exit(1);
            }
            
            /* encode a single tone sound */
            float t = 0;
            float tincr = (float)(2 * Math.PI * 440.0 / c.sample_rate);
            for (int i = 0; i < 200; i++)
            {
                /* make sure the frame is writable -- makes a copy if the encoder
                * kept a reference internally */
                if (!frame.MakeWritable())
                    Environment.Exit(1);

                var samples = frame.data[0].shorts;
                for (int j = 0; j < c.frame_size; j++)
                {
                    samples[2 * j] = (short)(int)(Math.Sin(t) * 10000);

                    for (int k = 1; k < c.channels; k++)
                        samples[2 * j + k] = samples[2 * j];
                    t += tincr;
                }
                encode(c, frame, pkt, f);
            }
            
            /* flush the encoder */
            encode(c, null, pkt, f);

            f.Dispose();
            frame.Dispose();
            pkt.Dispose();
            c.Dispose();
        }
    }
}
