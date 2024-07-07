#ifndef APDS9960_HPP
#define APDS9960_HPP

#include "APDS9960.hpp"


APDS9960::APDS9960(hwlib::i2c_bus &i2c_bus)
    : i2c_bus(i2c_bus) {}

uint8_t APDS9960::readReg(uint8_t reg) {
    {
        auto transaction = i2c_bus.write(APDS9960_I2C_ADDR);
        transaction.write(reg);
    }

    uint8_t value;
    {
        auto transaction = i2c_bus.read(APDS9960_I2C_ADDR);
        transaction.read(value);
    }
    return value;
}

uint16_t APDS9960::readRegBlock(uint8_t reg, uint8_t* buffer, uint8_t length) {
    {
        auto transaction = i2c_bus.write(APDS9960_I2C_ADDR);
        transaction.write(reg);
    }

    {
        auto transaction = i2c_bus.read(APDS9960_I2C_ADDR);
        for (uint8_t i = 0; i < length; i++) {
            transaction.read(buffer[i]);
        }
    }
    return length;
}

void APDS9960::writeReg(uint8_t reg, uint8_t value) {
    auto transaction = i2c_bus.write(APDS9960_I2C_ADDR);
    transaction.write(reg);
    transaction.write(value);
}



bool APDS9960::init() {
    // determines device id. If not 0xA8 then error is given.
    uint8_t id = readReg(APDS9960_ID);
    hwlib::cout << "APDS-9960 ID: " << hwlib::hex << id << hwlib::endl;
    if (id != 0xA8) {
        hwlib::cout << "APDS-9960 not found, check your wiring and try again!!\n";
        return 0;
    }

    // set integration time
    uint16_t iTimeMS = 10;
    float temp = 256 - (iTimeMS / 2.78);
    temp = temp > 255 ? 255 : (temp < 0 ? 0 : temp);
    writeReg(APDS9960_ATIME, static_cast<uint8_t>(temp));
    
    writeReg(APDS9960_CONTROL, APDS9960_CONTROL_AGAIN_4X);

    // Disable all modes
    enableGesture(false);
    enableProximity(false);
    enableColor(false);   

    writeReg(APDS9960_AICLEAR, 0);
    
    // restart
    writeReg(APDS9960_REG_ENABLE, 0x00);
    hwlib::wait_ms(50);
    writeReg(APDS9960_REG_ENABLE, APDS9960_ENABLE_PON);
    hwlib::wait_ms(50);

    resetCounts();

    return 1;
}

void APDS9960::enableProximity(bool enable) {
    if (enable) {
        writeReg(APDS9960_REG_ENABLE, APDS9960_ENABLE_PON | APDS9960_ENABLE_PEN);
    } else {
        writeReg(APDS9960_REG_ENABLE, 0x00);
    }
}

int APDS9960::readProximity() {
    uint8_t proximity = readReg(APDS9960_PDATA);
    return static_cast<int>(proximity);
}


void APDS9960::enableColor(bool enable) {
    if (enable) {
        writeReg(APDS9960_REG_ENABLE, APDS9960_ENABLE_AIEN | APDS9960_ENABLE_WEN | APDS9960_ENABLE_AEN | APDS9960_ENABLE_PON);
    } else {
        writeReg(APDS9960_REG_ENABLE, 0);
    }
}

bool APDS9960::isColorAvailable() {
    uint8_t status = readReg(APDS9960_REG_ENABLE);
    return (status & APDS9960_ENABLE_AEN) && (status & APDS9960_ENABLE_PON);
}

void APDS9960::getColorData(uint16_t &red, uint16_t &green, uint16_t &blue, uint16_t &clear) {
    hwlib::wait_ms(50);

    red = readReg(APDS9960_RDATAL);
    red |= (readReg(APDS9960_RDATAH) << 8);

    green = readReg(APDS9960_GDATAL);
    green |= (readReg(APDS9960_GDATAH) << 8);

    blue = readReg(APDS9960_BDATAL);
    blue |= (readReg(APDS9960_BDATAH) << 8);

    clear = readReg(APDS9960_CDATAL);
    clear |= (readReg(APDS9960_CDATAH) << 8);
}



void APDS9960::enableGesture(bool enable) {
    if (enable) {
        writeReg(APDS9960_REG_ENABLE, 0x69);
        writeReg(APDS9960_GCONFIG4, 0x01);
    } else {
        writeReg(APDS9960_REG_ENABLE, 0);
    }
}


bool APDS9960::isGestureAvailable(){
    uint8_t status = readReg(APDS9960_GSTATUS);
    return (status & APDS9960_GVALID);
}

void APDS9960::resetCounts() {
  gesture_count = 0;
  up_count = 0;
  down_count = 0;
  left_count = 0;
  right_count = 0;
}


bool APDS9960::processGestureData(uint8_t* buffer, uint8_t n_bytes_to_read, int& z_axis_diff, int& x_axis_diff) {
    readRegBlock(APDS9960_GFIFO_U, buffer, n_bytes_to_read);

    z_axis_diff = 0;
    x_axis_diff = 0;

    // Determine if de axis differences are significant
    if (abs((int)buffer[0] - (int)buffer[1]) > 13)
        z_axis_diff += (int)buffer[0] - (int)buffer[1];

    if (abs((int)buffer[2] - (int)buffer[3]) > 13)
        x_axis_diff += (int)buffer[2] - (int)buffer[3];

    // Return true if there is a significant difference
    return (z_axis_diff != 0 || x_axis_diff != 0);
}



uint8_t APDS9960::calculateGestureDirection( int z_axis_diff, int x_axis_diff ) {
    uint8_t gestureReceived = 0;

    // determine direction based on difference on x-axis
     if ( x_axis_diff != 0 ) {
        if ( x_axis_diff < 0 ) {
            if ( right_count > 0 ) {
                gestureReceived = APDS9960_LEFT;
            } else {
                left_count++;
            }
        } else if ( x_axis_diff > 0 ) {
            if ( left_count > 0 ) {
                gestureReceived = APDS9960_RIGHT;
            } else {
                right_count++;
            }
        }
    }

    // determine direction based on difference on x-axis
    if ( z_axis_diff != 0 ) {
        if ( z_axis_diff < 0 ) {
            if ( down_count > 0 ) {
                gestureReceived = APDS9960_UP;
            } else {
                up_count++;
            }
        } else if ( z_axis_diff > 0 ) {
            if (up_count > 0) {
                gestureReceived = APDS9960_DOWN;
            } else {
                down_count++;
            }
        }
    }

    return gestureReceived;
}

uint8_t APDS9960::getGesture() {
    uint8_t buffer[256];
    unsigned long t = 0;
    uint8_t gestureReceived = 0;
    int z_axis_diff = 0;
    int x_axis_diff = 0;

    while ( true ) {
        //wait for gesture data to be available
        if ( !isGestureAvailable() )
            return 0;

        hwlib::wait_ms(30);

        //read number of bytes that are available on FIFO
        uint8_t n_bytes_to_read = readReg( APDS9960_GFLVL );
        if ( n_bytes_to_read == 0 ) {
            continue;
        }

        // Process the gesture data
        if ( processGestureData( buffer, n_bytes_to_read, z_axis_diff, x_axis_diff ) ) {
            t = hwlib::now_us();
        }

        gestureReceived = calculateGestureDirection( z_axis_diff, x_axis_diff );

        // If the gesture is valid it is returned. Otherwise timeout.
        if ( gestureReceived || hwlib::now_us() - t > 3000 ) {
            resetCounts();
            return gestureReceived;
        }
    }
}

#endif // APDS9960_HPP