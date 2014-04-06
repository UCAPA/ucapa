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

#include <navdata.h>

namespace ucapa{
    Navdata::Navdata()
        : m_computeWorldData(false)
        , m_needToResetRotation(false)
        , m_state(0)
        , m_sequenceNumber(0)
        , m_vision(0)
        , m_batteryLvl(-1)
        , m_altitude(0)
    {
        m_navCallbackFunc[NAVDATA_DEMO_TAG]
                = std::function<void(const char*)>(std::bind(&Navdata::navdataDemo, this, std::placeholders::_1));
    }



    void Navdata::navdataDemo(const char *buffer)
    {
        // Retrieve the battery level
        int* i = (int*)buffer;
        i++; // Go to option data
        i++; // Jump the control state
        m_batteryLvl = *i;

        // Retrieve the rotation
        float* f = (float*)i;
        Vector3 oldRot = m_rotation;

        // Order of reception following our reference : (X,Z,Y)
        m_rotation.y = *(++f);
        m_rotation.z = *(++f);
        m_rotation.x = *(++f);
        m_rotation.x = -m_rotation.x;

        m_rotation /= 1000.0f;

        if (m_needToResetRotation) {
            if (std::abs(m_rotation.x - oldRot.x) > 3) {
                m_startingRotation.x = m_rotation.x;
                m_needToResetRotation = false;
            }
        }

        // Retrieve the altitude
        i = (int*)f;
        float previousAltitude = m_altitude;
        m_altitude = (*(++i))/1000.0f;

        // Retrieve the velocity
        f = (float*)i;
        // Order of reception folowing our reference : (Z,X,Y)
        m_localVelocity.z = *(++f);
        m_localVelocity.x = *(++f);
        m_localVelocity.y = *(++f);
        m_localVelocity /= 1000.0f;
        if (m_localVelocity.y == 0) {// If it is a buggy version of the drone firmware
            m_localVelocity.y = (m_altitude - previousAltitude)/m_navdataDeltaTime.count();
        }

        if (m_computeWorldData)
        {
            // Calculate world velocity
            Vector3 rot = m_rotation;
            rot.x = rot.x - m_startingRotation.x;
            rot = rot*(PI/180.0f);
            Quaternion q(rot.z, rot.x, rot.y);
            Matrix<float> T(q.getMatrix());
            Matrix<float> tT(T.transponate());
            m_worldVelocity = tT * m_localVelocity;

            // Update position
            m_worldPosition.x += m_worldVelocity.x * m_navdataDeltaTime.count();
            m_worldPosition.y += m_worldVelocity.y * m_navdataDeltaTime.count();
            m_worldPosition.z += m_worldVelocity.z * m_navdataDeltaTime.count();
        }
    }


    void Navdata::update(const std::string& navdata, std::chrono::duration<double> deltaTime)
    {
        m_mutex.lock();

        m_navdataDeltaTime = deltaTime;

        const char* navdataBuffer = navdata.c_str();

        // Retrieve of the navdatas
        int* i = (int*) navdataBuffer;
        int searchValue = 0x55667788;

        if (*i == searchValue)
        {
            i++;
            // Retrieve the drone's states
            m_state = (*i);
            i++;
            // Retrieve the sequence number
            m_sequenceNumber = (*i);
            i++;
            // Retrieve the Augmented Reality data stream
            m_vision = (*i);
            i++;

            unsigned int index = 16;

            unsigned short tag, size;

            while(index < navdata.size())
            {
                // Retrieve the tag of the option
                tag = *((unsigned short*)(navdataBuffer + index)); index += 2;
                // Retrieve the size of the option
                size = *((unsigned short*)(navdataBuffer + index)); index += 2;
                // Return to intial position
                index-=4;

                NavCallbackFuncs::iterator it = m_navCallbackFunc.find((NAVDATA_TAG)tag);
                if (it != m_navCallbackFunc.end())
                    (it->second)(navdataBuffer+index);

                index+=size;
            }
        }

        m_mutex.unlock();
    }


    void Navdata::setComputeWorldData(bool activate)
    {
        if (getState() & STATE_MASK::FLY_MASK)
            return;

        m_computeWorldData = activate;
    }

    void Navdata::resetWorldData()
    {
        if (getState() & STATE_MASK::FLY_MASK)
            return;

        if (!m_computeWorldData)
            return;

        m_mutex.lock();
        m_needToResetRotation = true;
        m_worldPosition = Vector3();
        m_startingRotation.x = m_rotation.x;
        m_mutex.unlock();
    }


    Vector3 Navdata::getRotation() const
    {
        m_mutex.lock();
        Vector3 rot = m_rotation;
        rot.x = rot.x - m_startingRotation.x;
        m_mutex.unlock();

        return rot;
    }

    Vector3 Navdata::getRotationInRad() const
    {
        return getRotation()*(PI/180.0f);
    }

    Vector3 Navdata::getLocalVelocity() const
    {
        m_mutex.lock();
        Vector3 vel = m_localVelocity;
        m_mutex.unlock();

        return vel;
    }

    Vector3 Navdata::getVelocity() const
    {
        m_mutex.lock();
        Vector3 vel = m_worldVelocity;
        m_mutex.unlock();

        return vel;
    }

    Vector3 Navdata::getPosition() const
    {
        m_mutex.lock();
        Vector3 pos = m_worldPosition;
        m_mutex.unlock();

        return pos;
    }
}
