#include "hwlib.hpp"
#include "APDS9960.hpp"
#include "MyServo.hpp"

void proximity_test( APDS9960 &sensor ){
    sensor.enableProximity(true);

    for (;;) {
        hwlib::cout << hwlib::dec << sensor.readProximity() << "\n";
        hwlib::wait_ms(50);
    }
}

void color_test( APDS9960 &sensor ){
    sensor.enableColor(true);
    
    while ( true ) {
        if ( sensor.isColorAvailable() ){
            uint16_t red, green, blue, clear;
            sensor.getColorData(red, green, blue, clear);

            hwlib::cout << "Red: " << hwlib::dec << red << ", Green: " << hwlib::dec << green << ", Blue: " << hwlib::dec << blue << ", Clear: " << hwlib::dec << clear << hwlib::endl;
        } else {
            hwlib::cout << "Color data not available." << hwlib::endl;
        }

        hwlib::wait_ms(1000);
    }
}

void gesture_application(APDS9960 &sensor) {

    int base_angle = 90;
    int arm1_angle = 90;
    int old_base_angle = 90;
    int old_arm1_angle = 90;

    sensor.enableProximity(true);
    sensor.enableGesture(true);

    // Define both servo's and make them go to starting coordinate
    auto base_servo_pin = hwlib::target::pin_out(hwlib::target::pins::d10);
    MyServo base_servo(base_servo_pin);
    base_servo.write(base_angle);

    auto arm1_servo_pin = hwlib::target::pin_out(hwlib::target::pins::d11); 
    MyServo arm1_servo(arm1_servo_pin);
    arm1_servo.write(arm1_angle);

    hwlib::cout << "Begin met bewegen" << hwlib::endl;

    while (true) {
        uint8_t gesture = sensor.getGesture();

        // if gesture is detected, the angle of servo is changed by 30 (if not at max/min already)
        if (gesture == APDS9960_DOWN) {
            hwlib::cout << "DOWN" << hwlib::endl;
            arm1_angle = (arm1_angle < 30) ? 0 : arm1_angle - 30;

            for (int i = old_arm1_angle; i >= arm1_angle; i--) {
                arm1_servo.write(i);
                hwlib::wait_ms(15);
            }
        }
        else if (gesture == APDS9960_UP) {
            hwlib::cout << "UP" << hwlib::endl;
            arm1_angle = (arm1_angle > 150) ? 180 : arm1_angle + 30;

            for (int i = old_arm1_angle; i <= arm1_angle; i++) {
                arm1_servo.write(i);
                hwlib::wait_ms(15);
            }
        }
        else if (gesture == APDS9960_LEFT) {
            hwlib::cout << "LEFT" << hwlib::endl;
            base_angle = (base_angle < 30) ? 0 : base_angle - 30;

            for (int i = old_base_angle; i >= base_angle; i--) {
                base_servo.write(i);
                hwlib::wait_ms(15);
            }
        }
        else if (gesture == APDS9960_RIGHT) {
            hwlib::cout << "RIGHT" << hwlib::endl;
            base_angle = (base_angle > 150) ? 180 : base_angle + 30;

            for (int i = old_base_angle; i <= base_angle; i++) {
                base_servo.write(i);
                hwlib::wait_ms(15);
            }
        }

        if (old_arm1_angle != arm1_angle) {
            hwlib::cout << "Arm1 angle: " << arm1_angle << hwlib::endl;
            old_arm1_angle = arm1_angle;
        }

        if (old_base_angle != base_angle) {
            hwlib::cout << "Base angle: " << base_angle << hwlib::endl;
            old_base_angle = base_angle;
        }
    }
}

void servo_test(){

    auto servo_pin = hwlib::target::pin_out(hwlib::target::pins::d10);
    MyServo my_servo(servo_pin);

    while ( true ) { 
        hwlib::cout << "Left" << hwlib::endl;
        for (int pos = 0; pos < 101; pos++) { 
            hwlib::cout << pos << hwlib::endl;
            my_servo.write(pos);              
            hwlib::wait_ms(5);                      
        }
        hwlib::cout << "Right" << hwlib::endl;
        for (int pos = 100; pos > 0; pos--) {
            my_servo.write(pos);              
            hwlib::wait_ms(5);                      
        }
    }
}

int main() {
    hwlib::wait_ms(500);

    auto scl = hwlib::target::pin_oc(hwlib::target::pins::scl);
    auto sda = hwlib::target::pin_oc(hwlib::target::pins::sda);
    auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);
    APDS9960 sensor(i2c_bus);


    // Initialise the sensor
    if (!sensor.init()) {
        hwlib::cout << "Sensor initialization failed!" << hwlib::endl;
        return -1;
    }
    
    // Only use proximity sensor
    // proximity_test(sensor);

    // use the gesture sensor in combination with robot arm
    gesture_application(sensor);

    // Only use color sensor
    // color_test(sensor);

    // Test servo
    // servo_test();

}
