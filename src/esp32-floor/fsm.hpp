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

  bool is_empty() const;
};

const Command nullcmd = {0, nullptr};

struct RoverMemory {
  int seenTagId;
};

inline RoverMemory mem = {-1};

class State {
private:
  OpMode next;
public:
  virtual ~State() = default;
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
private:
  static const unsigned long timeout = 2000; // 1 second
  unsigned long start_time;
public:
  Pickup();
  void run(const Command &cmd) override;
  void reset() override;
};

class Delivery: public State {
private:
  void process_tag(const String& input);
public:
  Delivery();
  void run(const Command &cmd) override;
  void reset() override;
};

class Dropoff: public State {
private:
  static const unsigned long timeout = 5000;
  unsigned long start_time;
public:
  Dropoff();
  void run(const Command &cmd) override;
  void reset() override;
};