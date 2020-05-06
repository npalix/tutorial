#include "mbed.h"
#include "standby.h"

DigitalOut myled(LED1);
Serial pc(USBTX, USBRX);

int main() {
    verification_standby();
    myled = 1; // LED is ON
    wait(0.2); // 200 ms
    myled = 0; // LED is OFF
        
    pc.printf("\n###### La carte rentre au mode STANDBY\n\r");
    standby_mode(10); // go to sleep for 120 sec
    pc.printf("\nFail..\n");
}