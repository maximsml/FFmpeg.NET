//////////////////////////////////////////////////////
// FFMPEG .NET Wrapper Examples
// Copyright (C) 2022 Maxim Kartavenkov (mkartavenkov@gmail.com)
// 
// Example of loading media file with libavformat decoding and scaling
// video frames and output it to playback with Winforms and GDI+ API
//////////////////////////////////////////////////////
using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
//////////////////////////////////////////////////////
[assembly: AssemblyTitle("video_playback")]
[assembly: AssemblyDescription("Example of loading media file with libavformat decoding and scaling"
+ "video frames and output it to playback with Winforms and GDI+ API")]
//////////////////////////////////////////////////////
namespace FFmpeg.NET
{
    class video_playback
    {
        [DllImport("Kernel32.dll")]
        private static extern uint GetTickCount();

        static bool s_bQuit = false;
        static bool s_bReset = false;

        private static void Console_CancelKeyPress(object sender, ConsoleCancelEventArgs e)
        {
            s_bQuit = true;
        }

        // Preview Form
        class PlaybackForm : Form
        {
            // Windows Messaging Thread
            Thread m_Thread = null;
            // Internal quit flag
            bool m_bQuit = false;
            // Image to draw
            Bitmap m_pBitmap = null;

            public PlaybackForm()
            {
                
            }

            protected override void OnClosed(EventArgs e)
            {
                s_bQuit = true;
                base.OnClosed(e);
            }

            protected override void OnShown(EventArgs e)
            {
                base.OnShown(e);
                if (this.Left + this.Width > Screen.PrimaryScreen.WorkingArea.Width)
                {
                    this.Left = Screen.PrimaryScreen.WorkingArea.Width - this.Width;
                }
                if (this.Top + this.Height > Screen.PrimaryScreen.WorkingArea.Height)
                {
                    this.Top = Screen.PrimaryScreen.WorkingArea.Height - this.Height;
                }
            }

            protected override void OnClientSizeChanged(EventArgs e)
            {
                s_bReset = true;
                base.OnClientSizeChanged(e);
            }

            protected override void OnPaintBackground(PaintEventArgs e)
            {
                if (!s_bReset)
                {
                    if (m_pBitmap != null)
                    {
                        lock (m_pBitmap)
                        {
                            e.Graphics.DrawImageUnscaled(m_pBitmap,0,0);
                        }
                    }
                }
            }

            void FormThread(object target)
            {
                PlaybackForm form = (PlaybackForm)target;
                form.BackColor = Color.Black;
                form.Show();
                while (!s_bQuit && !m_bQuit)
                {
                    Application.DoEvents();
                }
                form.Dispose();
            }

            public void Start(Bitmap _target)
            {
                m_pBitmap = _target;
                m_Thread = new Thread(new ParameterizedThreadStart(FormThread));
                m_Thread.Start(this);
            }
            public void Stop()
            {
                m_bQuit = true;
                if (m_Thread != null) m_Thread.Join();
            }
            public void Reset(Bitmap _target)
            {
                m_pBitmap = _target;
            }
        }

