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

#ifndef UCAPA_NAVDATA_H
#define UCAPA_NAVDATA_H

#include <atomic>
#include <chrono>
#include <mutex>
#include <iostream>
#include <map>
#include <string>

#include <quaternion.h>
#include <vector3.h>
#include <utils.h>

namespace ucapa{
    /**
     * @brief Manage Navigation data of ARDrone.
     *
     * Allow to access many informations of ARDrone flight.
     */
    class UCAPA_API Navdata
    {
    public:
        // Navdata Mask
        /**
         * @brief List of ARDrone states mask
         */
        enum STATE_MASK {
            FLY_MASK            = 1U <<  0, ///< If the drone is landed(0) or flying(1)
            VIDEO_MASK          = 1U <<  1, ///< If the video is disable(0) or enable(1)
            VISION_MASK         = 1U <<  2, ///< If the vision is disable(0) or enable(1)
            CONTROL_MASK        = 1U <<  3, ///< If the drone use Euler angles control(0) or Angular speed control(1)
            ALTITUDE_MASK       = 1U <<  4, ///< If the altitude controle is active(0) or inactive(1)
            USER_FEEDBACK_START = 1U <<  5, ///< USER feedback : Start button state
            COMMAND_MASK        = 1U <<  6, ///< If the drone has receive command(s)(1) or has not(0)
            CAMERA_MASK         = 1U <<  7, ///< If the camera is ready(1) or is not(0)
            TRAVELLING_MASK     = 1U <<  8, ///< If the Travelling mask is enable(1) or not(0)
            USB_MASK            = 1U <<  9, ///< If the USB key is ready(1) or not(0)
            NAVDATA_DEMO_MASK   = 1U << 10, ///< If the drone send only nadata demo(1) or all the navdata(0)
            NAVDATA_BOOTSTRAP   = 1U << 11, ///< If the drone send Navdata options(1) or not(0)
            MOTORS_MASK         = 1U << 12, ///< If the motors have a problem(1) or don't(0)
            COM_LOST_MASK       = 1U << 13, ///< If the communication is lost(0) or not(1)
            GYRO_MASK           = 1U << 14, ///< If there's a software problem(1) or not(0)
            VBAT_LOW            = 1U << 15, ///< If battery voltage is too low(1) or not(0)
            USER_EL             = 1U << 16, ///< If the user emergency landing is ON(1) or OFF(0)
            TIMER_ELAPSED       = 1U << 17, ///< If the timer is elapsed(1) or not(0)
            POWER_MASK          = 1U << 18, ///< If there magnetometer needs a calibration(1) or not(0)
            ANGLES_OUT_OF_RANGE = 1U << 19, ///< If the angles are ok(0) or not(1) (emergency if not)
            WIND_MASK           = 1U << 20, ///< If there's too many wind too fly(1) or not(0)
            ULTRASOUND_MASK     = 1U << 21, ///< If the Ultrasonic sensor is OK(0) or not(1)
            CUTOUT_MASK         = 1U << 22, ///< If the cutout system is detected(1) or not(0)
            PIC_VERSION_MASK    = 1U << 23, ///< If the PIC has a good version number(1) or not(0)
            ATCODEC_THREAD_ON   = 1U << 24, ///< If the ATCodec thread is ON(1) or OFF(0)
            NAVDATA_THREAD_ON   = 1U << 25, ///< If the Navdata thread is ON(1) or OFF(0)
            VIDEO_THREAD_ON     = 1U << 26, ///< If the Video thread is ON(1) or OFF(0)
            ACQ_THREAD_ON       = 1U << 27, ///< If the Acquisition thread is ON(1) or OFF(0)
            CTRL_WATCHDOG_MASK  = 1U << 28, ///< If there is a delay for control execution superior of 5ms(1) or not(0)
            ADC_WATCHDOG_MASK   = 1U << 29, ///< If the frequency of communication with adc is superior of 5ms(1) or not(0)
            COM_WATCHDOG_MASK   = 1U << 30, ///< If there are communication problem(0) or not(1)
            EMERGENCY_MASK      = 1U << 31  ///< If the drone is in emergency(1) or not(0)
        };

