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

#ifndef UCAPA_ARDRONE_H
#define UCAPA_ARDRONE_H

#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include <ardroneconnections.h>
#include <navdata.h>
#include <utils.h>
#include <vector3.h>
#include <video.h>


namespace ucapa{
    /**
     * @brief The ARDrone class.
     *
     * This defines high level functions to control the Drone.
     *
     * For all 3D stuffs (moves, positioning, rotation, ...), we define the following reference:
      @verbatim
      y z
      |/
      -- x

      Where x is the right of the drone, z the forward and y the top.
      @endverbatim
     * Note that this reference is different of the reference directly
     * used by the ARDrone in AT commands or in the navdata.
     */
    class UCAPA_API ARDrone
    {
    public:
        // Flight animation IDs
        /**
         * @brief List of available animation Id on ARDrone (implemented in ARDrone Firmware)
         */
        enum ANIMATION_ID {
            ANIM_PHI_M30_DEG = 0,
            ANIM_PHI_30_DEG = 1,
            ANIM_THETA_M30_DEG = 2,
            ANIM_THETA_30_DEG = 3,
            ANIM_THETA_20DEG_YAW_200DEG = 4,
            ANIM_THETA_20DEG_YAW_M200DEG = 5,
            ANIM_TURNAROUND = 6,
            ANIM_TURNAROUND_GODOWN = 7,
            ANIM_YAW_SHAKE = 8,
            ANIM_YAW_DANCE = 9,
            ANIM_PHI_DANCE = 10,
            ANIM_THETA_DANCE = 11,
            ANIM_VZ_DANCE = 12,
            ANIM_WAVE = 13,
            ANIM_PHI_THETA_MIXED = 14,
            ANIM_DOUBLE_PHI_THETA_MIXED = 15,
            ANIM_FLIP_AHEAD = 16, // AR.Drone 2.0
            ANIM_FLIP_BEHIND = 17, // AR.Drone 2.0
            ANIM_FLIP_LEFT = 18, // AR.Drone 2.0
            ANIM_FLIP_RIGHT = 19, // AR.Drone 2.0
            NB_ANIM_MAYDAY = 20
        };

        // LED animation IDs
        /**
         * @brief List of Led Animation (implemented in ARDrone firmware)
         */
        enum LED_ANIMATION_ID {
            LED_ANIM_BLINK_GREEN_RED = 0,
            LED_ANIM_BLINK_GREEN = 1,
            LED_ANIM_BLINK_RED = 2,
            LED_ANIM_BLINK_ORANGE = 3,
            LED_ANIM_SNAKE_GREEN_RED = 4,
            LED_ANIM_FIRE = 5,
            LED_ANIM_STANDARD = 6,
            LED_ANIM_RED = 7,
            LED_ANIM_GREEN = 8,
            LED_ANIM_RED_SNAKE = 9,
            LED_ANIM_BLANK = 10,
            LED_ANIM_RIGHT_MISSILE = 11,
            LED_ANIM_LEFT_MISSILE = 12,
            LED_ANIM_DOUBLE_MISSILE = 13,
            LED_ANIM_FRONT_LEFT_GREEN_OTHERS_RED = 14,
            LED_ANIM_FRONT_RIGHT_GREEN_OTHERS_RED = 15,
            LED_ANIM_REAR_RIGHT_GREEN_OTHERS_RED = 16,
            LED_ANIM_REAR_LEFT_GREEN_OTHERS_RED = 17,
            LED_ANIM_LEFT_GREEN_RIGHT_RED = 18,
            LED_ANIM_LEFT_RED_RIGHT_GREEN = 19,
            LED_ANIM_BLINK_STANDARD = 20,
            NB_LED_ANIM_MAYDAY = 21
        };

        // Enabled Camera
        /**
         * @brief Camera used for the video stream on port 5555
         */
        enum VIDEO_CAMERA {
            FRONT_CAMERA = 0,
            BOTTOM_CAMERA = 1
        };

        // Video Codec
        /**
         * @brief List of video codec supported by the ARDrone 2.0
         *
         * MP4 codec are less stable than H264
         * It's recommanded to use H264_360P_CODEC or H264_720P_CODEC
         */
        enum VIDEO_CODEC {
            H264_360P_CODEC = 0x81,
            MP4_360P_H264_720P_CODEC = 0x82,
            H264_720P_CODEC = 0x83,
            MP4_360P_H264_360P_CODEC = 0x88
        };

    protected:
        const int m_ATCmdsMaxLength = 1024;
        bool m_connected = true;

