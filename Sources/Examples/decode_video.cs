//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// video decoding with libavcodec API example
//////////////////////////////////////////////////////
using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("decode_video")]
[assembly: AssemblyDescription("C# video decoding with libavcodec API example")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class decode_video
    {
        const int INBUF_SIZE = 4096;
  
        static void pgm_save(IntPtr buf, int wrap, int xsize, int ysize, string filename)
        {
            Stream f = File.OpenWrite(filename);

            string s = string.Format("P5\n{0} {1}\n{2}\n", xsize, ysize, 255);
            IntPtr p = Marshal.StringToCoTaskMemAnsi(s);
            byte[] data = new byte[xsize > s.Length? xsize : s.Length];
            Marshal.Copy(p,data,0,s.Length);
            Marshal.FreeCoTaskMem(p);
            f.Write(data,0,s.Length);
            for (int i = 0; i < ysize; i++)
            {
                Marshal.Copy(buf + i * wrap,data,0,xsize);
                f.Write(data,0,xsize);
            }
            f.Dispose();
        }

        static void decode(AVCodecContext dec_ctx, AVPacket pkt, AVFrame frame, string filename)
        {
            /* send the packet with the compressed data to the decoder */
            var ret = dec_ctx.SendPacket(pkt);
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

                Console.WriteLine("saving frame {0}", dec_ctx.frame_number);

                /* the picture is allocated by the decoder. no need to
                    free it */
                pgm_save(frame.data[0], frame.linesize[0], frame.width, frame.height, 
                    string.Format("{0}-{1}", filename, dec_ctx.frame_number));

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
            AVCodec codec = AVCodec.FindDecoder(AVCodecID.MPEG1VIDEO);
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
                Console.WriteLine("Could not allocate video codec context\n");
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

            AVFrame frame = new AVFrame();
            if (!frame._IsValid)
            {
                Console.WriteLine("Could not allocate video frame\n");
                Environment.Exit(1);
            }

            AVMemPtr inbuf = new AVMemPtr(INBUF_SIZE + AVUtil.INPUT_BUFFER_PADDING_SIZE);
            
            /* decode until eof */
            byte[] array = new byte[INBUF_SIZE];

            while (true)
            {
                /* read raw data from the input file */
                int data_size = f.Read(array, 0, INBUF_SIZE);
                if (data_size <= 0) break;
                Marshal.Copy(array, 0, inbuf, data_size);
                /* use the parser to split the data into frames */
                IntPtr data = inbuf;
                while (data_size > 0)
                {
                    int ret = parser.Parse(c, pkt, data, data_size);
                    if (ret < 0)
                    {
                        Console.WriteLine("Error while parsing\n");
                        Environment.Exit(1);
                    }
                    data += ret;
                    data_size -= ret;

                    if (pkt.size > 0)
                        decode(c, pkt, frame, outfilename);
                }
            }

            /* flush the decoder */
            decode(c, null, frame, outfilename);

            f.Dispose();

            c.Dispose();
            parser.Dispose();
            frame.Dispose();
            pkt.Dispose();
            inbuf.Dispose();
        }
    }
}
