#include "PersistentData.h"

#ifdef USE_EEPROM
#include <EEPROM.h>
#else
char DATA_STORE[4096] = { 0, 0,
                          2,
                          0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
                          1,
                          0xCC, 3 };
                          
#endif

PersistentData::PersistentData() {
#ifdef USE_EEPROM
#else
#endif
}

char PersistentData::readByte(int addr) {
#ifdef USE_EEPROM
  return EEPROM.read(addr);
#else
  return DATA_STORE[addr];
#endif
}

void PersistentData::writeByte(int addr, char val) {
#ifdef USE_EEPROM
  EEPROM.write(addr, val);
#else
  DATA_STORE[addr] = val;
#endif  
}

PersistentData Persist;

