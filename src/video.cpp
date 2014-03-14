/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 UCAPA Team and other contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

#include <video.h>

namespace ucapa{

    Video::Video(const std::string& droneIP, unsigned short VideoPort)
        : m_droneIP(droneIP)
        , m_videoPort(VideoPort)
        // Init Video stream receiving
        , m_pFormatCtx(nullptr)
        , m_pCodecCtx(nullptr)
        , m_pFrame(nullptr)
        , m_pFrameRGB(nullptr)
        , m_pFrameBufferReturned(nullptr)
        , m_bufferRGB(nullptr)
        , m_pConvertCtx(nullptr)
        , m_isActive(false)
        , m_possiblyDeconnected(false)
        , m_firstConnection(true)
        , m_terminate(false)
    {
        try
        {
            // Prepare all codecs that light be used
            av_register_all();
            avcodec_register_all();
            avformat_network_init();
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }

    Video::~Video()
    {
        m_terminate = true;
        if(m_launcherReconnectThread.joinable())
            m_launcherReconnectThread.join();
        stop();
    }


    void Video::setCallbackInitFunc(std::function<void()> callbackInitFunc)
    {
        m_callbackInitFunc = callbackInitFunc;
    }


    int Video::init()
    {
        m_isActive = false;
        if (m_videoThread.joinable())
            m_videoThread.join();

        try {
            if (m_firstConnection)
            {
                m_firstConnection = false;
                if (m_callbackInitFunc)
                    m_callbackInitFunc();
            }

            // Prepare Name of the stream to open
            std::ostringstream oss;
            oss << "tcp://" << m_droneIP << ":" << 5555;

            // Try to open video stream
            if (avformat_open_input(&m_pFormatCtx, oss.str().c_str(), nullptr, nullptr) != 0)
            {
                std::cerr << "Could not open the video file" << std::endl;
                return -1;
            }

            // Recover stream information
            avformat_find_stream_info(m_pFormatCtx, nullptr);

            // Dump information to standard output
            av_dump_format(m_pFormatCtx, 0, oss.str().c_str(), 0);

            // Find the decoder for the video stream
            m_pCodecCtx = m_pFormatCtx->streams[0]->codec;
            AVCodec *m_pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
            if (m_pCodec == nullptr)
            {
                std::cerr << "Finding Codec failed" << std::endl;
                return -2;
            }

            // Open codec
            if (avcodec_open2(m_pCodecCtx, m_pCodec, nullptr) < 0)
            {
                std::cerr << "Opening Codec failed"  << std::endl;
                return -3;
            }

            // Allocate video frames and a buffer
            m_pFrame = av_frame_alloc();
            m_pFrameRGB = av_frame_alloc();
            m_bufferRGB = (uint8_t*)av_mallocz(avpicture_get_size(PIX_FMT_RGB24, m_pCodecCtx->width, m_pCodecCtx->height) * sizeof(uint8_t));

            // Fill the frame with the previous buffer
            avpicture_fill((AVPicture*)m_pFrameRGB, m_bufferRGB, PIX_FMT_RGB24, m_pCodecCtx->width, m_pCodecCtx->height);

            // Convert frame to RGB
            m_pConvertCtx = sws_getContext(m_pCodecCtx->width, m_pCodecCtx->height, m_pCodecCtx->pix_fmt, m_pCodecCtx->width, m_pCodecCtx->height, PIX_FMT_RGB24, SWS_SPLINE, nullptr, nullptr, nullptr);

            // Init return buffer size with the stream dim
            m_pFrameBufferReturned = new uint8_t[m_pCodecCtx->width * m_pCodecCtx->height * 3];

            m_possiblyDeconnected = false;
            m_videoThread = std::thread([this] () {
                                                    try {
                                                        m_isActive = true;
                                                        m_possiblyDeconnected = false;

                                                        if (m_isActive)
                                                            decode();
                                                        while (m_isActive && !m_terminate)
                                                        {
                                                            std::this_thread::sleep_for(std::chrono::milliseconds(16));
                                                            decode();
                                                        }

                                                        stopReconnectionThread();

                                                        release();

                                                        if (!m_terminate)
                                                        {
                                                            reconnect();
                                                        }
                                                    }
                                                    catch (...)
                                                    {
                                                        std::cerr << "Video Thread : Error in the Thread" << std::endl;
                                                        if (!m_terminate)
                                                        {
                                                            reconnect();
                                                        }
                                                    }
                                                });
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
            return 0;
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << "\n";
            m_isActive = false;
            return -256;
        }
    }

    void Video::reconnect()
    {
        if(m_launcherReconnectThread.joinable())
            m_launcherReconnectThread.join();

        m_launcherReconnectThread = std::thread([this] () {
                                                                try
                                                                {
                                                                    stop();
                                                                    tryToConnect();
                                                                }
                                                                catch(...)
                                                                {
                                                                    std::cerr << "Launcher Reconnect Thread : Error in the Thread" << std::endl;
                                                                }
                                                           });
    }

    void Video::decode()
    {
        m_videoMutex.lock();
        AVPacket packet;
        try
        {
            // get video frames
            av_init_packet(&packet);

            // Allocate the frame
            avpicture_fill((AVPicture*)m_pFrameRGB, m_bufferRGB, PIX_FMT_RGB24, m_pCodecCtx->width, m_pCodecCtx->height);

            int frameDecoded;
            // Decoding of a frame
            // read frame
            if(av_read_frame(m_pFormatCtx, &packet) < 0)
            {
                std::cerr << "Could not read frame !" << std::endl;
                av_free_packet(&packet);
                if (m_possiblyDeconnected)
                    m_isActive = false;
                else
                    m_possiblyDeconnected = true;
            }
            // decode the frame, if frameDecoded = 1 frame is decoded
            else if(avcodec_decode_video2(m_pCodecCtx, m_pFrame, &frameDecoded, &packet) < 0)
            {
                std::cerr << "Could not decode frame !"  << std::endl;
                if (m_possiblyDeconnected)
                    m_isActive = false;
                else
                    m_possiblyDeconnected = true;
            }
            // Frame decoded
            else if (frameDecoded)
            {
                // convert frame to RGB
                sws_scale(m_pConvertCtx, m_pFrame->data, m_pFrame->linesize, 0,
                          m_pCodecCtx->height, m_pFrameRGB->data, m_pFrameRGB->linesize);
            }
            av_free_packet(&packet);
        }
        catch (...)
        {
            std::cerr << "Decode : Error in Video Decoding Function" << std::endl;
            m_isActive = false;
            m_possiblyDeconnected = true;
            av_free_packet(&packet);
        }

        m_videoMutex.unlock();
    }

    void Video::tryToConnect()
    {
        try {
            if (m_reconnectionThread.joinable())
                m_reconnectionThread.join();

            release();
            if(!m_terminate)
                init();

            m_reconnectionThread = std::thread([this] () {
                                                try {
                                                    int i = 1;
                                                    int time = 30;
                                                    while (!m_terminate && m_possiblyDeconnected && !m_isActive && i < m_nbTryingReconnection) {
                                                        if (i%5 == 0) time *= 5;
                                                        std::this_thread::sleep_for(std::chrono::milliseconds(time));
                                                        init();
                                                        i++;
                                                    }
                                                }
                                                catch (...)
                                                {
                                                    std::cerr << "Reconnection Thread : Error in the Thread" << std::endl;
                                                }
                                             });
        }
        catch(...)
        {
            std::cerr << "Try connect : Error in the function" << std::endl;
        }
    }

    void Video::release()
    {
        m_videoMutex.lock();

        // Deallocate the frame
        if (m_pFrame)
        {
            av_free(m_pFrame);
            m_pFrame = nullptr;
        }

        // Deallocate the frame
        if (m_pFrameRGB)
        {
            av_free(m_pFrameRGB);
            m_pFrameRGB = nullptr;
        }

        // Deallocate the buffer
        if (m_bufferRGB)
        {
            av_free(m_bufferRGB);
            m_bufferRGB = nullptr;
        }

        // Deallocate the convert context
        if (m_pConvertCtx)
        {
            sws_freeContext(m_pConvertCtx);
            m_pConvertCtx = nullptr;
        }

        // Deallocate the codec
        if (m_pCodecCtx)
        {
            avcodec_close(m_pCodecCtx);
            m_pCodecCtx = nullptr;
        }

        // Deallocate the format context
        if (m_pFormatCtx)
        {
            avformat_close_input(&m_pFormatCtx);
            m_pFormatCtx = nullptr;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(80));

        // Deallocate the frame buffer
        if (m_pFrameBufferReturned)
        {
            delete[] m_pFrameBufferReturned;
            m_pFrameBufferReturned = nullptr;
        }

        m_videoMutex.unlock();
    }


    void Video::stopReconnectionThread()
    {
         m_possiblyDeconnected = false;
         if (m_reconnectionThread.joinable())
             m_reconnectionThread.join();
    }


    void Video::stopVideoThread()
    {
        m_isActive = false;
        if (m_videoThread.joinable())
            m_videoThread.join();
    }


    void Video::restart()
    {
        stop();
        m_possiblyDeconnected = true;

        tryToConnect();
    }

    void Video::stop()
    {
        // Destroy threads if they are always running
        stopReconnectionThread();
        stopVideoThread();

        release();
    }


    uint8_t* Video::getFrame() const
    {
        m_videoMutex.lock();

        if (m_pFrameRGB == nullptr || m_pFrameRGB->data == nullptr)
        {
            m_videoMutex.unlock();
            return nullptr;
        }

        memcpy(m_pFrameBufferReturned, *(m_pFrameRGB->data), m_pCodecCtx->width * m_pCodecCtx->height * 3);

        m_videoMutex.unlock();
        return m_pFrameBufferReturned;
    }

    int Video::getWidth() const
    {
        m_videoMutex.lock();

        if (m_pCodecCtx == NULL)
        {
            m_videoMutex.unlock();
            return -1;
        }

        int width = m_pCodecCtx->width;
        m_videoMutex.unlock();

        return width;
    }

    int Video::getHeight() const
    {
        m_videoMutex.lock();

        if (m_pCodecCtx == NULL)
        {
            m_videoMutex.unlock();
            return -1;
        }

        int height = m_pCodecCtx->height;
        m_videoMutex.unlock();

        return height;
    }
}
