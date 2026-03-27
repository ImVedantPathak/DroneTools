#include "kalman.h"

kalman::kalman() {
    Q_angle = 0.001f;
    Q_bias = 0.003f;
    R_measure = 0.03f;

    angle = 0.0f;
    bias = 0.0f;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            P[i][j] = 0.0f;
        }
    }
};

float kalman::getAngle(float newAngle, float newRate, float dt){
    P[0][0] += dt * (dt*P[1][1] - P[0][1] - P[1][0] + Q_angle);
    P[0][1] -= dt * P[1][1];
    P[1][0] -= dt * P[1][1];
    P[1][1] += Q_bias * dt;

    float S = P[0][0] + R_measure; // error
    float K[2]; // Kalman gain - This is a 2x1 vector
    K[0] = P[0][0] / S;
    K[1] = P[1][0] / S;

    float y = newAngle - angle; // angle diff
    angle += K[0] * y;
    bias += K[1] * y;

    float P00_temp = P[0][0];
    float P01_temp = P[0][1];

    P[0][0] -= K[0] * P00_temp;
    P[0][1] -= K[0] * P01_temp;
    P[1][0] -= K[1] * P00_temp;
    P[1][1] -= K[1] * P01_temp;

    return angle;
};

void Kalman::setAngle(float angle) { this->angle = angle; }; // set angle as start
float Kalman::getRate() { return this->rate; }; // return the unbiased rate

// tune kalman
void Kalman::setQangle(float Q_angle) { this->Q_angle = Q_angle; };
void Kalman::setQbias(float Q_bias) { this->Q_bias = Q_bias; };
void Kalman::setRmeasure(float R_measure) { this->R_measure = R_measure; };

float Kalman::getQangle() { return this->Q_angle; };
float Kalman::getQbias() { return this->Q_bias; };
float Kalman::getRmeasure() { return this->R_measure; };