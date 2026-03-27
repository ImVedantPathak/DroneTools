#ifndef Madgwick_h
#define Madgwick_h

#include "please_help_me_in_3d_math_bro.h"

class Madgwick{
    private:
        float q0,q1,q2,q3;
        float beta;
        float invSampleFreq;
        float roll, pitch, yaw;
        bool anglesComputed;

        void computeAngles();
    public:
        Madgwick();
        void begin(float sampleFrequency) { invSampleFreq = 1.0f / sampleFrequency; }

        void update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
        void update(float gx, float gy, float gz, float ax, float ay, float az);

        void setKp(float p) {
        beta = p;
        }
        void setKi(float i) {
        (void)i;
        }
        const Quaternion getQuaternion() const {
        return Quaternion(q0, q1, q2, q3);
        }
        const VectorFloat getEuler() {
        if (!anglesComputed) computeAngles();
        return VectorFloat(roll, pitch, yaw);
        }
};
#endif