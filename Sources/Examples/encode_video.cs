//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// video encoding with libavcodec API example
//////////////////////////////////////////////////////
using System;
using System.IO;
using System.Reflection;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("encode_video")]
[assembly: AssemblyDescription("C# video encoding with libavcodec API example")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class encode_video
    {
        static void encode(AVCodecContext ctx, AVFrame frame, AVPacket pkt, Stream outfile)
        {
            int ret;

            /* send the frame for encoding */
            if (frame != null)
                Console.WriteLine("Send frame {0}", frame.pts);

            ret = ctx.SendFrame(frame);
            if (ret < 0) {
                Console.WriteLine("Error sending a frame for encoding");
                Environment.Exit(1);
            }

            /* read all the available output packets (in general there may be any
             * number of them */
            while (ret >= 0) {
                ret = ctx.ReceivePacket(pkt);
                if (ret == AVRESULT.EAGAIN || ret == AVRESULT.EOF)
                    return;
                else if (ret < 0) {
                    Console.WriteLine("Error during encoding");
                    Environment.Exit(1);
                }
                outfile.Write(pkt.ToArray(), 0, pkt.size);
                pkt.Free();
            }
        }

        static void Main(string[] args)
        {
            if (args.Length <= 1)
            {
                Console.WriteLine("Usage: {0} <output file> <codec name>\n", 
                    Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            string filename = args[0];
            string codec_name = args[1];

            /* find the mpeg1video encoder */
            AVCodec codec = AVCodec.FindEncoder(codec_name);
            if (codec == null)
            {
                Console.WriteLine("Codec '{0}' not found\n",codec_name);
                Environment.Exit(1);
            }
            AVCodecContext c = new AVCodecContext(codec);
            if (c == null)
            {
                Console.WriteLine("Could not allocate video codec context\n");
                Environment.Exit(1);
            }

            /* put sample parameters */
            c.bit_rate = 400000;

            /* resolution must be a multiple of two */
            c.width = 352;
            c.height = 288;
            /* frames per second */
            c.time_base = new AVRational(1, 25);
            c.framerate = new AVRational(25, 1);

            /* emit one intra frame every ten frames
             * check frame pict_type before passing frame
             * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
             * then gop_size is ignored and the output of encoder
             * will always be I frame irrespective to gop_size
             */
            c.gop_size = 10;
            c.max_b_frames = 1;
            c.pix_fmt =  AVPixelFormat.YUV420P;

            if (codec.id == AVCodecID.H264)
            {
                var opt = new AVOptions(c.priv_data);
                opt.set("preset", "slow", 0);
            }

            /* open it */
            var ret = c.Open(codec);
            if (ret < 0) {
                Console.WriteLine("Could not open codec {0} \n",ret.ToString());
                Environment.Exit(1);
            }

            Stream f = File.OpenWrite(filename);
            if (f == null) {
                Console.WriteLine("Could not open {0}\n", filename);
                Environment.Exit(1);
            }

            /* frame containing input raw audio */
            AVFrame frame = new AVFrame();
            if (!frame._IsValid) {
                Console.WriteLine("Could not allocate video frame\n");
                Environment.Exit(1);
            }

            frame.format    = (int)c.pix_fmt;
            frame.width     = c.width;
            frame.height    = c.height;

            /* allocate the data buffers */
            ret = frame.AllocateBuffers(32);
            if ((int)ret < 0) {
                Console.WriteLine("Could not allocate the video frame data\n");
                Environment.Exit(1);
            }
            
            /* packet for holding encoded output */
            AVPacket pkt = new AVPacket();
            if (!pkt._IsValid) {
                Console.WriteLine("could not allocate the packet\n");
                Environment.Exit(1);
            }

            /* encode 1 second of video */
            for (int i = 0; i < 25; i++) {

                /* make sure the frame data is writable */
                if (!frame.MakeWritable())
                    Environment.Exit(1);

                /* prepare a dummy image */
                /* Y */
                for (int y = 0; y < c.height; y++) {
                    for (int x = 0; x < c.width; x++) {
                        frame.data[0][y * frame.linesize[0] + x] = (byte)(x + y + i * 3);
                    }
                }

                /* Cb and Cr */
                for (int y = 0; y < c.height/2; y++) {
                    for (int x = 0; x < c.width/2; x++) {
                        frame.data[1][y * frame.linesize[1] + x] = (byte)(128 + y + i * 2);
                        frame.data[2][y * frame.linesize[2] + x] = (byte)(64 + x + i * 5);
                    }
                }

                frame.pts = i;

                /* encode the image */
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
