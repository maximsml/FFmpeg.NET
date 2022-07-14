//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// Capture Screen with Windows GPI encode it with libavcodec 
// and saves into a file with libavformat
//////////////////////////////////////////////////////
using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("screen_capture")]
[assembly: AssemblyDescription("Capture Screen with Windows GDI scale with it libswscale, encode with libavcodec and saves into a file with libavformat")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class screen_capture
    {
        static bool s_bQuit = false;

        private static void Console_CancelKeyPress(object sender, ConsoleCancelEventArgs e)
        {
            s_bQuit = true;
        }

        class ScreenCapture : IDisposable
        {
            protected IntPtr m_hScreenDC = IntPtr.Zero;
            protected IntPtr m_hMemDC = IntPtr.Zero;
            protected IntPtr m_hBitmap = IntPtr.Zero;
            protected BitmapInfo m_bmi = new BitmapInfo();

            protected int m_nMaxWidth = 0;
            protected int m_nMaxHeight = 0;

            protected int m_nWidth = 0;
            protected int m_nHeight = 0;
            protected AVFrame m_Frame = null;

            public int Width { get { return m_nWidth; } set { m_nWidth = value; }  }
            public int Height { get { return m_nHeight; } set { m_nHeight = value; }  }

            public ScreenCapture()
            {
                m_hScreenDC = CreateDC("DISPLAY", null, null, IntPtr.Zero);
                m_nMaxWidth = GetDeviceCaps(m_hScreenDC, 8); // HORZRES
                m_nMaxHeight = GetDeviceCaps(m_hScreenDC, 10); // VERTRES

                m_nWidth = m_nMaxWidth;
                m_nHeight = m_nMaxHeight;
                m_hMemDC = CreateCompatibleDC(m_hScreenDC);
                m_bmi.bmiHeader = new BitmapInfoHeader();
            }

            public AVFrame get_frame()
            {
                if (m_hBitmap == IntPtr.Zero)
                {
                    m_hBitmap = CreateCompatibleBitmap(m_hScreenDC, m_nWidth, Math.Abs(m_nHeight));
                }
                m_bmi.bmiHeader.BitCount = 32; // RGBA 8 bit / pixel
                m_bmi.bmiHeader.Height = m_nHeight;
                m_bmi.bmiHeader.Width = m_nWidth;
                m_bmi.bmiHeader.Compression = 0; // BI_RGB
                m_bmi.bmiHeader.Planes = 1;
                m_bmi.bmiHeader.ImageSize = AVPicture.GetSize(AVPixelFormat.BGRA, m_nWidth, m_nHeight);

                if (m_Frame == null)
                {
                    m_Frame = new AVFrame();
                    m_Frame.format = (int)AVPixelFormat.BGRA;
                    m_Frame.width = m_nWidth;
                    m_Frame.height = m_nHeight;
                    m_Frame.AllocateBuffers();
                    m_Frame.MakeWritable();
                }
                IntPtr hOldBitmap = SelectObject(m_hMemDC, m_hBitmap);
                StretchBlt(m_hMemDC, 0, 0, m_nWidth, Math.Abs(m_nHeight), m_hScreenDC, 0, 0, m_nMaxWidth, m_nMaxHeight, TernaryRasterOperations.SRCCOPY);
                SelectObject(m_hMemDC, hOldBitmap);
                GetDIBits(m_hMemDC, m_hBitmap, 0, (uint)Math.Abs(m_nHeight), m_Frame.data[0],ref m_bmi, 0);

                return m_Frame;
            }

            public void Dispose()
            {
                if (m_hBitmap != IntPtr.Zero)
                {
                    DeleteObject(m_hBitmap);
                    m_hBitmap = IntPtr.Zero;
                }
                if (m_hScreenDC != IntPtr.Zero)
                {
                    DeleteDC(m_hScreenDC);
                    m_hScreenDC = IntPtr.Zero;
                }
                if (m_hMemDC != IntPtr.Zero)
                {
                    DeleteDC(m_hMemDC);
                    m_hMemDC = IntPtr.Zero;
                }
                if (m_Frame != null)
                {
                    m_Frame.Dispose();
                }
            }

            [StructLayout(LayoutKind.Sequential, Pack = 2)]
            [ComVisible(false)]
            public class BitmapInfoHeader
            {
                public int Size;
                public int Width;
                public int Height;
                public short Planes;
                public short BitCount;
                public int Compression;
                public int ImageSize;
                public int XPelsPerMeter;
                public int YPelsPerMeter;
                public int ClrUsed;
                public int ClrImportant;

                public BitmapInfoHeader()
                {
                    Size = Marshal.SizeOf(this.GetType());
                    Width = 0;
                    Height = 0;
                    Planes = 0;
                    BitCount = 0;
                    Compression = 0;
                    ImageSize = 0;
                    XPelsPerMeter = 0;
                    YPelsPerMeter = 0;
                    ClrUsed = 0;
                    ClrImportant = 0;
                }

                public int GetBitmapSize()
                {
                    return Width * Math.Abs(Height) * (BitCount + BitCount % 8) / 8;
                }
            }

            [StructLayout(LayoutKind.Sequential)]
            protected struct BitmapInfo
            {
                public BitmapInfoHeader bmiHeader;
                public int[] bmiColors;
            }

            private enum TernaryRasterOperations : uint
            {
                SRCCOPY = 0x00CC0020,
                SRCPAINT = 0x00EE0086,
                SRCAND = 0x008800C6,
                SRCINVERT = 0x00660046,
                SRCERASE = 0x00440328,
                NOTSRCCOPY = 0x00330008,
                NOTSRCERASE = 0x001100A6,
                MERGECOPY = 0x00C000CA,
                MERGEPAINT = 0x00BB0226,
                PATCOPY = 0x00F00021,
                PATPAINT = 0x00FB0A09,
                PATINVERT = 0x005A0049,
                DSTINVERT = 0x00550009,
                BLACKNESS = 0x00000042,
                WHITENESS = 0x00FF0062,
                CAPTUREBLT = 0x40000000
            }

            [DllImport("gdi32.dll")]
            private static extern IntPtr CreateDC(string lpszDriver, string lpszDevice, string lpszOutput, IntPtr lpInitData);

            [DllImport("gdi32.dll", SetLastError = true)]
            private static extern IntPtr CreateCompatibleDC(IntPtr hdc);

            [DllImport("gdi32.dll")]
            private static extern IntPtr CreateCompatibleBitmap(IntPtr hdc, int nWidth, int nHeight);

            [DllImport("gdi32.dll", ExactSpelling = true, PreserveSig = true, SetLastError = true)]
            private static extern IntPtr SelectObject(IntPtr hdc, IntPtr hgdiobj);

            [DllImport("gdi32.dll")]
            private static extern bool DeleteObject(IntPtr hObject);

            [DllImport("gdi32.dll")]
            private static extern bool DeleteDC(IntPtr hdc);

            [DllImport("gdi32.dll")]
            private static extern bool StretchBlt(IntPtr hdcDest, int nXOriginDest, int nYOriginDest,
                int nWidthDest, int nHeightDest,
                IntPtr hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
                TernaryRasterOperations dwRop);

            [DllImport("gdi32.dll")]
            private static extern int GetDIBits(IntPtr hdc, IntPtr hbmp, uint uStartScan,
               uint cScanLines, [Out] IntPtr lpvBits, ref BitmapInfo lpbmi, uint uUsage);

            [DllImport("gdi32.dll")]
            private static extern int GetDeviceCaps(IntPtr hdc, int nIndex);
        }

        public struct VideoSizeAbbr
        {
            public string abbr;
            public int width;
            public int height;

            public VideoSizeAbbr(string s, int w, int h) { abbr = s; width = w; height = h; }
        };

        static int av_parse_video_size(out int width_ptr, out int height_ptr, string str)
        {
            width_ptr = 0;
            height_ptr = 0;

            var video_size_abbrs = new[] {
                new VideoSizeAbbr("ntsc",      720, 480 ),
                new VideoSizeAbbr("pal",       720, 576 ),
                new VideoSizeAbbr("qntsc",     352, 240 ), /* VCD compliant NTSC */
                new VideoSizeAbbr("qpal",      352, 288 ), /* VCD compliant PAL */
                new VideoSizeAbbr("sntsc",     640, 480 ), /* square pixel NTSC */
                new VideoSizeAbbr("spal",      768, 576 ), /* square pixel PAL */
                new VideoSizeAbbr("film",      352, 240 ),
                new VideoSizeAbbr("ntsc-film", 352, 240 ),
                new VideoSizeAbbr("sqcif",     128,  96 ),
                new VideoSizeAbbr("qcif",      176, 144 ),
                new VideoSizeAbbr("cif",       352, 288 ),
                new VideoSizeAbbr("4cif",      704, 576 ),
                new VideoSizeAbbr("16cif",    1408,1152 ),
                new VideoSizeAbbr("qqvga",     160, 120 ),
                new VideoSizeAbbr("qvga",      320, 240 ),
                new VideoSizeAbbr("vga",       640, 480 ),
                new VideoSizeAbbr("svga",      800, 600 ),
                new VideoSizeAbbr("xga",      1024, 768 ),
                new VideoSizeAbbr("uxga",     1600,1200 ),
                new VideoSizeAbbr("qxga",     2048,1536 ),
                new VideoSizeAbbr("sxga",     1280,1024 ),
                new VideoSizeAbbr("qsxga",    2560,2048 ),
                new VideoSizeAbbr("hsxga",    5120,4096 ),
                new VideoSizeAbbr("wvga",      852, 480 ),
                new VideoSizeAbbr("wxga",     1366, 768 ),
                new VideoSizeAbbr("wsxga",    1600,1024 ),
                new VideoSizeAbbr("wuxga",    1920,1200 ),
                new VideoSizeAbbr("woxga",    2560,1600 ),
                new VideoSizeAbbr("wqsxga",   3200,2048 ),
                new VideoSizeAbbr("wquxga",   3840,2400 ),
                new VideoSizeAbbr("whsxga",   6400,4096 ),
                new VideoSizeAbbr("whuxga",   7680,4800 ),
                new VideoSizeAbbr("cga",       320, 200 ),
                new VideoSizeAbbr("ega",       640, 350 ),
                new VideoSizeAbbr("hd480",     852, 480 ),
                new VideoSizeAbbr("hd720",    1280, 720 ),
                new VideoSizeAbbr("hd1080",   1920,1080 ),
                new VideoSizeAbbr("2k",       2048,1080 ), /* Digital Cinema System Specification */
                new VideoSizeAbbr("2kflat",   1998,1080 ),
                new VideoSizeAbbr("2kscope",  2048, 858 ),
                new VideoSizeAbbr("4k",       4096,2160 ), /* Digital Cinema System Specification */
                new VideoSizeAbbr("4kflat",   3996,2160 ),
                new VideoSizeAbbr("4kscope",  4096,1716 ),
                new VideoSizeAbbr("nhd",       640,360  ),
                new VideoSizeAbbr("hqvga",     240,160  ),
                new VideoSizeAbbr("wqvga",     400,240  ),
                new VideoSizeAbbr("fwqvga",    432,240  ),
                new VideoSizeAbbr("hvga",      480,320  ),
                new VideoSizeAbbr("qhd",       960,540  ),
            };
            for (int i = 0; i < video_size_abbrs.Length; i++)
            {
                if (str.ToLower() == video_size_abbrs[i].abbr)
                {
                    width_ptr = video_size_abbrs[i].width;
                    height_ptr = video_size_abbrs[i].height;
                    return 0;
                }
            }
            string digits = "0123456789";
            int idx = 0;
            int pos = 0;
            while (pos < 2 && idx < str.Length)
            {
                while (idx < str.Length && digits.IndexOf(str[idx]) < 0)
                {
                    idx++;
                }
                string num = "";
                while (idx < str.Length && digits.IndexOf(str[idx]) >= 0)
                {
                    num += str[idx];
                    idx++;
                }
                if (!string.IsNullOrEmpty(num))
                {
                    if (pos == 0)
                    {
                        width_ptr = int.Parse(num);
                    }
                    if (pos == 1)
                    {
                        height_ptr = int.Parse(num);
                        return 0;
                    }
                }
                pos++;
            }
            return AVRESULT.EINVAL;
        }

        [DllImport("Kernel32.dll")]
        private static extern uint GetTickCount();

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

        static void Main(string[] args)
        {
            int dst_w = 0;
            int dst_h = 0;
            if (args.Length < 1)
            {
                Console.WriteLine("Usage: {0} <output file> <output resolution> <video codec>\n"
                    + "Example capture screen with Windows GDI API\n" 
                    + "Program shows how to scaling picture with libswscale encode it with libavcodec and saves into a file with libavformat\n"
                    + "This program generates a video <output file> encoded with specified <video codec> and with the given "
                    + "<output resolution> and saves them to an file named <output file>\n.",
                    Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            Console.CancelKeyPress += Console_CancelKeyPress;

            string filename = args[0];
            if (args.Length > 1)
            {
                string dst_size = args[1];
                av_parse_video_size(out dst_w, out dst_h, dst_size);
            }
            ScreenCapture cap = new ScreenCapture();

            if (dst_w != 0 && dst_h != 0)
            {
                cap.Width = dst_w;
                cap.Height = dst_h;
            }

            /* allocate the output media context */
            AVFormatContext oc = AVFormatContext.OpenOutput(filename);
            if (oc == null)
            {
                Console.WriteLine("Could not deduce output format from file extension: using MPEG.\n");
                oc = AVFormatContext.OpenOutput("mp4", filename);
            }
            if (oc == null)
            {
                Environment.Exit(1);
            }
            int nFramesPerSecond = 25;

            AVOutputFormat fmt = oc.oformat;

            /* find the encoder */
            AVCodec codec = null;
            if (args.Length > 2)
            {
                string codec_name = args[2];
                codec = AVCodec.FindEncoder(codec_name);
                if (codec == null) {
                    Console.WriteLine("Could not find encoder for '{0}' will be used default codec",codec_name);
                }
            }
            if (codec == null) {
                /* Add the video stream using the default format codecs
                * and initialize the codecs. */
                if (fmt.video_codec == AVCodecID.NONE)
                {
                    Console.WriteLine("Format not support video \n");
                    Environment.Exit(1);
                }
                codec = AVCodec.FindEncoder(fmt.video_codec);
                if (codec == null) {
                    Console.WriteLine("Could not find encoder for '{0}'\n",
                        AVCodec.GetCodecName(fmt.video_codec));
                    Environment.Exit(1);
                }
            }
            AVStream stream = oc.AddStream();
            if (stream == null) {
                Console.WriteLine("Could not allocate stream\n");
                Environment.Exit(1);
            }
            stream.id = oc.nb_streams-1;

            AVCodecContext encoder = new AVCodecContext(codec);
            if (!encoder._IsValid) {
                Console.WriteLine("Could not alloc an encoding context\n");
                Environment.Exit(1);
            }
            encoder.codec_id = codec.id;

            /* Resolution must be a multiple of two. */
            encoder.width    = cap.Width;
            encoder.height   = cap.Height;

            encoder.bit_rate = (encoder.width * encoder.height) << 1;
            /* timebase: This is the fundamental unit of time (in seconds) in terms
            * of which frame timestamps are represented. For fixed-fps content,
            * timebase should be 1/framerate and timestamp increments should be
            * identical to 1. */
            stream.time_base  = new AVRational( 1, nFramesPerSecond );
            encoder.time_base = stream.time_base;

            encoder.gop_size      = nFramesPerSecond; /* emit one intra frame every second at most */
            encoder.pix_fmt       = AVPixelFormat.YUV420P;
            encoder.max_b_frames = 2;

            /* Some formats want stream headers to be separate. */
            if ((oc.oformat.flags & AVfmt.GLOBALHEADER) != 0)
            {
                encoder.flags |= AVCodecFlag.GLOBAL_HEADER;
            }

            int ret = encoder.Open(codec);

            if (ret < 0) {
                Console.WriteLine("Could not open video codec: {0}\n", (AVRESULT)(ret));
                Environment.Exit(1);
            }

            /* copy the stream parameters to the muxer */
            ret = stream.codecpar.FromContext(encoder);

            AVFrame frame = new AVFrame();
            frame.width  = encoder.width;
            frame.height = encoder.height;
            frame.format = (int)encoder.pix_fmt;
            frame.AllocateBuffers(16);
            frame.MakeWritable();

            SwsContext sws_ctx = null;

            oc.DumpFormat(0, filename, true);

            if ((int)(fmt.flags & AVfmt.NOFILE) == 0) {
                oc.pb = new AVIOContext(filename,AvioFlag.WRITE);
                if (!oc.pb._IsValid) {
                    Console.WriteLine("Could not open output file '{0}'", filename);
                    Environment.Exit(1);
                }
            }

            int samples_count = 0;

            /* Write the stream header, if any. */
            ret = oc.WriteHeader();
            if (ret < 0) {
                Console.WriteLine("Error occurred when opening output file: {0}\n",(AVRESULT)ret);
                Environment.Exit(1);
            }
            
            AVPacket pkt = new AVPacket();
            uint start = GetTickCount();
            int[] linesize = new int[1];
            IntPtr[] ptr = new IntPtr[1];
            while (!s_bQuit) {
                if (Console.KeyAvailable)
                {
                    var ki = Console.ReadKey();
                    if (ki.Key == ConsoleKey.Escape)
                    {
                        break;
                    }
                }
                /* Capture screen frame */
                AVFrame tmp_frame = cap.get_frame();
                if (tmp_frame == null)
                {
                    break;
                }
                if (sws_ctx == null)
                {
                    sws_ctx = new SwsContext(tmp_frame, frame, SwsFlags.FastBilinear);
                }
                /* We need to flip vertical image as it captured bottom up */
                linesize[0] = -tmp_frame.linesize[0];
                ptr[0] = ((IntPtr)tmp_frame.data[0]) + (tmp_frame.height - 1) * tmp_frame.linesize[0];

                /* Convert it into required colorspace */
                sws_ctx.Scale(ptr,linesize,0,tmp_frame.height, frame);
                
                /* set pts */
                frame.pts = samples_count++;

                bool got_packet = false;
                /* encode the image */
                ret = encoder.EncodeVideo(pkt, frame, ref got_packet);
                if (ret < 0) {
                    Console.WriteLine("Error encoding video frame: {0}\n", (AVRESULT)(ret));
                    Environment.Exit(1);
                }

                if (got_packet) {
                    ret = write_frame(oc, encoder.time_base, stream, pkt);
                } else {
                    ret = 0;
                }
                pkt.Free();

                var current = GetTickCount() - start;
                uint elapsed = (uint)(samples_count * 1000 / nFramesPerSecond);
                if (elapsed > current)
                {
                    Thread.Sleep((int)(elapsed - current));
                }
            }

            /* Write the trailer, if any. The trailer must be written before you
             * close the CodecContexts open when you wrote the header; otherwise
             * av_write_trailer() may try to use memory that was freed on
             * av_codec_close(). */
            oc.WriteTrailer();

            /* Close the output file. */
            if (oc != null && (int)(fmt.flags & AVfmt.NOFILE) == 0) {
              if (oc.pb != null)
                {
                    oc.pb.Dispose();
                }
            }
            /* free the stream */
            if (oc != null) oc.Dispose();

            if (encoder != null) encoder.Dispose();
            if (frame != null) frame.Dispose();
            if (pkt != null) pkt.Dispose();
            if (sws_ctx != null) sws_ctx.Dispose();
            if (cap != null) cap.Dispose();

            Environment.Exit(0);
        }

    }
}