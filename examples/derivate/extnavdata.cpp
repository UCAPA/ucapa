#include "extnavdata.h"

ExtNavdata::ExtNavdata()
    : Navdata()
{
    m_navCallbackFunc[NAVDATA_RAW_MEASURES_TAG]
            = std::function<void(const char*)>(std::bind(&ExtNavdata::navdataRawMeasures, this, std::placeholders::_1));
}


void ExtNavdata::navdataRawMeasures(const char* buffer)
{
    int* i = (int*)buffer;
    i++; // Go to option data
    i++; // Jump the control state

    unsigned short* us = (unsigned short*)i;
    //unsigned short acc = *(++us);
    m_rawAccelerometers[0] = *us;
    ++us;
    m_rawAccelerometers[1] = *us;
    ++us;
    m_rawAccelerometers[2] = *us;
    ++us;

    short* s = (short*) us;
    m_rawGyrometers[0] = *s;
    ++s;
    m_rawGyrometers[1] = *s;
    ++s;
    m_rawGyrometers[2] = *s;
    ++s;

    ++s;
    ++s;

    unsigned int* ui = (unsigned int*)s;
    m_batteryVoltageRaw = *ui;
    ++ui;

}


ucapa::Vector3 ExtNavdata::getRawAccelerometers() const
{
    return ucapa::Vector3(m_rawAccelerometers[0], m_rawAccelerometers[1], m_rawAccelerometers[2]);
}

ucapa::Vector3 ExtNavdata::getRawGyrometers() const
{
    return ucapa::Vector3(m_rawGyrometers[0], m_rawGyrometers[1], m_rawGyrometers[2]);
}

unsigned int ExtNavdata::getRawBatteryVoltage() const
{
    return m_batteryVoltageRaw;
}


