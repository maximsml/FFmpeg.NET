//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// Example of loading media file with libavformat decoding and resampling
// audio data and output it to playback with WinMM Windows API
//////////////////////////////////////////////////////
using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;
using System.Collections.Generic;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("audio_playback")]
[assembly: AssemblyDescription("Example of loading media file with libavformat decoding and resampling"
+ "audio data and output it to playback with WinMM Windows API")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class audio_playback
    {
        static bool s_bQuit = false;

        private static void Console_CancelKeyPress(object sender, ConsoleCancelEventArgs e)
        {
            s_bQuit = true;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 2)]
        public class WaveFormatEx
        {
            public ushort wFormatTag;
            public ushort nChannels;
            public int nSamplesPerSec;
            public int nAvgBytesPerSec;
            public ushort nBlockAlign;
            public ushort wBitsPerSample;
            public ushort cbSize;
        }

        private class WaveOut : IDisposable
        {
            delegate void WaveOutProcDelegate(IntPtr hwo, uint uMsg,IntPtr dwInstance, IntPtr dwParam1, IntPtr dwParam2);

            public List<IntPtr> m_Process = new List<IntPtr>();
            public List<IntPtr> m_Free = new List<IntPtr>();

            private IntPtr m_hWave = IntPtr.Zero;
            private WaveOutProcDelegate m_Delegate = new WaveOutProcDelegate(waveOutProc);

            private static void waveOutProc(IntPtr hwo, uint uMsg, IntPtr dwInstance, IntPtr dwParam1, IntPtr dwParam2)
            {
                try
                {
                    if (dwInstance != IntPtr.Zero)
                    {
                        WaveOut _wave = (WaveOut)Marshal.GetObjectForIUnknown(dwInstance);

                        switch (uMsg)
                        {
                            case 0x3BD: /* WOM_DONE */
                                if (dwParam1 != IntPtr.Zero)
                                {
                                    WAVEHDR lpHdr = (WAVEHDR)Marshal.PtrToStructure(dwParam1, typeof(WAVEHDR));
                                    if (lpHdr.lpData != IntPtr.Zero)
                                    {
                                        bool found = false;
                                        lock (_wave.m_Process)
                                        {
                                            found = _wave.m_Process.Remove(lpHdr.lpData);
                                        }
                                        if (!found)
                                        {
                                            Marshal.FreeCoTaskMem(lpHdr.lpData);
                                        }
                                        else
                                        {
                                            lock (_wave.m_Free)
                                            {
                                                _wave.m_Free.Add(dwParam1);
                                            }
                                        }
                                    }
                                }
                                break;
                        }
                    }
                }
                catch
                {
                }
            }

            public static WaveOut Open(WaveFormatEx _format)
            {
                IntPtr _ptr = IntPtr.Zero;
                try
                {
                    _ptr = Marshal.AllocCoTaskMem(Marshal.SizeOf(_format));
                    Marshal.StructureToPtr(_format, _ptr, true);
                    WaveOut _wave = new WaveOut();
                    IntPtr _function = Marshal.GetFunctionPointerForDelegate(_wave.m_Delegate);
                    IntPtr _param = Marshal.GetIUnknownForObject(_wave);
                    int WAVE_MAPPER = -1;
                    var _result = waveOutOpen(ref _wave.m_hWave, (uint)WAVE_MAPPER, _ptr, _function, _param, 0x00030000); // CALLBACK_FUNCTION
                    if (_result == 0)
                    {
                        return _wave;
                    }

                }
                finally
                {
                    if (_ptr != IntPtr.Zero)
                    {
                        Marshal.FreeCoTaskMem(_ptr);
                        _ptr = IntPtr.Zero;
                    }
                }
                return null;
            }

            public void Dispose()
            {
                if (m_hWave != IntPtr.Zero)
                {
                    waveOutReset(m_hWave);
                    waveOutClose(m_hWave);
                    m_hWave = IntPtr.Zero;
                }
                lock (m_Process)
                {
                    while (m_Process.Count > 0)
                    {
                        Marshal.FreeCoTaskMem(m_Process[0]);
                        m_Process.RemoveAt(0);
                    }
                }
                lock (m_Free)
                {
                    while (m_Free.Count > 0)
                    {
                        WAVEHDR lpHdr = (WAVEHDR)Marshal.PtrToStructure(m_Free[0], typeof(WAVEHDR));
                        if (lpHdr.lpData != IntPtr.Zero)
                        {
                            Marshal.FreeCoTaskMem(lpHdr.lpData);
                        }
                        m_Free.RemoveAt(0);
                    }
                }
            }

            public int Write(IntPtr _data, int _length, int _flags)
            {
                int _result = 0x00;
                if (_data != IntPtr.Zero && _length > 0)
                {
                    uint _size = (uint)Marshal.SizeOf(typeof(WAVEHDR));
                    WAVEHDR _header = new WAVEHDR();
                    switch (_flags)
                    {
                        case 1: /* begin loop */
                            _header.dwFlags = 0x00000004; // WHDR_BEGINLOOP
                            _header.dwLoops = 0;
                            break;
                        case 2: /* end loop */
                            _header.dwFlags = 0x00000008; //WHDR_ENDLOOP
                            _header.dwLoops = 0;
                            break;
                        case 3: /* loop */
                            _header.dwFlags = 0;
                            _header.dwLoops = 1;
                            break;
                        default:
                            _header.dwFlags = 0;
                            _header.dwLoops = 0;
                            break;
                    }

                    _header.dwUser = IntPtr.Zero;
                    _header.lpNext = IntPtr.Zero;
                    _header.reserved = IntPtr.Zero;
                    _header.dwBufferLength = (uint)(_length);
                    _header.lpData = Marshal.AllocCoTaskMem((int)_header.dwBufferLength);
                    memmove(_header.lpData, _data, _length);
                    _result = waveOutPrepareHeader(m_hWave, _header, _size);
                    if (_result == 0)
                    {
                        _result = waveOutWrite(m_hWave, _header, _size);
                        if (_result == 0)
                        {
                            lock (m_Process)
                            {
                                m_Process.Add(_header.lpData);
                            }
                        }
                    }
                    if (_result < 0)
                    {
                        Marshal.FreeCoTaskMem(_header.lpData);
                    }
                    while (true)
                    {
                        IntPtr p = IntPtr.Zero;
                        lock (m_Free)
                        {
                            if (m_Free.Count == 0)
                            {
                                break;
                            }
                            p = m_Free[0];
                            m_Free.RemoveAt(0);
                        }
                        if (p != IntPtr.Zero)
                        {
                            WAVEHDR lpHdr = (WAVEHDR)Marshal.PtrToStructure(p, typeof(WAVEHDR));
                            if (lpHdr.lpData != IntPtr.Zero)
                            {
                                waveOutUnprepareHeader(m_hWave, lpHdr, _size);
                                Marshal.FreeCoTaskMem(lpHdr.lpData);
                            }
                        }
                    }
                }
                return _result;
            }

            [StructLayout(LayoutKind.Sequential)]
            private class WAVEHDR
            {
                public IntPtr lpData;
                public uint dwBufferLength;
                public uint dwBytesRecorded;
                public IntPtr dwUser;
                public uint dwFlags;
                public uint dwLoops;
                public IntPtr lpNext;
                public IntPtr reserved;
            }

            [DllImport("Winmm.dll")]
            private static extern int waveOutPrepareHeader([In] IntPtr hwo, [In, MarshalAs(UnmanagedType.LPStruct)] WAVEHDR pwh, [In] uint cbwh);

            [DllImport("Winmm.dll")]
            private static extern int waveOutUnprepareHeader([In] IntPtr hwo, [In, MarshalAs(UnmanagedType.LPStruct)] WAVEHDR pwh, [In] uint cbwh);

            [DllImport("Winmm.dll")]
            private static extern int waveOutReset([In] IntPtr hwo);

            [DllImport("Winmm.dll")]
            private static extern int waveOutClose([In] IntPtr hwo);

            [DllImport("Winmm.dll")]
            private static extern int waveOutOpen([In, Out] ref IntPtr phwo, uint uDeviceID, [In] IntPtr pwfx, [In] IntPtr dwCallback, [In] IntPtr dwCallbackInstance, [In] uint fdwOpen);

            [DllImport("Winmm.dll")]
            private static extern int waveOutWrite([In] IntPtr hwo, [In, MarshalAs(UnmanagedType.LPStruct)] WAVEHDR pwh, [In] uint cbwh);
        }

        [DllImport("kernel32.dll", EntryPoint = "RtlMoveMemory")]
        public static extern void memmove(IntPtr Destination, IntPtr Source, [MarshalAs(UnmanagedType.U4)] int Length);

        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("Usage: {0} <input file>\n",
                    Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            Console.CancelKeyPress += Console_CancelKeyPress;

            string filename = args[0];

            var _input = AVFormatContext.OpenInputFile(filename);
            if (_input == null || !_input.FindStreamInfo())
            {
                Console.WriteLine("Could not read the file\n");
                Environment.Exit(1);
            }

            _input.DumpFormat(0, filename, false);

            int idx = _input.FindBestStream(AVMediaType.AUDIO, -1, -1);
            AVStream _stream = idx >= 0 ? _input.streams[idx] : null;
            if (_stream == null)
            {
                Console.WriteLine("Audio stream not found\n");
                Environment.Exit(1);
            }
            /* find audio decoder */
            AVCodecContext _decoder = _stream.codec;
            
            AVCodec codec = AVCodec.FindDecoder(_decoder.codec_id);
            _decoder.sample_fmt = codec.sample_fmts != null ? codec.sample_fmts[0] : AVSampleFormat.FLTP;
            if (!_decoder.Open(codec))
            {
                Console.WriteLine("Could not open codec\n");
                Environment.Exit(1);
            }
            /* Init parse */
            AVCodecParserContext parser = new AVCodecParserContext(codec.id);
            if (!parser._IsValid)
            {
                Console.WriteLine("Parser not found\n");
                Environment.Exit(1);
            }
            AVFrame frame = new AVFrame();
            if (!frame._IsValid)
            {
                Console.WriteLine("Could not allocate audio frame\n");
                Environment.Exit(1);
            }
            // Playback
            WaveOut _wave = null;

            // Resampling context
            SwrContext swr = null;

            AVPacket pkt = new AVPacket();
            AVMemPtr source = null;
            AVMemPtr dest = null;
            bool bReset = true;
            bool bInit = true;
            IntPtr data = IntPtr.Zero;
            int data_size = 0;
            int position = 0;
            WaveFormatEx _wfx = null;
            int nSamples = 0;
            int nPrevSamples = 0;

            /* Reading frames*/
            while ((bool)_input.ReadFrame(pkt))
            {
                /* We interested in only audio */
                if (pkt.stream_index == _stream.index)
                {
                    /* Check whatever require to renew input buffer */
                    bReset = (source == null || (source != null && pkt.size > (source.size - data_size)));
                    if (bReset)
                    {
                        var temp = source;
                        source = new AVMemPtr(data_size + (pkt.size << 1));
                        if (temp)
                        {
                            if (data_size > 0)
                            {
                                memmove(source,temp,data_size);
                            }
                            temp.Dispose();
                        }
                    }
                    memmove(source + data_size,pkt.data,pkt.size);
                    data_size += pkt.size;
                    pkt.Free();
                    IntPtr p = source;
                    bool parsed = false;
                    while (true)
                    {
                        /* Parse input for chunks */
                        int ret = parser.Parse(_decoder, pkt, p, data_size);
                        if (ret <= 0)
                        {
                            if (!parsed && ret < 0)
                            {
                                /* nothing parsed and we got failure - we just shift data and reset parser */
                                parser.Dispose();
                                parser = new AVCodecParserContext(codec.id);
                                p += 1;
                                data_size -= 1;
                                continue;
                            }
                            break;
                        }
                        /* Got chunk of data */
                        parsed = true;
                        p += ret;
                        data_size -= ret;
                        bool got_frame = false;
                        /* Decode it*/
                        ret = _decoder.DecodeAudio(frame, ref got_frame, pkt);
                        if (got_frame)
                        {
                            if (bInit)
                            {
                                /* Initialize resampling if needed */
                                int sample_rate = 0;
                                if ((_decoder.sample_fmt != AVSampleFormat.S16 || _decoder.channels != 2 
                                    || (_decoder.sample_rate != 44100 && _decoder.sample_rate != 48000)) && swr == null)
                                {
                                    sample_rate = _decoder.sample_rate;
                                    if (sample_rate != 44100 && sample_rate != 48000)
                                    {
                                        if (sample_rate % 11025 == 0) sample_rate = 44100;
                                        if (sample_rate %  8000 == 0) sample_rate = 48000;
                                    }
                                    swr = new SwrContext(AVChannelLayout.LAYOUT_STEREO,
                                        AVSampleFormat.S16, sample_rate,
                                        _decoder.channel_layout, _decoder.sample_fmt, _decoder.sample_rate);
                                    swr.Init();
                                }
                                /* Setting up output buffer and format */
                                _wfx = new WaveFormatEx();
                                _wfx.wFormatTag = 1;
                                _wfx.nChannels = 2;
                                _wfx.nSamplesPerSec = sample_rate;
                                _wfx.wBitsPerSample = 16;
                                _wfx.nBlockAlign = (ushort)((_wfx.wBitsPerSample * _wfx.nChannels) >> 3);
                                _wfx.nAvgBytesPerSec = _wfx.nBlockAlign * _wfx.nSamplesPerSec;

                                AVLog.log(AVLogLevel.Info, string.Format("Playback: srate:{0}Hz fmt:{1} chlayout:{2}\n",
                                    (int)sample_rate,
                                    AVSampleFormat.S16.name,
                                    AVChannelLayout.LAYOUT_STEREO.ToString()));

                                dest = new AVMemPtr(_wfx.nAvgBytesPerSec << 1);
                                _wave = WaveOut.Open(_wfx);
                                bInit = false;
                            }
                            /* Fill output buffer */
                            IntPtr _ptr = dest + position;
                            if (swr != null)
                            {
                                int _count = swr.Convert(
                                    new IntPtr[] { _ptr },
                                    ((dest.size - position) / _wfx.nBlockAlign),
                                    frame.data.ToArray(),
                                    frame.nb_samples);
                                if (_count > 0)
                                {
                                    position += (_count * _wfx.nBlockAlign);
                                }
                            }
                            else
                            {
                                memmove(_ptr, frame.data[0], frame.nb_samples * _wfx.nBlockAlign);
                                position += frame.nb_samples * _wfx.nBlockAlign;
                            }
                            nSamples += position / _wfx.nBlockAlign;
                            frame.Free();

                            /* Deliver data chunks */
                            if (nSamples - nPrevSamples >= _wfx.nSamplesPerSec / 2)
                            {
                                Console.WriteLine("Playback Input: {0} Frames, Total: {1} Frames", position / _wfx.nBlockAlign,nSamples);

                                _wave.Write(dest, position, 0x00);
                                if (nPrevSamples != 0)
                                {
                                    Thread.Sleep(position * 950 / _wfx.nAvgBytesPerSec);
                                }
                                nPrevSamples = nSamples;
                                position = 0;
                            }
                        }
                        pkt.Free();
                    }
                    if (data_size > 0)
                    {
                        memmove(source,p,data_size);
                    }
                }
                /* Check For Quit */
                if (Console.KeyAvailable)
                {
                    var ki = Console.ReadKey();
                    if (ki.Key == ConsoleKey.Escape)
                    {
                        break;
                    }
                }
                if (s_bQuit)
                {
                    break;
                }
            }

            if (_wave != null) _wave.Dispose();
            if (source != null) source.Dispose();
            if (dest != null) dest.Dispose();
            if (swr != null) swr.Dispose();
            if (_input != null) _input.Dispose();
            parser.Dispose();
            frame.Dispose();
            pkt.Dispose();
        }
    }
}