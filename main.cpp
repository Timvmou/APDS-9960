#include "hwlib.hpp"

#include "APDS9960.hpp"

int main() {

    hwlib::wait_ms( 500 );

    auto scl = hwlib::target::pin_oc( hwlib::target::pins::scl );
    auto sda = hwlib::target::pin_oc( hwlib::target::pins::sda );
    auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );

    APDS9960 sensor(i2c_bus);

    if ( sensor.init() ) {
        sensor.enableProximity( true );

        for (;;){
            int proximity = sensor.readProximity();
            hwlib::cout << "Proximity: " << hwlib::dec << proximity << hwlib::endl;

            hwlib::wait_ms(500);
        }
    } 

    return 0;
}
