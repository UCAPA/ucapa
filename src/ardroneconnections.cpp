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

#include <ardroneconnections.h>

namespace ucapa{
    ARDroneConnections::ARDroneConnections(const std::string& droneIP,
                                           unsigned short ATCmdsPort,
                                           unsigned short NavdataPort,
                                           unsigned short VideoPort,
                                           unsigned short CtrlPort)
        : m_ioService()
        , m_ioServiceWork(new asio::io_service::work(m_ioService))
        , m_droneIP(droneIP)
        // Init AT Commands connection with the drone
        , m_ATCmdsEndpoint(asio::ip::address::from_string(droneIP), ATCmdsPort)
        , m_ATCmdsSocket(m_ioService, udp::endpoint(udp::v4(), 0))
        // Init Navdata connection with the drone
        , m_NavdataEndpoint(asio::ip::address::from_string(droneIP), NavdataPort)
        , m_NavdataSocket(m_ioService, udp::endpoint(udp::v4(), 0))
        // Init Video connection with the drone
        , m_VideoEndpoint(asio::ip::address::from_string(droneIP), VideoPort)
        , m_VideoSocket(m_ioService)
        // Init Control connection with the drone
        , m_CtrlEndpoint(asio::ip::address::from_string(droneIP), CtrlPort)
        , m_CtrlSocket(m_ioService)
    {
        static_assert( sizeof(float) == 4, "float must be coded on 4 Bytes.");
        try
        {
            // Connect TCP sockets
            m_VideoSocket.connect(m_VideoEndpoint);
            m_CtrlSocket.connect(m_CtrlEndpoint);
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }

    ARDroneConnections::~ARDroneConnections()
    {
        m_ioServiceWork.reset();
        m_ioService.stop();
        m_ioServiceRunningThread.join();
    }

    void ARDroneConnections::sendATCommand(const std::string& cmd)
    {
        try {
            m_ATCmdsSocket.send_to(asio::buffer(cmd), m_ATCmdsEndpoint);
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }


    void ARDroneConnections::sendNavdataStart()
    {
        const char * send_buf("\x01\x00\x00\x00");
        m_NavdataSocket.send_to(asio::buffer(send_buf, 4), m_NavdataEndpoint);
    }

    void ARDroneConnections::initNavdataReceptionThread(std::weak_ptr<Navdata> navd)
    {
        m_navdata = navd;

        // Init navdata connection
        try
        {
            // Receive nadata
            udp::endpoint sender_endpoint;
            auto buffer = asio::buffer(navdataBuffer, m_max_length);
            m_NavdataSocket.async_receive_from(buffer, sender_endpoint,
                                             [this](std::error_code ec, std::size_t bytes_recvd){ this->handleNavdata(ec, bytes_recvd);});

            // Start running asyncronous service
            m_ioServiceRunningThread = std::thread([this]() {this->m_ioService.run();});
        }
        catch(std::exception& e)
        {
            std::cerr << std::endl << "Error initNavdata: " << e.what() << std::endl << std::endl;
        }
    }

    void ARDroneConnections::handleNavdata(std::error_code ec, std::size_t bytes_recvd)
    {
        if (!ec)
        {
            std::cerr << std::endl << "error: handle " << ec.message() << std::endl << std::endl;
        }
        else //if(bytes_recvd > 0)
        {
            if (!m_navdata.expired())
            {
               std::shared_ptr<Navdata> nav = m_navdata.lock();
               auto receptionTime = std::chrono::steady_clock::now();
               nav->update(std::string(navdataBuffer, bytes_recvd), receptionTime - m_navdataLastReceptionTime);
               m_navdataLastReceptionTime = receptionTime;
            }
        }

        // Prepare the reception again
        udp::endpoint sender_endpoint;
        auto buffer = asio::buffer(navdataBuffer, m_max_length);
        m_NavdataSocket.async_receive_from(buffer, sender_endpoint,
                                         [this](std::error_code ec, std::size_t bytes_recvd){ this->handleNavdata(ec, bytes_recvd);} );
    }

    void ARDroneConnections::sendInitVideoData()
    {
        // Send data to active video stream
        const char * active_video_buffer = "\x01\x00\x00\x00";
        m_VideoSocket.send(asio::buffer(active_video_buffer, 4));
    }


    std::chrono::duration<double> ARDroneConnections::getLastNavdataReception() const
    {
        return std::chrono::steady_clock::now() - m_navdataLastReceptionTime;
    }
}
