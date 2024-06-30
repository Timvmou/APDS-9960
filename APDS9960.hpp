#ifndef APDS9960_HPP
#define APDS9960_HPP

#include "hwlib.hpp"

class APDS9960 {
private:
    hwlib::i2c_bus &i2c_bus;
    
    uint8_t readByte(uint8_t reg);
    void writeByte(uint8_t reg, uint8_t value);

public:
    APDS9960(hwlib::i2c_bus &i2c_bus);

    bool test();
    bool init();

    int readProximity();
    void enableProximity(bool enable);
};

#endif // APDS9960_HPP
