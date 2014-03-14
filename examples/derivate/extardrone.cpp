#include "extardrone.h"

ExtARDrone::ExtARDrone(std::string sessionId, std::string userId, std::string appId,
                       const std::string ardIp,
                       const unsigned short ardATCmdsPort,
                       const unsigned short ardNavdataPort,
                       const unsigned short ardVideoPort,
                       const unsigned short ardControlPort)
    : ARDrone(sessionId, userId, appId,
              ardIp, ardATCmdsPort, ardNavdataPort, ardVideoPort, ardControlPort,
              new ucapa::ARDroneConnections(ardIp, ardATCmdsPort, ardNavdataPort, ardVideoPort, ardControlPort),
              new ExtNavdata(),
              new ucapa::Video(ardIp, ardVideoPort))
{

}
