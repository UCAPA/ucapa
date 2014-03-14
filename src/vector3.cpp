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

#include <vector3.h>

namespace ucapa{
    const Vector3 Vector3::zero;
    const Vector3 Vector3::right(1, 0, 0);
    const Vector3 Vector3::left(-1, 0, 0);
    const Vector3 Vector3::up(0, 1, 0);
    const Vector3 Vector3::down(0, -1, 0);
    const Vector3 Vector3::forward(0, 0, 1);
    const Vector3 Vector3::backward(0, 0, -1);

    Vector3::Vector3()
        : x(0)
        , y(0)
        , z(0)
    {
    }

    Vector3::Vector3(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }


    float Vector3::magnitude() const
    {
        return std::sqrt(x*x + y*y + z*z);
    }

    Vector3 Vector3::normalized() const
    {
        float m = magnitude();
        return Vector3(x/m, y/m, z/m);
    }


    // Binary operators
    bool Vector3::operator!() const
    {
        return (x != 0 && y != 0 && z != 0);
    }

    bool Vector3::operator==(const Vector3& v) const
    {
        return (x == v.x && y == v.y && z == v.z);
    }

    bool Vector3::operator!=(const Vector3& v) const
    {
        return !(*this == v);
    }

    // Arithmetic operators
    Vector3 Vector3::operator-() const
    {
        return Vector3(-x, -y, -z);
    }

    Vector3 Vector3::operator+=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3 Vector3::operator-=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3 Vector3::operator/=(float f)
    {
        x /= f;
        y /= f;
        z /= f;
        return *this;
    }

    Vector3 Vector3::operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    Vector3 Vector3::operator+(const Vector3& v) const
    {
        Vector3 out;
        out += *this;
        out += v;
        return out;
    }

    Vector3 Vector3::operator-(const Vector3& v) const
    {
        Vector3 out;
        out += *this;
        out -= v;
        return out;
    }

    Vector3 Vector3::operator/(float f) const
    {
        Vector3 out;
        out.x = x / f;
        out.y = y / f;
        out.z = z / f;
        return out;
    }

    Vector3 Vector3::operator*(float f) const
    {
        Vector3 out;
        out.x = x * f;
        out.y = y * f;
        out.z = z * f;
        return out;
    }

    // Dot product
    float Vector3::dot(const Vector3& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }


    // Cross product
    Vector3 Vector3::cross(const Vector3& v) const
    {
        return Vector3(y * v.z - z * v.y,
                       z * v.x - x * v.z,
                       x * v.y - y * v.x);
    }


    float Vector3::angle(const Vector3& v) const
    {
        float angle = dot(v)/(magnitude()*v.magnitude());

        return acos(angle);
    }
}
