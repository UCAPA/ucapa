#include <iostream>

#include "extardrone.h"

int main(/*int argc, char *argv[]*/)
{
    ExtARDrone drone("994512fe", "ab55654ef", "c54456e23");

    for (int i = 0; i < 40; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        std::cout << "Battery Percentage: " << drone.nav().getBatteryPercentage() << std::endl;
        std::cout << "Battery Voltage: " << drone.nav().getRawBatteryVoltage() << std::endl;
        ucapa::Vector3 vec;
        vec = drone.nav().getRawAccelerometers();
        std::cout << "Accelerometers: (" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
        vec = drone.nav().getRawGyrometers();
        std::cout << "Gyrometers:     (" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
        std::cout << std::endl;
    }

    drone.animLeds(ucapa::ARDrone::LED_ANIM_BLINK_GREEN_RED, 5.0, 4);

    drone.takeOff();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    drone.land();

    return 0;
}


