#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>

template <typename T>
[[nodiscard]] inline constexpr T invSqrt(T x) noexcept {
    return T(1) / std::sqrt(x);
}

template<typename T>
class VectorBase;

class Quaternion
{
public:
    float w{1.f}, x{0.f}, y{0.f}, z{0.f};

    constexpr Quaternion() noexcept = default;

    constexpr Quaternion(float nw, float nx, float ny, float nz) noexcept 
        : w(nw), x(nx), y(ny), z(nz) {}

    [[nodiscard]] constexpr Quaternion operator*(const Quaternion& q) const noexcept
    {
        return {
            w * q.w - x * q.x - y * q.y - z * q.z,
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y - x * q.z + y * q.w + z * q.x,
            w * q.z + x * q.y - y * q.x + z * q.w
        };
    }

    constexpr Quaternion& operator*=(const Quaternion& q) noexcept
    {
        *this = *this * q;
        return *this;
    }

    [[nodiscard]] constexpr Quaternion getConjugate() const noexcept
    {
        return {w, -x, -y, -z};
    }

    [[nodiscard]] inline float getMagnitude() const noexcept
    {
        return std::sqrt(w * w + x * x + y * y + z * z);
    }

    inline void normalize() noexcept
    {
        const float m = invSqrt(w * w + x * x + y * y + z * z);
        *this *= m;
    }

    [[nodiscard]] inline Quaternion getNormalized() const noexcept
    {
        Quaternion r(w, x, y, z);
        r.normalize();
        return r;
    }

    [[nodiscard]] constexpr float get(size_t i) const noexcept
    {
        return i == 0 ? w : (i == 1 ? x : (i == 2 ? y : (i == 3 ? z : 0.f)));
    }

    [[nodiscard]] constexpr Quaternion operator*(float v) const noexcept
    {
        return {w * v, x * v, y * v, z * v};
    }

    constexpr Quaternion& operator*=(float v) noexcept
    {
        w *= v; x *= v; y *= v; z *= v;
        return *this;
    }

    [[nodiscard]] constexpr Quaternion operator/(float v) const noexcept
    {
        const float inv = 1.0f / v; // Multiply by inverse is faster than division
        return {w * inv, x * inv, y * inv, z * inv};
    }

    [[nodiscard]] constexpr Quaternion operator+(const Quaternion& q) const noexcept
    {
        return {w + q.w, x + q.x, y + q.y, z + q.z};
    }

    [[nodiscard]] static constexpr float dot(const Quaternion& q1, const Quaternion& q2) noexcept
    {
        return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
    }

    [[nodiscard]] static inline Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t) noexcept
    {
        return (q1 * (1.f - t) + q2 * t).getNormalized();
    }

    [[nodiscard]] static inline Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float pc) noexcept
    {
        Quaternion qa = q1.getNormalized();
        Quaternion qb = q2.getNormalized();

        float cosHalfTheta = dot(qa, qb);
        if (cosHalfTheta < 0.f)
        {
            qb *= -1.f;
            cosHalfTheta = -cosHalfTheta;
        }

        if (std::abs(cosHalfTheta) >= 0.995f)
        {
            return lerp(qa, qb, pc);
        }

        const float halfTheta = std::acos(cosHalfTheta);
        const float sinHalfTheta = std::sqrt(1.0f - cosHalfTheta * cosHalfTheta);

        if (std::abs(sinHalfTheta) < 0.001f)
        {
            return (qa + qb) * 0.5f;
        }

        const float invSinHalfTheta = 1.0f / sinHalfTheta;
        const float ra = std::sin((1.f - pc) * halfTheta) * invSinHalfTheta;
        const float rb = std::sin(pc * halfTheta) * invSinHalfTheta;

        return qa * ra + qb * rb;
    }

    template<typename T>
    inline void toAngleVector(float& angle, VectorBase<T>& v) const noexcept
    {
        const float halfTheta = std::acos(std::clamp(w, -1.0f, 1.0f));
        const float sinHalfTheta = std::sin(halfTheta);
        angle = 2.0f * halfTheta;
        
        if (std::abs(sinHalfTheta) < 1e-6f)
        {
            v.x = T(1); v.y = T(0); v.z = T(0);
        }
        else
        {
            const float invSin = 1.0f / sinHalfTheta;
            v.x = static_cast<T>(x * invSin);
            v.y = static_cast<T>(y * invSin);
            v.z = static_cast<T>(z * invSin);
        }
    }

    template<typename T>
    inline void fromAngleVector(float angle, const VectorBase<T>& v) noexcept
    {
        const float halfAngle = angle * 0.5f;
        const float sinHalfTheta = std::sin(halfAngle);
        w = std::cos(halfAngle);
        x = static_cast<float>(v.x) * sinHalfTheta;
        y = static_cast<float>(v.y) * sinHalfTheta;
        z = static_cast<float>(v.z) * sinHalfTheta;
    }

    template<typename T>
    inline void fromAngularVelocity(const VectorBase<T>& v, float dt) noexcept
    {
        const float theta = v.getMagnitude() * dt;
        fromAngleVector(theta, v.getNormalized());
    }
};

