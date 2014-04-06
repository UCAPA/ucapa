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

#include <ardrone.h>

namespace ucapa{
    ARDrone::ARDrone(std::string sessionId, std::string userId, std::string appId,
                     const std::string ardIp,
                     const unsigned short ardATCmdsPort,
                     const unsigned short ardNavdataPort,
                     const unsigned short ardVideoPort,
                     const unsigned short ardControlPort,
                     ARDroneConnections* connectionHandler,
                     Navdata* navdata,
                     Video* video)
        : m_sessionId(sessionId)
        , m_userId(userId)
        , m_appId(appId)
        , m_ardIp(ardIp)
        , m_ardATCmdsPort(ardATCmdsPort)
        , m_ardNavdataPort(ardNavdataPort)
        , m_ardVideoPort(ardVideoPort)
        , m_ardControlPort(ardControlPort)
        , m_altitudeMax(2.0f)
        , m_verticalSpeed(.7f)
        , m_rotationSpeed(3.0f)
        , m_euler_angle_max(0.26f)
        , m_isWithoutShell(false)
        , m_isOutdoor(false)
        , m_connectionsHandler(connectionHandler)
        , m_indexCmd(1)
        , m_navdata(navdata)
        , m_video(video)
    {
        m_video->setCallbackInitFunc([this]() {this->m_connectionsHandler->sendInitVideoData();});

        AT_CONFIG("custom:session_id", sessionId);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        AT_CONFIG("custom:profile_id", userId);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        AT_CONFIG("custom:application_id", appId);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        setAltitudeMax(m_altitudeMax);
        setVerticalSpeed(m_verticalSpeed);
        setRotationSpeed(m_rotationSpeed);
        setSpeed(m_euler_angle_max);
        setIsWithoutShell(false);
        setIsOutdoor(false);

        // Initialize the ARDrone video stream (Codecs, etc..)
        setDefaultConfig();

        initNavdata();
    }

    ARDrone::ARDrone(std::string sessionId, std::string userId, std::string appId,
                     const std::string ardIp,
                     const unsigned short ardATCmdsPort,
                     const unsigned short ardNavdataPort,
                     const unsigned short ardVideoPort,
                     const unsigned short ardControlPort)
        : ARDrone(sessionId, userId, appId,
                  ardIp, ardATCmdsPort, ardNavdataPort, ardVideoPort, ardControlPort,
                  new ARDroneConnections(ardIp, ardATCmdsPort, ardNavdataPort, ardVideoPort, ardControlPort),
                  new Navdata(),
                  new Video(ardIp, ardVideoPort))
    {

    }

    ARDrone::~ARDrone()
    {
        m_connected = false;

        if (m_navdataThread.joinable())
            m_navdataThread.join();

        m_connectionsHandler.reset();
    }


    void ARDrone::AT_REF(int ctrl)
    {
        std::ostringstream oss;
        oss << "AT*REF=" << m_indexCmd++ << "," << ctrl << "\r";
        m_connectionsHandler->sendATCommand(oss.str());
    }

    void ARDrone::AT_FTRIM()
    {
        if (isFlying())
            return;

        std::ostringstream oss;
        oss << "AT*FTRIM=" << m_indexCmd++ << "\r";
        m_connectionsHandler->sendATCommand(oss.str());
    }

    void ARDrone::AT_PCMD(int flags, float phi, float theta, float gaz, float yaw)
    {
        std::ostringstream oss;
        oss << "AT*PCMD=" << m_indexCmd++ << "," << flags << "," << *(int*)&phi << "," << *(int*)&theta << "," << *(int*)&gaz << "," << *(int*)&yaw << "\r";
        m_connectionsHandler->sendATCommand(oss.str());
    }

    void ARDrone::AT_CONFIG_IDS()
    {
        std::ostringstream oss;
        oss << "AT*CONFIG_IDS=" << m_indexCmd++ << ",\"" << m_sessionId << "\",\"" << m_userId << "\",\"" << m_appId << "\"\r";
        m_connectionsHandler->sendATCommand(oss.str());
    }

    template <class T>
    void ARDrone::AT_CONFIG(const std::string& name, const T& value)
    {
        AT_CONFIG_IDS();
        std::ostringstream oss;
        oss << "AT*CONFIG=" << m_indexCmd++ << ",\"" << name << "\",\"" << value << "\"\r";
        m_connectionsHandler->sendATCommand(oss.str());
    }

    void ARDrone::AT_COMWDG()
    {
        std::ostringstream oss;
        oss << "AT*COMWDG=" << m_indexCmd++ << "\r";
        m_connectionsHandler->sendATCommand(oss.str());
    }

    void ARDrone::AT_CALIB()
    {
        std::ostringstream oss;
        oss << "AT*CALIB=" << m_indexCmd++ << ",0\r";
        m_connectionsHandler->sendATCommand(oss.str());
    }


    void ARDrone::initNavdata()
    {
        m_connectionsHandler->sendNavdataStart();

        AT_CONFIG("general:navdata_demo", "FALSE");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::ostringstream oss;
        oss << "AT*CTRL=" << m_indexCmd++ << ",5\r";
        m_connectionsHandler->sendATCommand(oss.str());

        m_connectionsHandler->initNavdataReceptionThread(m_navdata);

        m_navdataThread = std::thread( [this]() {
                                            while (this->m_connected) {
                                                this->AT_COMWDG();
                                                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                                            }
                                        } );
    }

    void ARDrone::initVideo()
    {
        m_video->restart();
    }


    void ARDrone::takeOff()
    {
        if (m_navdata->isComputingWorldData())
            m_navdata->resetWorldData();
        const int arg = 1<<28 | 1<<24 | 1<<22 | 1<<20 | 1<<18 | 1<<9;
        AT_REF(arg);
    }

