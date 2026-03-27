# pragma once

#include "cmath"
#include "cstdint"

/*I like it fast, this just does (1/sqrt(x)) like really fast*/
inline float invSqrt(float x){
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
    #pragma GCC diagnostic ignored "-Wuninitialized"

        // prep values
        float halfx = 0.5f * x;
        float y = x;

        // reinterprets the bits of the float as an integer
        long i = *(long *)&y;

        // divdes exponent by 2 then subtracts from magic number
        // this ↓ magic number 1,597,463,007 in decimal
        i = 0x5f3759df - (i >> 1);

        // back to float, y is now a rough approximation
        y = *(float *)&i;

        // Newton-Raphson refinement
        y = y * (1.5f - (halfx * y * y));
        y = y * (1.5f - (halfx * y * y));

    #pragma GCC dignostic pop
        return y;
}

template<typename T>
class VectorBase;

class Quaternion
{
public:
    float w = 1.f;
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    Quaternion() = default;

    Quaternion(float nw, float nx, float ny, float nz) : w(nw), x(nx), y(ny), z(nz) {}

    Quaternion getProduct(const Quaternion& q) const
    {
        return Quaternion(
            w * q.w - x * q.x - y * q.y - z * q.z,
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y - x * q.z + y * q.w + z * q.x,
            w * q.z + x * q.y - y * q.x + z * q.w);
    }

    Quaternion operator*(const Quaternion& q) const
    {
        return getProduct(q);
    }

    Quaternion& operator*=(const Quaternion& q)
    {
        *this = getProduct(q);
        return *this;
    }

    Quaternion getConjugate() const
    {
        return Quaternion(w, -x, -y, -z);
    }

    float getMagnitude() const
    {
        return sqrt(w * w + x * x + y * y + z * z);
    }

    void normalize()
    {
        float m = invSqrt(w * w + x * x + y * y + z * z);
        (*this) *= m;
    }

    Quaternion getNormalized() const
    {
        Quaternion r(w, x, y, z);
        r.normalize();
        return r;
    }

    float get(size_t i) const
    {
        return i == 0 ? w : (i == 1 ? x : (i == 2 ? y : (i == 3 ? z : 0)));
    }

    Quaternion operator*(float v) const
    {
        return Quaternion(w * v, x * v, y * v, z * v);
    }

    Quaternion& operator*=(float v)
    {
        w *= v;
        x *= v;
        y *= v;
        z *= v;
        return *this;
    }

    Quaternion operator/(float v) const
    {
        return Quaternion(w / v, x / v, y / v, z / v);
    }

    Quaternion operator+(const Quaternion& q) const
    {
        return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
    }

    float static dot(const Quaternion& q1, const Quaternion& q2)
    {
        return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
    }

    Quaternion static lerp(const Quaternion& q1, const Quaternion& q2, float t)
    {
        return (q1 * (1.f - t) + q2 * t).getNormalized();
    }

    Quaternion static slerp(const Quaternion& q1, const Quaternion& q2, float pc)
    {
        Quaternion qa = q1.getNormalized();
        Quaternion qb = q2.getNormalized();

        float cosHalfTheta = dot(qa, qb);
        if (cosHalfTheta < 0)
        {
            qb = qb * -1.f;
            cosHalfTheta = -cosHalfTheta;
        }

        if (std::fabs(cosHalfTheta) >= 0.995f)
        {
            return lerp(qa, qb, pc);
        }

        float halfTheta = acosf(cosHalfTheta);
        float sinHalfTheta = sqrtf(1.0 - cosHalfTheta * cosHalfTheta);

        if (std::fabs(sinHalfTheta) < 0.001)
        {
            return (qa + qb) / 2.f;
        }

        float ra = sinf((1.f - pc) * halfTheta) / sinHalfTheta;
        float rb = sinf(pc * halfTheta) / sinHalfTheta;

        return qa * ra + qb * rb;
    }

    template<typename T>
    void toAngleVector(float &angle, VectorBase<T>& v) const
    {
        float halfTheta = acosf(w);
        float sinHalfTheta = sinf(halfTheta);
        angle = 2.0f * halfTheta;
        if (sinHalfTheta == 0)
        {
            v.x = 1.0;
            v.y = 0;
            v.z = 0;
        }
        else
        {
            v.x = x / sinHalfTheta;
            v.y = y / sinHalfTheta;
            v.z = z / sinHalfTheta;
        }
    }

