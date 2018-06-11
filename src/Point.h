/* Copyright 2018 Vojtěch Hauser <Vojtech.Hauser+dev@gmail.com>
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
 * Distributions of all or part of the Software intended to be used by the
 * recipients as they would use the unmodified Software, containing
 * modifications that substantially alter, remove, or disable functionality of
 * the Software, outside of the documented configuration mechanisms provided by
 * the Software, shall be modified such that the Original Author's bug reporting
 * email addresses and urls are either replaced with the contact information of
 * the parties responsible for the changes, or removed entirely.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef MADV_POINT_H
#define MADV_POINT_H


#include <tuple>
#include <ns3/vector.h>

template<typename... CoordTypes>
class Point
{
public:
    Point(ns3::Vector2D vector) : Point(vector.x, vector.y)
    {
    }

    Point(ns3::Vector3D vector) : Point(vector.x, vector.y, vector.z)
    {
    }

    Point(CoordTypes... coords) : coords{std::make_tuple(coords...)}
    {
    }

    auto get(const std::size_t position) const
    {
        return std::get<position>(coords);
    }

    operator ns3::Vector2D() const // NOLINT(google-explicit-constructor)
    {
        const double x = std::get<0>(coords);
        const double y = std::get<1>(coords);

        return ns3::Vector2D(x, y);
    }

    operator ns3::Vector3D() const // NOLINT(google-explicit-constructor)
    {
        const double x = std::get<0>(coords);
        const double y = std::get<1>(coords);
        const double z = std::get<2>(coords);

        return ns3::Vector3D(x, y, z);
    }

    template<std::size_t dimension>
    auto get_coord()
    {
        return std::get<dimension>(coords);
    }

private:
    std::tuple<CoordTypes...> coords;
};


#endif //MADV_POINT_H
