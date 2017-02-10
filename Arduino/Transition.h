#pragma once

class Transition
{
public:
  Transition(char* mask, int ledCount);

  virtual void reset() = 0;
  virtual void advance() = 0;
  virtual bool isCompleted() const = 0;

  char* mask() const { return _mask; }

protected:
  int _ledCount;
  char* _mask;
};

class FixedDurationTransition : public Transition
{
public:
  FixedDurationTransition(char* mask, int ledCount) : Transition(mask, ledCount) {}

  void setDuration(int frameCount);

  void reset();
  void advance();
  bool isCompleted() const { return _currentFrame >= _frameCount; }
  virtual void update() = 0;
  
protected:
  int frameCount() const { return _frameCount; }
  int currentFrame() const { return _currentFrame; }

private:
  int _frameCount = 12;
  int _currentFrame = 0;
};

class Fade : public FixedDurationTransition
{
public:
  Fade(char* mask, int ledCount) : FixedDurationTransition(mask, ledCount) {}

protected:
  void update();
};