        const std::string m_sessionId;
        const std::string m_userId;
        const std::string m_appId;

        // IP and port used by the drone
        const std::string m_ardIp;
        const unsigned short m_ardATCmdsPort;
        const unsigned short m_ardNavdataPort;
        const unsigned short m_ardVideoPort;
        const unsigned short m_ardControlPort;

        float m_altitudeMax;    ///< The maximum altitude the drone can reach (no limit, in meters).
        float m_verticalSpeed;  ///< The vertical speed of the drone (between 0.2 and 2.0, in meters per second).
        float m_rotationSpeed;  ///< The rotation speed of the drone (between 0.7 and 6.11, in radians per second).
        float m_euler_angle_max;///< The maximum bending angle of the drone (change speed of the drone) (between 0 and 0.52, in radians).
        bool m_isWithoutShell;  ///< Tell if the drone have the indoor or the outdoor hool.
        bool m_isOutdoor;       ///< Tell if the drone is flying indoor or outdoor (active/desactive the wind estimator).

        std::unique_ptr<ARDroneConnections> m_connectionsHandler; ///< Manage all connections with the drone
        int m_indexCmd; ///< Sequence number used in commands sent to the drone

        // Drone's Navdata
        std::thread m_navdataThread;
        std::shared_ptr<Navdata> m_navdata;

        std::unique_ptr<Video> m_video;

        ARDrone(std::string sessionId, std::string userId, std::string appId,
                const std::string ardIp,
                const unsigned short ardATCmdsPort,
                const unsigned short ardNavdataPort,
                const unsigned short ardVideoPort,
                const unsigned short ardControlPort,
                ARDroneConnections* connectionHandler,
                Navdata* navdata,
                Video* video);

        /**
         * @brief Send an AT_REF command to the drone using the specified argument.
         * @param ctrl This flag will determine the action that the drone will do.
         *             (like taking off, land, ...)
         * @pre The argument must be valid following the requirements defined by the drone documentation
         *      for the AT*REF command.
         */
        virtual void AT_REF(int ctrl);
        /**
         * @brief Send an AT_FTRIM command to the drone. This will do a flat trim on the drone.
         * @pre Drone is landed on a horizontal ground.
         */
        virtual void AT_FTRIM();
        /**
         * @brief Send an AT_PCMD command to the drone using the specified arguments.
         * @param flags This flag will determine the drone move policy.
         * @param phi Determine the drone left-right tilt
         * @param theta Determine the drone front-back tilt
         * @param gaz Determine the drone vertical speed
         * #param yaw Determine the drone angular speed
         * @pre Arguments must be valid following the requirements defined by the drone documentation
         *      for the AT*PCMD command.
         */
        virtual void AT_PCMD(int flags, float phi, float theta, float gaz, float yaw);

        /**
         * @brief Prepare the sending of a config command.
         */
        void AT_CONFIG_IDS();
        /**
         * @brief Set a configuration entry to a new value.
         * @param name The name of the entry that will be updated.
         * @param value The new value for the entry.
         */
        template <class T> void AT_CONFIG(const std::string& name, const T& value);

        /**
         * @brief Reset the watchdog.
         */
        virtual void AT_COMWDG();

        /**
         * @brief Calibrate the magnetometer.
         */
        virtual void AT_CALIB();

        /**
         * @brief Initialize navdata stream
         */
        virtual void initNavdata();

        /**
         * @brief Initialize video stream
         */
        virtual void initVideo();

    public:
        /**
         * @brief Construct an object of type ARdrone, which is an interface of communication with de ARDrone
         * @param sessionId session Id (arbitrary value)
         * @param userId user Id (arbitrary value)
         * @param appId specific Id for our application (arbitrary value)
         */
        ARDrone(std::string sessionId, std::string userId, std::string appId,
                const std::string ardIp = "192.168.1.1",
                const unsigned short ardATCmdsPort = 5556,
                const unsigned short ardNavdataPort = 5554,
                const unsigned short ardVideoPort = 5555,
                const unsigned short ardControlPort = 5559);
        virtual ~ARDrone();


        /**
         * @brief Make the drone take-off.
         */
        virtual void takeOff();
        /**
         * @brief Make the drone land.
         */
        virtual void land();
        /**
         * @brief Send an emergency signal to the drone.
         */
        virtual void emergency();

        /**
         * @brief Move the drone using specified factors.
         * @param m Move to apply based on the defined reference.
         */
        virtual void move(Vector3 m);
        /**
         * @brief Move the drone using specified factors.
         * @param m Move to apply based on the defined reference.
         * @param yr Determine the drone rotation on itself (around the y-axis)
         */
        virtual void move(Vector3 m, float yr);