    template<typename T>
    void fromAngleVector(float angle, const VectorBase<T>& v)
    {
        float halfAngle = angle * 0.5f;
        float sinHalfTheta = sinf(halfAngle);
        w = cosf(halfAngle);
        x = v.x * sinHalfTheta;
        y = v.y * sinHalfTheta;
        z = v.z * sinHalfTheta;
    }

    template<typename T>
    void fromAngularVelocity(const VectorBase<T>& v, float dt)
    {
        float theta = v.getMagnitude() * dt;
        fromAngleVector(theta, v.getNormalized());
    }
};

template<typename T>
class VectorBase
{
public:
    T x = T{};
    T y = T{};
    T z = T{};

    VectorBase() = default;
    VectorBase(const VectorBase& o) = default;
    VectorBase(T nx, T ny, T nz) : x{nx}, y{ny}, z{nz} {}

    VectorBase& operator=(const VectorBase& o)
    {
        if (this == &o)
            return *this;
        x = o.x;
        y = o.y;
        z = o.z;
        return *this;
    }

    T get(size_t i) const { return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : T())); }
    T operator[](size_t i) const { return get(i); }

    void set(size_t i, T v) { i == 0 ? x = v : (i == 1 ? y = v : (i == 2 ? z = v : false)); }

    operator VectorBase<float>() const
    {
        return VectorBase<float>(x, y, z);
    }

    float getMagnitude() const
    {
        return sqrtf(x * x + y * y + z * z);
    }

    VectorBase<T>& normalize()
    {
        float m = invSqrt(x * x + y * y + z * z);
        (*this) *= m;
        return *this;
    }

    VectorBase<T> getNormalized() const
    {
        VectorBase<T> r(x, y, z);
        r.normalize();
        return r;
    }

    void rotate(const Quaternion& q)
    {
        Quaternion p(0, x, y, z);
        p = q * p * q.getConjugate();
        x = p.x;
        y = p.y;
        z = p.z;
    }

    VectorBase<T> getRotated(const Quaternion& q) const
    {
        VectorBase<T> r(x, y, z);
        r.rotate(q);
        return r;
    }

    float static dotProduct(const VectorBase<T>& a, const VectorBase<T>& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    VectorBase<T> static crossProduct(const VectorBase<T>& a, const VectorBase<T>& b)
    {
        VectorBase<T> r;
        r.x = a.y * b.z - a.z * b.y;
        r.y = a.z * b.x - a.x * b.z;
        r.z = a.x * b.y - a.y * b.x;
        return r;
    }

    float dot(const VectorBase<T>& v) const
    {
        return dotProduct(*this, v);
    }

    VectorBase<T> cross(const VectorBase<T>& v) const
    {
        return crossProduct(*this, v);
    }

    VectorBase<T> accelToEuler() const
    {
        VectorBase<T> na = getNormalized();
        T x = atan2f(na.y, na.z);
        T y = -atan2f(na.x, sqrt(na.y * na.y + na.z * na.z));
        T z = 0.f;
        return VectorBase<T>(x, y, z);
    }

    Quaternion accelToQuaternion() const
    {
        return diffVectors(VectorBase<T>(T(0), T(0), T(1)), *this, 1.0f);
    }

    static Quaternion diffVectors(const VectorBase<T>& src, const VectorBase<T>& dst, float gain = 1.f)
    {
        VectorBase<T> src_n = src.getNormalized();
        VectorBase<T> dst_n = dst.getNormalized();
        Quaternion q;

        float angle = acosf(dst_n.dot(src_n));
        VectorBase<T> v = dst_n.cross(src_n).getNormalized();
        q.fromAngleVector(angle * gain, v);
        return q.getNormalized();
    }

    Quaternion eulerToQuaternion() const
    {
        Quaternion q;
        float cosX2 = cosf(x / 2.0f);
        float sinX2 = sinf(x / 2.0f);
        float cosY2 = cosf(y / 2.0f);
        float sinY2 = sinf(y / 2.0f);
        float cosZ2 = cosf(z / 2.0f);
        float sinZ2 = sinf(z / 2.0f);

        q.w = cosX2 * cosY2 * cosZ2 + sinX2 * sinY2 * sinZ2;
        q.x = sinX2 * cosY2 * cosZ2 - cosX2 * sinY2 * sinZ2;
        q.y = cosX2 * sinY2 * cosZ2 + sinX2 * cosY2 * sinZ2;
        q.z = cosX2 * cosY2 * sinZ2 - sinX2 * sinY2 * cosZ2;
        q.normalize();
        return q;
    }

    VectorBase<T> eulerFromQuaternion(const Quaternion& q)
    {
        x = atan2f(q.w * q.x + q.y * q.z, 0.5f - q.x * q.x - q.y * q.y);
        y = asinf(-2.0f * (q.x * q.z - q.w * q.y));
        z = atan2f(q.x * q.y + q.w * q.z, 0.5f - q.y * q.y - q.z * q.z);
        return *this;
    }

    VectorBase<T>& operator+=(const VectorBase<T>& o)
    {
        x += o.x;
        y += o.y;
        z += o.z;
        return *this;
    }

    VectorBase<T> operator+(const VectorBase<T>& o)
    {
        VectorBase<T> r(*this);
        r += o;
        return r;
    }

    VectorBase<T>& operator-=(const VectorBase<T>& o)
    {
        x -= o.x;
        y -= o.y;
        z -= o.z;
        return *this;
    }

    VectorBase<T> operator-(const VectorBase<T>& o)
    {
        VectorBase<T> r(*this);
        r -= o;
        return r;
    }

    VectorBase<T>& operator*=(const VectorBase<T>& o)
    {
        x *= o.x;
        y *= o.y;
        z *= o.z;
        return *this;
    }

    VectorBase<T> operator*(const VectorBase<T>& o)
    {
        VectorBase<T> r(*this);
        r *= o;
        return r;
    }

    VectorBase<T>& operator*=(T o)
    {
        x *= o;
        y *= o;
        z *= o;
        return *this;
    }

    VectorBase<T> operator*(T o)
    {
        VectorBase<T> r(*this);
        r *= o;
        return r;
    }

    VectorBase<T>& operator/=(T o)
    {
        x /= o;
        y /= o;
        z /= o;
        return *this;
    }

    VectorBase<T> operator/(T o)
    {
        VectorBase<T> r(*this);
        r /= o;
        return r;
    }
};

