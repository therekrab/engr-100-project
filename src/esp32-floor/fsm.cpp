#include "Arduino.h"
#include "fsm.hpp"
#include "utils.hpp"

State* get_state(OpMode opmode) {
  static State* vacant = new Vacant();
  static State* pickup = new Pickup();
  static State* delivery = new Delivery();
  static State* dropoff = new Dropoff();
  switch (opmode) {
  case OpMode::Vacant:
    return vacant;
  case OpMode::Pickup:
    return pickup;
  case OpMode::Delivery:
    return delivery;
  case OpMode::Dropoff:
    return dropoff;
  default:
    return vacant;
  }
}

void State::set_next(OpMode opmode) {
  next = opmode;
}

State* State::get_next() {
  State* state = get_state(next);
  if (state != this) {
    state->reset();
  }
  return state;
}

Vacant::Vacant() {
  reset();
}
Pickup::Pickup() {
  reset();
}
Delivery::Delivery() {
  reset();
}
Dropoff::Dropoff() {
  reset();
}

void Vacant::run(const Command& cmd) {
  if (cmd.type == 0 || cmd.cmd == nullptr) {
    // For now, we have nothing to do in this case. Stop.
    return;
  }
  const String& raw_input = *cmd.cmd;
  switch (cmd.type) {
      case 'L':
        Serial.print("D");
        follow_line(raw_input);
        break;
      case 'P':
        updatePID(raw_input);
        break;
      case 'T':
        process_tag(raw_input);
        break;
  }
}
void Vacant::process_tag(const String& input) {
  int tagId = input.toInt();
  if (tagId % 2 == 0) {
    Serial.println("S0,1");
  } else {
    Serial.println("S0,0");
  }
}

void Pickup::run(const Command& cmd) {}
void Delivery::run(const Command& cmd) {}
void Dropoff::run(const Command& cmd) {}

void Vacant::reset() {}
void Pickup::reset() {}
void Delivery::reset() {}
void Dropoff::reset() {}