template<typename T>
class VectorBase
{
public:
    T x{0}, y{0}, z{0};

    constexpr VectorBase() noexcept = default;
    constexpr VectorBase(T nx, T ny, T nz) noexcept : x{nx}, y{ny}, z{nz} {}

    [[nodiscard]] constexpr T get(size_t i) const noexcept { return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : T(0))); }
    [[nodiscard]] constexpr T operator[](size_t i) const noexcept { return get(i); }

    constexpr void set(size_t i, T v) noexcept { if(i == 0) x = v; else if(i == 1) y = v; else if(i == 2) z = v; }

    [[nodiscard]] explicit constexpr operator VectorBase<float>() const noexcept
    {
        return VectorBase<float>(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
    }

    [[nodiscard]] inline T getMagnitude() const noexcept
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    inline VectorBase<T>& normalize() noexcept
    {
        const T m = invSqrt(x * x + y * y + z * z);
        *this *= m;
        return *this;
    }

    [[nodiscard]] inline VectorBase<T> getNormalized() const noexcept
    {
        VectorBase<T> r(*this);
        return r.normalize();
    }

    inline void rotate(const Quaternion& q) noexcept
    {
        Quaternion p(0, static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
        p = q * p * q.getConjugate();
        x = static_cast<T>(p.x);
        y = static_cast<T>(p.y);
        z = static_cast<T>(p.z);
    }

    [[nodiscard]] inline VectorBase<T> getRotated(const Quaternion& q) const noexcept
    {
        VectorBase<T> r(*this);
        r.rotate(q);
        return r;
    }

    [[nodiscard]] static constexpr T dotProduct(const VectorBase<T>& a, const VectorBase<T>& b) noexcept
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    [[nodiscard]] static constexpr VectorBase<T> crossProduct(const VectorBase<T>& a, const VectorBase<T>& b) noexcept
    {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    [[nodiscard]] constexpr T dot(const VectorBase<T>& v) const noexcept
    {
        return dotProduct(*this, v);
    }

    [[nodiscard]] constexpr VectorBase<T> cross(const VectorBase<T>& v) const noexcept
    {
        return crossProduct(*this, v);
    }

    [[nodiscard]] inline VectorBase<T> accelToEuler() const noexcept
    {
        VectorBase<T> na = getNormalized();
        return {
            static_cast<T>(std::atan2(na.y, na.z)),
            static_cast<T>(-std::atan2(na.x, std::sqrt(na.y * na.y + na.z * na.z))),
            T(0)
        };
    }

    [[nodiscard]] inline Quaternion accelToQuaternion() const noexcept
    {
        return diffVectors(VectorBase<T>(T(0), T(0), T(1)), *this, 1.0f);
    }

    [[nodiscard]] static inline Quaternion diffVectors(const VectorBase<T>& src, const VectorBase<T>& dst, float gain = 1.f) noexcept
    {
        VectorBase<T> src_n = src.getNormalized();
        VectorBase<T> dst_n = dst.getNormalized();
        
        float angle = std::acos(std::clamp(static_cast<float>(dst_n.dot(src_n)), -1.0f, 1.0f));
        VectorBase<T> v = dst_n.cross(src_n).getNormalized();
        
        Quaternion q;
        q.fromAngleVector(angle * gain, v);
        return q.getNormalized();
    }

    [[nodiscard]] inline Quaternion eulerToQuaternion() const noexcept
    {
        const float halfX = static_cast<float>(x) * 0.5f;
        const float halfY = static_cast<float>(y) * 0.5f;
        const float halfZ = static_cast<float>(z) * 0.5f;
        
        const float cosX2 = std::cos(halfX), sinX2 = std::sin(halfX);
        const float cosY2 = std::cos(halfY), sinY2 = std::sin(halfY);
        const float cosZ2 = std::cos(halfZ), sinZ2 = std::sin(halfZ);

        Quaternion q;
        q.w = cosX2 * cosY2 * cosZ2 + sinX2 * sinY2 * sinZ2;
        q.x = sinX2 * cosY2 * cosZ2 - cosX2 * sinY2 * sinZ2;
        q.y = cosX2 * sinY2 * cosZ2 + sinX2 * cosY2 * sinZ2;
        q.z = cosX2 * cosY2 * sinZ2 - sinX2 * sinY2 * cosZ2;
        q.normalize();
        return q;
    }

    inline VectorBase<T>& eulerFromQuaternion(const Quaternion& q) noexcept
    {
        x = static_cast<T>(std::atan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)));
        y = static_cast<T>(std::asin(std::clamp(2.0f * (q.w * q.y - q.z * q.x), -1.0f, 1.0f)));
        z = static_cast<T>(std::atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)));
        return *this;
    }

    constexpr VectorBase<T>& operator+=(const VectorBase<T>& o) noexcept
    {
        x += o.x; y += o.y; z += o.z;
        return *this;
    }

    [[nodiscard]] constexpr VectorBase<T> operator+(const VectorBase<T>& o) const noexcept
    {
        return {x + o.x, y + o.y, z + o.z};
    }

    constexpr VectorBase<T>& operator-=(const VectorBase<T>& o) noexcept
    {
        x -= o.x; y -= o.y; z -= o.z;
        return *this;
    }

    [[nodiscard]] constexpr VectorBase<T> operator-(const VectorBase<T>& o) const noexcept
    {
        return {x - o.x, y - o.y, z - o.z};
    }

    constexpr VectorBase<T>& operator*=(const VectorBase<T>& o) noexcept
    {
        x *= o.x; y *= o.y; z *= o.z;
        return *this;
    }

    [[nodiscard]] constexpr VectorBase<T> operator*(const VectorBase<T>& o) const noexcept
    {
        return {x * o.x, y * o.y, z * o.z};
    }

    constexpr VectorBase<T>& operator*=(T o) noexcept
    {
        x *= o; y *= o; z *= o;
        return *this;
    }

    [[nodiscard]] constexpr VectorBase<T> operator*(T o) const noexcept
    {
        return {x * o, y * o, z * o};
    }

    constexpr VectorBase<T>& operator/=(T o) noexcept
    {
        // For floats/doubles, compiler will optimize to multiply by inverse
        x /= o; y /= o; z /= o;
        return *this;
    }

    [[nodiscard]] constexpr VectorBase<T> operator/(T o) const noexcept
    {
        return {x / o, y / o, z / o};
    }
};

