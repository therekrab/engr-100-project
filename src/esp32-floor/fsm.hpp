#pragma once

enum class OpMode : char {
  Unknown,
  Vacant,
  Pickup,
  Delivery,
  Dropoff,
};

struct Command {
  char type;
  String *cmd;
};

const Command nullcmd = {0, nullptr};

class State {
private:
  OpMode next;
public:
  void set_next(OpMode opmode);

  virtual void run(const Command &cmd) = 0;
  virtual void reset() = 0;

  State* get_next();
};

State* get_state(OpMode opmode);

class Vacant: public State {
private:
  void process_tag(const String& input);
public:
  Vacant();
  void run(const Command &cmd) override;
  void reset() override;
};

class Pickup: public State {
public:
  Pickup();
  void run(const Command &cmd) override;
  void reset() override;
};

class Delivery: public State {
public:
  Delivery();
  void run(const Command &cmd) override;
  void reset() override;
};

class Dropoff: public State {
public:
  Dropoff();
  void run(const Command &cmd) override;
  void reset() override;
};