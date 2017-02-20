#pragma once

class Transition
{
public:
  Transition(char* mask, unsigned int ledCount);

  virtual void reset() = 0;
  virtual void advance() = 0;
  virtual bool isCompleted() const = 0;

  char* mask() const { return _mask; }

protected:
  unsigned int _ledCount;
  char* _mask;
};

class FixedDurationTransition : public Transition
{
public:
  FixedDurationTransition(char* mask, int ledCount) : Transition(mask, ledCount) {}

  void reset();
  void advance();
  bool isCompleted() const { return _currentFrame >= duration(); }
  virtual unsigned int duration() const;
  virtual void update() = 0;
  
protected:
  unsigned int currentFrame() const { return _currentFrame; }

private:
  unsigned int _currentFrame = 0;
};

