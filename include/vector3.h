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

#ifndef UCAPA_VECTOR3_H
#define UCAPA_VECTOR3_H

#include <cmath>

namespace ucapa{
    /**
     * @brief A Vector3 class
     *
     * Allow to store triplet of value and allow to do some maths operations like dot and cross product, etc...
     */
    class Vector3
    {
    public:
        const static Vector3 zero;     ///< Vector3(0, 0, 0)
        const static Vector3 right;    ///< Vector3(1, 0, 0)
        const static Vector3 left;     ///< Vector3(-1, 0, 0)
        const static Vector3 up;       ///< Vector3(0, 1, 0)
        const static Vector3 down;     ///< Vector3(0, -1, 0)
        const static Vector3 forward;  ///< Vector3(0, 0, 1)
        const static Vector3 backward; ///< Vector3(0, 0, -1)

        float x;
        float y;
        float z;

        /**
         * @brief Construct a default Vector3 initialize to (0, 0, 0)
         */
        Vector3();
        /**
         * @brief Construct a Vector3 initialize to (x, y, z)
         */
        Vector3(float x, float y, float z);

        /**
         * @brief Return the magnitude of the vector
         */
        float magnitude() const;
        /**
         * @brief Return a normalized Vector
         * @return A new normalized vector
         */
        Vector3 normalized() const;

        // Binary operators
        bool operator!() const;
        bool operator==(const Vector3& v) const;
        bool operator!=(const Vector3& v) const;

        // Arithmetic operators
        Vector3 operator-() const;
        Vector3 operator+=(const Vector3& v);
        Vector3 operator-=(const Vector3& v);
        Vector3 operator/=(float f);
        Vector3 operator*=(float f);
        Vector3 operator+(const Vector3& v) const;
        Vector3 operator-(const Vector3& v) const;
        Vector3 operator/(float f) const;
        Vector3 operator*(float f) const;
        friend Vector3 operator*(float f, const Vector3& v) { return v*f;}

        /**
         * @brief Compute dot product
         * @param v Other vector
         * @return Dot product
         */
        float dot(const Vector3& v) const;

        /**
         * @brief Compute cross product
         * @param Other vector
         * @return Cross product
         */
        Vector3 cross(const Vector3& v) const;

        /**
         * @brief Compute angle
         * @param v Other vector
         * @return Angle between the two vector in range [-pi, pi]
         */
        float angle(const Vector3& v) const;

    };
}

#endif // UCAPA_VECTOR3_H
