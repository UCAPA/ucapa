#ifndef EXTNAVDATA_H
#define EXTNAVDATA_H

#include <atomic>
#include <vector>

#include <navdata.h>

class ExtNavdata : public ucapa::Navdata
{
protected:
    std::atomic<unsigned short> m_rawAccelerometers[3];
    std::atomic<short> m_rawGyrometers[3];
    std::atomic<unsigned int> m_batteryVoltageRaw;

    void navdataRawMeasures(const char* buffer);

public:
    ExtNavdata();

    ucapa::Vector3 getRawAccelerometers() const;
    ucapa::Vector3 getRawGyrometers() const;
    unsigned int getRawBatteryVoltage() const;

};

#endif // EXTNAVDATA_H
