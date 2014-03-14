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

#include <quaternion.h>

namespace ucapa{
    Quaternion::Quaternion()
        : x(0), y(0), z(0), w(0)
    {

    }

    Quaternion::Quaternion(float x, float y, float z, float w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    Quaternion::Quaternion(float x, float y, float z)
    {
        setFromEulerAngles(x, y, z);
    }

    Quaternion::Quaternion(const Vector3& v)
    {
        setFromEulerAngles(v.x, v.y, v.z);
    }

    Quaternion& Quaternion::setFromEulerAngles(float x, float y, float z)
    {
        double angle;

        angle = x * 0.5;
        const double cx = cos(angle);
        const double sx = sin(angle);

        angle = y * 0.5;
        const double cy = cos(angle);
        const double sy = sin(angle);

        angle = z * 0.5;
        const double cz = cos(angle);
        const double sz = sin(angle);

        const double cycz = cy * cz;
        const double sycz = sy * cz;
        const double cysz = cy * sz;
        const double sysz = sy * sz;

        this->x = (float)(sx * cycz - cx * sysz);
        this->y = (float)(cx * sycz + sx * cysz);
        this->z = (float)(cx * cysz - sx * sycz);
        this->w = (float)(cx * cycz + sx * sysz);

        *this = normalized();
        return *this;
    }

    Quaternion& Quaternion::setFromEulerAngles(const Vector3& v)
    {
        return setFromEulerAngles(v.x, v.y, v.z);
    }

    Quaternion Quaternion::normalized() const
    {
        float n = x*x + y*y + z*z + w*w;
        n = 1.0 / sqrt(n);
        return (*this * n);
    }

    bool Quaternion::operator!() const
    {
        return (x != 0 && y != 0 && z != 0 && w != 0);
    }

    bool Quaternion::operator==(const Quaternion& q) const
    {
        return (x == q.x && y == q.y && z == q.z && w == q.w);
    }

    bool Quaternion::operator!=(const Quaternion& q) const
    {
        return !(*this == q);
    }


    Quaternion Quaternion::operator+(const Quaternion& q) const
    {
        return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
    }

    Quaternion Quaternion::operator*(const Quaternion& q) const
    {
        Quaternion tmp;

        tmp.w = (q.w * w) - (q.x * x) - (q.y * y) - (q.z * z);
        tmp.x = (q.w * x) + (q.x * w) + (q.y * z) - (q.z * y);
        tmp.y = (q.w * y) + (q.y * w) + (q.z * x) - (q.x * z);
        tmp.z = (q.w * z) + (q.z * w) + (q.x * y) - (q.y * x);

        return tmp;
    }

    // multiplication operator
    Quaternion Quaternion::operator*(float s) const
    {
        return Quaternion(s*x, s*y, s*z, s*w);
    }

    // multiplication operator
    Quaternion& Quaternion::operator*=(float s)
    {
        x*=s;
        y*=s;
        z*=s;
        w*=s;
        return *this;
    }

    Matrix<float> Quaternion::getMatrix() const
    {
        Matrix<float> m(4, 4);
        m(0, 0) = 1.0f - 2.0f*y*y - 2.0f*z*z;
        m(1, 0) = 2.0f*x*y + 2.0f*z*w;
        m(2, 0) = 2.0f*x*z - 2.0f*y*w;
        m(3, 0) = 0.0f;

        m(0, 1) = 2.0f*x*y - 2.0f*z*w;
        m(1, 1) = 1.0f - 2.0f*x*x - 2.0f*z*z;
        m(2, 1) = 2.0f*z*y + 2.0f*x*w;
        m(3, 1) = 0.0f;

        m(0, 2) = 2.0f*x*z + 2.0f*y*w;
        m(1, 2) = 2.0f*z*y - 2.0f*x*w;
        m(2, 2) = 1.0f - 2.0f*x*x - 2.0f*y*y;
        m(3, 2) = 0.0f;

        m(0, 3) = 0;
        m(1, 3) = 0;
        m(2, 3) = 0;
        m(3, 3) = 1.f;

        return m;
    }


    float Quaternion::dot(const Quaternion& q) const
    {
        return x * q.x + y * q.y + z * q.z + w * q.w;
    }

    float Quaternion::getAngle() const
    {
        return 2.0*acos(w)*(180/PI);
    }

    Vector3 Quaternion::getVectorAxis() const
    {
        return Vector3(x, y, z).normalized();
    }

}
