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

#ifndef UCAPA_VIDEO_H
#define UCAPA_VIDEO_H

#include <atomic>
#include <mutex>
#include <iostream>
#include <sstream>
#include <thread>

extern "C" {
    #include <stdint.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

#include <config.h>

class ARDrone;

namespace ucapa{
    /**
     * @brief Manage ARDrone video stream.
     *
     * Receive video stream from the drone.
     */
    class UCAPA_API Video
    {
    protected:
        const std::string m_droneIP; ///< Drone IP.
        const unsigned short m_videoPort; ///< Drone video port.
        std::function<void()> m_callbackInitFunc; ///< Function that will send the init packet to the video port.

        // Video Data (codec, format, frame, etc..)
        std::thread m_videoThread; ///< Thread with the loop for decoding the video.
        std::thread m_reconnectionThread; ///< Thread for the test ofreconnection if it's needed.
        std::thread m_launcherReconnectThread;  ///< Thread for the automatic video reconnection.
        mutable std::mutex m_videoMutex; ///< Mutex that manage access to video variables.
        AVFormatContext* m_pFormatCtx; ///< Pointer on the Format of the video stream.
        AVCodecContext* m_pCodecCtx; ///< Pointer on the Codec to use for the video stream.
        AVFrame* m_pFrame; ///< Pointer on the frame decoded.
        AVFrame* m_pFrameRGB; ///< Pointer on the frame in color.
        uint8_t* m_pFrameBufferReturned; ///< Pointer on the buffer in color of the frame returned, copy of the buffer of pFrameRGB, because of the use of mutex.
        uint8_t* m_bufferRGB; ///< Buffer used for the frame allocation.
        SwsContext* m_pConvertCtx; ///< Converter, to convert the frame into RGB mode.
        std::atomic<bool> m_isActive; ///< State of video stream, enable or disable.
        std::atomic<bool> m_possiblyDeconnected; ///< Indicate if we have maybe a deconnection with the drone, return to false if another frame of video stream is received.
        std::atomic<bool> m_firstConnection;    ///< Indicate if it's the first time that we try to connect to video stream.
        std::atomic<bool> m_terminate;  ///< Indicate if the application need to terminate the video receiving.
        const int m_nbTryingReconnection = 16; ///< Number of attempts to try to reconnect Video Stream.

        /**
         * @brief Init the connection on ARDrone Video port.
         * @return 0 if no problem happen,<br>
         *         -1 if Could not open the video file,<br>
         *         -2 if Finding Codec failed,<br>
         *         -3 if Opening Codec failed,<br>
         *         -256 if an exception was thrown.
         */
        virtual int init();
        /**
         * @brief Decode a frame from ARDrone Video port.
         */
        virtual void decode();
        /**
         * @brief Try to connect Video Stream nbTryingReconnection times.
         */
        virtual void tryToConnect();
        /**
         * @brief Free the memory used by variables related to video stream.
         */
        virtual void release();
        /**
         * @brief Launch a thread that try to re open video stream. Only use when a video deconnection is detected.
         */
        virtual void reconnect();
        /**
         * @brief Request to stop the reconnection Thread and turn m_possiblyDeconnected to false.
         */
        virtual void stopReconnectionThread();
        /**
         * @brief Request to stop the video decoding Thread and turn m_isActive video state to false.
         */
        virtual void stopVideoThread();

    public:
        /**
         * @brief Video constructor.
         * @param droneIP Drone IP.
         * @param VideoPort Drone video port.
         * @param callbackInitFunc Function that will send the init packet to the video port.
         */
        Video(const std::string& droneIP, unsigned short VideoPort);
        virtual ~Video();

        /**
         * @brief Initialize the CallBack Function use only one time in the first init of the video.
         * @param callbackInitFunc function that will be call in the first init of video
         */
        virtual void setCallbackInitFunc(std::function<void()> callbackInitFunc);

        /**
         * @brief Try to init the video stream for receiving it.
         */
        virtual void restart();
        /**
         * @brief Disable the video stream.
         */
        virtual void stop();

        // Accessors
        /**
         * @brief Return buffer data of the current frame decoded from ARDrone video stream.
         *
         * Each pixel is represented by the sequence of the following 3 values: R, G, B.
         *
         * @return the current RGB frame data buffer.
         */
        virtual uint8_t* getFrame() const;
        /**
         * @brief Return the width of the current frame.
         * @return frame width,
         *         -1 if there is a problem.
         */
        virtual int getWidth() const;
        /**
         * @brief Return the height of the current frame.
         * @return frame height,
         *         -1 if there is a problem.
         */
        virtual int getHeight() const;
        /**
         * @brief Return if the video stream is activated.
         * @return video stream activation.
         */
        virtual bool isActive() const { return m_isActive; }
    };
}

#endif // UCAPA_VIDEO_H
