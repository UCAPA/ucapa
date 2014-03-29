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

#ifndef UCAPA_ARDRONECONNECTIONS_H
#define UCAPA_ARDRONECONNECTIONS_H

#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>
#include <type_traits>

#include <asio.hpp>

#include <config.h>
#include <navdata.h>
#include <video.h>

using asio::ip::tcp;
using asio::ip::udp;

namespace ucapa{
    /**
     * @brief Manage all drone connections.
     *
     * There are 4 different drone connections:
     * - a first connection for AT Commands on UDP port 5556. The recommanded frequency for AT commands is 30 times by seconds.
     * - a second is for receive navdata from the drone on UDP port 5554. They are sent approximatively 15 times per second in
     *     demo mode, and 200 times per second in full (debug) mode.
     * - a third is for video stream on port 5555, using UDP for AR.Drone 1.0 and TCP for AR.Drone 2.0. Here, only AR.Drone 2.0 is supported.
     * - the last, on TCP port 5559, is to transfer critical data. It used to retrieve configuration data, and to acknowledge important
     *     information such as the sending of configuration information.
     */
    class UCAPA_API ARDroneConnections
    {
    protected:
        asio::io_service m_ioService; ///< Network service
        std::unique_ptr<asio::io_service::work> m_ioServiceWork;
        std::thread m_ioServiceRunningThread;

        // Drone IP
        const std::string m_droneIP;

        // AT Commands related attributs
        udp::endpoint m_ATCmdsEndpoint;
        udp::socket m_ATCmdsSocket;

        // Navdata related attributs
        udp::endpoint m_NavdataEndpoint;
        udp::socket m_NavdataSocket;

        // Video stream related attributs
        tcp::endpoint m_VideoEndpoint;
        tcp::socket m_VideoSocket;

        // Control related attributs
        tcp::endpoint m_CtrlEndpoint;
        tcp::socket m_CtrlSocket;

        // Navdata
        const static int m_max_length = 1024;
        char navdataBuffer[m_max_length]; // Used by asio to write received bytes
        std::chrono::steady_clock::time_point m_navdataLastReceptionTime;
        std::weak_ptr<Navdata> m_navdata;


    public:
        /**
         * @brief Initialize all connections with the ARDrone on interested ports
         * @param droneIP Ip of the ARDrone
         * @param ATCmdsPort Port number for the sending of AT Commands
         * @param NavdataPort Port number for the reception of navdata
         * @param VideoPort Port number for the video stream
         * @param CtrlPort Port number for the configuration of the ARDrone
         */
        ARDroneConnections(const std::string& droneIP,
                           unsigned short ATCmdsPort,
                           unsigned short NavdataPort,
                           unsigned short VideoPort,
                           unsigned short CtrlPort);
        virtual ~ARDroneConnections();


        /**
         * @brief Send an AT command to the drone
         * @param cmd Send it to the drone as an AT command (ie. Using the right port and the right protocol)
         */
        virtual void sendATCommand(const std::string& cmd);

        /**
         * @brief Send a short trame of data to initialize navdata reception
         */
        virtual void sendNavdataStart();
        /**
         * @brief Init navdata pointer, and launch asyncronous service thread
         * @param navdata pointer
         */
        virtual void initNavdataReceptionThread(std::weak_ptr<Navdata> navd);
        /**
         * @brief Receives navdata
         * @param ec Error code if there is one
         * @param bytes_recvd Buffer size
         */
        virtual void handleNavdata(std::error_code ec, std::size_t bytes_recvd);


        /**
         * @brief Send initialisation packet to the drone video port.
         */
        virtual void sendInitVideoData();


        /**
         * @brief Return the duration since the last reception of Navdata
         * @return The duration
         */
        virtual std::chrono::duration<double> getLastNavdataReception() const;
    };
}

#endif // UCAPA_ARDRONECONNECTIONS_H
