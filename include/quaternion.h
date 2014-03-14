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

#ifndef UCAPA_QUATERNION_H
#define UCAPA_QUATERNION_H

#include <cmath>

#include <matrix.h>
#include <vector3.h>
#include <utils.h>

namespace ucapa{
    /**
     * @brief A Quaternion class allow to do some 3D rotations
     */
    class Quaternion
    {
    public:
        float x; // Vectorial imaginary part
        float y;
        float z;

        float w; // Real part


        /**
         * @brief Init the quaternion with default properties (0, 0, 0, 0)
         */
        Quaternion();
        /**
         * @brief Init the quaternion with its properties
         * @param x,y,z Vectorial imaginary part
         * @param w Real part
         */
        Quaternion(float x, float y, float z, float w);
        /**
         * @brief Init the quaternion with an euler angle in radian.
         * @param x,y,z Euler angle, in radian
         */
        Quaternion(float x, float y, float z);
        /**
         * @brief Init the quaternion with an euler angle in radian.
         * @paramv Euler angle, in radian
         */
        Quaternion(const Vector3& v);

        /**
         * @brief Set the quaternion with an euler angle in radian.
         * @param x,y,z Euler angle, in radian
         */
        Quaternion& setFromEulerAngles(float x, float y, float z);
        /**
         * @brief Set the quaternion with an euler angle in radian.
         * @paramv Euler angle, in radian
         */
        Quaternion& setFromEulerAngles(const Vector3& v);

        /**
         * @brief Return a normalized quaternion.
         * @return A new normalized quaternion
         */
        Quaternion normalized() const;

        // Binary operators
        bool operator!() const;
        bool operator==(const Quaternion& q) const;
        bool operator!=(const Quaternion& q) const;

        // Arithmetic operators
        Quaternion operator+(const Quaternion& q) const;
        Quaternion operator*(const Quaternion& q) const;
        Quaternion operator*(float s) const;
        Quaternion& operator*=(float s);

        /**
         * @brief Create a matrix from the quaternion
         * @return The generated matrix(4, 4)
         */
        Matrix<float> getMatrix() const;

        /**
         * @brief Compute dot product
         * @param q Other quaternion
         * @return Dot product
         */
        float dot(const Quaternion& q) const;

        float getAngle() const;
        Vector3 getVectorAxis() const;
    };
}

#endif // UCAPA_QUATERNION_H
