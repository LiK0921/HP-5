#include "imx6ul.h"

#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_beep.h"

int main(void) 
{
    u8 i = 0;
    u8 key_value = 0;
    u8 led_state = OFF;
    u8 beep_state = ON;

    im6ull_clk_init();
    clk_enble();
    led_init();
    beep_init();
    key_init();

    while (1)
    {     
        key_value = key_getvalue();
        if (key_value)
        {
            switch (key_value)
            {
                case KEY0_VALUE:
                    beep_state = !beep_state;
                    beep_switch(beep_state);
                    break;
                
                default:
                    break;
            }
        }
        i++;
        if (i == 20)
        {
            i = 0;
            led_state = !led_state;
			led_switch(LED0, led_state);
        } 
        delay(10);      
    }  
    return 0;
}




