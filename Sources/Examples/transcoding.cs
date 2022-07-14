//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// API example for demuxing, decoding, filtering, encoding and muxing
//////////////////////////////////////////////////////
using System;
using System.Reflection;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("transcoding")]
[assembly: AssemblyDescription("C# API example for demuxing, decoding, filtering, encoding and muxing")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class transcoding
    {
        static AVFormatContext ifmt_ctx = null;
        static AVFormatContext ofmt_ctx = null;

        class FilteringContext {
            public AVFilterContext buffersink_ctx = null;
            public AVFilterContext buffersrc_ctx = null;
            public AVFilterGraph filter_graph = null;
            public AVBufferSrc src = null; 
            public AVBufferSink sink = null; 
        }
        
        static FilteringContext[] filter_ctx = null;

        class StreamContext {
            public AVCodecContext dec_ctx = null;
            public AVCodecContext enc_ctx = null;
        }

        static StreamContext[] stream_ctx = null;

        static int open_input_file(string filename)
        {
            int ret;
            int i;

            ifmt_ctx = null;
            if ((ret = AVFormatContext.OpenInput(out ifmt_ctx, filename)) < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot open input file\n");
                return ret;
            }

            if ((ret = ifmt_ctx.FindStreamInfo(null)) < 0) {
                AVLog.log(AVLogLevel.Error, "Cannot find stream information\n");
                return ret;
            }

            stream_ctx = new StreamContext[ifmt_ctx.nb_streams];
            if (stream_ctx == null)
                return AVRESULT.ENOMEM;

            for (i = 0; i < ifmt_ctx.nb_streams; i++) {
                AVStream stream = ifmt_ctx.streams[i];
                AVCodec dec = AVCodec.FindDecoder(stream.codecpar.codec_id);
                AVCodecContext codec_ctx;
                if (dec == null) {
                    AVLog.log(AVLogLevel.Error, string.Format("Failed to find decoder for stream #{0}\n", i));
                    return AVRESULT.DECODER_NOT_FOUND;
                }
                codec_ctx = new AVCodecContext(dec);
                if (!codec_ctx._IsValid) {
                    AVLog.log(AVLogLevel.Error, string.Format("Failed to allocate the decoder context for stream #{0}\n", i));
                    return AVRESULT.ENOMEM;
                }
                ret = stream.codecpar.CopyTo(codec_ctx);
                if (ret < 0) {
                    AVLog.log(AVLogLevel.Error, "Failed to copy decoder parameters to input decoder context "
                           + string.Format("for stream #%{0}\n", i));
                    return ret;
                }
                /* Reencode video & audio and remux subtitles etc. */
                if (codec_ctx.codec_type == AVMediaType.VIDEO
                        || codec_ctx.codec_type == AVMediaType.AUDIO) {
                    if (codec_ctx.codec_type == AVMediaType.VIDEO)
                        codec_ctx.framerate = ifmt_ctx.GuessFrameRate(stream, null);
                    /* Open decoder */
                    ret = codec_ctx.Open(dec);
                    if (ret < 0) {
                        AVLog.log(AVLogLevel.Error, string.Format("Failed to open decoder for stream #{0}\n", i));
                        return ret;
                    }
                }
                stream_ctx[i] = new StreamContext();
                stream_ctx[i].dec_ctx = codec_ctx;
            }
            ifmt_ctx.DumpFormat(0, filename, false);

            return 0;
        }

        static int open_output_file(string filename)
        {
            AVStream out_stream;
            AVStream in_stream;
            AVCodecContext dec_ctx, enc_ctx;
            AVCodec encoder;
            int ret;
            int i;

            ofmt_ctx = AVFormatContext.OpenOutput(filename);
            if (ofmt_ctx == null) {
                AVLog.log(AVLogLevel.Error, "Could not create output context\n");
                return AVRESULT.UNKNOWN;
            }


            for (i = 0; i < ifmt_ctx.nb_streams; i++) {
                out_stream = ofmt_ctx.AddStream();
                if (out_stream == null) {
                    AVLog.log(AVLogLevel.Error, "Failed allocating output stream\n");
                    return AVRESULT.UNKNOWN;
                }

                in_stream = ifmt_ctx.streams[i];
                dec_ctx = stream_ctx[i].dec_ctx;

                if (dec_ctx.codec_type == AVMediaType.VIDEO
                        || dec_ctx.codec_type == AVMediaType.AUDIO) {
                    /* in this example, we choose transcoding to same codec */
                    encoder = AVCodec.FindEncoder(dec_ctx.codec_id);
                    if (encoder == null) {
                        AVLog.log(AVLogLevel.Fatal, "Necessary encoder not found\n");
                        return AVRESULT.INVALIDDATA;
                    }
                    enc_ctx = new AVCodecContext(encoder);
                    if (!enc_ctx._IsValid) {
                        AVLog.log(AVLogLevel.Fatal, "Failed to allocate the encoder context\n");
                        return AVRESULT.ENOMEM;
                    }

                    /* In this example, we transcode to same properties (picture size,
                     * sample rate etc.). These properties can be changed for output
                     * streams easily using filters */
                    if (dec_ctx.codec_type == AVMediaType.VIDEO) {
                        enc_ctx.height = dec_ctx.height;
                        enc_ctx.width = dec_ctx.width;
                        enc_ctx.sample_aspect_ratio = dec_ctx.sample_aspect_ratio;
                        /* take first format from list of supported formats */
                        if (encoder.pix_fmts != null && encoder.pix_fmts.Length > 0)
                            enc_ctx.pix_fmt = encoder.pix_fmts[0];
                        else
                            enc_ctx.pix_fmt = dec_ctx.pix_fmt;
                        /* video time_base can be set to whatever is handy and supported by encoder */
                        enc_ctx.time_base = dec_ctx.framerate.inv_q();
                    } else {
                        enc_ctx.sample_rate = dec_ctx.sample_rate;
                        enc_ctx.channel_layout = dec_ctx.channel_layout;
                        enc_ctx.channels = enc_ctx.channel_layout.channels;
                        /* take first format from list of supported formats */
                        enc_ctx.sample_fmt = encoder.sample_fmts[0];
                        enc_ctx.time_base = new AVRational(1, enc_ctx.sample_rate);
                    }

                    if ((int)(ofmt_ctx.oformat.flags & AVfmt.GLOBALHEADER) != 0)
                        enc_ctx.flags |= AVCodecFlag.GLOBAL_HEADER;

                    /* Second parameter can be used to pass settings to encoder */
                    ret = enc_ctx.Open(encoder, null);
                    if (ret < 0) {
                        AVLog.log(AVLogLevel.Error, string.Format("Cannot open video encoder for stream #{0}\n", i));
                        return ret;
                    }
                    ret = out_stream.codecpar.FromContext(enc_ctx);
                    if (ret < 0) {
                        AVLog.log(AVLogLevel.Error, string.Format("Failed to copy encoder parameters to output stream #{0}\n", i));
                        return ret;
                    }

                    out_stream.time_base = enc_ctx.time_base;
                    stream_ctx[i].enc_ctx = enc_ctx;
                } else if (dec_ctx.codec_type == AVMediaType.UNKNOWN) {
                    AVLog.log(AVLogLevel.Fatal, string.Format("Elementary stream #%{0} is of unknown type, cannot proceed\n", i));
                    return AVRESULT.INVALIDDATA;
                } else {
                    /* if this stream must be remuxed */
                    ret = in_stream.codecpar.CopyTo(out_stream.codecpar);
                    if (ret < 0) {
                        AVLog.log(AVLogLevel.Error, string.Format("Copying parameters for stream #{0} failed\n", i));
                        return ret;
                    }
                    out_stream.time_base = in_stream.time_base;
                }

            }
            ofmt_ctx.DumpFormat(0, filename, true);

            if ((int)(ofmt_ctx.oformat.flags & AVfmt.NOFILE) == 0) {
                ofmt_ctx.pb = new AVIOContext(filename,AvioFlag.WRITE);
                if (!ofmt_ctx.pb._IsValid) {
                    AVLog.log(AVLogLevel.Error, string.Format("Could not open output file '{0}'", filename));
                    return AVRESULT.EEXIST;
                }
            }

            /* init muxer, write output file header */
            ret = ofmt_ctx.WriteHeader();
            if (ret < 0) {
                AVLog.log(AVLogLevel.Error, "Error occurred when opening output file\n");
                return ret;
            }

            return 0;
        }

        static int init_filter(FilteringContext fctx, AVCodecContext dec_ctx,
                AVCodecContext enc_ctx, string filter_spec)
        {
            int ret = 0;
            AVFilter buffersrc = null;
            AVFilter buffersink = null;
            AVFilterContext buffersrc_ctx = null;
            AVFilterContext buffersink_ctx = null;
            AVFilterInOut outputs = new AVFilterInOut();
            AVFilterInOut inputs  = new AVFilterInOut();
            AVFilterGraph filter_graph = new AVFilterGraph();

            if (!outputs._IsValid || !inputs._IsValid || !filter_graph._IsValid) {
                ret = AVRESULT.ENOMEM;
                goto end;
            }

            if (dec_ctx.codec_type ==  AVMediaType.VIDEO) {
                buffersrc = AVFilter.GetByName("buffer");
                buffersink = AVFilter.GetByName("buffersink");
                if (!buffersrc._IsValid || !buffersink._IsValid) {
                    AVLog.log(AVLogLevel.Error, "filtering source or sink element not found\n");
                    ret = AVRESULT.UNKNOWN;
                    goto end;
                }

                string args = string.Format(
                        "video_size={0}x{1}:pix_fmt={2}:time_base={3}/{4}:pixel_aspect={5}/{6}",
                        dec_ctx.width, dec_ctx.height, (int)dec_ctx.pix_fmt,
                        dec_ctx.time_base.num, dec_ctx.time_base.den,
                        dec_ctx.sample_aspect_ratio.num,
                        dec_ctx.sample_aspect_ratio.den);

                ret = filter_graph.CreateFilter(out buffersrc_ctx, buffersrc, "in", args);
                if (ret < 0) {
                    AVLog.log(AVLogLevel.Error, "Cannot create buffer source\n");
                    goto end;
                }

                ret = filter_graph.CreateFilter(out buffersink_ctx, buffersink, "out");
                if (ret < 0) {
                    AVLog.log(AVLogLevel.Error, "Cannot create buffer sink\n");
                    goto end;
                }

                int[] pix_fmts = new int[] { (int)enc_ctx.pix_fmt };
                var options = new AVOptions(buffersink_ctx);
                ret = options.set_int_list("pix_fmts", pix_fmts, AVOptSearch.CHILDREN);
                if (ret < 0) {
                    AVLog.log(AVLogLevel.Error, "Cannot set output pixel format\n");
                    goto end;
                }
            } else if (dec_ctx.codec_type == AVMediaType.AUDIO) {
                buffersrc = AVFilter.GetByName("abuffer");
                buffersink = AVFilter.GetByName("abuffersink");
                if (buffersrc == null || buffersink == null) {
                    AVLog.log(AVLogLevel.Error, "filtering source or sink element not found\n");
                    ret = AVRESULT.UNKNOWN;
                    goto end;
                }

                if (dec_ctx.channel_layout == AVChannelLayout.UNKNOWN)
                    dec_ctx.channel_layout =
                        AVChannelLayout.get_default_channel_layout(dec_ctx.channels);
                string args = string.Format(
                        "time_base={0}/{1}:sample_rate={2}:sample_fmt={3}:channel_layout=0x{4:X}",
                        dec_ctx.time_base.num, dec_ctx.time_base.den, dec_ctx.sample_rate,
                        dec_ctx.sample_fmt.name,
                        (long)dec_ctx.channel_layout);
                ret = filter_graph.CreateFilter(out buffersrc_ctx, buffersrc, "in",args);
                if (ret < 0) {
                    AVLog.log(AVLogLevel.Error, "Cannot create audio buffer source\n");
                    goto end;
                }

                ret = filter_graph.CreateFilter(out buffersink_ctx, buffersink, "out");
                if (ret < 0) {
                    AVLog.log(AVLogLevel.Error, "Cannot create audio buffer sink\n");
                    goto end;
                }

                var options = new AVOptions(buffersink_ctx);

                ret = options.set_int_list("sample_fmts", 
                    new int[] { (int)enc_ctx.sample_fmt }, 
                    AVOptSearch.CHILDREN);
                if (ret < 0) {
                    AVLog.log(AVLogLevel.Error, "Cannot set output sample format\n");
                    goto end;
                }

                ret = options.set_int_list("channel_layouts", 
                    new long[] { (long)enc_ctx.channel_layout }, 
                    AVOptSearch.CHILDREN);
                if (ret < 0) {
                    AVLog.log(AVLogLevel.Error, "Cannot set output channel layout\n");
                    goto end;
                }

                ret = options.set_int_list("sample_rates", 
                    new int[] { (int)enc_ctx.sample_rate }, 
                    AVOptSearch.CHILDREN);
                if (ret < 0) {
                    AVLog.log(AVLogLevel.Error, "Cannot set output sample rate\n");
                    goto end;
                }
            } else {
                ret = AVRESULT.UNKNOWN;
                goto end;
            }

            /* Endpoints for the filter graph. */
            outputs.name       = "in";
            outputs.filter_ctx = buffersrc_ctx;
            outputs.pad_idx    = 0;
            outputs.next       = null;

            inputs.name       = "out";
            inputs.filter_ctx = buffersink_ctx;
            inputs.pad_idx    = 0;
            inputs.next       = null;

            if (string.IsNullOrEmpty(outputs.name) || string.IsNullOrEmpty(inputs.name)) {
                ret = AVRESULT.ENOMEM;
                goto end;
            }

            if ((ret = filter_graph.ParsePtr(filter_spec,inputs, outputs)) < 0)
                goto end;

            if ((ret = filter_graph.Config()) < 0)
                goto end;

            /* Fill FilteringContext */
            fctx.buffersrc_ctx = buffersrc_ctx;
            fctx.buffersink_ctx = buffersink_ctx;
            fctx.filter_graph = filter_graph;

            end:
            if (inputs != null) inputs.Dispose();
            if (outputs != null) outputs.Dispose();

            return ret;
        }

        static int init_filters()
        {
            string filter_spec;
            int i;
            int ret;
            filter_ctx = new FilteringContext[ifmt_ctx.nb_streams];
            if (filter_ctx == null)
                return AVRESULT.ENOMEM;

            for (i = 0; i < ifmt_ctx.nb_streams; i++) {
                filter_ctx[i] = new FilteringContext();
                filter_ctx[i].buffersrc_ctx  = null;
                filter_ctx[i].buffersink_ctx = null;
                filter_ctx[i].filter_graph   = null;
                if (!(ifmt_ctx.streams[i].codecpar.codec_type == AVMediaType.AUDIO
                        || ifmt_ctx.streams[i].codecpar.codec_type == AVMediaType.VIDEO))
                    continue;


                if (ifmt_ctx.streams[i].codecpar.codec_type == AVMediaType.VIDEO)
                    filter_spec = "null"; /* passthrough (dummy) filter for video */
                else
                    filter_spec = "anull"; /* passthrough (dummy) filter for audio */
                ret = init_filter(filter_ctx[i], stream_ctx[i].dec_ctx,
                        stream_ctx[i].enc_ctx, filter_spec);

                filter_ctx[i].src = new AVBufferSrc(filter_ctx[i].buffersrc_ctx);
                filter_ctx[i].sink = new AVBufferSink(filter_ctx[i].buffersink_ctx);
                if (ret != 0)
                    return ret;
            }
            return 0;
        }

        static int encode_write_frame(AVFrame filt_frame, int stream_index, out bool got_frame) {
            int ret;
            got_frame = false;
            AVPacket enc_pkt = new AVPacket();

            AVLog.log(AVLogLevel.Info, "Encoding frame\n");
            /* encode filtered frame */
            enc_pkt.data = null;
            enc_pkt.size = 0;
            if (ifmt_ctx.streams[stream_index].codecpar.codec_type == AVMediaType.VIDEO){
                ret = stream_ctx[stream_index].enc_ctx.EncodeVideo(enc_pkt, filt_frame, ref got_frame);
            }
            else {
                ret = stream_ctx[stream_index].enc_ctx.EncodeAudio(enc_pkt, filt_frame, ref got_frame);
            }
            
            if (filt_frame != null) filt_frame.Free();
            if (ret < 0) {
                enc_pkt.Dispose();
                return ret;
            }
            if (!got_frame) {
                enc_pkt.Dispose();
                return 0;
            }
            /* prepare packet for muxing */
            enc_pkt.stream_index = stream_index;
            enc_pkt.RescaleTS(stream_ctx[stream_index].enc_ctx.time_base,
                                 ofmt_ctx.streams[stream_index].time_base);

            AVLog.log(AVLogLevel.Debug, "Muxing frame\n");
            /* mux encoded frame */
            ret = ofmt_ctx.InterleavedWriteFrame(enc_pkt);
            enc_pkt.Dispose();
            return ret;
        }

        static int filter_encode_write_frame(AVFrame frame, int stream_index)
        {
            int ret;

            AVLog.log(AVLogLevel.Info, "Pushing decoded frame to filters\n");
            /* push the decoded frame into the filtergraph */
            ret = filter_ctx[stream_index].src.add_frame(frame);
            if (ret < 0) {
                AVLog.log(AVLogLevel.Error, "Error while feeding the filtergraph\n");
                return ret;
            }

            AVFrame filt_frame = new AVFrame();
            if (!filt_frame._IsValid) {
                return AVRESULT.ENOMEM;
            }
            /* pull filtered frames from the filtergraph */
            while (true) {
                filt_frame.Free();
                AVLog.log(AVLogLevel.Info, "Pulling filtered frame from filters\n");
                ret = filter_ctx[stream_index].sink.get_frame(filt_frame);
                if (ret < 0) {
                    /* if no more frames for output - returns AVERROR(EAGAIN)
                     * if flushed and no more frames for output - returns AVERROR_EOF
                     * rewrite retcode to 0 to show it as normal procedure completion
                     */
                    if (ret == AVRESULT.EAGAIN || ret == AVRESULT.EOF)
                        ret = 0;
                    filt_frame.Free();
                    break;
                }
                bool got_frame;
                filt_frame.pict_type = AVPictureType.NONE;
                ret = encode_write_frame(filt_frame, stream_index, out got_frame);
                if (ret < 0)
                    break;
            }
            filt_frame.Dispose();
            return ret;
        }

        static int flush_encoder(int stream_index)
        {
            int ret;
            bool got_frame;

            if ((stream_ctx[stream_index].enc_ctx.codec.capabilities & AVCodecCap.DELAY) == 0)
                return 0;

            while (true) {
                AVLog.log(AVLogLevel.Info, string.Format("Flushing stream #{0} encoder\n", stream_index));
                ret = encode_write_frame(null, stream_index, out got_frame);
                if (ret < 0)
                    break;
                if (!got_frame)
                    return 0;
            }
            return ret;
        }

        static void Main(string[] args)
        {
            AVPacket packet = new AVPacket();
            AVFrame frame = new AVFrame();
            int stream_index = 0;
            int ret;

            if (args.Length != 2)
            {
                Console.WriteLine("Usage: {0} <input file> <output file>\n"
                                ,Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            if ((ret = open_input_file(args[0])) < 0)
                goto end;
            if ((ret = open_output_file(args[1])) < 0)
                goto end;
            if ((ret = init_filters()) < 0)
                goto end;

            /* read all packets */
            while (true) {
                if ((ret = ifmt_ctx.ReadFrame(packet)) < 0)
                    break;
                stream_index = packet.stream_index;
                var type = ifmt_ctx.streams[packet.stream_index].codecpar.codec_type;
                AVLog.log(AVLogLevel.Debug, string.Format("Demuxer gave frame of stream_index {0}\n",stream_index));
                bool got_frame = false;
                if (filter_ctx[stream_index].filter_graph != null) {
                    AVLog.log(AVLogLevel.Debug, "Going to reencode&filter the frame\n");
                    frame.Free();
                    packet.RescaleTS(ifmt_ctx.streams[stream_index].time_base,
                                     stream_ctx[stream_index].dec_ctx.time_base);
                    if (type == AVMediaType.VIDEO){
                        ret = stream_ctx[stream_index].dec_ctx.DecodeVideo(frame, ref got_frame, packet);
                    }
                    else{
                        ret = stream_ctx[stream_index].dec_ctx.DecodeAudio(frame, ref got_frame, packet);
                    }
                    if (ret < 0) {
                        frame.Free();
                        AVLog.log(AVLogLevel.Error, "Decoding failed\n");
                        break;
                    }

                    if (got_frame) {
                        frame.pts = frame.best_effort_timestamp;
                        ret = filter_encode_write_frame(frame, stream_index);
                        frame.Free();
                        if (ret < 0)
                            goto end;
                    } else {
                        frame.Free();
                    }
                } else {
                    /* remux this frame without reencoding */
                    packet.RescaleTS(ifmt_ctx.streams[stream_index].time_base,
                                     ofmt_ctx.streams[stream_index].time_base);

                    ret = ofmt_ctx.InterleavedWriteFrame(packet);
                    if (ret < 0)
                        goto end;
                }
                packet.Free();
            }

            /* flush filters and encoders */
            for (int i = 0; i < ifmt_ctx.nb_streams; i++) {
                /* flush filter */
                if (filter_ctx[i].filter_graph == null)
                    continue;
                ret = filter_encode_write_frame(null, i);
                if (ret < 0) {
                    AVLog.log(AVLogLevel.Error, "Flushing filter failed\n");
                    goto end;
                }

                /* flush encoder */
                ret = flush_encoder(i);
                if (ret < 0) {
                    AVLog.log(AVLogLevel.Error, "Flushing encoder failed\n");
                    goto end;
                }
            }

            ofmt_ctx.WriteTrailer();

            end:

            packet.Dispose();
            frame.Dispose();
            for (int i = 0; i < ifmt_ctx.nb_streams; i++) {
                stream_ctx[i].dec_ctx.Dispose();
                if (ofmt_ctx != null && ofmt_ctx.nb_streams > i && ofmt_ctx.streams[i] != null && stream_ctx[i].enc_ctx != null)
                    stream_ctx[i].enc_ctx.Dispose();
                if (filter_ctx != null && filter_ctx[i].filter_graph != null)
                    filter_ctx[i].filter_graph.Dispose();
            }
            ifmt_ctx.Dispose();
            if (ofmt_ctx != null && (int)(ofmt_ctx.oformat.flags & AVfmt.NOFILE) != 0)
                ofmt_ctx.pb.Dispose();
            ofmt_ctx.Dispose();

            if (ret < 0)
            AVLog.log(AVLogLevel.Error, string.Format("Error occurred: {0}\n", (AVRESULT)(ret)));

            Environment.Exit(ret < 0 ? 1 : 0);
        }
    }
}
