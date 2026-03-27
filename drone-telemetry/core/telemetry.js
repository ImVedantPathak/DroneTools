export class TelemetryBuffer {
  constructor(maxSize = 2000) {
    this.buffer = [];
    this.maxSize = maxSize;
  }

  push(state) {
    this.buffer.push({
      time: performance.now(),
      pos: state.pos.clone(),
      rot: state.rot.clone(),
      vel: state.vel?.clone() || null
    });

    if (this.buffer.length > this.maxSize) {
      this.buffer.shift();
    }
  }

  getAll() {
    return this.buffer;
  }
}