template<typename T>
class RotationMatrix
{
public:
    constexpr RotationMatrix() noexcept = default;

    inline void init(const VectorBase<T>& v) noexcept
    {
        // Using std functions allows the compiler to group sin/cos calls 
        // using combined architecture instructions (e.g. fsincos on x86)
        const T cosx = std::cos(v.x);
        const T sinx = std::sin(v.x);
        const T cosy = std::cos(v.y);
        const T siny = std::sin(v.y);
        const T cosz = std::cos(v.z);
        const T sinz = std::sin(v.z);

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
        
        _m[2][0] = sinzsinx - (coszcosx * siny);
        _m[2][1] = coszsinx + (sinzcosx * siny);
        _m[2][2] = cosy * cosx;
    }

    [[nodiscard]] constexpr VectorBase<T> apply(const VectorBase<T>& v) const noexcept
    {
        return {
            _m[0][0] * v.x + _m[1][0] * v.y + _m[2][0] * v.z,
            _m[0][1] * v.x + _m[1][1] * v.y + _m[2][1] * v.z,
            _m[0][2] * v.x + _m[1][2] * v.y + _m[2][2] * v.z
        };
    }

private:
    T _m[3][3] = {
        {T(1), T(0), T(0)},
        {T(0), T(1), T(0)},
        {T(0), T(0), T(1)}
    };
};

using VectorFloat = VectorBase<float>;
using VectorInt16 = VectorBase<int16_t>;
using RotationMatrixFloat = RotationMatrix<float>;