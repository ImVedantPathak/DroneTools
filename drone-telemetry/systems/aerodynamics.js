import * as THREE from 'https://cdn.jsdelivr.net/npm/three@0.152.2/build/three.module.js';

export class AeroSystem {
  constructor(scene) {
    this.lift = new THREE.ArrowHelper(new THREE.Vector3(), new THREE.Vector3(), 1, 0x00ff00);
    this.drag = new THREE.ArrowHelper(new THREE.Vector3(), new THREE.Vector3(), 1, 0xff0000);
    this.thrust = new THREE.ArrowHelper(new THREE.Vector3(), new THREE.Vector3(), 1, 0x0000ff);

    scene.add(this.lift);
    scene.add(this.drag);
    scene.add(this.thrust);
  }

  update(drone, velocity) {
    const pos = drone.position;
    const vel = velocity.clone();
    const speed = vel.length();

    if (speed < 0.001) return;

    const velDir = vel.clone().normalize();

    // DRAG (opposite velocity)
    this.drag.position.copy(pos);
    this.drag.setDirection(velDir.clone().negate());
    this.drag.setLength(speed * 0.5);

    // LIFT (approx: drone up direction)
    const up = new THREE.Vector3(0, 1, 0).applyEuler(drone.rotation);
    this.lift.position.copy(pos);
    this.lift.setDirection(up);
    this.lift.setLength(speed * 0.3);

    // THRUST (same axis as lift for quadcopters)
    this.thrust.position.copy(pos);
    this.thrust.setDirection(up);
    this.thrust.setLength(2);
  }
}