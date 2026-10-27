#include "fsm.hpp"
#include "utils.hpp"

State* active_state = get_state(OpMode::Vacant);

void setup() {
  Serial.begin(115200);

  // Initialize camera BEFORE WiFi
  cameraServerSetup();

  // Connect to WiFi after camera memory has been allocated
  WiFiSetup();

  // Start camera stream after WiFi is connected
  Serial.println("Starting camera server...");
  startCameraServer();
  Serial.println("Camera server started.");
}

void loop() {
  WiFiClient client = server.accept();
  if (!client) {
    return;
  }
  while (client.connected()) {
    if (!client.available()) {
      active_state->run(nullcmd);
      active_state = active_state->get_next();
      continue;
    }
    char first = client.read();
    String raw_input = client.readStringUntil('\n');
    raw_input.trim();

    Command cmd = {first, &raw_input};

    active_state->run(cmd);
    active_state = active_state->get_next();
  }
  Serial.println("STOP");
  client.stop();
  // Serial.println("Client Disconnected.");
}
