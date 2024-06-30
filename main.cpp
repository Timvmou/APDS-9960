#include "hwlib.hpp"
#include "APDS9960.hpp"

int main() {
    // Wait for the terminal to be ready
    hwlib::wait_ms(500);

    // Create I2C pins
    auto scl = hwlib::target::pin_oc(hwlib::target::pins::scl);
    auto sda = hwlib::target::pin_oc(hwlib::target::pins::sda);
    auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

    // Instantiate the APDS9960 sensor object
    APDS9960 sensor(i2c_bus);

    // Initialize the sensor
    if (sensor.init()) {
        // Enable proximity sensing
        sensor.enableProximity(true);

        // Continuously read and print proximity data
        while (true) {
            int proximity = sensor.readProximity();
            hwlib::cout << "Proximity: " << proximity << hwlib::endl;
            hwlib::wait_ms(1000);  // Read every second
        }
    } else {
        hwlib::cout << "Sensor initialization failed!" << hwlib::endl;
    }

    return 0;
}
