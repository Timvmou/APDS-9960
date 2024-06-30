#include "APDS9960.hpp"

// APDS9960 I2C address and register addresses
#define APDS9960_I2C_ADDR 0x39

#define APDS9960_ID 0x92
#define EXPECTED_ID_1 0xAB
#define EXPECTED_ID_2 0xA8

#define APDS9960_ENABLE 0x80
#define APDS9960_PDATA 0x9C
#define APDS9960_CONTROL 0x8F
#define APDS9960_PILT 0x89
#define APDS9960_PIHT 0x8B
#define APDS9960_PPULSE 0x8E

APDS9960::APDS9960(hwlib::i2c_bus &i2c_bus)
    : i2c_bus(i2c_bus) {}

uint8_t APDS9960::readByte(uint8_t reg) {
    i2c_bus.write(APDS9960_I2C_ADDR, &reg, 1);

    uint8_t value;
    i2c_bus.read(APDS9960_I2C_ADDR, &value, 1);
    return value;
}

void APDS9960::writeByte(uint8_t reg, uint8_t value) {
    uint8_t data[] = { reg, value };
    i2c_bus.write(APDS9960_I2C_ADDR, data, 2);
}

bool APDS9960::test() {
    uint8_t id = readByte(APDS9960_ID);
    hwlib::cout << "ID: " << hwlib::hex << unsigned(id) << hwlib::endl;
    return (id == EXPECTED_ID_1 || id == EXPECTED_ID_2);
}

bool APDS9960::init() {
    if (!test()) {
        hwlib::cout << "BOOOO!" << hwlib::endl;
        return false;
    }

    writeByte(APDS9960_CONTROL, 0x05); 
    writeByte(APDS9960_PILT, 0);
    writeByte(APDS9960_PIHT, 255); 
    writeByte(APDS9960_PPULSE, 0x87);
    writeByte(APDS9960_ENABLE, 0x05); 

    hwlib::cout << "HUZZAH" << hwlib::endl;
    return true;
}

int APDS9960::readProximity() {
    uint8_t proximity = readByte(APDS9960_PDATA);
    return static_cast<int>(proximity);
}

void APDS9960::enableProximity(bool enable) {
    if (enable) {
        writeByte(APDS9960_ENABLE, 0b00000101);
    } else {
        writeByte(APDS9960_ENABLE, 0b00000000);
    }
}
