#pragma once

class PersistentData
{
public:
  PersistentData();
  
  char readByte(int addr);
  void writeByte(int addr, char byte);
};

extern PersistentData Persist;