        static void Main(string[] args)
        {
            int nDisplayFPS = 15;

            if (args.Length < 1)
            {
                Console.WriteLine("Usage: {0} <input file> <display fps>\n",
                    Assembly.GetExecutingAssembly().GetName().Name + ".exe");
                Environment.Exit(1);
            }

            Console.CancelKeyPress += Console_CancelKeyPress;

            string filename = args[0];
            if (args.Length > 1)
            {
                int.TryParse(args[1],out nDisplayFPS);
            }

            var _input = AVFormatContext.OpenInputFile(filename);
            if (_input == null || !_input.FindStreamInfo())
            {
                Console.WriteLine("Could not read the file\n");
                Environment.Exit(1);
            }

            _input.DumpFormat(0, filename, false);

            int idx = _input.FindBestStream(AVMediaType.VIDEO, -1, -1);
            AVStream _stream = idx >= 0 ? _input.streams[idx] : null;
            if (_stream == null)
            {
                Console.WriteLine("Audio stream not found\n");
                Environment.Exit(1);
            }
            /* find video decoder */
            AVCodecContext _decoder = _stream.codec;
            AVCodec codec = AVCodec.FindDecoder(_decoder.codec_id);
            _decoder.sample_fmt = codec.sample_fmts != null ? codec.sample_fmts[0] : AVSampleFormat.FLTP;
            if (!_decoder.Open(codec))
            {
                Console.WriteLine("Could not open codec\n");
                Environment.Exit(1);
            }
            AVFrame frame = new AVFrame();
            if (!frame._IsValid)
            {
                Console.WriteLine("Could not allocate video frame\n");
                Environment.Exit(1);
            }
            // Playback
            PlaybackForm form = new PlaybackForm();
            form.Text = filename;

            AVPacket pkt = new AVPacket();
            AVMemPtr source = null;
            AVMemPtr dest = null;
            Bitmap _target = null;
            SwsContext sws_ctx = null;
            uint start = GetTickCount();
            int[] linesize = new int[1];
            IntPtr[] ptr = new IntPtr[1];
            AVRational display = new AVRational(1, 1000);
            AVRational frame_rate = null;
            long start_playback = AVUtil.NOPTS_VALUE;
            long nFramesDrawn = 0;
            long nFrames = 0;
            bool bDisplayFrame = true;

            /* Reading frames*/
            while ((bool)_input.ReadFrame(pkt))
            {
                /* We interested in only video */
                if (pkt.stream_index == _stream.index)
                {
                    bool got_frame = false;
                    /* Decode it*/
                    int ret = _decoder.DecodeVideo(frame, ref got_frame, pkt);
                    if (got_frame)
                    {
                        /* Setup new image or viewport updated */
                        if (_target == null || s_bReset)
                        {
                            /* Initialize scaling context */
                            int borderX = form.Width - form.ClientSize.Width;
                            int borderY = form.Height - form.ClientSize.Height;

                            int width = frame.width;
                            int height = frame.height;
                            if (form.Visible)
                            {
                                width = form.ClientSize.Width;
                                height = form.ClientSize.Height;
                            }
                            else
                            {
                                if (width + borderX > Screen.PrimaryScreen.WorkingArea.Width)
                                {
                                    width = Screen.PrimaryScreen.WorkingArea.Width - borderX;
                                }
                                if (height + borderY > Screen.PrimaryScreen.WorkingArea.Height)
                                {
                                    height = Screen.PrimaryScreen.WorkingArea.Height - borderY;
                                }
                            }
                            if (width > 0 && height > 0)
                            {
                                var old = _target;
                                if (sws_ctx != null) sws_ctx.Dispose();
                                sws_ctx = new SwsContext(frame.width, frame.height, (AVPixelFormat)frame.format,
                                    width, height, AVPixelFormat.BGRA,
                                    SwsFlags.FastBilinear);

                                _target = new Bitmap(frame.width, frame.height, PixelFormat.Format32bppRgb);
                                if (!form.Visible)
                                {
                                    form.ClientSize = new Size(width, height);
                                    form.Start(_target);
                                }
                                else
                                {
                                    form.Reset(_target);
                                }
                                if (old != null) old.Dispose();
                            }
                            s_bReset = false;
                        }
                        /* Fill output image */
                        if (bDisplayFrame)
                        {
                            /* Skip unnecessary processing */
                            if (form.WindowState != FormWindowState.Minimized)
                            {
                                lock (_target)
                                {
                                    var data = _target.LockBits(new Rectangle(0, 0, _target.Width, _target.Height), ImageLockMode.WriteOnly, PixelFormat.Format32bppRgb);
                                    try
                                    {
                                        ptr[0] = data.Scan0;
                                        linesize[0] = data.Stride;
                                        sws_ctx.Scale(frame, 0, frame.height, ptr, linesize);
                                    }
                                    finally
                                    {
                                        _target.UnlockBits(data);
                                    }
                                }
                                form.Invalidate();
                            }
                            nFramesDrawn++;
                        }
                        nFrames++;
                        long elapsed = 0;
                        if (frame.pts == AVUtil.NOPTS_VALUE)
                        {
                            if (frame_rate == null)
                            {
                                frame_rate = new AVRational(_stream.r_frame_rate.den, _stream.r_frame_rate.num);
                            }
                            elapsed = AVMath.rescale_q(nFrames, frame_rate, display);
                        }
                        else
                        {
                            elapsed = AVMath.rescale_q(frame.pts, _stream.time_base, display);
                        }
                        if (start_playback == AVUtil.NOPTS_VALUE)
                        {
                            start_playback = elapsed;
                        }
                        elapsed -= start_playback;
                        var current = GetTickCount() - start;
                        Thread.Sleep(elapsed > (long)current ? (int)(elapsed - current) : 5);
                        bDisplayFrame = nDisplayFPS > 0 && current >= nFramesDrawn * 1000 / nDisplayFPS;
                        Console.WriteLine("Frames Input {0} Frames Displayed {1} FPS {2} Sleep {3}",
                            nFrames,nFramesDrawn,nFramesDrawn * 1000.0f / current, elapsed > (long)current ? (int)(elapsed - current) : 0);

                        frame.Free();
                    }
                }
                pkt.Free();
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
            s_bQuit = true;
            if (form != null) form.Stop();
            if (_target != null) _target.Dispose();
            if (sws_ctx != null) sws_ctx.Dispose();
            if (source != null) source.Dispose();
            if (dest != null) dest.Dispose();
            if (_input != null) _input.Dispose();
            frame.Dispose();
            pkt.Dispose();
        }
    }
}