        /**
         * @brief Make the drone enter hovering mode
         */
        virtual void enterHoveringMode();

        /**
         * @brief Launch an animation of drone LEDs.
         * @param animId Animation id.
         * @param freq Animation frequency.
         * @param duration Animation duration.
         */
        virtual void animLeds(LED_ANIMATION_ID animId, float freq, int duration);
        /**
         * @brief Launch a flight anim on the drone
         * @param anim Flight animation id.
         */
        virtual void anim(ANIMATION_ID anim);

        /**
         * @brief Permit to calibrate the magnetometer of the drone (drone must fly).
         */
        virtual void calibrate();
        /**
         * @brief Tells the drone it is lying horizontally (drone must be on an horizontal ground).
         */
        virtual void trim();

        /**
         * @brief Put the ARDrone Default config
         * Front Camera enable and Video Codec H264_360P_CODEC
         */
        virtual void setDefaultConfig();

        /**
         * @brief Enable one of the drone cameras.
         * @param channel Camera to enable
         */
        virtual void setCamera(VIDEO_CAMERA channel);

        /**
         * @brief Switch between Camera video codec.
         * @param codec The codec used for the video stream
         */
        virtual void setVideoCodec(VIDEO_CODEC codec);

        /**
         * @brief Toggle the video record state.
         * @param activate true to enable video recording
         */
        virtual void setVideoRecord(bool activate);

        /**
         * @brief Return the maximum altitude.
         */
        virtual float getAltitudeMax() const {return m_altitudeMax;}
        /**
         * @brief Return the vertical speed.
         */
        virtual float getVerticalSpeed() const {return m_verticalSpeed;}
        /**
         * @brief Return the rotation speed.
         */
        virtual float getRotationSpeed() const {return m_rotationSpeed;}
        /**
         * @brief Return the speed.
         */
        virtual float getSpeed() const {return m_euler_angle_max;}
        /**
         * @brief Return true if the drone should have its outDoorShell, false else.
         */
        virtual bool getIsWithoutShell() const {return m_isWithoutShell;}
        /**
         * @brief Return true if the drone is in "outdoor mode", false else.
         */
        virtual bool getIsOutdoor() const {return m_isOutdoor;}

        /**
         * @brief Check if drone is flying or not.
         * @return true if the drone is flying, false otherwise.
         */
        virtual bool isFlying() const;

        /**
         * @brief Give the drone's state wanted.
         * @param mask : the mask corresponding to the state wanted
         * @return The value of the state (0 or 1).
         */
        virtual int getState(Navdata::STATE_MASK mask) const;

        /**
         * @brief Return duration since last received navdata.
         */
        virtual std::chrono::duration<double> getLastNavdataReception() const {return m_connectionsHandler->getLastNavdataReception();}

        /**
         * @brief Permit to set the maximum altitude.
         * @param altitudeMax The maximum altitude.
         */
        virtual void setAltitudeMax(float altitudeMax);

        /**
         * @brief Permit to set the vertical speed.
         * @param verticalSpeed The vertical speed.
         */
        virtual void setVerticalSpeed(float verticalSpeed);

        /**
         * @brief Permit to set the rotation speed.
         * @param rotationSpeed The rotation speed.
         */
        virtual void setRotationSpeed(float rotationSpeed);

        /**
         * @brief Permit to set the speed.
         * @param speed The speed.
         */
        virtual void setSpeed(float speed);

        /**
         * @brief Permit to set if the drone should have its indoor hool or not.
         * @param isWithoutShellFalse : with indoor hool. True : with outdoor hool.
         */
        virtual void setIsWithoutShell(bool isWithoutShell);

        /**
         * @brief Permit to set if the drone is flying outdoor or not.
         * @param isOutdoor False : indoor. True : outdoor.
         */
        virtual void setIsOutdoor(bool isOutdoor);

        /**
         * @brief Enable or disable world data calculation in Navdata.
         *
         * Can only be done when drone is not flying.
         */
        virtual void setComputeWorldData(bool b);

        /**
         * @brief Accessor on navdata.
         * @return navdata content.
         */
        virtual const Navdata& nav() const {return *m_navdata;}

        /**
         * @brief Accessor on video stream.
         * @return video stream content.
         */
        virtual const Video& video() const {return *m_video;}
    };
}

#endif // UCAPA_ARDRONE_H
