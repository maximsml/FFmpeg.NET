//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// libswresample API use example.
//////////////////////////////////////////////////////
using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("resampling_audio")]
[assembly: AssemblyDescription("C# libswresample API use example.")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class resampling_audio
    {
        static int get_format_from_sample_fmt(out string fmt,AVSampleFormat sample_fmt,bool be)
        {
            fmt = "";
            AVSampleFormat[] fmts = new[] { AVSampleFormat.U8, AVSampleFormat.S16, AVSampleFormat.S32, AVSampleFormat.FLT, AVSampleFormat.DBL };
            string[] sbe = new[] { "u8", "s16be", "s32be", "f32be", "f64be" };
            string[] sle = new[] { "u8", "s16le", "s32le", "f32le", "f64le" };

            for (int i = 0; i < fmts.Length; i++)
            {
                if (fmts[i] == sample_fmt)
                {
                    fmt = be ? sbe[i] : sle[i];
                    return 0;
                }
            }
            Console.WriteLine("Sample format {0} is not supported as output format\n",
                    sample_fmt.name);
            return AVRESULT.EINVAL;
        }

        /**
         * Fill dst buffer with nb_samples, generated starting from t.
         */
        static void fill_samples(IntPtr dst, int nb_samples, int nb_channels, int sample_rate, ref double  t)
        {
            AVMemPtr p = new AVMemPtr(dst);
            var dstp = p.doubles;
            int idx = 0;
            int i, j;
            double tincr = 1.0 / sample_rate;
            const double c = 2 * Math.PI * 440.0;

            /* generate sin tone with 440Hz frequency and duplicated channels */
            for (i = 0; i < nb_samples; i++) {
                dstp[idx] = Math.Sin(c * t);
                for (j = 1; j < nb_channels; j++)
                    dstp[idx + j] = dstp[idx + 0];
                idx += nb_channels;
                t += tincr;
            }
        }

        static void Main(string[] args)
        {
            AVChannelLayout src_ch_layout = AVChannelLayout.LAYOUT_STEREO, dst_ch_layout = AVChannelLayout.LAYOUT_SURROUND;
            int src_rate = 48000, dst_rate = 44100;
            IntPtr[] src_data = null;
            IntPtr[] dst_data = null;
            int src_nb_channels = 0, dst_nb_channels = 0;
            int src_linesize, dst_linesize;
            int src_nb_samples = 1024, dst_nb_samples, max_dst_nb_samples;
            AVSampleFormat src_sample_fmt = AVSampleFormat.DBL, dst_sample_fmt = AVSampleFormat.S16;

            if (args.Length < 1)
            {
                Console.WriteLine("Usage: {0} output_file\n"
                                + "API example program to show how to resample an audio stream with libswresample.\n"
                                + "This program generates a series of audio frames, resamples them to a specified "
                                + "output format and rate and saves them to an output file named output_file.\n",
                                Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            string dst_filename = args[0];

            var dst_file = File.OpenWrite(dst_filename);
            if (dst_file == null)
            {
                Console.WriteLine("Could not open destination file {0}\n", dst_filename);
                Environment.Exit(1);
            }
            int ret;
            /* create resampler context */
            SwrContext swr_ctx = new SwrContext();
            if (!swr_ctx._IsValid)
            {
                Console.WriteLine("Could not allocate resampler context\n");
                ret = AVRESULT.ENOMEM;
                goto end;
            }

            /* set options */
            var options = new AVOptions(swr_ctx);

            swr_ctx.InChannelLayout = src_ch_layout;
            options.set_int("in_sample_rate",src_rate,AVOptSearch.None);
            swr_ctx.InSampleFormat = src_sample_fmt;

            swr_ctx.OutChannelLayout = dst_ch_layout;
            options.set_int("out_sample_rate",dst_rate,AVOptSearch.None);
            options.set_sample_fmt("out_sample_fmt",dst_sample_fmt,AVOptSearch.None);

            /* initialize the resampling context */
            if ((ret = swr_ctx.Init()) < 0) {
                Console.WriteLine("Failed to initialize the resampling context\n");
                goto end;
            }

            /* allocate source and destination samples buffers */

            src_nb_channels = src_ch_layout.channels;
            
            ret = AVSamples.alloc(out src_data, out src_linesize, src_nb_channels,
                                                     src_nb_samples, src_sample_fmt, false);
            if (ret < 0) {
                Console.WriteLine("Could not allocate source samples\n");
                goto end;
            }

            /* compute the number of converted samples: buffering is avoided
             * ensuring that the output buffer will contain at least all the
             * converted input samples */
            max_dst_nb_samples = dst_nb_samples =
                (int)AVMath.rescale_rnd(src_nb_samples, dst_rate, src_rate, AVRounding.UP);

            /* buffer is going to be directly written to a rawaudio file, no alignment */
            dst_nb_channels = dst_ch_layout.channels;

            ret = AVSamples.alloc(out dst_data, out dst_linesize, dst_nb_channels,
                                                     dst_nb_samples, dst_sample_fmt, true);
            if (ret < 0) {
                Console.WriteLine("Could not allocate destination samples\n");
                goto end;
            }
            byte[] data = new byte[1024 * 10];
            double t = 0;
            do {
                /* generate synthetic audio */
                fill_samples(src_data[0], src_nb_samples, src_nb_channels, src_rate, ref t);

                /* compute destination number of samples */
                dst_nb_samples = (int)AVMath.rescale_rnd(swr_ctx.GetDelay(src_rate) +
                                                src_nb_samples, dst_rate, src_rate, AVRounding.UP);
                if (dst_nb_samples > max_dst_nb_samples) {
                    AVSamples.free(dst_data);
                    ret = AVSamples.alloc(out dst_data, out dst_linesize, dst_nb_channels,
                                                     dst_nb_samples, dst_sample_fmt, true);
                    if (ret < 0)
                        break;
                    max_dst_nb_samples = dst_nb_samples;
                }

                /* convert to destination format */
                ret = swr_ctx.Convert(dst_data, dst_nb_samples, src_data, src_nb_samples);
                if (ret < 0) {
                    Console.WriteLine("Error while converting\n");
                    goto end;
                }
                int dst_bufsize = AVSamples.get_buffer_size(out dst_linesize, dst_nb_channels, ret, dst_sample_fmt, true);
                if (dst_bufsize < 0) {
                    Console.WriteLine("Could not get sample buffer size\n");
                    goto end;
                }
                Console.WriteLine("t:{0} in:{1} out:{2}", t, src_nb_samples, ret);
                if (dst_bufsize > data.Length)
                {
                    data = new byte[dst_bufsize];
                }
                Marshal.Copy(dst_data[0],data,0,dst_bufsize);
                dst_file.Write(data, 0, dst_bufsize);

            } while (t < 10);

            string fmt;
            if ((ret = get_format_from_sample_fmt(out fmt, dst_sample_fmt,false)) < 0)
                goto end;
            Console.WriteLine("Resampling succeeded. Play the output file with the command:\n"
                    + "ffplay -f {0} -channel_layout {1} -channels {2} -ar {3} {4}\n",
                    fmt, (int)dst_ch_layout, dst_nb_channels, dst_rate, dst_filename);

            end:

            AVSamples.free(src_data);
            AVSamples.free(dst_data);

            if (dst_file != null) dst_file.Dispose();
            if (swr_ctx != null) swr_ctx.Dispose();
        }
    }
}
