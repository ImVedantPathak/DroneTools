import * as THREE from 'https://cdn.jsdelivr.net/npm/three@0.152.2/build/three.module.js';

import { createScene } from './core/scene.js';
import { createDrone } from './core/drone.js';
import { TelemetryBuffer } from './core/telemetry.js';

import { AeroSystem } from './systems/aerodynamics.js';
import { createIMUChart } from './systems/imuGraph.js';

import { createSocket } from './network/websocket.js';

// =======================
// SETUP
// =======================

const { scene, camera, renderer } = createScene();

const drone = createDrone();
scene.add(drone);

const telemetry = new TelemetryBuffer();
const aero = new AeroSystem(scene);
const imuChart = createIMUChart();

let velocity = new THREE.Vector3();
let latestData = null;

// =======================
// WEBSOCKET CONNECTION
// =======================

createSocket((data) => {
  latestData = data;
});

// =======================
// APPLY TELEMETRY
// =======================

function updateFromTelemetry(data) {
  if (!data) return;

  // Convert degrees → radians (VERY IMPORTANT)
  const roll = THREE.MathUtils.degToRad(data.roll || 0);
  const pitch = THREE.MathUtils.degToRad(data.pitch || 0);
  const yaw = THREE.MathUtils.degToRad(data.yaw || 0);

  // Apply rotation (order matters)
  drone.rotation.set(pitch, yaw, roll);

  // Position (optional)
  drone.position.set(
    data.x || 0,
    data.y || 0,
    data.z || 0
  );

  // Velocity (temporary: using accel if no velocity provided)
  velocity.set(
    data.vx ?? data.ax ?? 0,
    data.vy ?? data.ay ?? 0,
    data.vz ?? data.az ?? 0
  );

  // Store telemetry
  telemetry.push({
    pos: drone.position,
    rot: drone.rotation,
    vel: velocity
  });
}

// =======================
// IMU GRAPH UPDATE
// =======================

function updateIMU(chart, data) {
  if (!data) return;

  chart.data.labels.push('');

  chart.data.datasets[0].data.push(data.ax || 0);
  chart.data.datasets[1].data.push(data.ay || 0);
  chart.data.datasets[2].data.push(data.az || 0);

  // Keep last 100 samples
  if (chart.data.labels.length > 100) {
    chart.data.labels.shift();
    chart.data.datasets.forEach(d => d.data.shift());
  }

  chart.update();
}

// =======================
// CAMERA (STABLE FOLLOW)
// =======================

function updateCamera() {
  const offset = new THREE.Vector3(0, 2, 5);
  const targetPos = drone.position.clone().add(offset);

  camera.position.lerp(targetPos, 0.1);
  camera.lookAt(drone.position);
}

// =======================
// MAIN LOOP
// =======================

function animate() {
  requestAnimationFrame(animate);

  if (latestData) {
    updateFromTelemetry(latestData);
    updateIMU(imuChart, latestData);
  }

  // Aerodynamic vectors
  aero.update(drone, velocity);

  // Camera follow
  updateCamera();

  renderer.render(scene, camera);
}

animate();