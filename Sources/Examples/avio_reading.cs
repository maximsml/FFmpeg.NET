//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// libavformat AVIOContext API example.
// Make libavformat demuxer access media content through a custom
// AVIOContext read callback.
//////////////////////////////////////////////////////
using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("avio_reading")]
[assembly: AssemblyDescription("C# Make libavformat demuxer access media content through a custom " +
    "AVIOContext read callback.")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class avio_reading
    {
        class IOTarget
        {
            // Target Stream
            public Stream stream = null;
            // Reading position
            public long position = 0;
            // Temporal buffer 
            public byte[] buffer = null;

            public IOTarget(Stream s, long pos) { stream = s; position = pos; }
        }

        static int read_packet(object opaque, IntPtr buf, int buf_size)
        {
            IOTarget io = (opaque as IOTarget);
            
            long available = (io.stream.Length - io.position);
            if (available == 0) return AVRESULT.EOF;
            if ((long)buf_size > available)
            {
                buf_size = (int)available;
            }
            /* Just doing double copy we can avoid that just interop some other API
             * or load file entirely into memory like it is done in original example */
            if (io.buffer.Length < buf_size)
            {
                io.buffer = new byte[buf_size];
            }
            buf_size = io.stream.Read(io.buffer,0,buf_size);
            Marshal.Copy(io.buffer, 0, buf,buf_size);

            io.position += buf_size;
            return buf_size;
        }

        static void Main(string[] args)
        {
            Stream s = null;
            AVFormatContext fmt_ctx = null;
            AVIOContext avio_ctx = null;
            AVMemPtr avio_ctx_buffer = null;

            int ret = 0;
            if (args.Length != 1)
            {
                Console.WriteLine("usage: {0} input_file\n"
                                + "API example program to show how to read from a custom buffer "
                                + "accessed through AVIOContext.\n"
                                ,Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }
            string input_filename  = args[0];
            /* slurp file content into buffer */
            s = File.OpenRead(input_filename);
            if (s == null) {
                ret = AVRESULT.STREAM_NOT_FOUND;
                goto end;
            }
            IOTarget _target = new IOTarget(s,0);

            fmt_ctx = new AVFormatContext();
            if (!fmt_ctx._IsValid) {
                ret = AVRESULT.ENOMEM;
                goto end; 
            }

            avio_ctx_buffer = new AVMemPtr(4096);
            if (!avio_ctx_buffer.allocated) {
                ret = AVRESULT.ENOMEM;
                goto end; 
            }

            avio_ctx = new AVIOContext(avio_ctx_buffer,0,_target,read_packet,null,null);
            if (!avio_ctx._IsValid) {
                ret = AVRESULT.ENOMEM;
                goto end; 
            }
            _target.buffer = new byte[avio_ctx_buffer.size];

            fmt_ctx.pb = avio_ctx;
            ret = fmt_ctx.OpenInput();
            if (ret < 0) {
                Console.Write("Could not find stream information\n");
                goto end;
            }
            

            ret = fmt_ctx.FindStreamInfo();
            if (ret < 0) {
                Console.Write("Could not find stream information\n");
                goto end;
            }

            fmt_ctx.DumpFormat(0, input_filename, false);

            end:

            if (s != null) s.Dispose();
            if (fmt_ctx != null) fmt_ctx.Dispose();
            if (avio_ctx_buffer != null) avio_ctx_buffer.Dispose();
            if (avio_ctx != null) avio_ctx.Dispose();

            if (ret < 0)
            {
                Console.WriteLine("Error occurred: {0}\n", (AVRESULT)ret);
                Environment.Exit(1);
            }

            Environment.Exit(0);
        }
    }
}