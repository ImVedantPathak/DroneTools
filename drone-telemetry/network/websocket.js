export function createSocket(onData) {
  const socket = new WebSocket("ws://localhost:8080");

  socket.onmessage = (event) => {
    const data = JSON.parse(event.data);
    onData(data);
  };

  socket.onopen = () => {
    console.log("Connected to bridge");
  };
}