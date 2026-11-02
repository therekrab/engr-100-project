#include "esp32-hal.h"
#include "Arduino.h"
#include "fsm.hpp"
#include "utils.hpp"

bool Command::is_empty() const {
  return cmd == nullptr || type == 0;
}

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
  if (cmd.is_empty()) {
    // For now, we have nothing to do in this case.
    return;
  }

  const String& raw_input = *cmd.cmd;
  switch (cmd.type) {
      case 'L':
        follow_line(raw_input);
        break;
      case 'T':
        process_tag(raw_input);
        break;
  }
}

void Vacant::process_tag(const String& input) {
  mem.seenTagId = input.toInt();
  stopcar();
  set_next(OpMode::Pickup);
}

void Pickup::run(const Command& cmd) {
  if (millis() - start_time > timeout) {
    set_next(OpMode::Delivery);
    send_set_state_message(0, 0);
  }
}

void Delivery::run(const Command& cmd) {
  if (cmd.is_empty()) {
    return;
  }

  const String& raw_input = *cmd.cmd;
  switch (cmd.type) {
      case 'L':
        follow_line(raw_input);
        break;
      case 'T':
        process_tag(raw_input);
        break;
  }
}

void Delivery::process_tag(const String& input) {
  int seen = input.toInt();
  if (mem.seenTagId == seen) {
    stopcar();
    set_next(OpMode::Dropoff);
  }
}

void Dropoff::run(const Command& cmd) {
  if (millis() - start_time > timeout) {
    set_next(OpMode::Vacant);
    send_set_state_message(0, 0);
  }
}

void Vacant::reset() {
  set_next(OpMode::Vacant);
  mem.seenTagId = -1;
  send_set_state_message(0, 1 << 0);
}
void Pickup::reset() {
  set_next(OpMode::Pickup);
  start_time = millis();
  send_set_state_message(0, 1 << 1);
}
void Delivery::reset() {
  set_next(OpMode::Delivery);
  send_set_state_message(0, 1 << 2);
}
void Dropoff::reset() {
  set_next(OpMode::Dropoff);
  start_time = millis();
  send_set_state_message(0, 1 << 3);
}