//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// libswscale API use example.
//////////////////////////////////////////////////////
using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("scaling_video")]
[assembly: AssemblyDescription("C# libswscale API use example.")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class scaling_video
    {
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

        static void fill_yuv_image(AVPicture pic,
                           int width, int height, int frame_index)
        {
            int x, y;

            /* Y */
            for (y = 0; y < height; y++)
                for (x = 0; x < width; x++)
                    pic.data[0][y * pic.linesize[0] + x] = (byte)(x + y + frame_index * 3);

            /* Cb and Cr */
            for (y = 0; y < height / 2; y++) {
                for (x = 0; x < width / 2; x++) {
                    pic.data[1][y * pic.linesize[1] + x] = (byte)(128 + y + frame_index * 2);
                    pic.data[2][y * pic.linesize[2] + x] = (byte)(64 + x + frame_index * 5);
                }
            }
        }

        static void Main(string[] args)
        {
            int src_w = 320, src_h = 240, dst_w, dst_h;
            AVPixelFormat src_pix_fmt =  AVPixelFormat.YUV420P, dst_pix_fmt = AVPixelFormat.RGB24;
            AVPicture src_data = null;
            AVPicture dst_data = null;
            int dst_bufsize;

            if (args.Length != 2)
            {
                Console.WriteLine("Usage: {0} output_file output_size\n"
                                + "API example program to show how to scale an image with libswscale.\n"
                                + "This program generates a series of pictures, rescales them to the given "
                                + "output_size and saves them to an output file named output_file\n.",
                                 Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            string dst_filename = args[0];
            string dst_size     = args[1];

            int ret = 0;

            if (av_parse_video_size(out dst_w, out  dst_h, dst_size) < 0) {
                Console.WriteLine("Invalid size '{0}', must be in the form WxH or a valid size abbreviation\n",
                        dst_size);
                Environment.Exit(1);
            }

            var dst_file = File.OpenWrite(dst_filename);
            if (dst_file == null) {
                Console.WriteLine("Could not open destination file {0}\n", dst_filename);
                Environment.Exit(1);
            }

            /* create scaling context */
            SwsContext sws_ctx = new SwsContext(src_w, src_h, src_pix_fmt,
                                     dst_w, dst_h, dst_pix_fmt,
                                     SwsFlags.Bilinear);
            if (!sws_ctx._IsValid) {
                Console.WriteLine(
                        "Impossible to create scale context for the conversion "
                        +"fmt:{0} s:{1}x{2} -> fmt:{3} s:{4}x{5}\n",
                        AVPixel.get_pix_fmt_name(src_pix_fmt), src_w, src_h,
                        AVPixel.get_pix_fmt_name(dst_pix_fmt), dst_w, dst_h);
                ret = AVRESULT.EINVAL;
                goto end;
            }

            /* allocate source and destination image buffers */

            src_data = new AVPicture();
            dst_data = new AVPicture();

            if ((ret = src_data.Alloc(src_pix_fmt, src_w, src_h,  16)) < 0) {
                Console.WriteLine("Could not allocate source image\n");
                goto end;
            }

            /* buffer is going to be written to rawvideo file, no alignment */
            if ((ret = dst_data.Alloc(dst_pix_fmt, dst_w, dst_h, 1)) < 0) {
                Console.WriteLine("Could not allocate destination image\n");
                goto end;
            }
            dst_bufsize = ret;
            byte[] data = new byte[dst_bufsize];

            for (int i = 0; i < 100; i++) {
                /* generate synthetic video */
                fill_yuv_image(src_data, src_w, src_h, i);

                /* convert to destination format */
                sws_ctx.Scale(src_data, 0, src_h, dst_data);

                /* write scaled image to file */
                Marshal.Copy((IntPtr)dst_data.data[0],data,0,dst_bufsize);
                dst_file.Write(data,0,dst_bufsize);
            }

            Console.WriteLine("Scaling succeeded. Play the output file with the command:\n"
                   + "ffplay -f rawvideo -pix_fmt {0} -video_size {1}x{2} {3}",
                   AVPixel.get_pix_fmt_name(dst_pix_fmt), dst_w, dst_h, dst_filename);

            end:
            sws_ctx.Dispose();

            if (src_data != null) src_data.Dispose();
            if (dst_data != null) dst_data.Dispose();
        }
    }
}
