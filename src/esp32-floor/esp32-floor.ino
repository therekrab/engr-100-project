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
  active_state = get_state(OpMode::Vacant);
  active_state->reset();

  while (client.connected()) {
    if (!client.available()) {
      // Let the states run anyways, in case there's anything to udpate.
      active_state->run(nullcmd);
      active_state = active_state->get_next();
      continue;
    }
    char first = client.read();
    String raw_input = client.readStringUntil('\n');
    raw_input.trim();

    Command cmd = {first, &raw_input};

    // Handle higher-level commands that are relevant regardless of current state.
    switch (cmd.type) {
      case 'P':
        updatePID(raw_input);
        cmd = nullcmd; // Reset it, I guess
        break;
    }

    active_state->run(cmd);
    active_state = active_state->get_next();
  }
  stopcar();
  client.stop();
  // Serial.println("Client Disconnected.");
}
