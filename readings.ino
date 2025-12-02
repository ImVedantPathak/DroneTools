#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Offsets
long ax_offset = 284, ay_offset = -21, az_offset = 4361;
long gx_offset = 101, gy_offset = 323, gz_offset = 126;

// Filtered angles
float angleX = 0, angleY = 0, angleZ = 0;
unsigned long lastTime = 0;

void calibrateMPU() {
  Serial.println("=== Calibration Starting ===");
  Serial.println("Keep the board perfectly still and flat...");

  long ax_sum = 0, ay_sum = 0, az_sum = 0;
  long gx_sum = 0, gy_sum = 0, gz_sum = 0;

  const int samples = 2000;

  for (int i = 0; i < samples; i++) {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    mpu.getAcceleration(&ax, &ay, &az);
    mpu.getRotation(&gx, &gy, &gz);

    ax_sum += ax;
    ay_sum += ay;
    az_sum += az - 16384;

    gx_sum += gx;
    gy_sum += gy;
    gz_sum += gz;

    delay(1);
  }

  ax_offset = ax_sum / samples;
  ay_offset = ay_sum / samples;
  az_offset = az_sum / samples;

  gx_offset = gx_sum / samples;
  gy_offset = gy_sum / samples;
  gz_offset = gz_sum / samples;


  Serial.print("Accel Offsets -> ");
  Serial.print("X: "); Serial.print(ax_offset);
  Serial.print("  Y: "); Serial.print(ay_offset);
  Serial.print("  Z: "); Serial.println(az_offset);

  Serial.print("Gyro Offsets  -> ");
  Serial.print("X: "); Serial.print(gx_offset);
  Serial.print("  Y: "); Serial.print(gy_offset);
  Serial.print("  Z: "); Serial.println(gz_offset);

  Serial.println("=== Calibration Done ===");
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  mpu.initialize();
  Serial.println(mpu.testConnection() ? "MPU Connected" : "MPU FAIL");

  // calibrateMPU();
  lastTime = micros();
}

void loop() {
  int16_t ax_raw, ay_raw, az_raw;
  int16_t gx_raw, gy_raw, gz_raw;

  mpu.getAcceleration(&ax_raw, &ay_raw, &az_raw);
  mpu.getRotation(&gx_raw, &gy_raw, &gz_raw);

  // Apply offsets
  float ax = ax_raw - ax_offset;
  float ay = ay_raw - ay_offset;
  float az = az_raw - az_offset;

  float gx = gx_raw - gx_offset;
  float gy = gy_raw - gy_offset;
  float gz = gz_raw - gz_offset;

  // Gyro degrees/s
  gx /= 131.0;
  gy /= 131.0;
  gz /= 131.0;

  // delta time
  unsigned long now = micros();
  float dt = (now - lastTime) / 1e6;
  lastTime = now;

  // Integrate yaw
  angleZ += gz * dt;

  // Accelerometer angle
  float accAngleX = atan2(ay, az) * 57.2958;
  float accAngleY = atan2(-ax, az) * 57.2958;

  // Complementary filter
  angleX = 0.98 * (angleX + gx * dt) + 0.02 * accAngleX;
  angleY = 0.98 * (angleY + gy * dt) + 0.02 * accAngleY;

  // For flipping purposes
  float visRoll  = angleX;   // roll
  float visPitch = angleY;   // pitch
  float visYaw   = angleZ;   // yaw 


  Serial.print(visRoll);
  Serial.print(",");
  Serial.print(visPitch);
  Serial.print(",");
  Serial.println(visYaw);
}