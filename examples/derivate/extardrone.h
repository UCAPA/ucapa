#ifndef EXTARDRONE_H
#define EXTARDRONE_H

#include <ardrone.h>

#include "extnavdata.h"

class ExtARDrone : public ucapa::ARDrone
{
public:
    ExtARDrone(std::string sessionId, std::string userId, std::string appId,
               const std::string ardIp = "192.168.1.1",
               const unsigned short ardATCmdsPort = 5556,
               const unsigned short ardNavdataPort = 5554,
               const unsigned short ardVideoPort = 5555,
               const unsigned short ardControlPort = 5559);

    virtual const ExtNavdata& nav() const {return *(static_cast<ExtNavdata*>(m_navdata.get()));}
};

#endif // EXTARDRONE_H