        // Navdata tags
        /**
         * @brief List of navdata option Ids
         */
        enum NAVDATA_TAG {
            NAVDATA_DEMO_TAG            =  0, ///< Minimal navigation data for all flights
            NAVDATA_TIME_TAG            =  1, ///< Timestamp
            NAVDATA_RAW_MEASURES_TAG    =  2, ///< Raw sensors measurements
            NAVDATA_PHYS_MEASURES_TAG   =  3,
            NAVDATA_GYROS_OFFSETS_TAG   =  4,
            NAVDATA_EULER_ANGLES_TAG    =  5,
            NAVDATA_REFERENCES_TAG      =  6,
            NAVDATA_TRIMS_TAG           =  7,
            NAVDATA_RC_REFERENCES_TAG   =  8,
            NAVDATA_PWM_TAG             =  9,
            NAVDATA_ALTITUDE_TAG        = 10,
            NAVDATA_VISION_RAW_TAG      = 11,
            NAVDATA_VISION_OF_TAG       = 12,
            NAVDATA_VISION_TAG          = 13,
            NAVDATA_VISION_PERF_TAG     = 14,
            NAVDATA_TRACKERS_SEND_TAG   = 15,
            NAVDATA_VISION_DETECT_TAG   = 16,
            NAVDATA_WATCHDOG_TAG        = 17,
            NAVDATA_IPHONE_ANGLES_TAG   = 18,
            NAVDATA_ADC_DATA_FRAME_TAG  = 18,
            NAVDATA_VIDEO_STREAM_TAG    = 19,
            NAVDATA_GAME_TAG            = 20,       // AR.Drone 1.7.4
            NAVDATA_PRESSURE_RAW_TAG    = 21,       // AR.Drone 2.0
            NAVDATA_MAGNETO_TAG         = 22,       // AR.Drone 2.0
            NAVDATA_WIND_TAG            = 23,       // AR.Drone 2.0
            NAVDATA_KALMAN_PRESSURE_TAG = 24,       // AR.Drone 2.0
            NAVDATA_HDVIDEO_STREAM_TAG  = 25,       // AR.Drone 2.0
            NAVDATA_WIFI_TAG            = 26,       // AR.Drone 2.0
            NAVDATA_ZIMMU3000_TAG       = 27,       // AR.Drone 2.0
            NAVDATA_GPS_TAG             = 27,       // AR.Drone 2.4.1
            NAVDATA_CKS_TAG             = 0xFFFF ///< Last navdata option that *must* be included at the end of all navdata packets
        };


    protected:
        typedef std::map<NAVDATA_TAG, std::function<void(const char*)> > NavCallbackFuncs;
        /// List of Callback functions to handle Navdata options
        NavCallbackFuncs m_navCallbackFunc;

        mutable std::mutex m_mutex;
        std::atomic<bool> m_computeWorldData; ///< Enable or Disable the world data computation
        std::atomic<bool> m_needToResetRotation; ///< Permit to know if the rotation needs to be reset
        std::chrono::duration<float> m_navdataDeltaTime; ///< Permit to know the time between two reception
        std::atomic<int> m_state;  ///< Number containing the drone's states
        std::atomic<int> m_sequenceNumber; ///< Sequence number of command send by the drone
        std::atomic<int> m_vision; ///< Number containing informations on Augmented reality data stream
        std::atomic<int> m_batteryLvl; ///< Battery level of the drone
        std::atomic<float> m_altitude; ///< Altitude of the drone
        Vector3 m_startingRotation; ///< The rotation just after the take off
        Vector3 m_rotation; ///< Rotation of the drone (comparing with the initial rotation)
        Vector3 m_localVelocity; ///< Local velocity of the drone
        Vector3 m_worldVelocity; ///< Velocity of the drone
        Vector3 m_worldPosition; ///< Position of the drone (comparing with the take off position)

        /**
         * @brief Retrieves informations contained in NAVDATA_DEMO option
         * @param buffer Navdata demo option
         */
        void navdataDemo(const char *buffer); ///< Manage the attribute contained in the demo part of the sequence receive from the drone

    public:
        /**
         * @brief Construct a Navdata object
         * @param computeWorldData Specify if the world velocity and the world position of the drone
         *                         need to be computed. Set this to false can avoid some calculation.
         */
        Navdata();

        /**
         * @brief Retrieves differents informations in the navdata stream
         * @param navdata buffer containing the navdata
         * @param deltaTime time between two updates call
         */
        virtual void update(const std::string& navdata, std::chrono::duration<double> deltaTime); ///< Update all the attributes

        /**
         * @brief Check if world data are computed.
         * @return true if world data are computed.
         */
        virtual bool isComputingWorldData() const {return m_computeWorldData;}
        /**
         * @brief Enable or disable world data calculation.
         *
         * Can only be done when drone is not flying.
         */
        virtual void setComputeWorldData(bool activate);

        /**
         * @brief Set position to 0, Set y and z orientation to 0, x is not reset because it's the current orientation
         */
        virtual void resetWorldData();

        virtual int getState() const {return m_state;} ///< Return the number containing the drone's states
        virtual int getSequenceNumber() const {return m_sequenceNumber;} ///< Return the sequence number of command send by the drone
        virtual int getVisionFlags() const {return m_vision;} ///< Return the number containing vision's informations (RA)

        /**
         * @brief Return the battery level in percentage.
         */
        virtual int getBatteryPercentage() const {return m_batteryLvl;}
        /**
         * @brief Give the drone altitude based on the ground.
         * @return The altitude in meters.
         */
        virtual float getAltitude() const {return m_altitude;}
        /**
         * @brief Return euler angles of the drone, in degrees.
         */
        virtual Vector3 getRotation() const;
        /**
         * @brief Return euler angles of the drone, in radians.
         */
        virtual Vector3 getRotationInRad() const;
        /**
         * @brief Return the velocity of the drone, in meters, in drone's local coordinates.
         */
        virtual Vector3 getLocalVelocity() const;
        /**
         * @brief Return the velocity of the drone, in meters, in world coordinates.
         */
        virtual Vector3 getVelocity() const;
        /**
         * @brief Return the estimated position of the drone, in meters, in world coordinates.
         *
         * The take off position is considered as the origin (0, 0, 0).
         */
        virtual Vector3 getPosition() const;
    };
}

#endif // UCAPA_NAVDATA_H
