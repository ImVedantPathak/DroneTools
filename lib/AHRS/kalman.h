#ifndef _kalman_h_
#define _kalman_h_

class kalman {
public:
    kalman();

    float getAngle(float newAngle, float newRate, float dt);

    void setAngle(float angle);
    float getRate();

    void setQangle(float Q_angle);
    void setQbias(float Q_bias);
    void setRmeasure(float R_measure);

    float getQangle();
    float getQbias();
    float getRmeasure();

private: 
    float Q_angle;
    float Q_bias;
    float R_measure;

    float angle;
    float bias;
    float rate;

    float P[2][2];
};

#endif