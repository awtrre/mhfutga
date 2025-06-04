#include <reg52.h>
#include "buttons.h"
#include "motor.h"  

void main(void) {
    Buttons_Init();

    while (1) {
        Button1_Handle();
				Button2_Handle();


        delay_ms(10);
    }
}
