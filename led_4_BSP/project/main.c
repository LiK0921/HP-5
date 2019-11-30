#include "main.h"

#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"


int main(void) 
{
    clk_enble();

    led_init();

    while (1)
    {     
        led_switch(LED0, ON);
        delay(100);
        led_switch(LED0, OFF);
        delay(500);
    }  
    return 0;
}




