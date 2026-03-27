import processing.serial.*;

Serial myPort;

float visRoll = 0;
float visPitch = 0;
float visYaw = 0;

void setup() {
  size(900, 700, P3D);

  // Change this to your COM port:
  try {
    myPort = new Serial(this, "COM3", 115200);
  } catch(Exception e) {
    println("Serial failed: " + e);
  }

  myPort.bufferUntil('\n');

  strokeWeight(3);
}

void draw() {
  background(20);
  lights();

  translate(width/2, height/2, width/2);
  translate(0, 100, -700);

  // X-frame correction
  //rotateY(radians(45));

  // Correct rotation order and mapping
  rotateY(radians(visYaw));    // yaw
  rotateX(radians(visPitch));  // pitch
  rotateZ(radians(visRoll));   // roll

  drawAxes(200);
  drawDrone();
}

void serialEvent(Serial p) {
  String line = p.readStringUntil('\n');
  if (line == null) return;

  line = trim(line);
  if (!line.contains(",")) return;

  String[] parts = split(line, ',');

  if (parts.length >= 3) {
    visRoll = float(parts[0]);
    visPitch = float(parts[1]);
    visYaw = float(parts[2]);
  }
  
  //visRoll = float(parts[0]);
  //visPitch = float(parts[1]);
  //visYaw = float(parts[2]);
}

void drawAxes(float size) {
  strokeWeight(4);

  // X axis (RED) - roll
  stroke(255, 0, 0);
  line(0, 0, 0, size, 0, 0);

  // Y axis (GREEN) - pitch
  stroke(0, 255, 0);
  line(0, 0, 0, 0, size, 0);

  // Z axis (BLUE) - yaw
  stroke(0, 0, 255);
  line(0, 0, 0, 0, 0, size);

  strokeWeight(3);
  stroke(255); // reset stroke color
}


void drawDrone() {

  // Center plates
  pushMatrix();
  fill(40);
  box(120, 15, 120);
  popMatrix();

  pushMatrix();
  translate(0, -10, 0);
  fill(60);
  box(140, 10, 140);
  popMatrix();

  // X-frame arms (45° rotated)
  drawArm( 150,  150);  // front-right
  drawArm(-150,  150);  // front-left
  drawArm(-150, -150);  // back-left
  drawArm( 150, -150);  // back-right
}

void drawArm(float x, float z) {
  pushMatrix();

  // Move to middle of the arm
  translate(x/2, 0, z/2);

  // Compute angle to motor
  float angle = atan2(z, x);

  // Rotate so the beam points toward the motor
  rotateY(-angle);

  // Draw a thin arm (length = distance from center to motor)
  float armLength = dist(0, 0, x, z);
  fill(80);
  box(armLength, 20, 20);

  // Move to motor location
  translate(armLength/2, 0, 0);

  // Motor bell
  fill(200, 200, 80);
  cylinder(30, 40);

  // Prop disc
  fill(150, 150, 250);
  cylinder(45, 5);

  popMatrix();
}


// Utility cylinder
void cylinder(float radius, float height) {
  int sides = 40;
  float angleStep = TWO_PI / sides;

  // Side walls
  beginShape(QUAD_STRIP);
  for (float angle = 0; angle <= TWO_PI + angleStep; angle += angleStep) {
    float x = cos(angle) * radius;
    float z = sin(angle) * radius;
    vertex(x, -height/2, z);
    vertex(x,  height/2, z);
  }
  endShape();

  // Top cap
  beginShape(TRIANGLE_FAN);
  vertex(0, -height/2, 0);
  for (float angle = 0; angle <= TWO_PI + angleStep; angle += angleStep) {
    float x = cos(angle) * radius;
    float z = sin(angle) * radius;
    vertex(x, -height/2, z);
  }
  endShape();

  // Bottom cap
  beginShape(TRIANGLE_FAN);
  vertex(0, height/2, 0);
  for (float angle = 0; angle <= TWO_PI + angleStep; angle += angleStep) {
    float x = cos(angle) * radius;
    float z = sin(angle) * radius;
    vertex(x, height/2, z);
  }
  endShape();
}