    void ARDrone::land()
    {
        const int arg = 1<<28 | 1<<24 | 1<<22 | 1<<20 | 1<<18 | 0<<9;
        AT_REF(arg);
    }

    void ARDrone::emergency()
    {
        const int arg = 1<<28 | 1<<24 | 1<<22 | 1<<20 | 1<<18 | 1<<8;
        AT_REF(arg);
    }

    void ARDrone::move(Vector3 m)
    {
        move(m, 0);
    }

    void ARDrone::move(Vector3 m, float yr)
    {
        if (m.x < -1 || m.x > 1) return;
        if (m.z < -1 || m.z > 1) return;
        if (m.y < -1 || m.y > 1) return;
        if (yr < -1 || yr > 1) return;

        int flags = 0
    //                | 1<<2  // Enable absolute control
    //                | 1<<1  // Enable combined yaw
                    | 1<<0; // Enable progressive commands (stop hovering mode)

        AT_PCMD(flags, m.x, -m.z, m.y, yr);
    }

    void ARDrone::enterHoveringMode()
    {
        AT_PCMD(0, 0, 0, 0, 0);
    }

    void ARDrone::animLeds(LED_ANIMATION_ID animId, float freq, int duration)
    {
        std::ostringstream ossValues;
        ossValues << animId << "," << *(int*)& freq << "," << duration;
        AT_CONFIG("leds:leds_anim", ossValues.str());
    }

    void ARDrone::anim(ANIMATION_ID anim)
    {
        const int duration[] {
        1000, // ANIM_PHI_M30_DEG
        1000, // ANIM_PHI_30_DEG
        1000, // ANIM_THETA_M30_DEG
        1000, // ANIM_THETA_30_DEG
        1000, // ANIM_THETA_20DEG_YAW_200DEG
        1000, // ANIM_THETA_20DEG_YAW_M200DEG
        5000, // ANIM_TURNAROUND
        5000, // ANIM_TURNAROUND_GODOWN
        2000, // ANIM_YAW_SHAKE
        5000, // ANIM_YAW_DANCE
        5000, // ANIM_PHI_DANCE
        5000, // ANIM_THETA_DANCE
        5000, // ANIM_VZ_DANCE
        5000, // ANIM_WAVE
        5000, // ANIM_PHI_THETA_MIXED
        5000, // ANIM_DOUBLE_PHI_THETA_MIXED
        200,  // ANIM_FLIP_AHEAD
        200,  // ANIM_FLIP_BEHIND
        200,  // ANIM_FLIP_LEFT
        200,  // ANIM_FLIP_RIGHT
        };

        std::ostringstream ossValues;
        ossValues << anim << "," << duration[anim];

        AT_CONFIG("control:flight_anim", ossValues.str());
    }

    void ARDrone::calibrate()
    {
        if(isFlying())
            AT_CALIB();
    }

    void ARDrone::trim()
    {
        if(!isFlying())
            AT_FTRIM();
    }

    void ARDrone::setComputeWorldData(bool b)
    {
        m_navdata->setComputeWorldData(b);
    }


    void ARDrone::setDefaultConfig()
    {
        setCamera(FRONT_CAMERA);
        setVideoCodec(H264_360P_CODEC);
    }

    void ARDrone::setCamera(VIDEO_CAMERA c)
    {
        AT_CONFIG("video:video_channel", c);
    }

    void ARDrone::setVideoCodec(VIDEO_CODEC codec)
    {
        // Stop Video Stream if is active
        m_video->stop();

        // Send codec config to ARDrone
        AT_CONFIG("video:video_codec", codec);

        // Restart Video Stream
        m_video->restart();
    }

    void ARDrone::setVideoRecord(bool activate)
    {
        m_video->stop();

        // Enable or disable video record
        if (activate)
            AT_CONFIG("video:video_on_usb", "TRUE");
        else
            AT_CONFIG("video:video_on_usb", "FALSE");

        // Reset the configuration
        setDefaultConfig();
    }

    bool ARDrone::isFlying() const
    {
        if(m_navdata->getState() & Navdata::FLY_MASK)
            return true;
        else
            return false;
    }

    int ARDrone::getState(Navdata::STATE_MASK mask) const
    {
        if(m_navdata->getState() & mask)
            return 1;
        else
            return 0;
    }

    void ARDrone::setAltitudeMax(float altitudeMax)
    {
        m_altitudeMax = altitudeMax;
        AT_CONFIG("control:altitude_max", (int)mToMm(m_altitudeMax));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void ARDrone::setVerticalSpeed(float verticalSpeed)
    {
        m_verticalSpeed  = verticalSpeed;
        AT_CONFIG("control:control_vz_max", mToMm(m_verticalSpeed));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void ARDrone::setRotationSpeed(float rotationSpeed)
    {
        m_rotationSpeed = rotationSpeed;
        AT_CONFIG("control:control_yaw", m_rotationSpeed);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void ARDrone::setSpeed(float speed)
    {
        m_euler_angle_max = speed;
        AT_CONFIG("control:euler_angle_max", m_euler_angle_max);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void ARDrone::setIsWithoutShell(bool isWithoutShell)
    {
        m_isWithoutShell = isWithoutShell;
        if(m_isWithoutShell)
            AT_CONFIG("control:flight_without_shell", "TRUE");
        else
            AT_CONFIG("control:flight_without_shell", "FALSE");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void ARDrone::setIsOutdoor(bool isOutdoor)
    {
        m_isOutdoor = isOutdoor;
        if(m_isOutdoor)
            AT_CONFIG("control:outdoor", "TRUE");
        else
            AT_CONFIG("control:outdoor", "FALSE");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
