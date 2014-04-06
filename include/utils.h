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

#ifndef UCAPA_UTILS_H
#define UCAPA_UTILS_H

#include <cmath>

#include <config.h>

const float PI = 3.1415926535897932384626433832795f;

/**
 * @brief Compute the truncate floor value of number value.
 * @param value Number that will be floor and truncate.
 * @param nbDecimal Number of decimal you want to keep.
 * @return The truncate floor value.
 */
template <typename T> inline T floorValue(const T& value, int nbDecimal);
/// @brief Conversion from millimeters to meters.
template <typename T> inline T mmToM(T mm);
/// @brief Conversion from meters to millimeters.
template <typename T> inline T mToMm(T m);
/// @brief Conversion from degrees to radians.
template <typename T> inline T degToRad(T deg);
/// @brief Conversion from radians to degrees.
template <typename T> inline T radToDeg(T rad);

template <typename T>
inline T floorValue(const T& value, int nbDecimal)
{
    if (nbDecimal < 0)
        nbDecimal = 0;
    double power = pow(10, nbDecimal);
    return (floor(value * power)) / (T)power;
}

template <typename T>
T mmToM(T mm)
{
    return mm/1000;
}

template <typename T>
T mToMm(T m)
{
    return m*1000;
}

template <typename T>
T degToRad(T deg)
{
    return deg*PI/180;
}

template <typename T>
T radToDeg(T rad)
{
    return rad*180/PI;
}

#endif // UCAPA_UTILS_H
