//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// Shows how the metadata API can be used in application programs.
//////////////////////////////////////////////////////
using System;
using System.Reflection;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("metadata")]
[assembly: AssemblyDescription("C# Shows how the metadata API can be used in application programs.")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class metadata
    {
        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("Usage: {0} <input file> <codec name>\n"
                                + "example program to demonstrate the use of the libavformat metadata API.\n", 
                                Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }
            int ret;
            AVFormatContext fmt_ctx = null;
            if ((ret = AVFormatContext.OpenInput(out fmt_ctx, args[0])) != 0)
            {
                Environment.Exit(ret);
            }

            if ((ret = fmt_ctx.FindStreamInfo()) < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot find stream information\n");
                Environment.Exit(ret);
            }

            AVDictionary metadata = fmt_ctx.metadata;
            if (metadata != null)
            {
                foreach (AVDictionaryEntry tag in metadata)
                {
                    Console.WriteLine("{0}={1}", tag.key, tag.value);
                }
            }
            fmt_ctx.Dispose();
        }
    }
}