template<typename T>
class RotationMatrix
{
public:
    RotationMatrix() = default;

    void init(const VectorBase<T>& v)
    {
        const T cosx = cosf(v.x);
        const T sinx = sinf(v.x);
        const T cosy = cosf(v.y);
        const T siny = sinf(v.y);
        const T cosz = cosf(v.z);
        const T sinz = sinf(v.z);

        const T coszcosx = cosz * cosx;
        const T sinzcosx = sinz * cosx;
        const T coszsinx = sinx * cosz;
        const T sinzsinx = sinx * sinz;

        _m[0][0] = cosz * cosy;
        _m[0][1] = -cosy * sinz;
        _m[0][2] = siny;
        _m[1][0] = sinzcosx + (coszsinx * siny);
        _m[1][1] = coszcosx - (sinzsinx * siny);
        _m[1][2] = -sinx * cosy;
        _m[2][0] = (sinzsinx) - (coszcosx * siny);
        _m[2][1] = (coszsinx) + (sinzcosx * siny);
        _m[2][2] = cosy * cosx;
    }

    VectorBase<T> apply(const VectorBase<T>& v)
    {
        const T x = _m[0][0] * v.x + _m[1][0] * v.y + _m[2][0] * v.z;
        const T y = _m[0][1] * v.x + _m[1][1] * v.y + _m[2][1] * v.z;
        const T z = _m[0][2] * v.x + _m[1][2] * v.y + _m[2][2] * v.z;
        return VectorBase<T>{x, y, z};
    }

private:
    T _m[3][3] = {
        {T{1}, T{0}, T{0}},
        {T{0}, T{1}, T{0}},
        {T{0}, T{0}, T{1}},
    };
};

using VectorFloat = VectorBase<float>;
using VectorInt16 = VectorBase<int16_t>;
using RotationMatrixFloat = RotationMatrix<float>;