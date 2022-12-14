#pragma once

#include "Vector.h"

namespace nc
{
struct Quaternion
{
    float x, y, z, w;

    Quaternion();
    Quaternion(float X, float Y, float Z, float W);

    Vector3 ToEulerAngles() const noexcept;
    void ToAxisAngle(Vector3* axisOut, float* angleOut) const noexcept;

    static Quaternion Identity() { return Quaternion{0.0f, 0.0f, 0.0f, 1.0f}; }
    static Quaternion FromEulerAngles(const Vector3& angles);
    static Quaternion FromEulerAngles(float x, float y, float z);
    static Quaternion FromAxisAngle(const Vector3& axis, float radians); // axis cannot be zero
};

/** Return a normalized quaternion. */
Quaternion Normalize(const Quaternion& quat);

/** note: For consistency with DirectXMath, the argument order is reversed from the order in which they are
 *  multiplied. In other words, this computes the lhs rotation followed by rhs (or the product rhs*lhs). */
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

/** Finds rotation between lhs and rhs such that result * lhs == rhs. */
Quaternion Difference(const Quaternion& lhs, const Quaternion& rhs);

/** Interpolates from lhs to rhs. */
Quaternion Slerp(const Quaternion& lhs, const Quaternion& rhs, float factor);

/** Slerp from Identity to quat. */
Quaternion Scale(const Quaternion& quat, float factor);

inline bool operator==(const Quaternion& lhs, const Quaternion& rhs)
{
    return FloatEqual(lhs.x, rhs.x) &&
           FloatEqual(lhs.y, rhs.y) &&
           FloatEqual(lhs.z, rhs.z) &&
           FloatEqual(lhs.w, rhs.w);
}

inline bool operator!=(const Quaternion& lhs, const Quaternion& rhs)
{
    return !(lhs == rhs);
}
} // namespace nc
