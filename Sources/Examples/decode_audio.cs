//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// audio decoding with libavcodec API example
//////////////////////////////////////////////////////
using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("decode_audio")]
[assembly: AssemblyDescription("C# audio decoding with libavcodec API example")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class decode_audio
    {
        const int AUDIO_INBUF_SIZE = 20480;
        const int AUDIO_REFILL_THRESH = 4096;

        [DllImport("kernel32.dll", EntryPoint = "RtlMoveMemory")]
        public static extern void memmove(IntPtr Destination, IntPtr Source, [MarshalAs(UnmanagedType.U4)] int Length);

        static void decode(AVCodecContext dec_ctx, AVPacket pkt, AVFrame frame, Stream outfile)
        {
            int i, ch;
            int ret, data_size;
            
            /* send the packet with the compressed data to the decoder */
            ret = dec_ctx.SendPacket(pkt);
            if (ret < 0) {
                Console.WriteLine("Error submitting the packet to the decoder\n");
                Environment.Exit(1);
            }
            
            /* read all the output frames (in general there may be any number of them */
            while (ret >= 0) {
                ret = dec_ctx.ReceiveFrame(frame);
                if (ret == AVRESULT.EAGAIN || ret == AVRESULT.EOF)
                    return;
                else if (ret < 0) {
                    Console.WriteLine("Error during decoding\n");
                    Environment.Exit(1);
                }
                data_size = dec_ctx.sample_fmt.bytes_per_sample;
                if (data_size < 0) {
                    /* This should not occur, checking just for paranoia */
                    Console.WriteLine("Failed to calculate data size\n");
                    Environment.Exit(1);
                }

                byte[] data = new byte[data_size];
                for (i = 0; i < frame.nb_samples; i++)
                {
                    for (ch = 0; ch < dec_ctx.channels; ch++)
                    {
                        Marshal.Copy((IntPtr)frame.data[ch] + data_size * i, data, 0, data_size);
                        outfile.Write(data, 0, data_size);
                    }
                }
            }
        }

        static void Main(string[] args)
        {
            if (args.Length <= 1)
            {
                Console.WriteLine("Usage: {0} <input file> <output file>\n", 
                    Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            string filename     = args[0];
            string outfilename  = args[1];

            AVPacket pkt = new AVPacket();
            
            /* find the MPEG audio decoder */
            AVCodec codec = AVCodec.FindDecoder(AVCodecID.MP2);
            if (codec == null)
            {
                Console.WriteLine("Codec not found\n");
                Environment.Exit(1);
            }

            AVCodecParserContext parser = new AVCodecParserContext(codec.id);
            if (!parser._IsValid)
            {
                Console.WriteLine("Parser not found\n");
                Environment.Exit(1);
            }

            AVCodecContext c = new AVCodecContext(codec);
            if (!c._IsValid)
            {
                Console.WriteLine("Could not allocate audio codec context\n");
                Environment.Exit(1);
            }

            /* open it */
            if (c.Open(codec) < 0)
            {
                Console.WriteLine("Could not open codec\n");
                Environment.Exit(1);
            }

            Stream f = File.OpenRead(filename);
            if (f == null) {
                Console.WriteLine("Could not open {0}\n", filename);
                Environment.Exit(1);
            }

            Stream outfile = File.OpenWrite(outfilename);
            if (outfile == null) {
                c.Dispose();
                Console.WriteLine("Could not open output {0}\n", outfilename);
                Environment.Exit(1);
            }
            AVFrame decoded_frame = new AVFrame();
            if (!decoded_frame._IsValid)
            {
                Console.WriteLine("Could not allocate audio frame\n");
                Environment.Exit(1);
            }

            AVMemPtr inbuf = new AVMemPtr(AUDIO_INBUF_SIZE + AVUtil.INPUT_BUFFER_PADDING_SIZE);
            IntPtr data = inbuf;
            /* decode until eof */
            byte[] array = new byte[AUDIO_INBUF_SIZE];
            int data_size = f.Read(array,0,AUDIO_INBUF_SIZE);
            Marshal.Copy(array, 0, data, data_size);
            while (data_size > 0)
            {
                int ret = parser.Parse(c, pkt, data, data_size);
                if (ret < 0) {
                    Console.WriteLine("Error while parsing\n");
                    Environment.Exit(1);
                }
                data      += ret;
                data_size -= ret;

                if (pkt.size > 0)
                    decode(c, pkt, decoded_frame, outfile);

                if (data_size < AUDIO_REFILL_THRESH)
                {
                    memmove(inbuf, data, data_size);
                    data = inbuf;
                    int len = f.Read(array,0,AUDIO_INBUF_SIZE - data_size);
                    if (len > 0)
                    {
                        Marshal.Copy(array, 0, data + data_size, len);
                        data_size += len;
                    }
                }
            }

            /* flush the decoder */
            pkt.data = null;
            pkt.size = 0;
            decode(c, pkt, decoded_frame, outfile);

            outfile.Dispose();
            f.Dispose();

            c.Dispose();
            parser.Dispose();
            decoded_frame.Dispose();
            pkt.Dispose();
            inbuf.Dispose();
        }
    }
}
